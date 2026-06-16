/****************************************************************************
 * vendor/artinchip/chips/d12x/include/irq.h
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

#ifndef __VENDOR_ARTINCHIP_CHIPS_D12X_INCLUDE_IRQ_H
#define __VENDOR_ARTINCHIP_CHIPS_D12X_INCLUDE_IRQ_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Interrupt Number Definition (NuttX IRQ numbers) */

#define D12X_IRQ_NUM_BASE 0
#define D12X_IRQ(n) (RISCV_IRQ_ASYNC + D12X_IRQ_NUM_BASE + (n))

#define D12X_IRQ_DCE D12X_IRQ(31)
#define D12X_IRQ_DMA D12X_IRQ(32)
#define D12X_IRQ_SPI_ENC D12X_IRQ(41)
#define D12X_IRQ_QSPI0 D12X_IRQ(44)
#define D12X_IRQ_QSPI1 D12X_IRQ(45)
#define D12X_IRQ_SDMC0 D12X_IRQ(46)
#define D12X_IRQ_SDMC1 D12X_IRQ(47)
#define D12X_IRQ_XSPI D12X_IRQ(49)
#define D12X_IRQ_MTOP D12X_IRQ(51)
#define D12X_IRQ_AUDIO D12X_IRQ(54)
#define D12X_IRQ_LCD D12X_IRQ(55)
#define D12X_IRQ_DE D12X_IRQ(59)
#define D12X_IRQ_GE D12X_IRQ(60)
#define D12X_IRQ_VE D12X_IRQ(61)
#define D12X_IRQ_WDT D12X_IRQ(64)
#define D12X_IRQ_GPIO D12X_IRQ(68) /* 68~75 */
#define D12X_IRQ_UART0 D12X_IRQ(76)
#define D12X_IRQ_UART1 D12X_IRQ(77)
#define D12X_IRQ_UART2 D12X_IRQ(78)
#define D12X_IRQ_UART3 D12X_IRQ(79)
#define D12X_IRQ_I2C0 D12X_IRQ(84)
#define D12X_IRQ_I2C1 D12X_IRQ(85)
#define D12X_IRQ_CAN0 D12X_IRQ(88)
#define D12X_IRQ_CAN1 D12X_IRQ(89)
#define D12X_IRQ_PWM D12X_IRQ(90)
#define D12X_IRQ_GPAI D12X_IRQ(92)
#define D12X_IRQ_RTP D12X_IRQ(93)
#define D12X_IRQ_TSEN D12X_IRQ(94)
#define D12X_IRQ_CIR D12X_IRQ(95)

#define NR_IRQS 128

#endif /* __VENDOR_ARTINCHIP_CHIPS_D12X_INCLUDE_IRQ_H */
