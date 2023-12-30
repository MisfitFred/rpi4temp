/**
 * Copyright (c) 2022 Andrew McDonnell
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * Copyright (c) 2023 Misfit Fred
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * applied changes on of the original file from Andrew McDonnell:
 *  - Change main function name to start_udp
 *  - remove stdio_init_all() call
 *  - remove poll implementation
 */

#include <string.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/dhcp.h"

#include "wifi_manager.h"

const UBaseType_t WIFI_MANAGER_TASK_PRIORITY = tskIDLE_PRIORITY + 2UL;

typedef enum
{
    WIFI_CONNECTION_PROCEDURE_STATUS_IDLE,
    WIFI_CONNECTION_PROCEDURE_STATUS_CONNECTING,
    WIFI_CONNECTION_PROCEDURE_STATUS_CONNECTED,
    WIFI_CONNECTION_PROCEDURE_STATUS_FAILED,
    WIFI_CONNECTION_PROCEDURE_STATUS_FAILED_BAD_AUTH
} wifi_connection_procedure_status_t;

typedef enum
{
    WIFI_MANAGER_UNINITIALIZED,
    WIFI_MANAGER_STOPPED,
    WIFI_MANAGER_INIT,
    WIFI_MANAGER_RUN,
    WIFI_MANAGER_DEINIT
} wifi_manager_state_t;

const uint32_t WIFI_FAILED_CONNECTION_WAIT_TIME_TILL_RETRY_MS = 10000;
const uint32_t WIFI_FAILED_BAD_AUTH_WAIT_TIME_TILL_RETRY_MS = 120000;
const uint32_t WIFI_CONNECTING_TIMEOUT_MS = 30000;

const uint32_t WIFI_MANAGER_TASK_INTERVAL_MS = 100;

const uint32_t WIFI_MANAGER_RUNNABLE_DELAY_COUNT_VALUE(const uint32_t delay_ms) { return delay_ms / WIFI_MANAGER_TASK_INTERVAL_MS; }

typedef struct
{
    int8_t linkStatus;

    uint16_t connectionRetries;
    const uint16_t BADAUTH_MAX_CONNECTION_RETRIES;
    uint8_t badAuthRetries;
    uint32_t timeoutCounter;
} wifi_connection_t;

typedef struct
{
    wifi_manager_state_t state;   /**< State of the wifi manager */
    TaskHandle_t *task;           /**< Pointer to the wifi manager task */
    wifi_connection_t connection; /**< Connection state */

    bool connected; /**< Flag indicating if the UDP beacon is connected */
} wifi_manager_attributes_t;

static wifi_manager_attributes_t this = {
    .state = WIFI_MANAGER_UNINITIALIZED,
    .task = NULL,
    .connection = {
        .linkStatus = CYW43_LINK_DOWN,
        .connectionRetries = 0,
        .BADAUTH_MAX_CONNECTION_RETRIES = 3,
        .badAuthRetries = 0,
        .timeoutCounter = 0,
    },
};

void wifi_manager_task(__unused void *params);

/**
 * @brief Initialization of wifi manager
 *
 *  Creates the wifi manager task
 *
 */
wifi_manager_error_t wifi_manager_init(void)
{
    if (WIFI_MANAGER_UNINITIALIZED == this.state)
    {
        if (pdPASS == xTaskCreate(wifi_manager_task, "wifi manager task", configMINIMAL_STACK_SIZE, NULL, WIFI_MANAGER_TASK_PRIORITY, this.task))
        {
            this.state = WIFI_MANAGER_STOPPED;
        }
        else
        {
            return WIFI_MANAGER_ERROR_MEM;
        }
    }
    return WIFI_MANAGER_ERROR_NONE;
}

/**
 * @brief  Supervise the connection procedure and keep the device connected to the wifi
 *
 */
