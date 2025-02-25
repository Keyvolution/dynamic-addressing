#ifndef BIT_H
#define BIT_H

#include <Wire.h>

template <typename T>
void sendBytes(T value)
{
    for (size_t i = sizeof(T) - 1; i < sizeof(T); --i)
    {
        Wire.write((value >> (i * 8)) & 0xFF);
    }
}

template <typename T>
T receiveBytes()
{
    T value = 0;

    for (size_t i = sizeof(T) - 1; i < sizeof(T); --i)
    {
        value |= Wire.read() << (i * 8);
    }

    return value;
}

#endif
