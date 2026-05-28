/*
 * Copyright (c) 2022-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: matteo <duanmt@artinchip.com>
 */

#include <nuttx/config.h>
#include <nuttx/timers/pwm.h>

#include "aic_core.h"
#include "aic_hal_clk.h"
#include "hal_pwm.h"

static struct pwm_lowerhalf_s g_aic_pwm;

static int pwm_setup(struct pwm_lowerhalf_s *dev) {
  struct aic_pwm_action action0 = {/*       CBD,          CBU,          CAD, */
                                   PWM_ACT_NONE, PWM_ACT_NONE, PWM_ACT_NONE,
                                   /*      CAU,           PRD,         ZRO  */
                                   PWM_ACT_LOW, PWM_ACT_NONE, PWM_ACT_HIGH};
  struct aic_pwm_action action1 = {/*       CBD,          CBU,          CAD, */
                                   PWM_ACT_NONE, PWM_ACT_LOW, PWM_ACT_NONE,
                                   /*      CAU,           PRD,         ZRO  */
                                   PWM_ACT_NONE, PWM_ACT_NONE, PWM_ACT_HIGH};

  hal_pwm_init();
  hal_pwm_ch_init(0, PWM_MODE_UP_COUNT, 0, &action0, &action1);
  return OK;
}

static int pwm_shutdown(struct pwm_lowerhalf_s *dev) {
  hal_pwm_deinit();
  return OK;
}

static int pwm_start(struct pwm_lowerhalf_s *dev,
                     const struct pwm_info_s *info) {
  int ret = OK;
  uint32_t duty_ns, period_ns;

  period_ns = 1000000000 / info->frequency;
  duty_ns = (info->duty * period_ns) / 0xffff;

  ret = hal_pwm_set(0, duty_ns, period_ns, PWM_SET_CMPA_CMPB);
  if (ret < 0) {
    return ret;
  }

  ret = hal_pwm_enable(0);
  if (ret < 0) {
    return ret;
  }

  return OK;
}

static int pwm_stop(struct pwm_lowerhalf_s *dev) { return hal_pwm_disable(0); }

static int pwm_ioctl(struct pwm_lowerhalf_s *dev, int cmd, unsigned long arg) {
  return -ENOTTY;
}

static const struct pwm_ops_s g_pwmops = {
    .setup = pwm_setup,
    .shutdown = pwm_shutdown,
    .start = pwm_start,
    .stop = pwm_stop,
    .ioctl = pwm_ioctl,
};

int drv_pwm_nuttx_init(void) {
  g_aic_pwm.ops = &g_pwmops;
  return pwm_register("/dev/pwm0", &g_aic_pwm);
}
