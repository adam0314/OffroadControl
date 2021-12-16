#include "helpers.hpp"

namespace helpers
{
    void printHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes and a newline
    {
        char tmp[16];
        for (int i = 0; i < length; i++)
        {
            sprintf(tmp, "%.2X", data[i]);
            Serial.print(tmp);
            Serial.print(" ");
        }
        Serial.println();
    }

    void printHex8(uint8_t data) // prints 8-bit data in hex with leading zeroes
    {
        char tmp[16];
        sprintf(tmp, "%.2X", data);
        Serial.print(tmp);
        Serial.print(" ");
    }
}
