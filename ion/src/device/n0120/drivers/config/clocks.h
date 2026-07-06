#ifndef ION_DEVICE_N0120_CONFIG_CLOCKS_H
#define ION_DEVICE_N0120_CONFIG_CLOCKS_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Clocks {
namespace Config {

/* STM32H725 PLL1 clock tree.
 * HSE = 8 MHz, SYSCLK = HCLK = 192 MHz, PLL1Q = 48 MHz for USB. */

constexpr static int HSE = 8;
constexpr static int PLL_M = 8;
constexpr static int PLL_N = 384;
constexpr static int PLL_P = 2;
constexpr static int PLL_Q = 8;

/* Values written to RCC registers (see RM0468). */
constexpr static int PLL_N1_Reg = PLL_N - 1;
constexpr static int PLL_P1_Reg = PLL_P/2 - 1;
constexpr static int PLL_Q1_Reg = PLL_Q - 1;

constexpr static int SYSCLKFrequency = ((HSE/PLL_M)*PLL_N)/PLL_P;
constexpr static int AHBPrescaler = 1;
constexpr static Regs::RCC::D1CFGR::HPRE AHBPrescalerReg = Regs::RCC::D1CFGR::HPRE::Div1;
constexpr static Regs::RCC::D1CFGR::HPRE AHBLowFrequencyPrescalerReg = Regs::RCC::D1CFGR::HPRE::Div8;
constexpr static int AHBLowFrequencyPrescaler = 8;
constexpr static int HCLKFrequency = SYSCLKFrequency/AHBPrescaler;
static_assert(HCLKFrequency == 192, "HCLK frequency changed!");
constexpr static int HCLKLowFrequency = SYSCLKFrequency/AHBLowFrequencyPrescaler;
constexpr static int AHBFrequency = HCLKFrequency;

constexpr static Regs::RCC::D2CFGR::D2PPRE APB1PrescalerReg = Regs::RCC::D2CFGR::D2PPRE::Div4;
constexpr static int APB1Prescaler = 4;
constexpr static int APB1LowFrequency = HCLKLowFrequency/APB1Prescaler;
constexpr static int APB1TimerLowFrequency = 2*APB1LowFrequency;

constexpr static Regs::RCC::D2CFGR::D2PPRE APB2PrescalerReg = Regs::RCC::D2CFGR::D2PPRE::Div2;

}
}
}
}

#endif
