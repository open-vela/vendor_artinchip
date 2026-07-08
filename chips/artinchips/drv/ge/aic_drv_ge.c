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

#include "aic_core.h"
#include "aic_common.h"
#include "aic_osal.h"
#include "aic_log.h"
#include <hal/aic_hal_ge.h>
#include <drv/aic_drv_ge.h>

static int ge_open(FAR struct file *filep)
{
    struct aic_ge_client *client = hal_ge_open();
    if (!client)
        return -ENOMEM;

    filep->f_priv = client;
    return OK;
}

static int ge_close(FAR struct file *filep)
{
    struct aic_ge_client *client = (struct aic_ge_client *)filep->f_priv;
    if (!client)
        return -EINVAL;

    hal_ge_close(client);
    return OK;
}

static ssize_t ge_write(FAR struct file *filep, FAR const char *buffer,
                        size_t buflen)
{
    struct aic_ge_client *client = (struct aic_ge_client *)filep->f_priv;
    if (!client)
        return -EINVAL;

    return hal_ge_write(client, buffer, buflen);
}

static int ge_ioctl(FAR struct file *filep, int cmd, unsigned long arg)
{
    struct aic_ge_client *client = (struct aic_ge_client *)filep->f_priv;
    if (!client)
        return -EINVAL;

    return hal_ge_control(client, cmd, (void *)(uintptr_t)arg);
}

static const struct file_operations ge_fops = {
    .open  = ge_open,
    .close = ge_close,
    .write = ge_write,
    .ioctl = ge_ioctl,
};

int aic_ge_probe(void)
{
    int ret;

    ret = hal_ge_init();
    if (ret < 0)
        return ret;

    ret = register_driver("/dev/ge", &ge_fops, 0666, NULL);
    if (ret < 0) {
        hal_log_err("Failed to register /dev/ge: %d\n", ret);
        return ret;
    }

    hal_log_info("GE driver registered as /dev/ge\n");
    return OK;
}
