#ifndef VG_CORE_OS_WINDOWS_WINDOW_HPP
#define VG_CORE_OS_WINDOWS_WINDOW_HPP

#include "platform.hpp"

namespace vg
{
    class Image;
    class AbstractWindow;
    class Window : public AbstractWindow
    {
        private:
            static const wchar_t* const ClassName;
            static void registerClass();
            static LRESULT CALLBACK handleGlobalEvent(HWND windowHandle, UINT messageType, WPARAM wParam, LPARAM lParam);

            HWND windowHandle;
            BITMAPINFO bitmapInfo;
            HDC frontDeviceContext;
            HDC backDeviceContext;
            HANDLE backOldHandle;
            HBITMAP backSurface;
            void* backBuffer;

            bool windowRectInitialized;
            RECT windowRect;

            Image* image;
            bool visible;
            bool open;
            bool focused;
            bool fullscreen;
            bool mouseContained;

            std::string title;
            std::wstring wideTitle;

            void show();
            void hide();
            void dispose();
            void disposeScreen();
            bool updateResolution();
        public:
            Window();
            ~Window();

            LRESULT CALLBACK handleEvent(UINT messageType, WPARAM wParam, LPARAM lParam);

            // Implementation of AbstractWindow
            bool isVisible() const;
            void setVisible(bool visible);
            bool isOpen() const;
            bool hasFocus() const;
            void refresh();
            Image* getImage() const;
            void setImage(Image* image);
            bool isFullscreen() const;
            void setFullscreen(bool fullscreen);
            std::string getTitle() const;
            void setTitle(std::string title);
    };
}

#endif
