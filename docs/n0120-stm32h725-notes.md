# N0120 STM32H725 Notes

This branch currently carries a compileable N0120 scaffold, but it is not a full STM32H725 hardware port yet.

## Current status
- The device target exists and builds against the current legacy register model.
- The board and config files are still using placeholder limits that keep the tree buildable.
- The repo does not yet contain a real STM32H725 register model, so H7-specific clock, power, flash, and bus-domain behavior is not final.

## Files that still need a real H725 pass
- ion/src/device/n0120/drivers/board.cpp
- ion/src/device/n0120/drivers/config/clocks.h
- ion/src/device/n0120/drivers/config/display.h
- ion/src/device/n0120/drivers/config/internal_flash.h
- ion/src/device/n0120/drivers/config/serial_number.h
- ion/src/device/n0120/drivers/config/usb.h
- ion/src/device/n0120/flash.ld
- ion/src/device/n0120/internal_flash.ld

## Guidance for contributors
- Keep changes compileable against the current register layer.
- Do not introduce H7-only register calls unless the shared regs layer has been updated first.
- Verify real STM32H725 limits against the board schematic and reference manual before converting placeholder constants into final values.
