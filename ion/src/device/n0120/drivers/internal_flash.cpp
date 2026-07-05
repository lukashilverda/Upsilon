#include "internal_flash.h"
#include <drivers/cache.h>
#include <drivers/config/internal_flash.h>
#include <regs/config/flash.h>
#include <assert.h>
#include <algorithm>

namespace Ion {
namespace Device {
namespace InternalFlash {

using namespace Regs;

#if REGS_FLASH_CONFIG_H725
constexpr size_t FlashWordSize = 32;
#endif

static inline void wait() {
  /* Issue a DSB instruction to guarantee the completion of a previous access
   * to the flash control register or data write operation. */
  Cache::dsb();
  while (FLASH.SR()->getBSY()) {
  }
}

static void open() {
  if (FLASH.CR()->getLOCK()) {
    FLASH.KEYR()->set(0x45670123);
    FLASH.KEYR()->set(0xCDEF89AB);
  }
  assert(FLASH.CR()->getLOCK() == false);
  FLASH.CR()->setPSIZE(MemoryAccessWidth);
}

static void open_protection() {
#if REGS_FLASH_CONFIG_H725
  if (FLASH.OPTCR()->getOPTLOCK()) {
    FLASH.OPTKEYR()->set(0x08192A3B);
    FLASH.OPTKEYR()->set(0x4C5D6E7F);
  }
#else
  if (FLASH.OPTCR()->getLOCK()) {
    FLASH.OPTKEYR()->set(0x08192A3B);
    FLASH.OPTKEYR()->set(0x4C5D6E7F);
  }
#endif
}

static void close_protection() {
#if REGS_FLASH_CONFIG_H725
  if (!FLASH.OPTCR()->getOPTLOCK()) {
    FLASH.OPTCR()->setOPTLOCK(true);
  }
#else
  if (!FLASH.OPTCR()->getLOCK()) {
    FLASH.OPTCR()->setLOCK(true);
  }
#endif
}

#if REGS_FLASH_CONFIG_H725
static void set_sector_protection(int i, bool protect) {
  if (FLASH.OPTCR()->getOPTLOCK()) {
    return;
  }
  uint8_t wrpsn = FLASH.WPSN_PRG1()->getWRPSN();
  if (protect) {
    wrpsn &= static_cast<uint8_t>(~(1 << i));
  } else {
    wrpsn |= static_cast<uint8_t>(1 << i);
  }
  FLASH.WPSN_PRG1()->setWRPSN(wrpsn);
  FLASH.OPTCR()->setOPTSTART(true);
  wait();
}
#else
static void disable_protection_at(int i) {
  if (!FLASH.OPTCR()->getLOCK()) {
    switch (i)
    {
    case 0:
      FLASH.OPTCR()->setnWRP0(true);
      break;
    case 1:
      FLASH.OPTCR()->setnWRP1(true);
      break;
    case 2:
      FLASH.OPTCR()->setnWRP2(true);
      break;
    case 3:
      FLASH.OPTCR()->setnWRP3(true);
      break;
    case 4:
      FLASH.OPTCR()->setnWRP4(true);
      break;
    case 5:
      FLASH.OPTCR()->setnWRP5(true);
      break;
    case 6:
      FLASH.OPTCR()->setnWRP6(true);
      break;
    case 7:
      FLASH.OPTCR()->setnWRP7(true);
      break;
    default:
      break;
    }
  }
}

static void enable_protection_at(int i) {
  if (!FLASH.OPTCR()->getLOCK()) {
    switch (i)
    {
    case 0:
      FLASH.OPTCR()->setnWRP0(false);
      break;
    case 1:
      FLASH.OPTCR()->setnWRP1(false);
      break;
    case 2:
      FLASH.OPTCR()->setnWRP2(false);
      break;
    case 3:
      FLASH.OPTCR()->setnWRP3(false);
      break;
    case 4:
      FLASH.OPTCR()->setnWRP4(false);
      break;
    case 5:
      FLASH.OPTCR()->setnWRP5(false);
      break;
    case 6:
      FLASH.OPTCR()->setnWRP6(false);
      break;
    case 7:
      FLASH.OPTCR()->setnWRP7(false);
      break;
    default:
      break;
    }
  }
}
#endif

static void close() {
  class FLASH::SR sr(0);
#if REGS_FLASH_CONFIG_H725
  sr.setWRPERR(true);
  sr.setPGSERR(true);
  sr.setOPERR(true);
  sr.setEOP(true);
#else
  sr.setERSERR(true);
  sr.setPGPERR(true);
  sr.setPGAERR(true);
  sr.setWRPERR(true);
  sr.setEOP(true);
#endif
  FLASH.SR()->set(sr);

#if REGS_FLASH_CONFIG_H725
  assert(!FLASH.CR()->getBER());
#else
  assert(!FLASH.CR()->getMER());
#endif
  assert(!FLASH.CR()->getSER());
  assert(!FLASH.CR()->getPG());
  FLASH.CR()->setLOCK(true);

#if REGS_FLASH_CONFIG_ART
  if (FLASH.ACR()->getARTEN()) {
    FLASH.ACR()->setARTEN(false);
    FLASH.ACR()->setARTRST(true);
    FLASH.ACR()->setARTRST(false);
    FLASH.ACR()->setARTEN(true);
  }
#elif !REGS_FLASH_CONFIG_H725
  if (FLASH.ACR()->getDCEN()) {
    FLASH.ACR()->setDCEN(false);
    FLASH.ACR()->setDCRST(true);
    FLASH.ACR()->setDCRST(false);
    FLASH.ACR()->setDCEN(true);
  }
  if (FLASH.ACR()->getICEN()) {
    FLASH.ACR()->setICEN(false);
    FLASH.ACR()->setICRST(true);
    FLASH.ACR()->setICRST(false);
    FLASH.ACR()->setICEN(true);
  }
#endif
}

static inline constexpr size_t clog2(size_t input) {
  return (input == 1) ? 0 : clog2(input/2)+1;
}

template <typename T>
static inline T * align(void * input) {
  size_t k = clog2(sizeof(T));
  return reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(input) & ~((1<<k) - 1));
}

template <typename T>
static inline T eat(void * ptr) {
  T * pointer = *reinterpret_cast<T **>(ptr);
  T result = *pointer;
  *reinterpret_cast<T **>(ptr) = pointer+1;
  return result;
}

static inline ptrdiff_t byte_offset(void * p1, void * p2) {
  return reinterpret_cast<uint8_t *>(p2) - reinterpret_cast<uint8_t *>(p1);
}

#if REGS_FLASH_CONFIG_H725
static void write_flash_word(uint32_t * destination, uint32_t * source) {
  for (size_t i = 0; i < FlashWordSize / sizeof(uint32_t); i++) {
    destination[i] = source[i];
  }
  wait();
}

static void flash_memcpy(uint8_t * destination, uint8_t * source, size_t length) {
  uint8_t * alignedDestination = reinterpret_cast<uint8_t *>(align<uint32_t>(destination));
  ptrdiff_t headerDelta = byte_offset(alignedDestination, destination);
  assert(headerDelta >= 0 && headerDelta < static_cast<ptrdiff_t>(FlashWordSize));

  if (headerDelta > 0) {
    uint32_t flashWord[FlashWordSize / sizeof(uint32_t)];
    uint8_t * flashWordBytes = reinterpret_cast<uint8_t *>(flashWord);
    for (size_t i = 0; i < FlashWordSize; i++) {
      flashWordBytes[i] = alignedDestination[i];
    }
    for (size_t i = static_cast<size_t>(headerDelta); i < FlashWordSize && length > 0; i++) {
      flashWordBytes[i] = eat<uint8_t>(&source);
      length--;
    }
    write_flash_word(reinterpret_cast<uint32_t *>(alignedDestination), flashWord);
    alignedDestination += FlashWordSize;
  }

  while (length >= FlashWordSize) {
    uint32_t flashWord[FlashWordSize / sizeof(uint32_t)];
    uint8_t * flashWordBytes = reinterpret_cast<uint8_t *>(flashWord);
    for (size_t i = 0; i < FlashWordSize; i++) {
      flashWordBytes[i] = eat<uint8_t>(&source);
    }
    write_flash_word(reinterpret_cast<uint32_t *>(alignedDestination), flashWord);
    alignedDestination += FlashWordSize;
    length -= FlashWordSize;
  }

  if (length > 0) {
    uint32_t flashWord[FlashWordSize / sizeof(uint32_t)];
    uint8_t * flashWordBytes = reinterpret_cast<uint8_t *>(flashWord);
    for (size_t i = 0; i < FlashWordSize; i++) {
      flashWordBytes[i] = alignedDestination[i];
    }
    for (size_t i = 0; i < length; i++) {
      flashWordBytes[i] = eat<uint8_t>(&source);
    }
    write_flash_word(reinterpret_cast<uint32_t *>(alignedDestination), flashWord);
  }
}
#else
static void flash_memcpy(uint8_t * destination, uint8_t * source, size_t length) {
  static_assert(
    sizeof(MemoryAccessType) == 1 ||
    sizeof(MemoryAccessType) == 2 ||
    sizeof(MemoryAccessType) == 4 ||
    sizeof(MemoryAccessType) == 8,
  "Invalid MemoryAccessType");

  MemoryAccessType * alignedDestination = align<MemoryAccessType>(destination);
  ptrdiff_t headerDelta = byte_offset(alignedDestination, destination);
  assert(headerDelta >= 0 && headerDelta < static_cast<ptrdiff_t>(sizeof(MemoryAccessType)));

  if (headerDelta > 0) {
    MemoryAccessType header = *alignedDestination;
    uint8_t * headerStart = reinterpret_cast<uint8_t *>(&header);
    uint8_t * headerDataStart = headerStart + headerDelta;
    uint8_t * headerDataEnd = std::min(
      headerStart + sizeof(MemoryAccessType),
      headerDataStart + length
    );
    for (uint8_t * h = headerDataStart; h<headerDataEnd; h++) {
      *h = eat<uint8_t>(&source);
    }
    *alignedDestination++ = header;
    wait();
  }

  MemoryAccessType * lastAlignedDestination = align<MemoryAccessType>(destination + length);
  while (alignedDestination < lastAlignedDestination) {
    *alignedDestination++ = eat<MemoryAccessType>(&source);
    wait();
  }

  ptrdiff_t footerLength = byte_offset(alignedDestination, destination + length);
  assert(footerLength < static_cast<ptrdiff_t>(sizeof(MemoryAccessType)));
  if (footerLength > 0) {
    assert(alignedDestination == lastAlignedDestination);
    MemoryAccessType footer = *alignedDestination;
    uint8_t * footerPointer = reinterpret_cast<uint8_t *>(&footer);
    for (ptrdiff_t i=0; i<footerLength; i++) {
      footerPointer[i] = eat<uint8_t>(&source);
    }
    *alignedDestination = footer;
    wait();
  }
}
#endif

int SectorAtAddress(uint32_t address) {
  for (int i = 0; i < Config::NumberOfSectors; i++) {
    if (address >= Config::SectorAddresses[i] && address < Config::SectorAddresses[i+1]) {
      return i;
    }
  }
  return -1;
}

void MassErase() {
  open();
#if REGS_FLASH_CONFIG_H725
  FLASH.CR()->setBER(true);
  FLASH.CR()->setSTART(true);
  wait();
  FLASH.CR()->setBER(false);
#else
  FLASH.CR()->setMER(true);
  FLASH.CR()->setSTRT(true);
  wait();
  FLASH.CR()->setMER(false);
#endif
  close();
}

void EraseSector(int i) {
  assert(i >= 0 && i < Config::NumberOfSectors);
  open();
  FLASH.CR()->setSNB(i);
  FLASH.CR()->setSER(true);
#if REGS_FLASH_CONFIG_H725
  FLASH.CR()->setSTART(true);
#else
  FLASH.CR()->setSTRT(true);
#endif
  wait();
  FLASH.CR()->setSNB(0);
  FLASH.CR()->setSER(false);
  close();
}

void WriteMemory(uint8_t * destination, uint8_t * source, size_t length) {
  open();
  FLASH.CR()->setPG(true);
  flash_memcpy(destination, source, length);
  FLASH.CR()->setPG(false);
  close();
}

void EnableProtection() {
  close_protection();
}

void DisableProtection() {
  open_protection();
}

void SetSectorProtection(int i, bool protect) {
#if REGS_FLASH_CONFIG_H725
  set_sector_protection(i, protect);
#else
  if (protect) {
    enable_protection_at(i);
  } else {
    disable_protection_at(i);
  }
#endif
}

void EnableSessionLock() {
#if REGS_FLASH_CONFIG_H725
  if (FLASH.OPTCR()->getOPTLOCK()) {
#else
  if (FLASH.OPTCR()->getLOCK()) {
#endif
    FLASH.OPTKEYR()->set(0x00000000);
    FLASH.OPTKEYR()->set(0xFFFFFFFF);
  }
}

void EnableFlashInterrupt() {
  open();
#if REGS_FLASH_CONFIG_H725
  FLASH.CR()->setWRPERRIE(true);
  wait();
  FLASH.CR()->setPGSERRIE(true);
  wait();
  FLASH.CR()->setOPERRIE(true);
#else
  FLASH.CR()->setERRIE(true);
  wait();
  FLASH.CR()->setEOPIE(true);
  wait();
  FLASH.CR()->setRDERRIE(true);
#endif
  wait();
  close();
}

void ClearErrors() {
  class FLASH::SR sr(0);
#if REGS_FLASH_CONFIG_H725
  sr.setWRPERR(true);
  sr.setPGSERR(true);
  sr.setOPERR(true);
  sr.setEOP(true);
#else
  sr.setERSERR(true);
  sr.setPGPERR(true);
  sr.setPGAERR(true);
  sr.setWRPERR(true);
  sr.setEOP(true);
#endif
  FLASH.SR()->set(sr);
}

}
}
}
