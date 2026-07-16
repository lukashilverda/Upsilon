#ifndef ION_DEVICE_N0120_CONFIG_INTERNAL_FLASH_H
#define ION_DEVICE_N0120_CONFIG_INTERNAL_FLASH_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace InternalFlash {
namespace Config {


/*
 * STM32H725VET6 internal flash:
 *
 * Size:
 * 512 KB
 *
 * Organization:
 * 1 bank
 * 4 sectors
 *
 * Sector size:
 * 128 KB
 */


constexpr static uint32_t StartAddress = 0x08000000;
constexpr static uint32_t EndAddress   = 0x08080000;


constexpr static int NumberOfSectors = 4;


constexpr static uint32_t SectorAddresses[NumberOfSectors + 1] = {
  0x08000000,
  0x08020000,
  0x08040000,
  0x08060000,
  0x08080000
};


/*
 * STM32H7 OTP memory
 *
 * 32 blocks
 * 32 bytes per block
 */

constexpr static uint32_t OTPStartAddress = 0x08FFF000;
constexpr static uint32_t OTPLocksAddress = 0x08FFF400;

constexpr static int NumberOfOTPBlocks = 32;
constexpr static uint32_t OTPBlockSize = 0x20;


constexpr uint32_t OTPAddress(int block) {
  return OTPStartAddress + block * OTPBlockSize;
}


constexpr uint32_t OTPLockAddress(int block) {
  return OTPLocksAddress + block;
}


}
}
}
}

#endif