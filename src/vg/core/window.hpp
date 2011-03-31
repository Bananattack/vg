#ifndef VG_CORE_WINDOW_HPP
#define VG_CORE_WINDOW_HPP

#include <string>
#include "platform.hpp"

namespace vg
{
    class Image;

    // This describes the abstract behaviour of a window.
    // See the Window class under an an OS implementation for the version
    // of an AbstractWindow actually used for the platform.
    class AbstractWindow
    {
        public:
            static const char* const DefaultTitle;

            virtual ~AbstractWindow()
            {
            }

            virtual bool isVisible() const = 0;
            virtual void setVisible(bool visible) = 0;
            virtual bool isOpen() const = 0;
            virtual bool hasFocus() const = 0;
            virtual void refresh() = 0;
            virtual Image* getImage() const = 0;
            virtual void setImage(Image* image) = 0;
            virtual bool isFullscreen() const = 0;
            virtual void setFullscreen(bool fullscreen) = 0;
            virtual std::string getTitle() const = 0;
            virtual void setTitle(std::string title) = 0;
    };
}

#ifdef VG_WIN32
#include "os/windows/window.hpp"
#endif

#endif