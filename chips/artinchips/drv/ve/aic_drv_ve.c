/*
 * Copyright (c) 2026, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: Ning Fang <ning.fang@artinchip.com>
 */

#include <nuttx/config.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <nuttx/fs/fs.h>

#include <hal/aic_hal_ve.h>
#include <drv/aic_drv_ve.h>

static int ve_open(FAR struct file *filep)
{
    struct aic_ve_client *client = hal_ve_open();
    if (!client)
        return -ENODEV;

    filep->f_priv = client;
    return OK;
}

static int ve_close(FAR struct file *filep)
{
    struct aic_ve_client *client = (struct aic_ve_client *)filep->f_priv;
    if (!client)
        return -EINVAL;

    hal_ve_close(client);
    return OK;
}

static int ve_ioctl(FAR struct file *filep, int cmd, unsigned long arg)
{
    struct aic_ve_client *client = (struct aic_ve_client *)filep->f_priv;
    if (!client)
        return -EINVAL;

    return hal_ve_control(client, cmd, (void *)(uintptr_t)arg);
}

static const struct file_operations ve_fops = {
    .open  = ve_open,
    .close = ve_close,
    .ioctl = ve_ioctl,
};

int aic_ve_probe(void)
{
    int ret;

    ret = hal_ve_probe();
    if (ret < 0)
        return ret;

    ret = register_driver("/dev/ve", &ve_fops, 0666, NULL);
    if (ret < 0)
        return ret;

    return OK;
}
