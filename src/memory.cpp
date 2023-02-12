#include <algorithm>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>

#include "bits.h"
#include "memory.h"

static void ReportInvalidAccess(Word addr)
{
  std::stringstream errStream;
  errStream << "Invalid memory access: ";
  errStream << std::hex << addr;
  throw std::runtime_error(errStream.str());
}

Memory8::Memory8(const std::size_t memBase) : memLow_(memBase), memory_() {
  memory_.fill(0x0);
}

Word Memory8::FetchWord(Word addr) const
{
  if (addr < memLow_ || addr >= memSize_ - 1) {
    ReportInvalidAccess(addr);
  }

  const auto msb = memory_.at(addr);
  const auto lsb = memory_.at(addr + 1);

  return bits8::fuseBytes(msb, lsb);
}

Byte Memory8::FetchByte(Word addr) const
{
  if (addr < memLow_ || addr >= memSize_) {
    ReportInvalidAccess(addr);
  }

  return memory_.at(addr);
}

void Memory8::FetchSequence(Word addr, Word size,
                            std::vector<Byte> &buf) const {
  if (addr < memLow_) {
    ReportInvalidAccess(addr);
  } else if (addr + size > memSize_) {
    ReportInvalidAccess(addr + size);
  }

  buf.reserve(size);
  std::copy(memory_.begin() + addr, memory_.begin() + addr + size,
            std::back_inserter(buf));
}

void Memory8::SetWord(Word addr, Word val)
{
  if (addr < memLow_ || addr >= memSize_ - 1) {
    ReportInvalidAccess(addr);
  }

  auto [msb, lsb] = bits8::splitWord(val);

  memory_[addr] = msb;
  memory_[addr + 1] = lsb;
}

void Memory8::SetByte(Word addr, Byte val)
{
  if (addr < memLow_ || addr >= memSize_) {
    ReportInvalidAccess(addr);
  }

  memory_[addr] = val;
}

void Memory8::SetSequence(Word addr, Word size, const std::vector<Byte> &buf) {
  if (addr < memLow_) {
    ReportInvalidAccess(addr);
  } else if (addr + size > memSize_) {
    ReportInvalidAccess(addr + size);
  }

  std::copy(buf.begin(), buf.end(), memory_.begin() + addr);
}

void Memory8::CoreDump(const std::string& coreFile) const
{
  std::fstream coreStream(coreFile, std::ios::binary | std::ios::out);

  if (!coreStream.good()) {
    throw std::runtime_error("Could not open corefile: " + coreFile);
  }

  // note that this will write out actual byte values rather than
  // ASCII integers, but it will default to the native system
  // endian-ness, and may require translation to Chip-8 format
  std::copy(memory_.begin(), memory_.end(),
            std::ostream_iterator<Byte>(coreStream));

  coreStream.close();
}
