/****************************************************************************
 * vendor/artinchip/chips/artinchips/drv/i2c/aic_drv_i2c.c
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

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/arch.h>
#include <nuttx/mutex.h>
#include <nuttx/i2c/i2c_master.h>
#ifdef CONFIG_AIC_I2C_INTERRUPT_MODE
#include <nuttx/semaphore.h>
#endif

#include <aic_core.h>
#include <hal_i2c.h>
#include <aic_drv_i2c.h>

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct aic_i2c_priv_s {
  struct i2c_master_s dev; /* I2C master device implementation */
  aic_i2c_ctrl aic_bus;    /* AIC_USING HAL I2C control */
  mutex_t lock;            /* Bus access lock */
  int refs;                /* Reference count */
#ifdef CONFIG_AIC_I2C_INTERRUPT_MODE
  sem_t waitsem; /* Reserved for future IRQ mode */
#endif
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int aic_i2c_setup(FAR struct i2c_master_s *dev);
static int aic_i2c_shutdown(FAR struct i2c_master_s *dev);
static int aic_i2c_transfer(FAR struct i2c_master_s *dev,
                            FAR struct i2c_msg_s *msgs, int count);
static int aic_i2c_transfer_poll(FAR struct aic_i2c_priv_s *priv,
                                 FAR struct i2c_msg_s *msgs, int count);
#ifdef CONFIG_AIC_I2C_INTERRUPT_MODE
static int aic_i2c_transfer_irq(FAR struct aic_i2c_priv_s *priv,
                                FAR struct i2c_msg_s *msgs, int count);
#endif

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* I2C operations */

static const struct i2c_ops_s g_aic_i2c_ops = {
    .transfer = aic_i2c_transfer,
    .setup = aic_i2c_setup,
    .shutdown = aic_i2c_shutdown,
};

#ifdef CONFIG_AIC_USING_I2C0
static struct aic_i2c_priv_s g_aic_i2c0_priv = {
    .dev = {&g_aic_i2c_ops},
    .aic_bus =
        {
            .index = 0,
            .reg_base = I2C0_BASE,
            .device_name = "i2c0",
            .addr_bit = I2C_7BIT_ADDR,
            .target_rate = CONFIG_AIC_DEV_I2C0_SPEED,
            .bus_mode = I2C_MASTER_MODE,
            .irq_index = I2C0_IRQn + RISCV_IRQ_ASYNC,
            .clk_id = CLK_I2C0,
        },
    .lock = NXMUTEX_INITIALIZER,
    .refs = 0,
#ifdef CONFIG_AIC_I2C_INTERRUPT_MODE
    .waitsem = NXSEM_INITIALIZER(0, 0),
#endif
};
#endif

#ifdef CONFIG_AIC_USING_I2C1
static struct aic_i2c_priv_s g_aic_i2c1_priv = {
    .dev = {&g_aic_i2c_ops},
    .aic_bus =
        {
            .index = 1,
            .reg_base = I2C1_BASE,
            .device_name = "i2c1",
            .addr_bit = I2C_7BIT_ADDR,
            .target_rate = CONFIG_AIC_DEV_I2C1_SPEED,
            .bus_mode = I2C_MASTER_MODE,
            .irq_index = I2C1_IRQn + RISCV_IRQ_ASYNC,
            .clk_id = CLK_I2C1,
        },
    .lock = NXMUTEX_INITIALIZER,
    .refs = 0,
#ifdef CONFIG_AIC_I2C_INTERRUPT_MODE
    .waitsem = NXSEM_INITIALIZER(0, 0),
#endif
};
#endif

/* Add more instances as needed */

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int aic_i2c_setup(FAR struct i2c_master_s *dev) {
  FAR struct aic_i2c_priv_s *priv = (FAR struct aic_i2c_priv_s *)dev;
  int ret;

  nxmutex_lock(&priv->lock);

  if (priv->refs == 0) {
    ret = hal_i2c_init(&priv->aic_bus);
    if (ret < 0) {
      nxmutex_unlock(&priv->lock);
      return ret;
    }
  }

  priv->refs++;
  nxmutex_unlock(&priv->lock);

  return OK;
}

