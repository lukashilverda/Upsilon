# N0120 STM32H725 Notes

This branch currently carries a compileable N0120 scaffold, but it is not a full STM32H725 hardware port yet.

## Current status
- The device target exists and builds against the STM32H725 register model.
- Register **base addresses** in `ion/src/device/n0120/regs/` match the STM32H725 CMSIS header (`stm32h725xx.h`, RM0468 §2.3.2).
- **RCC**, **PWR**, and **FLASH** register layouts and drivers have been ported to H725 semantics.
- The board and config files are still using placeholder limits that keep the tree buildable.
- Register layouts for FMC, OCTOSPI, SDMMC, ADC, DMA, EXTI, SYSCFG, OTG are not yet fully ported to H7 semantics.

## Register base addresses (verified against CMSIS)
| Peripheral | Address | CMSIS symbol |
|------------|---------|--------------|
| EXTI | `0x58000000` | `EXTI_BASE` |
| SYSCFG | `0x58000400` | `SYSCFG_BASE` |
| RTC | `0x58004000` | `RTC_BASE` |
| GPIOA–H | `0x58020000` + n×`0x400` | `GPIOx_BASE` |
| RCC | `0x58024400` | `RCC_BASE` |
| PWR | `0x58024800` | `PWR_BASE` |
| CRC | `0x58024C00` | `CRC_BASE` |
| FLASH (regs) | `0x52002000` | `FLASH_R_BASE` |
| FMC | `0x52004000` | `FMC_R_BASE` |
| OCTOSPI1 (regs) | `0x52005000` | `OCTOSPI1_R_BASE` |
| SDMMC1 | `0x52007000` | `SDMMC1_BASE` |
| DMA1 / DMA2 | `0x40020000` / `0x40020400` | `DMAx_BASE` |
| ADC1 | `0x40022000` | `ADC1_BASE` |
| USB OTG HS | `0x40040000` | `USB1_OTG_HS_PERIPH_BASE` |
| RNG | `0x48021800` | `RNG_BASE` |
| USART / SPI / TIM | unchanged (D2 APB) | same offsets as F7 |

Cortex-M7 core peripherals (NVIC, SCB, MPU, ITM) are architecture-defined and unchanged.

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
