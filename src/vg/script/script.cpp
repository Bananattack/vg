#include "global.hpp"
#include "script.hpp"

namespace vg
{
    namespace script
    {
        std::unordered_map<lua_State*, Script*> Script::instances;

        Script::Script():
            window(0)
        {
            state = luaL_newstate();
            instances.insert(std::make_pair(state, this));
            luaL_openlibs(state);
            script::bindLibrary(state);
        }

        Script::~Script()
        {
            if(state && instances.find(state) != instances.end())
            {
                instances.erase(state);
                lua_close(state);
            }
        }
    }
}