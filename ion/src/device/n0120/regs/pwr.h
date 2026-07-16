#ifndef REGS_PWR_H
#define REGS_PWR_H

#include "register.h"
#include <regs/config/pwr.h>

namespace Ion {
namespace Device {
namespace Regs {

class PWR {
public:
  class CR1 : Register32 {
  public:
    REGS_BOOL_FIELD(LPDS, 0);
    REGS_BOOL_FIELD(DBP, 8);
    REGS_BOOL_FIELD(FLPS, 9);
    enum class SVOS : uint8_t {
      VOS3 = 0,
      VOS2 = 1,
      VOS1 = 2,
      VOS0 = 3
    };
    REGS_FIELD_W(SVOS, SVOS, 15, 14);
  };

  class CSR1 : Register32 {
  public:
    REGS_BOOL_FIELD_R(PVDO, 4);
    REGS_BOOL_FIELD_R(ACTVOSRDY, 13);
    enum class ACTVOS : uint8_t {
      VOS3 = 0,
      VOS2 = 1,
      VOS1 = 2,
      VOS0 = 3
    };
    REGS_FIELD_R(ACTVOS, ACTVOS, 15, 14);
  };

  class CR2 : Register32 {
  public:
    REGS_BOOL_FIELD_W(BREN, 0);
  };

  class CR3 : Register32 {
  public:
    REGS_BOOL_FIELD(BYPASS, 0);
    REGS_BOOL_FIELD(LDOEN, 1);
    REGS_BOOL_FIELD(SMPSEN, 2);
  };

  class CPUCR : Register32 {
  public:
    REGS_BOOL_FIELD_W(PDDS_D1, 0);
    REGS_BOOL_FIELD_W(PDDS_D2, 1);
    REGS_BOOL_FIELD_W(PDDS_D3, 2);
    REGS_BOOL_FIELD_R(STOPF, 5);
    REGS_BOOL_FIELD_R(SBF, 6);
    REGS_BOOL_FIELD_W(CSSF, 9);
  };

  class D3CR : Register32 {
  public:
    REGS_BOOL_FIELD_R(VOSRDY, 13);
    REGS_BOOL_FIELD(VOS0, 14);
    REGS_BOOL_FIELD(VOS1, 15);
    enum class VOS : uint8_t {
      VOS3 = 0,
      VOS2 = 1,
      VOS1 = 2,
      VOS0 = 3
    };
    REGS_FIELD_W(VOS, VOS, 15, 14);
  };

  class WKUPCR : Register32 {
  public:
    REGS_BOOL_FIELD_W(WKUPC1, 0);
  };

  class WKUPEPR : Register32 {
  public:
    REGS_BOOL_FIELD_W(WKUPEN1, 0);
    REGS_BOOL_FIELD_W(WKUPP1, 8);
  };

  constexpr PWR() {};
  REGS_REGISTER_AT(CR1, 0x00);
  REGS_REGISTER_AT(CSR1, 0x04);
  REGS_REGISTER_AT(CR2, 0x08);
  REGS_REGISTER_AT(CR3, 0x0C);
  REGS_REGISTER_AT(CPUCR, 0x10);
  REGS_REGISTER_AT(D3CR, 0x18);
  REGS_REGISTER_AT(WKUPCR, 0x20);
  REGS_REGISTER_AT(WKUPEPR, 0x28);
private:
  constexpr uint32_t Base() const {
    return 0x58024800;
  };
};

constexpr PWR PWR;

}
}
}

#endif
