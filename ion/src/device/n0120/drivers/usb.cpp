// Should work for the N0120. Check the usb wiring in ion/src/device/n0120/drivers/config/usb.h for the actual pinout.


#include <drivers/usb.h>
#include <drivers/config/usb.h>

namespace Ion {
namespace Device {

using namespace Regs;

namespace USB {

void initVbus() {
  Config::VbusPin.group().MODER()->setMode(Config::VbusPin.pin(), GPIO::MODER::Mode::Input);
  Config::VbusPin.group().PUPDR()->setPull(Config::VbusPin.pin(), GPIO::PUPDR::Pull::None);
}

}
}
}
