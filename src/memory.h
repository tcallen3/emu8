#ifndef EMU8_MEMORY_H
#define EMU8_MEMORY_H

#include <array>
#include <string>
#include <vector>

#include "common.h"

class Memory8 {
public:
  Memory8(const std::size_t);
  ~Memory8() {}

  // all fetch methods are bounds-checked, throwing a runtime
  // exception if the starting (or ending) address is illegal
  Word FetchWord(Word) const;
  Byte FetchByte(Word) const;
  void FetchSequence(Word, Word, std::vector<Byte> &) const;

  // all set methods are bounds-checked, throwing a runtime
  // exception if the starting (or ending) address is illegal
  void SetWord(Word, Word);
  void SetByte(Word, Byte);
  void SetSequence(Word, Word, const std::vector<Byte> &);

  // dump full memory image to specified file for debugging
  void CoreDump(const std::string &) const;

private:
  // the Chip-8 only has 4k total memory
  static constexpr std::size_t memSize_ = 0x1000;

  const std::size_t memLow_;
  std::array<Byte, memSize_> memory_;
};

#endif /* EMU8_MEMORY_H */
