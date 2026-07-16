// Working for N0120

#ifndef REGS_PWR_H
#define REGS_PWR_H
#include "register.h"
class PWR {
public:
  /* NOTE: CR/CSR below are the legacy F4-style registers. On the H725 the PWR
   * peripheral has a completely different map (CR1/CR2/CR3/CSR1/D3CR/...).
   * They're kept here only in case other code still references PWR.CR()/CSR(),
   * but LPDS/PPDS/DBP/FPDS/BRE/BRR do NOT correspond to real H725 bits at
   * these offsets - don't rely on them for H725 bring-up. */
  class CR : Register32 {
  public:
    REGS_BOOL_FIELD(LPDS, 0);
    REGS_BOOL_FIELD(PPDS, 1);
    REGS_BOOL_FIELD(DBP, 8);
    REGS_BOOL_FIELD(FPDS, 9);
  };
  class CSR : Register32 {
  public:
    REGS_BOOL_FIELD(BRE, 9);
    REGS_BOOL_FIELD_R(BRR, 3);
  };

  /* --- H725 registers actually needed for bring-up --- */

  /* CR3: system supply configuration (LDO / SMPS / bypass). Must be
   * configured before touching D3CR, and before raising SYSCLK. */
  class CR3 : Register32 {
  public:
    REGS_BOOL_FIELD(BYPASS, 0);   // PMU bypass (external supply only)
    REGS_BOOL_FIELD(LDOEN, 1);    // LDO regulator enable
    REGS_BOOL_FIELD(SMPSEN, 2);   // SMPS enable
    // SMPSLEVEL (bits 4:5), SMPSEXTHP, VBE, VBRS, USB33DEN/USBREGEN
    // intentionally omitted: not needed for direct-SMPS-supply bring-up.
  };

  /* CSR1: status flags. ACTVOSRDY must be set before D3CR.VOS may be
   * changed (RM0468 "system supply configuration" sequence). */
  class CSR1 : Register32 {
  public:
    REGS_BOOL_FIELD_R(PVDO, 4);
    REGS_BOOL_FIELD_R(ACTVOSRDY, 13);
    REGS_BOOL_FIELD_R(AVDO, 16);
  };

  /* D3CR: voltage scaling (VOS) for the D3 domain regulator.
   * VOS field is bits [15:14]; Scale1 (needed for 192 MHz) = both bits set.
   * Modelled as two bools instead of a single 2-bit field since only
   * single-bit macros are confirmed available. */
  class D3CR : Register32 {
  public:
    REGS_BOOL_FIELD(VOS0, 14);
    REGS_BOOL_FIELD(VOS1, 15);
    REGS_BOOL_FIELD_R(VOSRDY, 13);
  };

  constexpr PWR() {};
  REGS_REGISTER_AT(CR, 0x00);
  REGS_REGISTER_AT(CSR, 0x04);
  // Real H725 PWR_TypeDef layout (RM0468): CR1=0x00, CSR1=0x04, CR2=0x08,
  // CR3=0x0C, CPUCR=0x10, D3CR=0x18. CSR1 aliases the same address as the
  // legacy CSR above - that's fine, they're just two named views of it.
  REGS_REGISTER_AT(CR3, 0x0C);
  REGS_REGISTER_AT(CSR1, 0x04);
  REGS_REGISTER_AT(D3CR, 0x18);
private:
  constexpr uint32_t Base() const {
    return 0x40007000;
  };
};
constexpr PWR PWR;
#endif
