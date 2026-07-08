/*
 * Copyright (c) 2026, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * GE VFS adapter - routes MPP GE calls through VFS /dev/ge
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <drv/aic_drv_ge.h>

int aic_ge_open(void)
{
    return open("/dev/ge", O_RDWR);
}

int aic_ge_close(int fd)
{
    return close(fd);
}

int aic_ge_write(int fd, const char *buff, size_t count)
{
    return write(fd, buff, count);
}

int aic_ge_ioctl(int fd, int cmd, void *arg)
{
    return ioctl(fd, cmd, (unsigned long)arg);
}
