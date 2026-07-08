/*
 * Copyright (c) 2026, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * VE VFS adapter - routes MPP VE calls through VFS /dev/ve
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <drv/aic_drv_ve.h>

int drv_ve_open(void)
{
    return open("/dev/ve", O_RDWR);
}

int drv_ve_close(int fd)
{
    return close(fd);
}

int drv_ve_control(int fd, int cmd, void *arg)
{
    return ioctl(fd, cmd, (unsigned long)arg);
}