static void wifi_superviseConnection_run(void)
{

    static wifi_connection_procedure_status_t wifiConnectionProcedureStatus = WIFI_CONNECTION_PROCEDURE_STATUS_IDLE;

    static const uint8_t STATE_HISTORY_SIZE = 50;                                                         // remove only for debugging purposes
    static uint8_t stateHistoryIndex = 0;                                                                 // remove only for debugging purposes
    static wifi_connection_procedure_status_t stateHistory[50] = {WIFI_CONNECTION_PROCEDURE_STATUS_IDLE}; // remove only for debugging purposes

    this.connection.linkStatus = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);

    switch (wifiConnectionProcedureStatus)
    {
    case WIFI_CONNECTION_PROCEDURE_STATUS_IDLE:
        if (CYW43_LINK_DOWN != this.connection.linkStatus)
        {
            // looks like we are in the wrong state, however the appropriate handle is taken care of in failure state
            wifiConnectionProcedureStatus = WIFI_CONNECTION_PROCEDURE_STATUS_FAILED;
        }
        else
        {
            wifiConnectionProcedureStatus = WIFI_CONNECTION_PROCEDURE_STATUS_CONNECTING;
            if (cyw43_arch_wifi_connect_async(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK))
            {
                // failed to start connection
                wifiConnectionProcedureStatus = WIFI_CONNECTION_PROCEDURE_STATUS_FAILED;
            }
        }
        break;

    case WIFI_CONNECTION_PROCEDURE_STATUS_CONNECTING:
        if (CYW43_LINK_UP == this.connection.linkStatus)
        {
            // We are connected and got an IP address
            this.connection.badAuthRetries = 0;
            wifiConnectionProcedureStatus = WIFI_CONNECTION_PROCEDURE_STATUS_CONNECTED;
        }
        else if (CYW43_LINK_BADAUTH == this.connection.linkStatus)
        {
            // bad authentication
            this.connection.badAuthRetries++;
            wifiConnectionProcedureStatus = WIFI_CONNECTION_PROCEDURE_STATUS_FAILED;
        }
        else if (CYW43_LINK_DOWN > this.connection.linkStatus)
        {
            // something went wrong
            wifiConnectionProcedureStatus = WIFI_CONNECTION_PROCEDURE_STATUS_FAILED;
        }
        else if (CYW43_LINK_JOIN == this.connection.linkStatus || CYW43_LINK_NOIP == this.connection.linkStatus)
        {
            // still connecting check for timeout @todo
            if (this.connection.timeoutCounter > WIFI_MANAGER_RUNNABLE_DELAY_COUNT_VALUE(WIFI_CONNECTING_TIMEOUT_MS))
            {
                // timeout
                this.connection.timeoutCounter = 0;
                wifiConnectionProcedureStatus = WIFI_CONNECTION_PROCEDURE_STATUS_FAILED;
            }
            else
            {
                this.connection.timeoutCounter++;
            }
        }
        else
        {
            // something went wrong
            wifiConnectionProcedureStatus = WIFI_CONNECTION_PROCEDURE_STATUS_FAILED;
        }
        break;

    case WIFI_CONNECTION_PROCEDURE_STATUS_CONNECTED:
        if (CYW43_LINK_UP == this.connection.linkStatus)
        {
            // We are connected and got an IP address
            this.connection.badAuthRetries = 0;
            wifiConnectionProcedureStatus = WIFI_CONNECTION_PROCEDURE_STATUS_CONNECTED;
        }
        else
        {
            // something went wrong
            wifiConnectionProcedureStatus = WIFI_CONNECTION_PROCEDURE_STATUS_FAILED;
        }
        break;

    case WIFI_CONNECTION_PROCEDURE_STATUS_FAILED:
        if (CYW43_LINK_UP == this.connection.linkStatus)
        {
            // We are connected and got an IP address
            wifiConnectionProcedureStatus = WIFI_CONNECTION_PROCEDURE_STATUS_CONNECTED;
        } // max authentication failures reached
        else if (this.connection.badAuthRetries > this.connection.BADAUTH_MAX_CONNECTION_RETRIES)
        {
            // max authentication failures reached
            wifiConnectionProcedureStatus = WIFI_CONNECTION_PROCEDURE_STATUS_FAILED_BAD_AUTH;
        }
        // wait for timeout
        else if (this.connection.timeoutCounter > WIFI_MANAGER_RUNNABLE_DELAY_COUNT_VALUE(WIFI_FAILED_CONNECTION_WAIT_TIME_TILL_RETRY_MS))
        {
            // timeout start a retry
            this.connection.timeoutCounter = 0;
            wifiConnectionProcedureStatus = WIFI_CONNECTION_PROCEDURE_STATUS_CONNECTING;
            if (cyw43_arch_wifi_connect_async(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK))
            {
                // failed to start connection
                wifiConnectionProcedureStatus = WIFI_CONNECTION_PROCEDURE_STATUS_FAILED;
            }
        }
        else
        {
            this.connection.timeoutCounter++;
        }
        break;
    case WIFI_CONNECTION_PROCEDURE_STATUS_FAILED_BAD_AUTH:
        // do nothing

        this.connection.timeoutCounter++;
        if (this.connection.timeoutCounter > WIFI_MANAGER_RUNNABLE_DELAY_COUNT_VALUE(WIFI_FAILED_BAD_AUTH_WAIT_TIME_TILL_RETRY_MS))
        {
            // timeout start a retry
            this.connection.timeoutCounter = 0;
            wifiConnectionProcedureStatus = WIFI_CONNECTION_PROCEDURE_STATUS_CONNECTING;
        }
        break;
    default:
        // do nothing
        break;
    };

    stateHistory[stateHistoryIndex] = wifiConnectionProcedureStatus;
    stateHistoryIndex++;
    if (stateHistoryIndex >= STATE_HISTORY_SIZE)
    {
        stateHistoryIndex = 0;
    }
}

