// IDK if it works, I have to look at it. 


#include <drivers/board.h>
#include <drivers/cache.h>
#include <drivers/internal_flash.h>
#include <drivers/config/clocks.h>
#include <drivers/config/internal_flash.h>
#include <drivers/external_flash.h>
#include <drivers/backlight.h>
#include <drivers/battery.h>
#include <drivers/console.h>
#include <drivers/display.h>
#include <drivers/keyboard.h>
#include <drivers/led.h>
#include <drivers/rtc.h>
#include <drivers/swd.h>
#include <drivers/timing.h>
#include <drivers/usb.h>
#include <regs/regs.h>
#include <ion.h>

typedef void(*ISR)(void); // Type of an interrupt service routine
extern ISR InitialisationVector[]; // The initialisation vector is defined in the linker script

// Public Ion methods

const char * Ion::fccId() {    //FCC ID, N0120
  return "2ALWP-N0120";        
}

// Private Ion::Device methods

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;


// I will look at this function later, I think it needs some changes to work with the N0120.

void bootloaderMPU() {
  // 1. Disable the MPU (memory protection unit)
  // 1.1 Memory barrier (Data Memory Barrier) to ensure that all explicit memory accesses before this instruction are completed before any subsequent instructions are executed.
  Cache::dmb();

  // 1.3 Disable the MPU and clear the control register
  MPU.CTRL()->setENABLE(false);

  MPU.RNR()->setREGION(7);
  MPU.RBAR()->setADDR(0x90000000);        // Base address of QuadSPI
  MPU.RASR()->setXN(false);
  MPU.RASR()->setENABLE(true);

  // 2.3 Enable MPU because the bootloader needs to access the external flash memory
  MPU.CTRL()->setENABLE(true);

  // 3. Data/instruction synchronisation barriers to ensure that the new MPU configuration is used by subsequent instructions.
  Cache::disable();
  Cache::dsb();
  Cache::isb();
}

void initMPU() {
  // 1. Disable the MPU
  // 1.1 Memory barrier
  Cache::dmb();

  // 1.2 Disable fault exceptions
  CORTEX.SHCRS()->setMEMFAULTENA(false);

  // 1.3 Disable the MPU and clear the control register
  MPU.CTRL()->setENABLE(false);

  // 2. MPU settings
  // 2.1 Configure a MPU region for the FMC memory area
  /* This is needed for interfacing with the LCD
   * We define the whole FMC memory bank 1 as strongly ordered, non-executable
   * and not accessible. We define the FMC command and data addresses as
   * writeable non-cacheable, non-buffereable and non shareable. */
  int sector = 0;
  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(0x60000000);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_256MB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::NoAccess);
  MPU.RASR()->setXN(true);
  MPU.RASR()->setTEX(2);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(0);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(0x60000000);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_32B);
  MPU.RASR()->setXN(true);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::RW);
  MPU.RASR()->setTEX(2);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(0);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(0x60000000+0x20000);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_32B);
  MPU.RASR()->setXN(true);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::RW);
  MPU.RASR()->setTEX(2);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(0);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  // 2.2 Configure MPU regions for the QUADSPI peripheral
  /* L1 Cache can issue speculative reads to any memory address. But, when the
   * Quad-SPI is in memory-mapped mode, if an access is made to an address
   * outside of the range defined by FSIZE but still within the 256Mbytes range,
   * then an AHB error is given (AN4760). To prevent this to happen, we
   * configure the MPU to define the whole Quad-SPI addressable space as
   * strongly ordered, non-executable and not accessible. Plus, we define the
   * Quad-SPI region corresponding to the External Chip as executable and
   * fully accessible (AN4861). */
  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(0x90000000);        // Base address of QuadSPI
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_256MB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::NoAccess);
  MPU.RASR()->setXN(true);
  MPU.RASR()->setTEX(0);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(0);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(0x90000000);        // Base address of QuadSPI
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_8MB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::RW);
  MPU.RASR()->setXN(false);
  MPU.RASR()->setTEX(0);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(1);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  // 2.3 Enable MPU
  MPU.CTRL()->setPRIVDEFENA(true);
  MPU.CTRL()->setENABLE(true);

  // 3. Data/instruction synchronisation barriers to ensure that the new MPU configuration is used by subsequent instructions.
  Cache::dsb();
  Cache::isb();
}

