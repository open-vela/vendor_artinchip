/*
 * Copyright (c) 2022, Artinchip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __AIC_DRV_GPIO_H__
#define __AIC_DRV_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "aic_hal_gpio.h"

/* GPIO interrupt callback function type */
typedef void (*gpio_irq_handler_t)(void *arg);

/* GPIO interrupt configuration structure */
struct gpio_irq_config_s {
  uint32_t pin;               /* GPIO pin number */
  uint32_t intr_type;         /* Interrupt trigger type */
  gpio_irq_handler_t handler; /* Interrupt handler function */
  void *arg;                  /* Argument passed to the handler function */
  bool enabled;               /* Whether enabled */
};

void drv_pin_bias_set(unsigned int pin, unsigned int pull);
void drv_pin_drive_set(unsigned int pin, unsigned int strength);
void drv_pin_mux_set(unsigned int pin, unsigned int func);
unsigned int drv_pin_mux_get(unsigned int pin);
long drv_pin_get(const char *name);
int drv_pin_init(void);

int gpio_irq_initialize(void);
int gpio_irq_detach(uint32_t irqn);
int gpio_irq_attach(uint32_t irqn, gpio_irq_handler_t handler, void *arg);
int gpio_irq_enable(uint32_t irqn);
int gpio_irq_disable(uint32_t irqn);
int gpio_irq_arch_configure(uint32_t gpio_pin, uint32_t intr_type);
int gpio_get_value(uint32_t pin);
int gpio_set_value(uint32_t pin, bool value);
#ifdef __cplusplus
}
#endif

#endif /* __AIC_DRV_GPIO_H__ */
