/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: matteo <duanmt@artinchip.com>
 */

#ifndef _ARTINCHIP_MPP_VIN_DEV_H_
#define _ARTINCHIP_MPP_VIN_DEV_H_

/* The command of Display Bus Interface */

#define DBI_CMD_SOFT_RESET          0x01
#define DBI_CMD_SLEEP_OUT           0x11
#define DBI_CMD_DISP_OFF            0x28
#define DBI_CMD_DISP_ON             0x29
#define DBI_CMD_COL_ADDR_SET        0x2A /* width */
#define DBI_CMD_PAGE_ADDR_SET       0x2B /* height */
#define DBI_CMD_PIXEL_FMT           0x3A /* Bit3: ArtInChip customed */
#define DBI_CMD_BRIGHTNESS          0x51
#define DBI_CMD_AIC_GE_CTL          0xAC /* ArtInChip customed */
#define DBI_CMD_FR_CTL              0xB1
#define DBI_CMD_BLANK_PORCH_CTL     0xB5

#define DBI_DAT_MAX_LEN             16

#define DBI_CMD_DPI_MASK            GENMASK(6, 4)
#define DBI_CMD_DPI_SHIFT           4
#define DBI_CMD_DBI_FMT_FLAG        BIT(3)
#define DBI_CMD_DBI_MASK            GENMASK(2, 0)
#define DBI_CMD_DBI_SHIFT           0

#define DBI_CMD_GE_H_FLIP           BIT(6)
#define DBI_CMD_GE_V_FLIP           BIT(5)
#define DBI_CMD_GE_SCALE            BIT(4)
#define DBI_CMD_GE_ROT_MASK         GENMASK(1, 0)

#define DBI_CMD_FR_MASK             GENMASK(7, 4)
#define DBI_CMD_FR_SHIFT            4

#define DBI_CMD_VP_MASK             GENMASK(4, 0)

enum dpi_rgb_if {
    DPI_RGB_IF_16BIT = 5,
    DPI_RGB_IF_18BIT = 6,
    DPI_RGB_IF_24BIT = 7
};

enum dbi_mcu_if {
    DBI_MCU_IF_3BIT = 1,
    DBI_MCU_IF_16BIT = 5,
    DBI_MCU_IF_18BIT = 6,
    DBI_MCU_IF_24BIT = 7
};

enum dbi_rotation {
    DBI_CMD_ROTATION_0 = 0,
    DBI_CMD_ROTATION_90,
    DBI_CMD_ROTATION_180,
    DBI_CMD_ROTATION_270
};

#define dbi_byte2short(h, l)        ((h & 0xff) << 8 | (l & 0xff))

struct aic_dbi_cmd {
    u8 code;
    u8 data_len;
    char name[16];
    int (*proc)(u8 code, u8 *data);
};

enum mdi_bus_mode {
    MDI_BUS_MODE_8080 = 0,
    MDI_BUS_MODE_6800 = 1,
    MDI_BUS_MODE_SPI = 2
};

enum mdi_spi_disp_fmt {
    MDI_SPI_DISP_FMT_RGB888 = 0,
    MDI_SPI_DISP_FMT_RGB565 = 1,
    MDI_SPI_DISP_FMT_RGB888_2 = 2, // only for 2SDA
};

enum mdi_spi_rd_fmt {
    MDI_SPI_RD_FMT_8BIT = 0,
    MDI_SPI_RD_FMT_24BIT = 1,
    MDI_SPI_RD_FMT_32BIT = 2
};

enum mdi_spi_mode {
    MDI_SPI_MODE_3WIRE = 0,
    MDI_SPI_MODE_4WIRE = 1,
    MDI_SPI_MODE_2SDA = 2,
    MDI_SPI_MODE_4SDA = 3
};

enum mdi_x16_seq {
    MDI_X16_SEQ_RGB888 = 0,
    MDI_X16_SEQ_RGBX = 1,
    MDI_X16_SEQ_RGBX666 = 2,
    MDI_X16_SEQ_XRGB666 = 3,
    MDI_X16_SEQ_RGB565 = 4
};

enum mdi_x8_seq {
    MDI_X8_SEQ_RGB888 = 0,
    MDI_X8_SEQ_RGB565 = 1
};

enum mdi_dat_endian {
    MDI_DAT_ENDIAN_LOW_8 = 0,
    MDI_DAT_ENDIAN_HIGH_8 = 1
};

enum mdi_pin_ctl {
    MDI_PIN_CTL_X8 = 0,
    MDI_PIN_CTL_X16 = 1
};

/* Information of SPI bus */
struct mdi_bus_spi_cfg {
    enum mdi_spi_mode       mode;
    enum mdi_spi_disp_fmt   seq;
    enum mdi_spi_rd_fmt     rd_fmt;
};

/* Information of 8080/6800 bus */
struct mdi_bus_8080_cfg {
    enum mdi_pin_ctl        pin;
    union {
        enum mdi_x8_seq x8;
        enum mdi_x16_seq x16;
    } seq;
};

struct aic_mdi_cfg {
    /* Input format, should be decided by some GPIO */
    enum mdi_bus_mode       bus_mode;
    enum mdi_dat_endian     big_endian;
    struct mdi_bus_8080_cfg bus_8080;
    struct mdi_bus_spi_cfg  bus_spi;

    /* Input pixel format, set by Host write command */
    u32 width;
    u32 height;

    /* Output pixel format, depends on the panel */
    u32 stride;
    u32 sizeimage;
};

int mpp_vin_dev_init(u32 cnt, struct aic_mdi_cfg *cfg);
void mpp_vin_dev_deinit(void);
void mpp_mdi_show_buf_info(void);

#endif