void init() {
  initFPU(); // Enable the floating point unit
  initMPU(); // Enable the memory protection unit
  initClocks(); // Configure the clock tree and enable peripheral clocks

  // The bootloader leaves its own after flashing
  //SYSCFG.MEMRMP()->setMEM_MODE(SYSCFG::MEMRMP::MemMode::MainFlashmemory);
  // Ensure right location of interrupt vectors
  CORTEX.VTOR()->setVTOR((void*)&InitialisationVector);

  // Put all inputs as Analog Input, No pull-up nor pull-down
  // Except for the SWD port (PB3, PA13, PA14)
  GPIOA.MODER()->set(0xEBFFFFFF);
  GPIOA.PUPDR()->set(0x24000000);
  GPIOB.MODER()->set(0xFFFFFFBF);
  GPIOB.PUPDR()->set(0x00000000);
  for (int g=2; g<5; g++) {
    GPIO(g).MODER()->set(0xFFFFFFFF); // All to "Analog"
    GPIO(g).PUPDR()->set(0x00000000); // All to "None"
  }

  ExternalFlash::init(); // Initialize the external flash memory
 
  Cache::enable(); // Enable the L1 cache (instruction and data)
}

void initClocks() {
  /* STM32H725 clock tree: HSE -> PLL1 -> SYSCLK/HCLK at 192 MHz. */

  // Enable the HSI and wait for it to be ready (64 MHz on H7)
  RCC.CR()->setHSION(true);
  while(!RCC.CR()->getHSIRDY()) {
  }

  // Enable the HSE and wait for it to be ready
  RCC.CR()->setHSEON(true);
  while(!RCC.CR()->getHSERDY()) {
  }

  /* Given the crystal used on our device, the HSE will oscillate at 8 MHz. By
   * piping it through PLL1 we can derive other frequencies for the system. */
  RCC.PLLCKSELR()->setPLLSRC(RCC::PLLCKSELR::PLLSRC::HSE);
  RCC.PLLCKSELR()->setDIVM1(Clocks::Config::PLL_M);
  RCC.PLLCFGR()->setPLL1RGE(0); // 1-2 MHz PLL input (HSE / PLL_M)
  RCC.PLLCFGR()->setPLL1VCOSEL(false); // Wide VCO range (192-836 MHz)
  RCC.PLL1DIVR()->setN1(Clocks::Config::PLL_N1_Reg);
  RCC.PLL1DIVR()->setP1(Clocks::Config::PLL_P1_Reg);
  RCC.PLL1DIVR()->setQ1(Clocks::Config::PLL_Q1_Reg);
  RCC.PLLCFGR()->setPLL1PEN(true);
  RCC.PLLCFGR()->setPLL1QEN(true);

  // Voltage scale 1 (VOS1) for 192 MHz operation on STM32H725
  PWR.D3CR()->setVOS(PWR::D3CR::VOS::VOS1);
  while (!PWR.D3CR()->getVOSRDY()) {}

  /* After reset the Flash runs as fast as the CPU. When we clock the CPU faster
   * the flash memory cannot follow and therefore flash memory accesses need to
   * wait a little bit. */
  FLASH.ACR()->setLATENCY(2);

  // 192 MHz is too fast for APB1. Divide it by four to reach 48 MHz
  RCC.D2CFGR()->setD2PPRE1(Clocks::Config::APB1PrescalerReg);
  // 192 MHz is too fast for APB2. Divide it by two to reach 96 MHz
  RCC.D2CFGR()->setD2PPRE2(Clocks::Config::APB2PrescalerReg);

  // Enable PLL1 and wait for it to be ready
  RCC.CR()->setPLL1ON(true);
  while(!RCC.CR()->getPLL1RDY()) {
  }

  // Use PLL1 as SYSCLK source
  RCC.CFGR()->setSW(RCC::CFGR::SW::PLL1);
  while (RCC.CFGR()->getSWS() != RCC::CFGR::SW::PLL1) {
  }

  // Now that we don't need use it anymore, turn the HSI off
  RCC.CR()->setHSION(false);

  // Peripheral clocks

  // AHB4 bus: GPIO and CRC
  class RCC::AHB4ENR ahb4enr(0);
  ahb4enr.setGPIOAEN(true);
  ahb4enr.setGPIOBEN(true);
  ahb4enr.setGPIOCEN(true);
  ahb4enr.setGPIODEN(true);
  ahb4enr.setGPIOEEN(true);
  RCC.AHB4ENR()->set(ahb4enr);

  // AHB1 bus: DMA and USB
  RCC.AHB1ENR()->setDMA2EN(true);
  RCC.AHB1ENR()->setUSB1OTGHSEN(true);

  // AHB3 bus: FMC (LCD) and OCTOSPI (external flash)
  RCC.AHB3ENR()->setFMCEN(true);

  // APB1 bus: TIM3 for the LEDs
  RCC.APB1LENR()->setTIM3EN(true);

  // APB2 bus
  RCC.APB2ENR()->setUSART6EN(true); // TODO required if building bench target only?

  // APB4 bus: SYSCFG and RTC
  RCC.APB4ENR()->setSYSCFGEN(true);
  RCC.APB4ENR()->setRTCAPBEN(true);

  // Configure clocks in sleep mode
  class RCC::AHB4LPENR ahb4lpenr(0);
  ahb4lpenr.setGPIOALPEN(true); // Charging/USB plug/Keyboard pins
  ahb4lpenr.setGPIOBLPEN(true); // LED pins
  ahb4lpenr.setGPIOCLPEN(true); // LED/Keyboard pins
  ahb4lpenr.setGPIODLPEN(false); // LCD
  ahb4lpenr.setGPIOELPEN(true); // Keyboard/Battery pins
  ahb4lpenr.setGPIOFLPEN(false);
  ahb4lpenr.setGPIOGLPEN(false);
  ahb4lpenr.setGPIOHLPEN(false);
  ahb4lpenr.setCRCLPEN(false);
  RCC.AHB4LPENR()->set(ahb4lpenr);

  class RCC::AHB1LPENR ahb1lpenr(0);
  ahb1lpenr.setDMA2LPEN(false);
  ahb1lpenr.setUSB1OTGHSLPEN(false);
  ahb1lpenr.setUSB1OTGHSULPILPEN(false);
  RCC.AHB1LPENR()->set(ahb1lpenr);

  class RCC::AHB2LPENR ahb2lpenr(0);
  ahb2lpenr.setRNGLPEN(false);
  RCC.AHB2LPENR()->set(ahb2lpenr);

  class RCC::AHB3LPENR ahb3lpenr(0);
  ahb3lpenr.setFMCLPEN(false);
  ahb3lpenr.setOSPI1LPEN(false);
  RCC.AHB3LPENR()->set(ahb3lpenr);

  class RCC::APB1LLPENR apb1llpenr(0);
  apb1llpenr.setTIM3LPEN(true); // TIM3 in sleep mode for LEDs
  RCC.APB1LLPENR()->set(apb1llpenr);

  class RCC::APB2LPENR apb2lpenr(0);
  apb2lpenr.setUSART6LPEN(false);
  RCC.APB2LPENR()->set(apb2lpenr);

  class RCC::APB4LPENR apb4lpenr(0);
  apb4lpenr.setSYSCFGLPEN(false);
  apb4lpenr.setRTCAPBLPEN(false);
  RCC.APB4LPENR()->set(apb4lpenr);
}

