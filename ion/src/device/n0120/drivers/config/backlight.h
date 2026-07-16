// Just works for N0120

#ifndef ION_DEVICE_N0120_CONFIG_BACKLIGHT_H
#define ION_DEVICE_N0120_CONFIG_BACKLIGHT_H

#include <regs/regs.h>

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PD3 | Backlight Enable  | Output                |
 */

namespace Ion {
namespace Device {
namespace Backlight {
namespace Config {

using namespace Regs;

constexpr static GPIOPin BacklightPin = GPIOPin(GPIOD, 3);

}
}
}
}

#endif
