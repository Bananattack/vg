#include "global.hpp"

namespace vg
{
    namespace script
    {
        namespace
        {
            const char* const MetaName = "vg";

            const FunctionTable Functions[] = {
                //{"getWindow", getWindow},
                //{"setWindow", setWindow},
                //{"getScreen", getScreen},
                //{"setScreen", setScreen},
                {0, 0},
            };

            const PropertyTable Properties[] = {
                //{"window", getWindow, setWindow},
                //{"screen", getScreen, setScreen},
                {0, 0, 0},
            };
        }

        void bindLibrary(lua_State* state)
        {
            luaL_register(state, MetaName, Functions);
        }
    }
}