void shutdownClocks(bool keepLEDAwake) {
  RCC.APB2ENR()->set(0);
  RCC.APB4ENR()->set(0);
  RCC.AHB2ENR()->set(0);
  RCC.AHB3ENR()->set(0);

  class RCC::APB1LENR apb1lenr(0);
  class RCC::AHB4ENR ahb4enr(0);
  class RCC::AHB1ENR ahb1enr(0);
  if (keepLEDAwake) {
    apb1lenr.setTIM3EN(true);
    ahb4enr.setGPIOBEN(true);
  }
  RCC.APB1LENR()->set(apb1lenr);
  RCC.AHB4ENR()->set(ahb4enr);
  RCC.AHB1ENR()->set(ahb1enr);
}

constexpr int k_pcbVersionOTPIndex = 0;

/* As we want the PCB versions to be in ascending order chronologically, and
 * because the OTP are initialized with 1s, we store the bitwise-not of the
 * version number. This way, devices with blank OTP are considered version 0. */

PCBVersion pcbVersion() {
#if IN_FACTORY
  /* When flashing for the first time, we want all systems that depend on the
   * PCB version to function correctly before flashing the PCB version. This
   * way, flashing the PCB version can be done last. */
  return PCB_LATEST;
#else
  PCBVersion version = readPCBVersionInMemory();
  return (version == k_alternateBlankVersion ? 0 : version);
#endif
}

PCBVersion readPCBVersionInMemory() {
  return ~(*reinterpret_cast<const PCBVersion *>(InternalFlash::Config::OTPAddress(k_pcbVersionOTPIndex)));
}

