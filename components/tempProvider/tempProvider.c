/**
 * Copyright (c) 2023 Misfit Fred
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

#include <string.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "tempProvider.h"

const uint16_t TEMP_PROVIDER_RECEIVING_PORT = 4445;
const uint16_t TEMP_PROVIDER_TRANSMITTING_PORT = 4446;
const uint16_t TEMP_PROVIDER_MSG_LEN_MAX = 127;
const char* TEMP_PROVIDER_TARGET ="255.255.255.255"; 
const UBaseType_t TEMP_PROVIDER_TASK_PRIORITY = tskIDLE_PRIORITY + 1UL; 
const uint32_t TEMP_PROVIDER_TASK_INTERVAL_MS = 100; /*< interval of the  task and therefore the delay between the messages */
const uint32_t TEMP_PROVIDER_SEND_COUNT = 10; /*< amount of messages till the stop sending messages */

typedef enum
{
    TEMP_PROVIDER_UNINITIALIZED,
    TEMP_PROVIDER_STOPPED,
    TEMP_PROVIDER_INIT,
    TEMP_PROVIDER_RUN,
    TEMP_PROVIDER_DEINIT
} tempProvider_state_t;



const uint32_t TEMP_PROVIDER_RUNNABLE_DELAY_COUNT_VALUE(const uint32_t delay_ms) { return delay_ms / TEMP_PROVIDER_TASK_INTERVAL_MS; }

typedef struct
{
    TaskHandle_t *task;       /**< Pointer to the handling task */
    struct udp_pcb *pcb;      /**< Pointer to the udp PCB */
    ip_addr_t addr;           /**< target address */
    int counter;             
    tempProvider_state_t state; /**< Component state */
} tempProvider_attributes_t;

static tempProvider_attributes_t this = {
    .task = NULL,
    .pcb = NULL,
    .addr = {0},
    .counter = 0,
    .state = TEMP_PROVIDER_UNINITIALIZED};

void tempProvider_task(__unused void *params);

/**
 * @brief Initialization of temp provider 
 *
 *  create the temp provider task
 *
 */
tempProvider_error_t tempProvider_init(void)
{
    if (this.state == TEMP_PROVIDER_UNINITIALIZED)
    {
        if (pdPASS == xTaskCreate(tempProvider_task, "temp provider  task", configMINIMAL_STACK_SIZE, NULL, TEMP_PROVIDER_TASK_PRIORITY, this.task))
        {
            this.state = TEMP_PROVIDER_STOPPED;
        }
        else
        {
            return TEMP_PROVIDER_ERROR_MEM;
        }
    }
    return TEMP_PROVIDER_ERROR_NONE;
}

/**
 * @brief Initialization of temp provider  after FreeRTOS started
 *
 * @pre Must be called in task context.
 * @note User must not call this function directly, because it is done by tempProvider_task itself.
 */
void tempProvider_init_run(void)
{

    cyw43_arch_lwip_begin();
    this.pcb = udp_new();
    cyw43_arch_lwip_end();
    ipaddr_aton(TEMP_PROVIDER_TARGET, &this.addr);
    this.counter = 0;

    this.state = TEMP_PROVIDER_RUN;
}

/**
 * @brief Deinitialization of temp provider
 *
 * @pre Must be called in task context
 */

static void tempProvider_deinit_run(void)
{
    cyw43_arch_lwip_begin();
    udp_remove(this.pcb);
    cyw43_arch_lwip_end();
}

/**
 * @brief Stop the wifi manager
 *
 *  wifi manager will deinitialize the wifi but keep the task running. To start the wifi manager again call tempProvider_start()
 */
tempProvider_error_t tempProvider_stop(void)
{
    if (this.state != TEMP_PROVIDER_UNINITIALIZED)
    {
        if (this.state != TEMP_PROVIDER_STOPPED)
        {
            this.state = TEMP_PROVIDER_DEINIT;
        }
        return TEMP_PROVIDER_ERROR_NONE;
    }
    else
    {
        return TEMP_PROVIDER_ERROR_UNINITIALIZED;
    }
}

/**
 * @brief Start the wifi manager
 *
 * wifi manager will initialize the wifi, to stop the wifi manager call tempProvider_stop()
 */
tempProvider_error_t tempProvider_start(void)
{
    if (this.state != TEMP_PROVIDER_UNINITIALIZED)
    {
        if (this.state == TEMP_PROVIDER_STOPPED)
        {
            this.state = TEMP_PROVIDER_INIT;
        }
        return TEMP_PROVIDER_ERROR_NONE;
    }
    else
    {
        return TEMP_PROVIDER_ERROR_UNINITIALIZED;
    }
}

/**
 * @brief Run temp provider
 *
 * @pre Must be called in task context
 */
static void tempProvider_run(void)
{

    if (CYW43_LINK_UP == cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA))
    {

        cyw43_arch_lwip_begin();
        struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, TEMP_PROVIDER_MSG_LEN_MAX + 1, PBUF_RAM);
        char *req = (char *)p->payload;
        
        memset(req, 0, TEMP_PROVIDER_MSG_LEN_MAX + 1);
        snprintf(req, TEMP_PROVIDER_MSG_LEN_MAX, "%d", this.counter);
        err_t er = udp_sendto(this.pcb, p, &(this.addr), TEMP_PROVIDER_TRANSMITTING_PORT);
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
        if (this.counter >= TEMP_PROVIDER_SEND_COUNT)
        {
            this.state = TEMP_PROVIDER_STOPPED;
        }
    }
}

void tempProvider_udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
    if (p != NULL) {
        // Here you can process the received frame stored in 'p'
        // For example, you can print the data
        printf("Received data: %s\n", (char*)p->payload);

        // Don't forget to free the pbuf
        pbuf_free(p);
    }
}

void tempProvider_setup_udp_receiver(void) {
   
    err_t err;

    // Create a new UDP control block
    this.pcb = udp_new();

    if (this.pcb) {
        // Bind the upcb to the UDP_PORT port
        // IP_ADDR_ANY allow the upcb to be used by any local interface
        err = udp_bind(this.pcb, IP_ADDR_ANY, TEMP_PROVIDER_RECEIVING_PORT);

        if (err == ERR_OK) {
            // Set a receive callback for the upcb
            udp_recv(this.pcb, tempProvider_udp_receive_callback, NULL);
        } else {
            printf("Unable to bind to port\n");
        }
    } else {
        printf("Unable to create UDP control block\n");
    }
}



void tempProvider_task(__unused void *params)
{

    while (true)
    {
        if (this.state == TEMP_PROVIDER_INIT)
        {
            tempProvider_init_run();
        }

        if (this.state == TEMP_PROVIDER_RUN)
        {
            tempProvider_run();
        }

        if (this.state == TEMP_PROVIDER_DEINIT)
        {
            tempProvider_deinit_run();
        }
        vTaskDelay(TEMP_PROVIDER_TASK_INTERVAL_MS);
    }

    vTaskDelete(NULL);
}
