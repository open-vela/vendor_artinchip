include $(APPDIR)/Make.defs

PROGNAME = pic_dec_test
PRIORITY = 100
STACKSIZE = 65536
MODULE = $(CONFIG_AIC_PIC_DEC_TEST)

MAINSRC = $(TOPDIR)/../vendor/artinchip/chips/artinchips/mpp/mpp_test/pic_dec_test.c

CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/vendor/artinchip/chips/artinchips/common/include
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/vendor/artinchip/chips/artinchips/mpp/include
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/vendor/artinchip/chips/artinchips/mpp/base/include
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/vendor/artinchip/chips/artinchips/include
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/vendor/artinchip/chips/artinchips/include/drv
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/vendor/artinchip/chips/artinchips/include/hal
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/vendor/artinchip/chips/artinchips/include/uapi

include $(APPDIR)/Application.mk
