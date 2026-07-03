DEVICE_COMMON_DIR = ion/src/device/n0120

include ion/src/device/shared/boot/Makefile
ifneq ($(filter bootloader, ${MAKECMDGOALS}), bootloader)
include ion/src/device/shared/usb/Makefile
else
include ion/src/device/bootloader/usb/Makefile
endif
include ion/src/device/shared/drivers/Makefile

ion_device_src += $(addprefix $(DEVICE_COMMON_DIR)/, \
  events.cpp \
  stack.cpp \
)

ion_device_src += $(addprefix $(DEVICE_COMMON_DIR)/drivers/, \
  cache.cpp \
  external_flash.cpp \
)

ifeq ($(filter bootloader, ${MAKECMDGOALS}), bootloader)
ion_device_src += $(addprefix bootloader/drivers/, \
  board.cpp \
)
ion_device_src += $(addprefix bootloader/boot/, \
  rt0.cpp \
)
else
ion_device_src += $(addprefix $(DEVICE_COMMON_DIR)/boot/, \
  rt0.cpp \
)
endif

ion_device_src += $(addprefix $(DEVICE_COMMON_DIR)/, \
  platform_info.cpp \
)

LDSCRIPT ?= ion/src/device/n0120/flash.ld
