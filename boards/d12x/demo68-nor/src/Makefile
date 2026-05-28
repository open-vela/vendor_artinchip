#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

include $(TOPDIR)/Make.defs

ifneq ($(CONFIG_ARTINCHIP_BINARY_ONLY),y)

# Ensure chip headers are available to board code
AIC_CHIP_DIR = $(TOPDIR)/../vendor/artinchip/chips/artinchips
AIC_HAL_DIR  = $(AIC_CHIP_DIR)/hal
AIC_HAL_INCDIR = $(AIC_HAL_DIR)/include/hal

CFLAGS += -I$(AIC_CHIP_DIR)/include
CFLAGS += -I$(AIC_CHIP_DIR)/include/hal
CFLAGS += -I$(AIC_CHIP_DIR)/include/uapi
CFLAGS += -I$(AIC_CHIP_DIR)/common/include
CFLAGS += -I$(AIC_HAL_INCDIR)
CFLAGS += -I$(AIC_HAL_DIR)
CFLAGS += -I$(AIC_HAL_DIR)/gpio
CFLAGS += -I$(AIC_HAL_DIR)/timer

# Add local include directory
CFLAGS += -I$(TOPDIR)/../vendor/artinchip/boards/d12x/demo68-nor/include

CSRCS = d12x_boot.c pinmux.c sys_clk.c board_reset.c
endif

ifeq ($(CONFIG_ETC_ROMFS),y)
  RCSRCS += etc/init.d/rc.sysinit etc/init.d/rcS
 # RCRAWS += etc/key.avb
  RCRAWS += etc/build.prop
  RCRAWS += etc/factory.sh
 # RCRAWS += etc/rsa_public_key.pem
endif

ifeq ($(CONFIG_LIBC_LOCALTIME),y)
RCRAWS += etc/zoneinfo
endif

ifeq ($(CONFIG_MEDIA_SERVER),y)
RCRAWS += etc/media
endif

ifeq ($(CONFIG_SYSTEM_PACKAGE_SERVICE), y)
RCRAWS += etc/package.cfg
endif

ifeq ($(CONFIG_SYSTEM_ACTIVITY_SERVICE), y)
RCRAWS += etc/lmk.cfg
endif

ifeq ($(CONFIG_ARTINCHIP_BINARY_ONLY),y)
all: libboard$(LIBEXT)
context:
depend:
clean:
	$(call DELFILE, libboard$(LIBEXT))
distclean: clean
	$(call DELFILE, Make.dep)
	$(call DELFILE, .depend)

libboard$(LIBEXT):
	$(Q) cp $(TOPDIR)/../vendor/artinchip/libs/board/demo68-nor/libboard_artinchip$(LIBEXT) $@
else
include $(TOPDIR)/boards/Board.mk
endif