void writePCBVersion(PCBVersion version) {
  uint8_t * destination = reinterpret_cast<uint8_t *>(InternalFlash::Config::OTPAddress(k_pcbVersionOTPIndex));
  PCBVersion formattedVersion = ~version;
  InternalFlash::WriteMemory(destination, reinterpret_cast<uint8_t *>(&formattedVersion), sizeof(formattedVersion));
}

void lockPCBVersion() {
  uint8_t * destination = reinterpret_cast<uint8_t *>(InternalFlash::Config::OTPLockAddress(k_pcbVersionOTPIndex));
  uint8_t zero = 0;
  InternalFlash::WriteMemory(destination, &zero, sizeof(zero));
}

bool pcbVersionIsLocked() {
  return *reinterpret_cast<const uint8_t *>(InternalFlash::Config::OTPLockAddress(k_pcbVersionOTPIndex)) == 0;
}

void jumpToInternalBootloader() {}

void initFPU() {
  // Enable CP10 and CP11 (Floating Point Unit)
  CORTEX.CPACR()->setAccess(10, CORTEX::CPACR::Access::Full);
  CORTEX.CPACR()->setAccess(11, CORTEX::CPACR::Access::Full);

  // Flush pipeline so subsequent instructions see the updated CPACR
  __asm volatile ("dsb");
  __asm volatile ("isb");
}

void initCompensationCell() {
  /* The output speed of some GPIO pins is set to high, in which case,
   * the compensation cell should be enabled. */
  SYSCFG.CMPCR()->setCMP_PD(true);
  while (!SYSCFG.CMPCR()->getREADY()) {
  }
}

void shutdownCompensationCell() {
  SYSCFG.CMPCR()->setCMP_PD(false); // Disable the compensation cell to save power
}

void initPeripherals(bool initBacklight) { // Initialise all peripherals
  initCompensationCell();
  Display::init();
  if (initBacklight) {
    Backlight::init();
  }
  Keyboard::init();
  LED::init();
  Battery::init();
  USB::init();
  Console::init();
  SWD::init();
  Timing::init();
}

void shutdownPeripherals(bool keepLEDAwake) { // Shutdown all peripherals, except the LCD if exam mode is on
  Timing::shutdown();
  SWD::shutdown();
  Console::shutdown();
  USB::shutdown();
  Battery::shutdown();
  if (!keepLEDAwake) {
    LED::shutdown();
  }
  Keyboard::shutdown();
  Backlight::shutdown();
  Display::shutdown();
  shutdownCompensationCell();
}

static Frequency sStandardFrequency = Frequency::High;

Frequency standardFrequency() {
  return sStandardFrequency;
}

void setStandardFrequency(Frequency f) {
  sStandardFrequency = f;
}

void updateTIM3Clock() {  // Update TIM3 timing (used after HCLK/APB clock change)
  uint32_t tim3Clock = Clocks::Config::APB1Frequency * 2;  // Fix for correct clock frequency

  // Keep the same LED blink frequency
  TIM3->PSC = (tim3Clock / 1000000) - 1; // 1 MHz timer tick
  TIM3->ARR = 1000 - 1;                 // 1 ms resolution

  TIM3->EGR = TIM_EGR_UG; // Force update
}

void setClockFrequency(Frequency f) {
  if (f == Frequency::High) {
    RCC.D1CFGR()->setHPRE(RCC::D1CFGR::HPRE::Div1);
    Device::Timing::setSysTickFrequency(Ion::Device::Clocks::Config::HCLKFrequency);
  } else {
    assert(f == Frequency::Low);

    // Change the systick frequency to compensate the HCLK frequency change
    Device::Timing::setSysTickFrequency(Ion::Device::Clocks::Config::HCLKLowFrequency);
    RCC.D1CFGR()->setHPRE(Clocks::Config::AHBLowFrequencyPrescalerReg);
  }

  // Update TIM3 timing after HCLK/APB clock change
  updateTIM3Clock();
}



}
}


namespace Ion {
namespace Board {

using namespace Device::Board;

void lockUnlockedPCBVersion() {
  if (pcbVersionIsLocked()) {
    return;
  }
  /* PCB version is unlocked : the device is a N0110 that has been
   * produced prior to the pcb revision 3.43. */
  PCBVersion version = Device::Board::pcbVersion();
  if (version != 0) {
    /* Some garbage has been written in OTP0. We overwrite it fully, which is
     * interepreted as blank. */
    writePCBVersion(k_alternateBlankVersion);
  }
  lockPCBVersion();
}

}
}
