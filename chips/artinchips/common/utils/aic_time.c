/****************************************************************************
 * vendor/artinchip/chips/artinchips/common/utils/aic_time.c
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

#include <aic_core.h>
#include "sys_freq.h"

void aic_udelay(u32 us) {
  u64 start = aic_get_ticks();
  u32 cnt = us * (drv_get_sys_freq() / 1000000U);

  while (1) {
    u64 cur = aic_get_ticks();

    if (start > cur) {
      if ((start - cur) >= cnt)
        break;
    } else {
      if (cur - start >= cnt)
        break;
    }
  }
}

void aic_mdelay(u32 ms) { aic_udelay(ms * 1000); }

u64 aic_get_ticks(void) {
  u32 hi, lo;

  do {
    hi = csi_coret_get_valueh();
    lo = csi_coret_get_value();
  } while (hi != csi_coret_get_valueh());

  return ((u64)hi << 32) | lo;
}

u64 aic_get_time_us(void) {
  u64 cnt = (drv_get_sys_freq() / 1000000U);

  return (aic_get_ticks() / cnt);
}

u64 aic_get_time_ms(void) { return aic_get_time_us() / 1000; }

u64 aic_get_time_us64(void) {
  u64 us_cnt = drv_get_sys_freq() / 1000000U;

  return (u64)(aic_get_ticks() / us_cnt);
  ;
}

u64 aic_get_time_ms64(void) { return aic_get_time_us64() / 1000; }