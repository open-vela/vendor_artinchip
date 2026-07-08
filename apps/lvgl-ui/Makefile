include $(APPDIR)/Make.defs

PROGNAME = lv_demo
PRIORITY = $(CONFIG_AIC_LVGL_DEMO_PRIORITY)
STACKSIZE = $(CONFIG_AIC_LVGL_DEMO_STACKSIZE)
MODULE = $(CONFIG_AIC_LVGL_DEMO_APP)

MAINSRC = lv_demo.c

# App root and all subdirectory include paths
APP_DIR := $(APPDIR)/vendor/artinchip/apps/lvgl-ui
CFLAGS += ${INCDIR_PREFIX}$(APP_DIR)
CFLAGS += $(addprefix ${INCDIR_PREFIX}$(APP_DIR)/,$(shell find aic_demo/dashboard_demo -type d 2>/dev/null))

ifeq ($(CONFIG_AIC_LVGL_DASHBOARD_DEMO),y)
  CSRCS += $(shell find aic_demo/dashboard_demo -name "*.c" 2>/dev/null)

  # Copy assets to pack resources (rodata partition)
  PACK_RODATA := $(APPDIR)/vendor/artinchip/pack/resources/rodata
  ASSETS_SRC  := $(APP_DIR)/aic_demo/dashboard_demo/assets
  $(shell mkdir -p $(PACK_RODATA)/lvgl_data && cp -ru $(ASSETS_SRC)/* $(PACK_RODATA)/lvgl_data/ 2>/dev/null)
endif

ifeq ($(CONFIG_AIC_LVGL_UI_BUILDER),y)
  CFLAGS += $(addprefix ${INCDIR_PREFIX}$(APP_DIR)/,$(shell find aic_demo/ui_builder -type d 2>/dev/null))
  CSRCS += $(shell find aic_demo/ui_builder -name "*.c" 2>/dev/null)

  # Copy assets to pack resources (rodata partition)
  PACK_RODATA := $(APPDIR)/vendor/artinchip/pack/resources/rodata
  ASSETS_SRC  := $(APP_DIR)/aic_demo/ui_builder/assets
  $(shell mkdir -p $(PACK_RODATA)/lvgl_data && cp -ru $(ASSETS_SRC)/* $(PACK_RODATA)/lvgl_data/ 2>/dev/null)
endif

include $(APPDIR)/Application.mk
