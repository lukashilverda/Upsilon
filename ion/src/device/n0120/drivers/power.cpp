#include <ion/battery.h>
#include <ion/keyboard.h>
#include <ion/led.h>
#include <ion/rtc.h>
#include <ion/usb.h>
#include <drivers/board.h>
#include <drivers/battery.h>
#include <drivers/external_flash.h>
#include <drivers/keyboard.h>
#include <drivers/led.h>
#include <drivers/power.h>
#include <drivers/usb.h>
#include <drivers/reset.h>
#include <drivers/wakeup.h>
#include <drivers/timing.h>
#include <regs/regs.h>
#include <regs/config/pwr.h>
#include <regs/config/rcc.h>
#include "events_keyboard_platform.h"

namespace Ion {
namespace Power {

/* We isolate the standby code that needs to be executed from the internal
 * flash (because the external flash is then shut down). We forbid inlining to
 * avoid inlining these instructions in the external flash. */

void standby() {
  Device::Power::waitUntilOnOffKeyReleased();
  Device::Power::standbyConfiguration();
  Device::Board::shutdownPeripherals();
  Device::Power::internalFlashStandby();
}

void suspend(bool checkIfOnOffKeyReleased) {
  bool isLEDActive = Ion::LED::getColor() != KDColorBlack;
  bool plugged = USB::isPlugged();

  if (checkIfOnOffKeyReleased) {
    Device::Power::waitUntilOnOffKeyReleased();
  }

  /* First, shutdown all peripherals except LED. Indeed, the charging pin state
   * might change when we shutdown peripherals that draw current. */
  Device::Board::shutdownPeripherals(true);

  while (1) {
    // Update LED color according to plug and charge state
    Device::Battery::initGPIO();
    Device::USB::initGPIO();
    Device::LED::init();
    isLEDActive = LED::updateColorWithPlugAndCharge() != KDColorBlack;

    // Configure low-power mode
    if (isLEDActive || Ion::RTC::mode() == Ion::RTC::Mode::HSE) {
      Device::Power::sleepConfiguration();
    } else {
      Device::Power::stopConfiguration();
    }

    // Shutdown all peripherals (except LED if active)
    Device::Board::shutdownPeripherals(isLEDActive);

    /* Wake up on:
     * - Power key
     * - Plug/Unplug USB
     * - Stop charging */
    Device::Power::configWakeUp();

    Device::Power::internalFlashSuspend(isLEDActive);

    // Check power key
    Device::Keyboard::init();
    Keyboard::State scan = Keyboard::scan();
    Ion::Keyboard::State OnlyOnOffKeyDown = Keyboard::State(Keyboard::Key::OnOff);

    // Check plugging state
    Device::USB::initGPIO();
    if (scan == OnlyOnOffKeyDown || (!plugged && USB::isPlugged())) {
      // Wake up
      break;
    } else {
      /* The wake up event can be an unplug event or a battery charging event.
       * In both cases, we want to update static observed states like
       * sLastUSBPlugged or sLastBatteryCharging. */
      Events::getPlatformEvent();
    }
    plugged = USB::isPlugged();
  }

  // Reset normal frequency
  Device::Board::setStandardFrequency(Device::Board::Frequency::High);
  Device::Board::initPeripherals(true);
  // Update LED according to plug and charge state
  LED::updateColorWithPlugAndCharge();
  /* If the USB has been unplugged while sleeping, the USB should have been
   * soft disabled but as part of the USB peripheral was asleep, this could
   * not be done before. */
  if (USB::isPlugged()) {
    USB::disable();
  }
}

}
}

namespace Ion {
namespace Device {
namespace Power {

void configWakeUp() {
  Device::WakeUp::onOnOffKeyDown();
  Device::WakeUp::onUSBPlugging();
  Device::WakeUp::onChargingEvent();
}

using namespace Device::Regs;

void standbyConfiguration() {
  PWR.CPUCR()->setPDDS_D1(true);
  PWR.CPUCR()->setPDDS_D2(true);
  PWR.CPUCR()->setPDDS_D3(true);
  PWR.CPUCR()->setCSSF(true);
  PWR.CR2()->setBREN(false);

  /* The pin A0 is about to be configured as a wakeup pin. However, the matrix
   * keyboard connects pin A0 (row B) with other pins (column 1, column 3...).
   * We thus shutdown this pins to avoid the potential pull-up on pin A0 due to
   * a keyboard event. For example, if the "Home" key is down, pin A0 is
   * pulled-up so enabling it as the wake up pin would trigger a wake up flag
   * instantly. */
  Device::Keyboard::shutdown();
  PWR.WKUPCR()->setWKUPC1(true);
  PWR.WKUPEPR()->setWKUPEN1(true);
  PWR.WKUPEPR()->setWKUPP1(false);

  CORTEX.SCR()->setSLEEPDEEP(true);
}

void stopConfiguration() {
  PWR.CR1()->setLPDS(true);
  PWR.CR1()->setFLPS(true);
  PWR.CR1()->setSVOS(PWR::CR1::SVOS::VOS3);

  CORTEX.SCR()->setSLEEPDEEP(true);
}

void sleepConfiguration() {
  Device::Board::setStandardFrequency(Device::Board::Frequency::Low);
  Device::Board::setClockFrequency(Device::Board::standardFrequency());

  CORTEX.SCR()->setSLEEPDEEP(false);
}

void waitUntilOnOffKeyReleased() {
  /* Wait until power is released to avoid restarting just after suspending */
  bool isPowerDown = true;
  while (isPowerDown) {
    Keyboard::State scan = Keyboard::scan();
    isPowerDown = scan.keyDown(Keyboard::Key::OnOff);
  }
  Ion::Timing::msleep(100);
}

void __attribute__((noinline)) internalFlashSuspend(bool isLEDActive) {
  // Shutdown the external flash
  Device::ExternalFlash::shutdown();
  // Shutdown all clocks (except the ones used by LED if active)
  Device::Board::shutdownClocks(isLEDActive);

  Device::Power::enterLowPowerMode();

  /* A hardware event triggered a wake up, we determine if the device should
   * wake up. We wake up when:
   * - only the power key was down
   * - the unplugged device was plugged
   * - the battery stopped charging */
  Device::Board::initClocks();
  // Init external flash
  Device::ExternalFlash::init();
}

void __attribute__((noinline)) internalFlashStandby() {
  Device::ExternalFlash::shutdown();
  Device::Board::shutdownClocks();
  Device::Power::enterLowPowerMode();
  Device::Reset::coreWhilePlugged();
}

void enterLowPowerMode() {
  /* To enter sleep, we need to issue a WFE instruction, which waits for the
   * event flag to be set and then clears it. However, the event flag might
   * already be on. So the safest way to make sure we actually wait for a new
   * event is to force the event flag to on (SEV instruction), use a first WFE
   * to clear it, and then a second WFE to wait for a _new_ event. */
  asm("sev");
  asm("wfe");
  asm("nop");
  asm("wfe");
}

}
}
}