/**
 * @brief Initialization of wifi manager
 *
 * @pre Must be called in task context
 */
static void wifi_init_run(void)
{
    int32_t result = 0;

    //result = cyw43_arch_init();
    cyw43_arch_enable_sta_mode();

    if (result != 0)
    {
        this.state = WIFI_MANAGER_STOPPED;
    }
    else
    {
        this.state = WIFI_MANAGER_RUN;
    }
}
/**
 * @brief Deinitialization of wifi manager
 *
 * @pre Must be called in task context
 */

static void wifi_manager_deinit_run(void)
{

    cyw43_arch_disable_sta_mode();
    cyw43_arch_deinit();

    this.state = WIFI_MANAGER_STOPPED;
}

/**
 * @brief Stop the wifi manager
 *
 *  wifi manager will deinitialize the wifi but keep the task running. To start the wifi manager again call wifi_manager_start()
 */
wifi_manager_error_t wifi_manager_stop(void)
{
    if (this.state != WIFI_MANAGER_UNINITIALIZED)
    {
        if (this.state != WIFI_MANAGER_STOPPED)
        {
            this.state = WIFI_MANAGER_DEINIT;
        }
        return WIFI_MANAGER_ERROR_NONE;
    }
    else
    {
        return WIFI_MANAGER_ERROR_UNINITIALIZED;
    }
}

/**
 * @brief Start the wifi manager
 *
 * wifi manager will initialize the wifi, to stop the wifi manager call wifi_manager_stop()
 */
wifi_manager_error_t wifi_manager_start(void)
{
    if (this.state != WIFI_MANAGER_UNINITIALIZED)
    {
        if (this.state == WIFI_MANAGER_STOPPED)
        {
            this.state = WIFI_MANAGER_INIT;
        }
        return WIFI_MANAGER_ERROR_NONE;
    }
    else
    {
        return WIFI_MANAGER_ERROR_UNINITIALIZED;
    }
}

/**
 * @brief  Task for the wifi manager
 *
 * @param params
 */
void wifi_manager_task(__unused void *params)
{
    while (true)
    {

        if (this.state == WIFI_MANAGER_INIT)
        {
            wifi_init_run();
        }

        if (this.state == WIFI_MANAGER_RUN)
        {
            wifi_superviseConnection_run();
        }

        if (this.state == WIFI_MANAGER_DEINIT)
        {
            wifi_manager_deinit_run();
        }

        vTaskDelay(WIFI_MANAGER_TASK_INTERVAL_MS);
    }
    vTaskDelete(NULL);
}
