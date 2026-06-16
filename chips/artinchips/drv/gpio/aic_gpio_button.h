
/****************************************************************************
 * vendor/artinchip/chips/artinchips/drv/gpio/aic_gpio_button.h
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

#ifndef __AIC_GPIO_BUTTON_H
#define __AIC_GPIO_BUTTON_H

#include <stdint.h>
#include <stdbool.h>

int aic_gpio_button_register(uint32_t pin, uint32_t id, bool pressed_state,
                             const char *devname);

int aic_gpio_button_unregister(uint32_t pin);

#endif