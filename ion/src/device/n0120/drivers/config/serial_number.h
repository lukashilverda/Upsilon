#ifndef ION_DEVICE_N0120_CONFIG_SERIAL_NUMBER_H
#define ION_DEVICE_N0120_CONFIG_SERIAL_NUMBER_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace SerialNumber {
namespace Config {

/* N0120 STM32H725 unique device ID address.
 * Based on STM32H7 series specifications:
 * - 96-bit unique device identifier in system memory
 * - Address: 0x1FF1E800
 */

constexpr uint32_t UniqueDeviceIDAddress = 0x1FF1E800;

}
}
}
}

#endif
