#ifndef VG_CORE_COMMON_H
#define VG_CORE_COMMON_H

namespace vg
{
    inline int minimum(int a, int b)
    {
        return a < b ? a : b;
    }

    inline int maximum(int a, int b)
    {
        return a > b ? a : b;
    }
}

#endif