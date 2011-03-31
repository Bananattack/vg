#include <iostream>

#include "window.hpp"
#include "platform.hpp"
#include "../graphics/image.hpp"
#include "../script/script.hpp"

namespace vg
{
    bool run(std::vector<std::string> arguments)
    {
        script::Script* script = new script::Script();

        Window* window = new Window();
        window->setImage(new Image(320, 240));
        window->setVisible(true);
        while(window->isOpen())
        {
            window->refresh();
        }
        delete window->getImage();
        delete window;

        delete script;
        return true;
    }
} 