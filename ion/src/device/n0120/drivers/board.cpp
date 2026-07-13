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


// Minimal MPU setup used by the bootloader: it only needs the external
// (Q)SPI flash mapped in as executable/cacheable memory, unlike initMPU()
// which also sets up the FMC/LCD regions.

void bootloaderMPU() {
  // 1. Memory barrier before touching the MPU.
  Cache::dmb();

  // 2. Disable the MPU while we reconfigure it.
  MPU.CTRL()->setENABLE(false);

  // 3. Region 7: the QSPI-mapped external flash at 0x90000000. This must
  // define SIZE/AP/TEX/S/C/B like every other region in initMPU() below -
  // leaving them unset left the region's size and access permissions
  // undefined, which is why this never reliably mapped the flash as
  // executable code. Mirrors the executable QSPI region in initMPU().
  MPU.RNR()->setREGION(7);
  MPU.RBAR()->setADDR(0x90000000);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_8MB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::RW);
  MPU.RASR()->setXN(false);
  MPU.RASR()->setTEX(0);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(1);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  // 4. Synchronisation barriers before the new MPU config takes effect.
  Cache::dsb();
  Cache::isb();

  // 5. Enable the MPU, then the cache. The previous version disabled the
  // cache here instead of enabling it, which was backwards - without the
  // MPU region marking QSPI as cacheable (step 3), enabling the L1 cache
  // beforehand risks speculative reads outside FSIZE per AN4760.
  MPU.CTRL()->setENABLE(true);
  Cache::enable();
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
  MPU.RBAR()->setADDR(0x90000000);
  MPU.RASR()->setSIZE(MPU::RASR::RegionSize::_256MB);
  MPU.RASR()->setAP(MPU::RASR::AccessPermission::NoAccess);
  MPU.RASR()->setXN(true);
  MPU.RASR()->setTEX(0);
  MPU.RASR()->setS(0);
  MPU.RASR()->setC(0);
  MPU.RASR()->setB(0);
  MPU.RASR()->setENABLE(true);

  MPU.RNR()->setREGION(sector++);
  MPU.RBAR()->setADDR(0x90000000);
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

  /* System supply configuration (RM0468 §6.8.1). This board runs off a
   * 3.7V single-cell battery through the H725's own SMPS step-down, with no
   * external LDO/SMPS inductor network in front of it - i.e. "Direct SMPS
   * supply" (LDO bypassed). This MUST be configured before VOS is touched:
   * VOS is not allowed to change until the regulator reports ACTVOSRDY for
   * the newly selected supply. */
  PWR.CR3()->setLDOEN(false);
  PWR.CR3()->setBYPASS(false);
  PWR.CR3()->setSMPSEN(true);
  while (!PWR.CSR1()->getACTVOSRDY()) {}

  // Voltage scale 1 (VOS1) for 192 MHz operation on STM32H725.
  // VOS is a 2-bit field at D3CR[15:14]; Scale1 = both bits set.
  PWR.D3CR()->setVOS0(true);
  PWR.D3CR()->setVOS1(true);
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

PCBVersion pcbVersion() {
#if IN_FACTORY
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
  CORTEX.CPACR()->setAccess(10, CORTEX::CPACR::Access::Full);
  CORTEX.CPACR()->setAccess(11, CORTEX::CPACR::Access::Full);
}

void initCompensationCell() {
  SYSCFG.CMPCR()->setCMP_PD(true);
  while (!SYSCFG.CMPCR()->getREADY()) {
  }
}

void shutdownCompensationCell() {
  SYSCFG.CMPCR()->setCMP_PD(false);
}

void initPeripherals(bool initBacklight) {
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

void shutdownPeripherals(bool keepLEDAwake) {
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

void setClockFrequency(Frequency f) {
  if (f == Frequency::High) {
    RCC.D1CFGR()->setHPRE(RCC::D1CFGR::HPRE::Div1);
    Device::Timing::setSysTickFrequency(Ion::Device::Clocks::Config::HCLKFrequency);
  } else {
    assert(f == Frequency::Low);
    Device::Timing::setSysTickFrequency(Ion::Device::Clocks::Config::HCLKLowFrequency);
    RCC.D1CFGR()->setHPRE(Clocks::Config::AHBLowFrequencyPrescalerReg);
  }
}

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
  PCBVersion version = Device::Board::pcbVersion();
  if (version != 0) {
    writePCBVersion(k_alternateBlankVersion);
  }
  lockPCBVersion();
}

}
}
