#ifndef ION_DEVICE_N0120_CONFIG_USB_H
#define ION_DEVICE_N0120_CONFIG_USB_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace USB {
namespace Config {

/* N0120 USB wiring placeholder.
 * The current repo still uses the older device register model, so the USB
 * setup here is only a compileable interim configuration. */

using namespace Regs;

/* Verify the VBUS pin against the STM32H725 board schematic before shipping.
 * This is kept as a placeholder for the current bring-up.
 */
constexpr static AFGPIOPin VbusPin = AFGPIOPin(GPIOA, 9, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast);

constexpr static AFGPIOPin DmPin = AFGPIOPin(GPIOA, 11, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast);
constexpr static AFGPIOPin DpPin = AFGPIOPin(GPIOA, 12, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast);

constexpr static const char * InterfaceStringDescriptor = "@Flash/0x08000000/04*016Kg/0x90000000/08*004Kg,01*032Kg,63*064Kg,64*064Kg";

}
}
}
}

#endif
