#pragma once

#include <cstdint>
#include <memory>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

struct varint    // Completely unessasary
{
    std::shared_ptr<u8[]> data;

    varint(u64 x)
    {
        u8 size;
        if (x <= 0x7F)
            size = 1;
        else if (x <= 0x3FFF)
            size = 2;
        else if (x <= 0x1FFFFF)
            size = 3;
        else if (x <= 0xFFFFFFF)
            size = 4;
        else
            size = 5;

        data = std::make_shared<u8[]>(size);
        size = 0;
        do {
            u8 temp = (u8)(x & 0b01111111);
            x >>= 7;
            if (x != 0) temp |= 0x80;
            data[size++] = temp;

        } while (x != 0);
    }

    inline u64 operator()()
    {
        u8  numRead = 0;
        u64 result  = 0;
        u8  read;
        do {
            read       = data[numRead];
            long value = (read & 0x7F);
            result |= (value << (7 * numRead));

            numRead++;
            if (numRead > 10) std::__throw_length_error("Varint size over max");
        } while ((read & 0x80) != 0);

        return result;
    }
};
