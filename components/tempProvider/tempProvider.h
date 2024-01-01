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
extern "C" {
#endif

typedef enum {
    TEMP_PROVIDER_ERROR_NONE = 0,
    TEMP_PROVIDER_ERROR_UNINITIALIZED = -1,
    TEMP_PROVIDER_ERROR_MEM = -2,
}tempProvider_error_t;

tempProvider_error_t tempProvider_init(void);
tempProvider_error_t tempProvider_start(void);
tempProvider_error_t tempProvider_stop(void);

#ifdef __cplusplus
}
#endif
