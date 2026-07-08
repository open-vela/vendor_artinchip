/*
 * Copyright (c) 2022-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ARTINCHIP_AIC_HAL_H__
#define __ARTINCHIP_AIC_HAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "aic_hal_reset.h"
#include "aic_hal_clk.h"
#include "aic_hal_gpio.h"
#include "aic_hal_uart.h"
#ifdef CONFIG_AIC_GE_DRV
#include "aic_hal_ge.h"
#endif
#ifdef CONFIG_AIC_VE_DRV
#include "aic_hal_ve.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ARTINCHIP_AIC_HAL_H__ */
