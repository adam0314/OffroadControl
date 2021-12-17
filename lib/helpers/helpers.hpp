#ifndef helpers_hpp
#define helpers_hpp
#include <arduino.h>

namespace helpers
{
    void printHex8(uint8_t*, uint8_t, bool = false);
    void printHex8(uint8_t);
} // namespace helpers


#endif