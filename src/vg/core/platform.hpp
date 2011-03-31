#ifndef VG_CORE_PLATFORM_HPP
#define VG_CORE_PLATFORM_HPP

#include <string>
#include <vector>

namespace vg
{
    bool run(std::vector<std::string> arguments);
}

#ifdef VG_WIN32
#include "os/windows/platform.hpp"
#endif

#endif
