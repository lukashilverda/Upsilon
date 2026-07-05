#ifndef REGS_RCC_H
#define REGS_RCC_H

#include "register.h"
#include <regs/config/rcc.h>

namespace Ion {
namespace Device {
namespace Regs {

class RCC {
public:
  class CR : public Register32 {
  public:
    REGS_BOOL_FIELD(HSION, 0);
    REGS_BOOL_FIELD_R(HSIRDY, 2);
    REGS_BOOL_FIELD(HSEON, 16);
    REGS_BOOL_FIELD_R(HSERDY, 17);
    REGS_BOOL_FIELD(PLL1ON, 24);
    REGS_BOOL_FIELD_R(PLL1RDY, 25);
  };

  class CFGR : public Register32 {
  public:
    enum class SW : uint8_t {
      HSI = 0,
      CSI = 1,
      HSE = 2,
      PLL1 = 3
    };
    void setSW(SW s) volatile { setBitRange(2, 0, (uint8_t)s); }
    SW getSWS() volatile { return (SW)getBitRange(5, 3); }
    REGS_FIELD(RTCPRE, uint8_t, 13, 8);
  };

  class D1CFGR : public Register32 {
  public:
    enum class HPRE : uint8_t {
      Div1 = 0,
      Div2 = 8,
      Div4 = 9,
      Div8 = 10,
      Div16 = 11,
      Div64 = 12,
      Div128 = 13,
      Div256 = 14,
      Div512 = 15
    };
    void setHPRE(HPRE p) volatile { setBitRange(3, 0, (uint32_t)p); }
  };

  class D2CFGR : public Register32 {
  public:
    enum class D2PPRE : uint8_t {
      Div1 = 0,
      Div2 = 4,
      Div4 = 5,
      Div8 = 6,
      Div16 = 7
    };
    void setD2PPRE1(D2PPRE p) volatile { setBitRange(6, 4, (uint32_t)p); }
    void setD2PPRE2(D2PPRE p) volatile { setBitRange(10, 8, (uint32_t)p); }
  };

  class PLLCKSELR : public Register32 {
  public:
    enum class PLLSRC : uint8_t {
      HSI = 0,
      CSI = 1,
      HSE = 2
    };
    void setPLLSRC(PLLSRC s) volatile { setBitRange(1, 0, (uint8_t)s); }
    REGS_FIELD(DIVM1, uint8_t, 9, 4);
  };

  class PLLCFGR : public Register32 {
  public:
    REGS_BOOL_FIELD(PLL1FRACEN, 0);
    REGS_BOOL_FIELD(PLL1VCOSEL, 1);
    REGS_FIELD(PLL1RGE, uint8_t, 3, 2);
    REGS_BOOL_FIELD(PLL1PEN, 16);
    REGS_BOOL_FIELD(PLL1QEN, 17);
    REGS_BOOL_FIELD(PLL1REN, 18);
  };

  class PLL1DIVR : public Register32 {
  public:
    /* Register values are (divider - 1). */
    REGS_FIELD(N1, uint16_t, 8, 0);
    REGS_FIELD(P1, uint8_t, 15, 9);
    REGS_FIELD(Q1, uint8_t, 22, 16);
    REGS_FIELD(R1, uint8_t, 30, 24);
  };

  class BDCR : public Register32 {
  public:
    REGS_BOOL_FIELD(RTCEN, 15);
    REGS_BOOL_FIELD(BDRST, 16);
    REGS_FIELD(RTCSEL, uint8_t, 9, 8);
  };

  class CSR : public Register32 {
  public:
    REGS_BOOL_FIELD(LSION, 0);
    REGS_BOOL_FIELD_R(LSIRDY, 1);
  };

  class AHB3RSTR : Register32 {
  public:
    REGS_BOOL_FIELD(OSPI1RST, 14);
    REGS_BOOL_FIELD(OSPI2RST, 19);
  };

  class AHB3ENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(FMCEN, 12);
    REGS_BOOL_FIELD(OSPI1EN, 14);
    REGS_BOOL_FIELD(SDMMC1EN, 16);
    REGS_BOOL_FIELD(OSPI2EN, 19);
  };

  class AHB1ENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(DMA2EN, 1);
    REGS_BOOL_FIELD(ADC12EN, 5);
    REGS_BOOL_FIELD(USB1OTGHSEN, 25);
    REGS_BOOL_FIELD(USB1OTGHSULPIEN, 26);
  };

  class AHB2ENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(RNGEN, 6);
  };

