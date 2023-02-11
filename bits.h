#ifndef EMU8_BITS_H
#define EMU8_BITS_H

#include <utility>

#include "common.h"

using BytePair = std::pair<Byte, Byte>;

namespace bits8 {

/* 
 * combine 8-bit msb and 8-bit lsb into single 16-bit word 
 * as (msb,lsb)
 */
constexpr Word fuseBytes(Byte msb, Byte lsb)
{

}

/*
 * perform inverse of fuseBytes(), splitting a single 16-bit word
 * into its most-significant and least-significant bytes, returning
 * them as a pair in the order (msb, lsb)
 */
constexpr BytePair splitWord(Word word)
{

}

} /* bits8 */

#endif /* EMU8_BITS_H */
