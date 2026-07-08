/*
 * Copyright (c) 2022-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <nuttx/config.h>
#include <nuttx/video/fb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>

#include "aic_osal.h"
#include "aic_log.h"
#include "mpp_fb.h"
#include <uapi/mpp_types.h>

struct mpp_fb {
    int fd;
    unsigned int ref_count;
};

static struct mpp_fb *g_mpp_fb = NULL;

struct mpp_fb *mpp_fb_open(void)
{
    struct mpp_fb *fb;

    if (g_mpp_fb) {
        g_mpp_fb->ref_count++;
        return g_mpp_fb;
    }

    fb = aicos_malloc(MEM_DEFAULT, sizeof(struct mpp_fb));
    if (!fb) {
        hal_log_err("alloc mpp fb failed\n");
        return NULL;
    }

    fb->fd = open("/dev/fb0", O_RDWR);
    if (fb->fd < 0) {
        hal_log_err("open /dev/fb0 failed: %d\n", errno);
        aicos_free(MEM_DEFAULT, fb);
        return NULL;
    }

    fb->ref_count = 1;
    g_mpp_fb = fb;

    return fb;
}

void mpp_fb_close(struct mpp_fb *fb)
{
    if (!fb)
        return;

    fb->ref_count--;

    if (fb->ref_count == 0) {
        if (fb->fd >= 0)
            close(fb->fd);
        g_mpp_fb = NULL;
        aicos_free(MEM_DEFAULT, fb);
    }
}

int mpp_fb_ioctl(struct mpp_fb *fb, int cmd, void *args)
{
    if (!fb || fb->fd < 0)
        return -EINVAL;

    if (cmd == AICFB_GET_SCREENINFO) {
        struct fb_videoinfo_s vinfo;
        struct fb_planeinfo_s pinfo;
        struct aicfb_screeninfo *info = (struct aicfb_screeninfo *)args;
        int ret;

        ret = ioctl(fb->fd, FBIOGET_VIDEOINFO, (unsigned long)&vinfo);
        if (ret < 0)
            return ret;

        ret = ioctl(fb->fd, FBIOGET_PLANEINFO, (unsigned long)&pinfo);
        if (ret < 0)
            return ret;

        info->format = MPP_FMT_RGB_888;
        info->bits_per_pixel = pinfo.bpp;
        info->stride = pinfo.stride;
        info->framebuffer = (unsigned char *)pinfo.fbmem;
        info->width = vinfo.xres;
        info->height = vinfo.yres;
        info->smem_len = (int)pinfo.fblen;
        return 0;
    }

    return ioctl(fb->fd, cmd, (unsigned long)(uintptr_t)args);
}
