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
    UDP_BEACON_ERROR_NONE = 0,
    UDP_BEACON_ERROR_UNINITIALIZED = -1,
    UDP_BEACON_ERROR_MEM = -2,
}udp_beacon_error_t;

udp_beacon_error_t udp_beacon_init(void);
udp_beacon_error_t udp_beacon_start(void);
udp_beacon_error_t upd_beacon_stop(void);

#ifdef __cplusplus
}
#endif
