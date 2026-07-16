// N0120 chatgpt fix:

#ifndef ION_DEVICE_N0120_CONFIG_EXTERNAL_FLASH_H
#define ION_DEVICE_N0120_CONFIG_EXTERNAL_FLASH_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {
namespace Config {

using namespace Regs;

/*
 * External flash:
 * AT25SF641
 *
 * Capacity:
 * 64 Mbit = 8 MByte
 *
 * Address range:
 * 0x90000000 - 0x90800000
 *
 * Interface:
 * Quad SPI / QSPI
 *
 * STM32H725:
 * Uses OCTOSPI1 peripheral configured in QuadSPI mode.
 */


/* Memory mapped QSPI region */
constexpr static uint32_t StartAddress = 0x90000000;
constexpr static uint32_t EndAddress   = 0x90800000;


/*
 * AT25SF641 erase layout:
 *
 * 4KB sectors
 * 32KB blocks
 * 64KB blocks
 *
 * Total:
 * 8MB
 */

constexpr static int NumberOf4KSectors  = 8;
constexpr static int NumberOf32KSectors = 1;
constexpr static int NumberOf64KSectors = 127;

constexpr static int NumberOfSectors =
    NumberOf4KSectors +
    NumberOf32KSectors +
    NumberOf64KSectors;


/*
 * STM32H725 OCTOSPI1 pins
 *
 * Pin      Function
 * --------------------------
 * PB2      CLK       AF9
 * PB10     NCS       AF9
 * PB12     IO0       AF10
 * PB13     IO2/WP    AF10
 * PD12     IO1       AF9
 * PD13     IO3/HOLD  AF9
 */

constexpr static AFGPIOPin Pins[] = {
  AFGPIOPin(
    GPIOB,
    2,
    GPIO::AFR::AlternateFunction::AF9,
    GPIO::PUPDR::Pull::None,
    GPIO::OSPEEDR::OutputSpeed::VeryHigh
  ),

  AFGPIOPin(
    GPIOB,
    10,
    GPIO::AFR::AlternateFunction::AF9,
    GPIO::PUPDR::Pull::None,
    GPIO::OSPEEDR::OutputSpeed::VeryHigh
  ),

  AFGPIOPin(
    GPIOB,
    12,
    GPIO::AFR::AlternateFunction::AF10,
    GPIO::PUPDR::Pull::None,
    GPIO::OSPEEDR::OutputSpeed::VeryHigh
  ),

  AFGPIOPin(
    GPIOB,
    13,
    GPIO::AFR::AlternateFunction::AF10,
    GPIO::PUPDR::Pull::None,
    GPIO::OSPEEDR::OutputSpeed::VeryHigh
  ),

  AFGPIOPin(
    GPIOD,
    12,
    GPIO::AFR::AlternateFunction::AF9,
    GPIO::PUPDR::Pull::None,
    GPIO::OSPEEDR::OutputSpeed::VeryHigh
  ),

  AFGPIOPin(
    GPIOD,
    13,
    GPIO::AFR::AlternateFunction::AF9,
    GPIO::PUPDR::Pull::None,
    GPIO::OSPEEDR::OutputSpeed::VeryHigh
  ),
};


/*
 * AT25SF641 configuration
 */

constexpr static uint32_t FlashSizeBytes = 8 * 1024 * 1024;

constexpr static uint32_t PageSize = 256;

constexpr static uint32_t SectorSize = 4096;


/*
 * Flash commands
 */

constexpr static uint8_t ReadID = 0x9F;
constexpr static uint8_t WriteEnable = 0x06;
constexpr static uint8_t ReadStatusRegister1 = 0x05;
constexpr static uint8_t ReadStatusRegister2 = 0x35;
constexpr static uint8_t WriteStatusRegister = 0x01;

constexpr static uint8_t QuadRead = 0x6B;
constexpr static uint8_t QuadIORead = 0xEB;

constexpr static uint8_t PageProgram = 0x02;
constexpr static uint8_t QuadPageProgram = 0x32;

constexpr static uint8_t SectorErase = 0x20;
constexpr static uint8_t BlockErase32K = 0x52;
constexpr static uint8_t BlockErase64K = 0xD8;

constexpr static uint8_t EnableQPI = 0x38;
constexpr static uint8_t DisableQPI = 0xFF;


}
}
}
}

#endif




//old:
/* #ifndef ION_DEVICE_N0120_CONFIG_EXTERNAL_FLASH_H
#define ION_DEVICE_N0120_CONFIG_EXTERNAL_FLASH_H

#include <regs/regs.h>

/*  Pin | Role                 | Mode                  | Function
 * -----+----------------------+-----------------------+-----------------
 *  PB2 | QUADSPI CLK          | Alternate Function  9 | QUADSPI_CLK
 * PB10 | QUADSPI BK1_NCS      | Alternate Function  9 | QUADSPI_BK1_NCS
 * PB13 | QUADSPI BK1_IO2/WP   | Alternate Function 10 | QUADSPI_BK1_IO2
 * PB12 | QUADSPI BK1_IO0/SO   | Alternate Function 10 | QUADSPI_BK1_IO0
 * PD12 | QUADSPI BK1_IO1/SI   | Alternate Function  9 | QUADSPI_BK1_IO1
 * PD13 | QUADSPI BK1_IO3/HOLD | Alternate Function  9 | QUADSPI_BK1_IO3
 /

namespace Ion {
namespace Device {
namespace ExternalFlash {
namespace Config {

using namespace Regs;

constexpr static uint32_t StartAddress = 0x90000000;
constexpr static uint32_t EndAddress = 0x90800000;

constexpr static int NumberOf4KSectors = 8;
constexpr static int NumberOf32KSectors = 1;
constexpr static int NumberOf64KSectors = 128 - 1;
constexpr static int NumberOfSectors = NumberOf4KSectors + NumberOf32KSectors + NumberOf64KSectors;

constexpr static AFGPIOPin Pins[] = {
  AFGPIOPin(GPIOB, 2,  GPIO::AFR::AlternateFunction::AF9, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast),
  AFGPIOPin(GPIOB, 12,  GPIO::AFR::AlternateFunction::AF10,  GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast),
  AFGPIOPin(GPIOB, 13,  GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast),
  AFGPIOPin(GPIOD, 12, GPIO::AFR::AlternateFunction::AF9, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast),
  AFGPIOPin(GPIOD, 13, GPIO::AFR::AlternateFunction::AF9, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast),
  AFGPIOPin(GPIOB, 10,  GPIO::AFR::AlternateFunction::AF9, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast),
};

}
}
}
}

#endif

*/