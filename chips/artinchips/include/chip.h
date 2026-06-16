/****************************************************************************
 * vendor/artinchip/chips/d12x/include/chip.h
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

#ifndef __VENDOR_ARTINCHIP_CHIPS_D12X_INCLUDE_CHIP_H
#define __VENDOR_ARTINCHIP_CHIPS_D12X_INCLUDE_CHIP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* TODO:  Confirm interrupt number from SDK header file */
#ifndef NR_IRQS
#define NR_IRQS MAX_IRQn
#endif
#define CACHE_LINE_SIZE 32
#define GPIO_MAX_PINS 192
#define GPIO_GROUP_MAX 6
/* CPU core base address for D12X (E907) */
#define CPU_BASE 0x20000000UL

#endif /* __VENDOR_ARTINCHIP_CHIPS_D12X_INCLUDE_CHIP_H */
