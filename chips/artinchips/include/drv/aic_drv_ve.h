/*
 * Copyright (c) 2022, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: <qi.xu@artinchip.com>
 */

int drv_ve_open(void);
int drv_ve_close(int fd);
int drv_ve_control(int fd, int cmd, void *arg);
int aic_ve_probe(void);
