#ifndef ION_DEVICE_N0120_CONFIG_INTERNAL_FLASH_H
#define ION_DEVICE_N0120_CONFIG_INTERNAL_FLASH_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace InternalFlash {
namespace Config {

/* N0120 STM32H725 flash and OTP map.
 * Based on STM32H7 series specifications:
 * - 512KB Flash organized in 8KB sectors
 * - OTP area at 0x08FFF000
 * - 1024 bytes OTP (32 blocks of 32 bytes each)
 */

constexpr static uint32_t StartAddress = 0x08000000;
constexpr static uint32_t EndAddress = 0x08080000;
constexpr static int NumberOfSectors = 64;
constexpr static uint32_t SectorAddresses[NumberOfSectors+1] = {
  0x08000000, 0x08002000, 0x08004000, 0x08006000, 0x08008000, 0x0800A000, 0x0800C000, 0x0800E000,
  0x08010000, 0x08012000, 0x08014000, 0x08016000, 0x08018000, 0x0801A000, 0x0801C000, 0x0801E000,
  0x08020000, 0x08022000, 0x08024000, 0x08026000, 0x08028000, 0x0802A000, 0x0802C000, 0x0802E000,
  0x08030000, 0x08032000, 0x08034000, 0x08036000, 0x08038000, 0x0803A000, 0x0803C000, 0x0803E000,
  0x08040000, 0x08042000, 0x08044000, 0x08046000, 0x08048000, 0x0804A000, 0x0804C000, 0x0804E000,
  0x08050000, 0x08052000, 0x08054000, 0x08056000, 0x08058000, 0x0805A000, 0x0805C000, 0x0805E000,
  0x08060000, 0x08062000, 0x08064000, 0x08066000, 0x08068000, 0x0806A000, 0x0806C000, 0x0806E000,
  0x08070000, 0x08072000, 0x08074000, 0x08076000, 0x08078000, 0x0807A000, 0x0807C000, 0x0807E000,
  0x08080000
};

constexpr static uint32_t OTPStartAddress = 0x08FFF000;
constexpr static uint32_t OTPLocksAddress = 0x08FFF400;
constexpr static int NumberOfOTPBlocks = 32;
constexpr static uint32_t OTPBlockSize = 0x20;
constexpr uint32_t OTPAddress(int block) { return OTPStartAddress + block * OTPBlockSize; };
constexpr uint32_t OTPLockAddress(int block) { return OTPLocksAddress + block; }

}
}
}
}

#endif
