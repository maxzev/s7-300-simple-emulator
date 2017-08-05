#include "s7common.h"

#include <cstring>

namespace S7Cmd
{

    void uint16_to_5_byte_string( uint16_t value, char* out )
    {
        /*
         * This func convert 2 byte number to 5 byte string
         * if the result is less than 5 characters
         * it will be filled with '0' in the beginning.
         *
         * Ex1: uint16_t a = 120; char s [6];
         * uint16_to_5_byte_string(a, s);
         * s => |0|0|1|2|0|null-term|
         *
         * Ex2: uint16_t a = 65535; char s [6];
         * uint16_to_5_byte_string(a, s);
         * s => |6|5|5|3|5|null-term|
         */

        int counter = 0;
        int max = 5;
        uint16_t q = value;
        char digits[11] = "0123456789";
        char* p = out;

        while(counter < max)
        {
            *p = '0';
            ++p;
            ++counter;
        }

        *p = '\0';

        do
        {
            --p;
            *p = digits[q % 10];
            q = q / 10;
        }while (q);
    }



}//namespace