  class AHB4ENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(GPIOAEN, 0);
    REGS_BOOL_FIELD(GPIOBEN, 1);
    REGS_BOOL_FIELD(GPIOCEN, 2);
    REGS_BOOL_FIELD(GPIODEN, 3);
    REGS_BOOL_FIELD(GPIOEEN, 4);
    REGS_BOOL_FIELD(GPIOFEN, 5);
    REGS_BOOL_FIELD(GPIOGEN, 6);
    REGS_BOOL_FIELD(GPIOHEN, 7);
    REGS_BOOL_FIELD(CRCEN, 19);
  };

  class APB1LENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(TIM3EN, 1);
    REGS_BOOL_FIELD(USART2EN, 17);
    REGS_BOOL_FIELD(USART3EN, 18);
  };

  class APB2ENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(USART6EN, 5);
    REGS_BOOL_FIELD(SPI1EN, 12);
  };

  class APB4ENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(SYSCFGEN, 1);
    REGS_BOOL_FIELD(RTCAPBEN, 16);
  };

  class AHB3LPENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(FMCLPEN, 12);
    REGS_BOOL_FIELD(OSPI1LPEN, 14);
    REGS_BOOL_FIELD(OSPI2LPEN, 19);
  };

  class AHB1LPENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(DMA2LPEN, 1);
    REGS_BOOL_FIELD(ADC12LPEN, 5);
    REGS_BOOL_FIELD(USB1OTGHSLPEN, 25);
    REGS_BOOL_FIELD(USB1OTGHSULPILPEN, 26);
  };

  class AHB2LPENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(RNGLPEN, 6);
  };

  class AHB4LPENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(GPIOALPEN, 0);
    REGS_BOOL_FIELD(GPIOBLPEN, 1);
    REGS_BOOL_FIELD(GPIOCLPEN, 2);
    REGS_BOOL_FIELD(GPIODLPEN, 3);
    REGS_BOOL_FIELD(GPIOELPEN, 4);
    REGS_BOOL_FIELD(GPIOFLPEN, 5);
    REGS_BOOL_FIELD(GPIOGLPEN, 6);
    REGS_BOOL_FIELD(GPIOHLPEN, 7);
    REGS_BOOL_FIELD(CRCLPEN, 19);
  };

  class APB1LLPENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(TIM3LPEN, 1);
    REGS_BOOL_FIELD(USART2LPEN, 17);
    REGS_BOOL_FIELD(USART3LPEN, 18);
  };

  class APB2LPENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(USART6LPEN, 5);
    REGS_BOOL_FIELD(SPI1LPEN, 12);
  };

  class APB4LPENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(SYSCFGLPEN, 1);
    REGS_BOOL_FIELD(RTCAPBLPEN, 16);
  };

  constexpr RCC() {};
  REGS_REGISTER_AT(CR, 0x00);
  REGS_REGISTER_AT(CFGR, 0x10);
  REGS_REGISTER_AT(D1CFGR, 0x18);
  REGS_REGISTER_AT(D2CFGR, 0x1C);
  REGS_REGISTER_AT(PLLCKSELR, 0x28);
  REGS_REGISTER_AT(PLLCFGR, 0x2C);
  REGS_REGISTER_AT(PLL1DIVR, 0x30);
  REGS_REGISTER_AT(BDCR, 0x70);
  REGS_REGISTER_AT(CSR, 0x74);
  REGS_REGISTER_AT(AHB3RSTR, 0x7C);
  REGS_REGISTER_AT(AHB3ENR, 0xD4);
  REGS_REGISTER_AT(AHB1ENR, 0xD8);
  REGS_REGISTER_AT(AHB2ENR, 0xDC);
  REGS_REGISTER_AT(AHB4ENR, 0xE0);
  REGS_REGISTER_AT(APB1LENR, 0xE8);
  REGS_REGISTER_AT(APB2ENR, 0xF0);
  REGS_REGISTER_AT(APB4ENR, 0xF4);
  REGS_REGISTER_AT(AHB3LPENR, 0xFC);
  REGS_REGISTER_AT(AHB1LPENR, 0x100);
  REGS_REGISTER_AT(AHB2LPENR, 0x104);
  REGS_REGISTER_AT(AHB4LPENR, 0x108);
  REGS_REGISTER_AT(APB1LLPENR, 0x110);
  REGS_REGISTER_AT(APB2LPENR, 0x118);
  REGS_REGISTER_AT(APB4LPENR, 0x11C);
private:
  constexpr uint32_t Base() const {
    return 0x58024400;
  }
};

constexpr RCC RCC;

}
}
}

#endif
