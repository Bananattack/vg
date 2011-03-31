#ifndef VG_SCRIPT_SCRIPT_HPP
#define VG_SCRIPT_SCRIPT_HPP

extern "C"
{
	#include <lua/lua.h>
	#include <lua/lualib.h>
	#include <lua/lauxlib.h>
}

#include <unordered_map>

#include "../core/window.hpp"

namespace vg
{
    namespace script
    {
        typedef luaL_Reg FunctionTable;

        struct PropertyTable
        {
            const char* name;
            lua_CFunction getter;
            lua_CFunction setter;
        };

        class Script
        {
            private:
                static std::unordered_map<lua_State*, Script*> instances;

                lua_State* state;
                Window* window;

            public:
                static Script* getScript(lua_State* state)
                {
                    return instances[state];
                }

                Script();
                ~Script();

                lua_State* getState() const
                {
                    return state;
                }

                Window* getWindow() const
                {
                    return window;
                }

                void setWindow(Window* window)
                {
                    this->window = window;
                }
        };
    }
}

#endif