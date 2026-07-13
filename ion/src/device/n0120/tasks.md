# N0120 Port Completion Tasks

Goal: get a working N0120 build that boots on real hardware through the DFU-only flow.

## 1. Replace the placeholder H725 board bring-up
- [x] Rewrite the reset/clock/power/flash/MPU initialization in [drivers/board.cpp](drivers/board.cpp) so it matches the STM32H725 instead of the legacy F4/F7-style bring-up.
- [x] Add or finish the H7-specific register support needed by that init path in [regs/regs.h](regs/regs.h) and the related register headers under [regs/](regs/) and [regs/config/](regs/config/).
- [x] Recheck the clock tree constants and peripheral prescalers in [drivers/config/clocks.h](drivers/config/clocks.h) against the real N0120/H725 clock plan.
- [x] Verify the MPU regions for internal flash, external flash, and FMC/OSPI access in [drivers/board.cpp](drivers/board.cpp).

## 2. Fix the memory map for the actual board
- [ ] Replace the placeholder internal-flash layout in [flash.ld](flash.ld) and [internal_flash.ld](internal_flash.ld) with the real STM32H725/N0120 addresses and sector sizes.
- [ ] Update the flash/OTP assumptions in [drivers/config/internal_flash.h](drivers/config/internal_flash.h) to match the real chip layout.
- [ ] Confirm the unique ID and OTP base addresses in [drivers/config/serial_number.h](drivers/config/serial_number.h).
- [ ] Make sure the DFU and exam-mode sections still land in valid locations after the final memory map is set.

## 3. Finish external flash bring-up
- [ ] Replace the temporary comments and unfinished assumptions in [drivers/external_flash.cpp](drivers/external_flash.cpp).
- [ ] Verify the external flash pinout, size, and memory-mapped mode settings in [drivers/config/external_flash.h](drivers/config/external_flash.h).
- [ ] Check that cache maintenance and MPU setup still match the actual external flash behavior in [drivers/cache.cpp](drivers/cache.cpp) and [drivers/board.cpp](drivers/board.cpp).

## 4. Validate display and backlight wiring
- [ ] Confirm the LCD/FSMC timing and GPIO mapping in [drivers/config/display.h](drivers/config/display.h) and [drivers/display.cpp](drivers/display.cpp).
- [ ] Verify backlight PWM and enable pins in [drivers/config/backlight.h](drivers/config/backlight.h) and [drivers/backlight.cpp](drivers/backlight.cpp).
- [ ] Make sure the display and backlight startup order in [drivers/board.cpp](drivers/board.cpp) is safe for boot.

## 5. Confirm input and power peripherals
- [x] Validate the keyboard matrix wiring and scan order in [drivers/config/keyboard.h](drivers/config/keyboard.h) and [drivers/keyboard.cpp](drivers/keyboard.cpp).
- [x] Confirm battery sensing and charging-state detection in [drivers/config/battery.h](drivers/config/battery.h) and [drivers/battery.cpp](drivers/battery.cpp).
- [x] Verify the RGB LED pins and PWM channels in [drivers/config/led.h](drivers/config/led.h) and [drivers/led.cpp](drivers/led.cpp).
- [ ] Check RTC, SWD, console, reset, and timing setup in [drivers/rtc.cpp](drivers/rtc.cpp), [drivers/swd.cpp](drivers/swd.cpp), [drivers/console_uart.cpp](drivers/console_uart.cpp), [drivers/reset.cpp](drivers/reset.cpp), and [drivers/timing.cpp](drivers/timing.cpp).

## 6. Finish USB and DFU boot flow
- [x] Confirm USB VBUS, D+/D-, and alternate-function setup in [drivers/config/usb.h](drivers/config/usb.h) and [drivers/usb.cpp](drivers/usb.cpp).
- [x] Validate the DFU descriptors and RAM relocation path in [usb/](usb/), especially [usb/dfu.ld](usb/dfu.ld), [usb/dfu_relocated.cpp](usb/dfu_relocated.cpp), and [usb/calculator.cpp](usb/calculator.cpp).
- [ ] Keep the target DFU-only and do not add a bootloader programming flow.
- [x] Make sure the startup handoff from [boot/rt0.cpp](boot/rt0.cpp) and [drivers/board.cpp](drivers/board.cpp) cleanly jumps to external flash after init.

## 7. Confirm PCB version handling
- [ ] Keep the factory PCB version logic aligned with the N0120 hardware in [drivers/board.cpp](drivers/board.cpp).
- [x] Verify that the N0120 build uses `PCB_LATEST = 344` in [../../build/platform.device.n0120.mak](../../build/platform.device.n0120.mak).
- [ ] Check that PCB version writes and locks still work with the OTP layout defined in [drivers/config/internal_flash.h](drivers/config/internal_flash.h).

## 8. Final build and boot validation
- [ ] Build the N0120 target through [Makefile](Makefile), [../Makefile](../Makefile), and the N0120 device makefile in [Makefile](Makefile).
- [ ] Flash the resulting image to a real N0120 and verify it reaches the application instead of aborting in early boot.
- [ ] Smoke-test screen, keyboard, USB, external flash, battery, and LED behavior on hardware.
- [ ] Remove or replace any remaining placeholder comments once the hardware path is confirmed.
