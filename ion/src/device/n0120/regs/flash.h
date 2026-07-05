#ifndef REGS_FLASH_H
#define REGS_FLASH_H

#include "register.h"
#include <regs/config/flash.h>

namespace Ion {
namespace Device {
namespace Regs {

class FLASH {
public:
  class ACR : public Register32 {
  public:
    REGS_FIELD(LATENCY, uint8_t, 3, 0);
#if !REGS_FLASH_CONFIG_H725
    REGS_BOOL_FIELD(PRFTEN, 8);
#if REGS_FLASH_CONFIG_ART
    REGS_BOOL_FIELD(ARTEN, 9);
    REGS_BOOL_FIELD(ARTRST, 11);
#else
    REGS_BOOL_FIELD(ICEN, 9);
    REGS_BOOL_FIELD(DCEN, 10);
    REGS_BOOL_FIELD(ICRST, 11);
    REGS_BOOL_FIELD(DCRST, 12);
#endif
#endif
  };

  class KEYR : public Register32 {
  };

  class OPTKEYR : public Register32 {
  };

  class CR : public Register32 {
  public:
    enum class PSIZE : uint8_t {
      X8 = 0,
      X16 = 1,
      X32 = 2,
#if REGS_FLASH_CONFIG_H725
      X64 = 3
#else
      X64 = 3
#endif
    };
#if REGS_FLASH_CONFIG_H725
    REGS_BOOL_FIELD(LOCK, 0);
    REGS_BOOL_FIELD(PG, 1);
    REGS_BOOL_FIELD(SER, 2);
    REGS_BOOL_FIELD(BER, 3);
    REGS_TYPE_FIELD(PSIZE, 5, 4);
    REGS_BOOL_FIELD(START, 7);
    REGS_FIELD(SNB, uint8_t, 10, 8);
    REGS_BOOL_FIELD(EOPIE, 16);
    REGS_BOOL_FIELD(WRPERRIE, 17);
    REGS_BOOL_FIELD(PGSERRIE, 18);
    REGS_BOOL_FIELD(OPERRIE, 22);
#else
    REGS_BOOL_FIELD(PG, 0);
    REGS_BOOL_FIELD(SER, 1);
    REGS_BOOL_FIELD(MER, 2);
    REGS_FIELD(SNB, uint8_t, 6, 3);
    REGS_TYPE_FIELD(PSIZE, 9, 8);
    REGS_BOOL_FIELD(STRT, 16);
    REGS_BOOL_FIELD(EOPIE, 24);
    REGS_BOOL_FIELD(ERRIE, 25);
    REGS_BOOL_FIELD(RDERRIE, 26);
    REGS_BOOL_FIELD(LOCK, 31);
#endif
  };

  class SR : public Register32 {
  public:
    using Register32::Register32;
#if REGS_FLASH_CONFIG_H725
    REGS_BOOL_FIELD(BSY, 0);
    REGS_BOOL_FIELD(EOP, 16);
    REGS_BOOL_FIELD(WRPERR, 17);
    REGS_BOOL_FIELD(PGSERR, 18);
    REGS_BOOL_FIELD(OPERR, 22);
#else
    REGS_BOOL_FIELD(BSY, 16);
    REGS_BOOL_FIELD(ERSERR, 7);
    REGS_BOOL_FIELD(PGPERR, 6);
    REGS_BOOL_FIELD(PGAERR, 5);
    REGS_BOOL_FIELD(WRPERR, 4);
    REGS_BOOL_FIELD(EOP, 0);
#endif
  };

  class OPTCR : public Register32 {
  public:
#if REGS_FLASH_CONFIG_H725
    REGS_BOOL_FIELD(OPTLOCK, 0);
    REGS_BOOL_FIELD(OPTSTART, 1);
#else
    REGS_BOOL_FIELD(nWRP0, 16);
    REGS_BOOL_FIELD(nWRP1, 17);
    REGS_BOOL_FIELD(nWRP2, 18);
    REGS_BOOL_FIELD(nWRP3, 19);
    REGS_BOOL_FIELD(nWRP4, 20);
    REGS_BOOL_FIELD(nWRP5, 21);
    REGS_BOOL_FIELD(nWRP6, 22);
    REGS_BOOL_FIELD(nWRP7, 23);
    REGS_BOOL_FIELD(LOCK, 0);
#endif
  };

#if REGS_FLASH_CONFIG_H725
  class WPSN_PRG1 : public Register32 {
  public:
    REGS_FIELD(WRPSN, uint8_t, 7, 0);
  };
#endif

  constexpr FLASH() {};
  REGS_REGISTER_AT(ACR, 0x00);
  REGS_REGISTER_AT(KEYR, 0x04);
  REGS_REGISTER_AT(OPTKEYR, 0x08);
  REGS_REGISTER_AT(CR, 0x0C);
  REGS_REGISTER_AT(SR, 0x10);
  REGS_REGISTER_AT(OPTCR, 0x18);
#if REGS_FLASH_CONFIG_H725
  REGS_REGISTER_AT(WPSN_PRG1, 0x3C);
#endif
private:
  constexpr uint32_t Base() const {
    return 0x52002000;
  }
};

constexpr FLASH FLASH;

}
}
}

#endif
