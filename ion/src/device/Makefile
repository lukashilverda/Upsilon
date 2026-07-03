# Makefile belows should augment $(ion_device_src)
ifeq (${MODEL},n0120)
include ion/src/device/n0120/Makefile
else
include ion/src/device/shared/Makefile
endif
include ion/src/device/bench/Makefile
include ion/src/device/flasher/Makefile
ifneq (${MODEL},n0120)
include ion/src/device/$(MODEL)/Makefile
endif

$(call object_for,ion/src/device/n0100/platform_info.cpp ion/src/device/n0110/platform_info.cpp ion/src/device/n0120/platform_info.cpp ion/src/device/bootloader/platform_info.cpp): SFLAGS += -DHEADER_SECTION="__attribute__((section(\".header\")))"

ifeq ($(EPSILON_TELEMETRY),1)
ion_src += ion/src/shared/telemetry_console.cpp
endif

ion_src += ion/src/shared/collect_registers.cpp

IN_FACTORY ?= 0

ifeq (${MODEL},n0120)
ION_DEVICE_SFLAGS = -Iion/src/device/n0120 -DPCB_LATEST=$(PCB_LATEST) -DIN_FACTORY=$(IN_FACTORY)
else
ION_DEVICE_SFLAGS = -Iion/src/device/$(MODEL) -Iion/src/device/shared -DPCB_LATEST=$(PCB_LATEST) -DIN_FACTORY=$(IN_FACTORY)
endif

$(call object_for,$(ion_device_src) $(ion_device_flasher_src) $(ion_device_bench_src)): SFLAGS += $(ION_DEVICE_SFLAGS)

ion_src += $(ion_device_src)