static int aic_i2c_shutdown(FAR struct i2c_master_s *dev) {
  FAR struct aic_i2c_priv_s *priv = (FAR struct aic_i2c_priv_s *)dev;

  nxmutex_lock(&priv->lock);

  if (priv->refs > 0) {
    priv->refs--;
    if (priv->refs == 0) {
      hal_i2c_module_disable(&priv->aic_bus);
    }
  }

  nxmutex_unlock(&priv->lock);

  return OK;
}

static int aic_i2c_transfer(FAR struct i2c_master_s *dev,
                            FAR struct i2c_msg_s *msgs, int count) {
  FAR struct aic_i2c_priv_s *priv = (FAR struct aic_i2c_priv_s *)dev;
  int ret;

  if (msgs == NULL || count <= 0) {
    return -EINVAL;
  }

  nxmutex_lock(&priv->lock);

#ifdef CONFIG_AIC_I2C_INTERRUPT_MODE
  ret = aic_i2c_transfer_irq(priv, msgs, count);
#else
  ret = aic_i2c_transfer_poll(priv, msgs, count);
#endif

  nxmutex_unlock(&priv->lock);

  return ret;
}

static int aic_i2c_transfer_poll(FAR struct aic_i2c_priv_s *priv,
                                 FAR struct i2c_msg_s *msgs, int count) {
  int ret = OK;
  int i;
  int32_t bytes;
  bool is_last;
  struct aic_i2c_msg aic_msg;

  for (i = 0; i < count; i++) {
    is_last = (i == (count - 1));

    priv->aic_bus.addr_bit =
        (msgs[i].flags & I2C_M_TEN) ? I2C_10BIT_ADDR : I2C_7BIT_ADDR;

    aic_msg.addr = msgs[i].addr;
    aic_msg.flags = 0;
    aic_msg.len = msgs[i].length;
    aic_msg.buf = msgs[i].buffer;

    if ((msgs[i].flags & I2C_M_READ) != 0) {
      bytes =
          hal_i2c_master_receive_msg(&priv->aic_bus, &aic_msg, is_last ? 1 : 0);
    } else {
      bytes =
          hal_i2c_master_send_msg(&priv->aic_bus, &aic_msg, is_last ? 1 : 0);
    }

    if (bytes < 0) {
      i2cerr("I2C HAL transfer error at msg %d: %ld\n", i, (long)bytes);
      ret = (int)bytes;
      break;
    }

    if (bytes != msgs[i].length) {
      i2cerr("I2C transfer incomplete at msg %d: %ld/%d\n", i, (long)bytes,
             msgs[i].length);
      ret = -EIO;
      break;
    }
  }

  return ret;
}

#ifdef CONFIG_AIC_I2C_INTERRUPT_MODE
static int aic_i2c_transfer_irq(FAR struct aic_i2c_priv_s *priv,
                                FAR struct i2c_msg_s *msgs, int count) {
  /* Keep build-time compatibility while IRQ flow is being implemented in HAL.
   * Default configuration keeps this path disabled.
   */

  UNUSED(priv);
  UNUSED(msgs);
  UNUSED(count);
  i2cerr(
      "I2C IRQ mode is not ready in NuttX path, fallback to polling config\n");
  return -ENOTSUP;
}
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/
FAR struct i2c_master_s *aic_i2cbus_initialize(int port) {
  FAR struct aic_i2c_priv_s *priv = NULL;
  FAR struct i2c_master_s *dev = NULL;
  int ret;
  switch (port) {
#ifdef CONFIG_AIC_USING_I2C0
  case 0:
    priv = &g_aic_i2c0_priv;
    break;
#endif
#ifdef CONFIG_AIC_USING_I2C1
  case 1:
    priv = &g_aic_i2c1_priv;
    break;
#endif
  default:
    return NULL;
  }

  dev = &priv->dev;
  if (dev == NULL) {
    i2cerr("Failed to get I2C device.\n");
    return NULL;
  }
  ret = i2c_register(dev, port);
  if (ret < 0) {
    return NULL;
  }

  aic_i2c_setup(dev);
  return dev;
}

int aic_i2cbus_uninitialize(FAR struct i2c_master_s *dev) {
  /* Typically nothing to do here besides shutdown if needed */
  // aic_i2c_shutdown(dev);
  return OK;
}
