#include <string>
#include <vector>
#include <io.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>

#include "../../platform.hpp"

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showWindowSettings)
{
    const bool ShowConsole = true;
    if(ShowConsole)
    {
        AllocConsole();
        FILE* f = _fdopen(_open_osfhandle((intptr_t) GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT), "w");
        *stdout = *f;
        setvbuf(stdout, NULL, _IONBF, 0);
    }

    std::vector<std::string> arguments;
    assert(GetModuleHandle(0) == instance);
	return vg::run(arguments) ? EXIT_SUCCESS : EXIT_FAILURE;
}


