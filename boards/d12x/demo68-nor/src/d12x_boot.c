/****************************************************************************
 * vendor/artinchip/boards/d12x/demo_board/src/d12x_boot.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/board.h>
#include <nuttx/arch.h>
#include <arch/board/board.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <arch/chip/aic_soc.h>
#include <aic_core.h>
#include <aic_hal.h>
#include <aic_utils.h>
#include <aic_hal_gpio.h>
#include <syslog.h>
#include <nuttx/video/fb.h>
#include <nuttx/lcd/lcd.h>
#include <nuttx/timers/pwm.h>
#include <drv/drv_rtc.h>
#include <nuttx/timers/watchdog.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/
#ifdef CONFIG_BOARD_EARLY_INITIALIZE
extern void d12x_board_initialize(void);
void board_early_initialize(void) { d12x_board_initialize(); }
#endif

#ifdef CONFIG_BOARD_LATE_INITIALIZE
extern void d12x_board_late_initialize(void);
void board_late_initialize(void) { d12x_board_late_initialize(); }
#endif

int board_app_initialize(uintptr_t arg) { return 0; }
