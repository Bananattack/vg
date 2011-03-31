#ifndef VG_GRAPHICS_COLOR_HPP
#define VG_GRAPHICS_COLOR_HPP

#include <string.h>
#include <stdlib.h>

namespace vg
{
    enum ColorChannelIndex
    {
        RedChannel = 2,
        GreenChannel = 1,
        BlueChannel = 0,
        AlphaChannel = 3
    };
    typedef unsigned char ColorChannel;

    // Raw unsigned Colors assume a 32-bit BGRA format
    enum
    {
        ColorWhite = 0xFFFFFFFF,
        ColorRed = 0xFFFF0000,
        ColorGreen = 0xFF00FF00,
        ColorBlue = 0xFF0000FF,
        ColorMagenta = 0xFFFF00FF,
        ColorCyan = 0xFF00FFFF,
        ColorYellow = 0xFFFFFF00,
        ColorBlack = 0xFF000000
    };

    namespace
    {
        inline bool isHexChar(char c)
        {
            return c >= '0' && c <= '9'
                || c >= 'A' && c <= 'Z'
                || c >= 'a' && c <= 'z';
        }

        inline bool isHexString(const char* str)
        {
            for(size_t i = 0; i < strlen(str); i++)
            {
                if(!isHexChar(str[i]))
                {
                    return false;
                }
            }
            return true;
        }
    }

    struct Color
    {
        unsigned int value;

        static bool isParseable(const char* str)
        {
            if(str[0] != '#')
            {
                return false;
            }
            if(!isHexString(str + 1))
            {
                return false;
            }
            switch(strlen(str))
            {
                case 4:
                case 5:
                case 7:
                case 9:
                    return true;
                default:
                    return false;
            }
        }

        operator unsigned int() const
        {
            return value;
        }

        bool operator!=(Color& rhs) const
        {
            return value != rhs.value;
        }

        bool operator==(Color& rhs) const
        {
            return value == rhs.value;
        }

        ColorChannel& operator[](ColorChannelIndex i) const
        {
            return ((ColorChannel*) &value)[i];
        }

        Color():
            value(0)
        {
        }

        Color(unsigned int value):
            value(value)
        {
        }

        Color(int r, int g, int b):
            value((ColorChannel(r) << 16) | (ColorChannel(g) << 8) | ColorChannel(b) | 0xFF000000)
        {
        }

        Color(int r, int g, int b, int a):
            value((ColorChannel(r) << 16) | (ColorChannel(g) << 8) | ColorChannel(b) | (ColorChannel(a) << 24))
        {
        }

        Color(const char* str):
            value(ColorBlack)
        {
            if(str[0] != '#')
            {
                return;
            }
            if(!isHexString(str + 1))
            {
                return;
            }
            unsigned int v = strtoul(str + 1, 0, 16);
            switch(strlen(str))
            {
                case 4:
                    (*this)[RedChannel] = (((v >> 8) & 0xF) << 4) | ((v >> 8) & 0xF);
                    (*this)[GreenChannel] = (((v >> 4) & 0xF) << 4) | ((v >> 4) & 0xF);
                    (*this)[BlueChannel] = ((v & 0xF) << 4) | v & 0xF;
                    (*this)[AlphaChannel] = 0xFF;
                    break;
                case 5:
                    (*this)[RedChannel] = (((v >> 8) & 0xF) << 4) | ((v >> 8) & 0xF);
                    (*this)[GreenChannel] = (((v >> 4) & 0xF) << 4) | ((v >> 4) & 0xF);
                    (*this)[BlueChannel] = ((v & 0xF) << 4) | v & 0xF;
                    (*this)[AlphaChannel] = (((v >> 12) & 0xF) << 4) | ((v >> 12) & 0xF);
                    break;
                case 7:
                    (*this)[RedChannel] = (v >> 16) & 0xFF;
                    (*this)[GreenChannel] = (v >> 8) & 0xFF;
                    (*this)[BlueChannel] = v & 0xFF;
                    (*this)[AlphaChannel] = 0xFF;
                    break;
                case 9:
                    (*this)[RedChannel] = (v >> 16) & 0xFF;
                    (*this)[GreenChannel] = (v >> 8) & 0xFF;
                    (*this)[BlueChannel] = v & 0xFF;
                    (*this)[AlphaChannel] = (v >> 24) & 0xFF;
                    break;
                default:
                    break;
            }
        }
    };
}

#endif