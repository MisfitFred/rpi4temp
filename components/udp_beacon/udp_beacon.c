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

const uint16_t UDP_BEACON_PORT = 4444;
const uint16_t UDP_BEACON_MSG_LEN_MAX = 127;
const char* UDP_BEACON_TARGET ="255.255.255.255";
const UBaseType_t UDP_TASK_PRIORITY = tskIDLE_PRIORITY + 1UL;

typedef enum
{
    UDP_BEACON_UNINITIALIZED,
    UDP_BEACON_STOPPED,
    UDP_BEACON_INIT,
    UDP_BEACON_RUN,
    UDP_BEACON_DEINIT
} udp_beacon_state_t;

const uint32_t BEACON_TASK_INTERVAL_MS = 100;

const uint32_t BEACON_RUNNABLE_DELAY_COUNT_VALUE(const uint32_t delay_ms) { return delay_ms / BEACON_TASK_INTERVAL_MS; }

typedef struct
{
    TaskHandle_t *task;       /**< Pointer to the udp task */
    struct udp_pcb *pcb;      /**< Pointer to the udp PCB */
    ip_addr_t addr;           /**< target address */
    int counter;              /**< counter for the amount of beacon message */
    udp_beacon_state_t state; /**< State of the udp beacon */
} udp_beacon_attributes_t;

static udp_beacon_attributes_t this = {
    .task = NULL,
    .pcb = NULL,
    .addr = {0},
    .counter = 0,
    .state = UDP_BEACON_UNINITIALIZED};

void udp_beacon_task(__unused void *params);

/**
 * @brief Initialization of udp beacon
 *
 *  create the udp beacon task
 *
 */
udp_beacon_error_t udp_beacon_init(void)
{
    if (this.state == UDP_BEACON_UNINITIALIZED)
    {
        if (pdPASS == xTaskCreate(udp_beacon_task, "udp beacon task", configMINIMAL_STACK_SIZE, NULL, UDP_TASK_PRIORITY, this.task))
        {
            this.state = UDP_BEACON_STOPPED;
        }
        else
        {
            return UDP_BEACON_ERROR_MEM;
        }
    }
    return UDP_BEACON_ERROR_NONE;
}

/**
 * @brief Initialization of udp beacon after FreeRTOS started
 *
 * @pre Must be called in task context.
 * @note User must not call this function directly, because it is done by udp_beacon_task itself.
 */
void udp_beacon_init_run(void)
{

    cyw43_arch_lwip_begin();
    this.pcb = udp_new();
    cyw43_arch_lwip_end();
    ipaddr_aton(UDP_BEACON_TARGET, &this.addr);
    this.counter = 0;

    this.state = UDP_BEACON_RUN;
}

/**
 * @brief Deinitialization of udp beacon
 *
 * @pre Must be called in task context
 */

static void udp_beacon_deinit_run(void)
{
    cyw43_arch_lwip_begin();
    udp_remove(this.pcb);
    cyw43_arch_lwip_end();
}

/**
 * @brief Stop the wifi manager
 *
 *  wifi manager will deinitialize the wifi but keep the task running. To start the wifi manager again call udp_beacon_start()
 */
udp_beacon_error_t udp_beacon_stop(void)
{
    if (this.state != UDP_BEACON_UNINITIALIZED)
    {
        if (this.state != UDP_BEACON_STOPPED)
        {
            this.state = UDP_BEACON_DEINIT;
        }
        return UDP_BEACON_ERROR_NONE;
    }
    else
    {
        return UDP_BEACON_ERROR_UNINITIALIZED;
    }
}

/**
 * @brief Start the wifi manager
 *
 * wifi manager will initialize the wifi, to stop the wifi manager call udp_beacon_stop()
 */
udp_beacon_error_t udp_beacon_start(void)
{
    if (this.state != UDP_BEACON_UNINITIALIZED)
    {
        if (this.state == UDP_BEACON_STOPPED)
        {
            this.state = UDP_BEACON_INIT;
        }
        return UDP_BEACON_ERROR_NONE;
    }
    else
    {
        return UDP_BEACON_ERROR_UNINITIALIZED;
    }
}

/**
 * @brief Run udp beacon
 *
 * @pre Must be called in task context
 */
static void udp_beacon_run(void)
{

    if (CYW43_LINK_UP == cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA))
    {

        cyw43_arch_lwip_begin();
        struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, UDP_BEACON_MSG_LEN_MAX + 1, PBUF_RAM);
        char *req = (char *)p->payload;
        memset(req, 0, UDP_BEACON_MSG_LEN_MAX + 1);
        snprintf(req, UDP_BEACON_MSG_LEN_MAX, "%d", this.counter);
        err_t er = udp_sendto(this.pcb, p, &(this.addr), UDP_BEACON_PORT);
        pbuf_free(p);
        cyw43_arch_lwip_end();
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
}

void udp_beacon_task(__unused void *params)
{

    while (true)
    {
        if (this.state == UDP_BEACON_INIT)
        {
            udp_beacon_init_run();
        }

        if (this.state == UDP_BEACON_RUN)
        {
            udp_beacon_run();
        }

        if (this.state == UDP_BEACON_DEINIT)
        {
            udp_beacon_deinit_run();
        }
        vTaskDelay(BEACON_TASK_INTERVAL_MS);
    }

    vTaskDelete(NULL);
}
