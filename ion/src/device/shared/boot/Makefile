ifeq ($(filter bootloader, ${MAKECMDGOALS}), bootloader)
ion_device_src += $(addprefix bootloader/boot/, \
  isr.c \
)
else
ion_device_src += $(addprefix $(DEVICE_COMMON_DIR)/boot/, \
  isr.c \
)
endif
