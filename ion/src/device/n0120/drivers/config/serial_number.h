#ifndef ION_DEVICE_N0120_CONFIG_SERIAL_NUMBER_H
#define ION_DEVICE_N0120_CONFIG_SERIAL_NUMBER_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace SerialNumber {
namespace Config {

/* N0120 placeholder unique-ID address.
 * Confirm against the STM32H725 reference manual before using this on real
 * hardware. */

constexpr uint32_t UniqueDeviceIDAddress = 0x1FF07A10;

}
}
}
}

#endif
