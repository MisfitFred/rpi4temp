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

#include "udp_beacon.h"

#define UDP_PORT 4444
#define BEACON_MSG_LEN_MAX 127u
#define BEACON_TARGET "255.255.255.255"
#define UDP_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)

const uint32_t WIFI_FAILED_CONNECTION_WAIT_TIME_TILL_RETRY_MS = 10000;
const uint32_t WIFI_FAILED_BAD_AUTH_WAIT_TIME_TILL_RETRY_MS = 120000;
const uint32_t WIFI_CONNECTING_TIMEOUT_MS = 30000;

const uint32_t BEACON_TASK_INTERVAL_MS = 100;

// Calculate the number of times the delay_ms value will fit into the BEACON_TASK_INTERVAL_MS
const uint32_t BEACON_RUNNABLE_DELAY_COUNT_VALUE(const uint32_t delay_ms) { return delay_ms / BEACON_TASK_INTERVAL_MS; }

/**
 * @brief Structure representing the state of the UDP beacon.
 */
typedef struct
{
    int8_t linkStatus;

    uint16_t connectionRetries;
    const uint16_t BADAUTH_MAX_CONNECTION_RETRIES;
    uint8_t badAuthRetries;
    uint32_t timeoutCounter;
} cyw43_connection_t;

typedef struct
{
    TaskHandle_t *task;            /**< Pointer to the udp task */
    struct udp_pcb *pcb;           /**< Pointer to the udp PCB */
    ip_addr_t addr;                /**< target address */
    int counter;                   /**< counter for the amount of beacon message */
    bool initialised;              /**< Flag indicating if the UDP beacon is initialised */
    bool running;                  /**< Flag to to keep the UDP beacon running */
    cyw43_connection_t connection; /**< Connection state */

    bool connected; /**< Flag indicating if the UDP beacon is connected */
} udp_beacon_state_t;

static udp_beacon_state_t this = {
    .task = NULL,
    .pcb = NULL,
    .addr = {0},
    .counter = 0,
    .initialised = false,
    .running = false,
    .connection = {
        .linkStatus = CYW43_LINK_DOWN,
        .connectionRetries = 0,
        .BADAUTH_MAX_CONNECTION_RETRIES = 3,
        .badAuthRetries = 0,
        .timeoutCounter = 0,
    },

    .connected = false,
};

void udp_beacon_task(__unused void *params);

/**
 * @brief Initialization of udp beacon
 *
 */
void udp_beacon_init(void)
{
    this.running = true;
    xTaskCreate(udp_beacon_task, "udp beacon task", configMINIMAL_STACK_SIZE, NULL, UDP_TASK_PRIORITY, this.task);
}

static void wifi_init_run(void)
{

    this.initialised = true;

    if (cyw43_arch_init())
    {
        printf("failed to initialise\n");
        this.initialised = false;
    }

    cyw43_arch_enable_sta_mode();
}

void wifi_superviseConnection_run(void)
{
    typedef enum
    {
        WIFI_CONNECTION_PROCEDURE_STATUS_IDLE,
        WIFI_CONNECTION_PROCEDURE_STATUS_CONNECTING,
        WIFI_CONNECTION_PROCEDURE_STATUS_CONNECTED,
        WIFI_CONNECTION_PROCEDURE_STATUS_DISCONNECTED,
        WIFI_CONNECTION_PROCEDURE_STATUS_FAILED,
        WIFI_CONNECTION_PROCEDURE_STATUS_FAILED_BAD_AUTH
    } wifi_connection_procedure_status_t;

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
            if (this.connection.timeoutCounter > BEACON_RUNNABLE_DELAY_COUNT_VALUE(WIFI_CONNECTING_TIMEOUT_MS))
            {
                // timeout
                this.connection.timeoutCounter = 0;
                asdfasdf
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
        else if (this.connection.timeoutCounter > BEACON_RUNNABLE_DELAY_COUNT_VALUE(WIFI_FAILED_CONNECTION_WAIT_TIME_TILL_RETRY_MS))
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
        if (this.connection.timeoutCounter > BEACON_RUNNABLE_DELAY_COUNT_VALUE(WIFI_FAILED_BAD_AUTH_WAIT_TIME_TILL_RETRY_MS))
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
 * @brief Initialization of udp beacon after FreeRTOS started
 *
 * @pre Must be called in task context.
 * @note User must not call this function directly, because it is done by udp_beacon_task itself.
 */
void udp_beacon_init_run(void)
{

    this.pcb = udp_new();
    ipaddr_aton(BEACON_TARGET, &this.addr);
    this.counter = 0;
}

/**
 * @brief Deinitialization of udp beacon
 *
 * @pre Must be called in task context
 */

static void udp_beacon_deinit_run(void)
{
    udp_remove(this.pcb);
    cyw43_arch_disable_sta_mode();
    cyw43_arch_deinit();
}

/**
 * @brief Deinitialization of udp beacon after FreeRTOS stopped
 *
 */
void udp_beacon_deinit(void)
{
    this.running = false;
}

/**
 * @brief Run udp beacon
 *
 * @pre Must be called in task context
 */
static void udp_beacon_run(void)
{

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, BEACON_MSG_LEN_MAX + 1, PBUF_RAM);
    char *req = (char *)p->payload;
    memset(req, 0, BEACON_MSG_LEN_MAX + 1);
    snprintf(req, BEACON_MSG_LEN_MAX, "%d", this.counter);
    err_t er = udp_sendto(this.pcb, p, &(this.addr), UDP_PORT);
    pbuf_free(p);
    if (er != ERR_OK)
    {
        printf("Failed to send UDP packet! error=%d", er);
    }
    else
    {
        printf("Sent packet %d\n", this.counter);
        this.counter++;
    }
}

void udp_beacon_task(__unused void *params)
{
    wifi_init_run();
    udp_beacon_init_run();

    while (this.running && this.initialised)
    {
        wifi_superviseConnection_run();

        if (this.connection.linkStatus == CYW43_LINK_UP)
        {
            udp_beacon_run();
        }
        vTaskDelay(BEACON_TASK_INTERVAL_MS);
    }

    udp_beacon_deinit_run();
    vTaskDelete(NULL);
}
