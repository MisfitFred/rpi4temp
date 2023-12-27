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

void udp_beacon_init(void);
void upd_beacon_deinit(void);

#ifdef __cplusplus
}
#endif
