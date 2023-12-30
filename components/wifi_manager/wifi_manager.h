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

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        WIFI_MANAGER_ERROR_NONE = 0,
        WIFI_MANAGER_ERROR_UNINITIALIZED = -1,
        WIFI_MANAGER_ERROR_MEM = -2,

    } wifi_manager_error_t;
    wifi_manager_error_t wifi_manager_init(void);
    wifi_manager_error_t wifi_manager_start(void);
    wifi_manager_error_t wifi_manager_stop(void);
#ifdef __cplusplus
}
#endif
