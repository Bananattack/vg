#include <string>
#include <math.h>
#include <string.h>
#include "../../window.hpp"
#include "../../../graphics/image.hpp"

namespace vg
{
    const wchar_t* const Window::ClassName = L"vg_Window";

    void Window::registerClass()
    {
        static bool registered = false;
        if(!registered)
        {
            WNDCLASS windowClass;
            memset(&windowClass, 0, sizeof(windowClass));

            windowClass.style = CS_OWNDC;
            windowClass.lpfnWndProc = handleGlobalEvent;
            windowClass.hInstance = GetModuleHandle(0);
            //windowClass.hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(1));
            windowClass.hCursor = LoadCursor(0, IDC_ARROW);
            windowClass.lpszClassName = ClassName;

            RegisterClass(&windowClass);

            registered = true;
        }
    }

    LRESULT CALLBACK Window::handleGlobalEvent(HWND windowHandle, UINT messageType, WPARAM wParam, LPARAM lParam)
    {
        if(messageType == WM_CREATE)
        {
            SetWindowLongPtr(windowHandle, GWLP_USERDATA, (LONG_PTR) ((CREATESTRUCT*) lParam)->lpCreateParams);
        }
        Window* window = (Window*) GetWindowLongPtr(windowHandle, GWL_USERDATA);
        if(window)
        {
            return window->handleEvent(messageType, wParam, lParam);
        }
        else
        {
            return DefWindowProc(windowHandle, messageType, wParam, lParam);
        }
    }

    void Window::show()
    {
        open = true;
        if(!windowHandle)
        {
            registerClass();
            windowHandle = CreateWindowEx(0, ClassName, wideTitle.c_str(), WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION, 0, 0, 0, 0, 0, 0, GetModuleHandle(0), this);
        }
        if(updateResolution())
        {
            ShowWindow(windowHandle, SW_SHOWNA);
            SetWindowText(windowHandle, wideTitle.c_str());
            visible = true;
        }
        else
        {
            hide();
            open = false;
        }
    }

    void Window::hide()
    {
        if(windowHandle)
        {
            ShowWindow(windowHandle, SW_HIDE);
        }
        visible = false;
    }

    void Window::dispose()
    {
        disposeScreen();
        DestroyWindow(windowHandle);
        open = false;
        visible = false;
        windowHandle = 0;
    }

    void Window::disposeScreen()
    {
        if(frontDeviceContext)
        {
            SelectObject(backDeviceContext, backOldHandle);
            DeleteObject(backSurface);
            DeleteDC(backDeviceContext);
            ReleaseDC(windowHandle, frontDeviceContext);
        }
    }

    bool Window::updateResolution()
    {
        if(!image)
        {
            return false;
        }
        else
        {
            if(!windowRectInitialized)
            {
                windowRect.left = 0;
                windowRect.top = 0;
                windowRect.right = image->getWidth();
                windowRect.bottom = image->getHeight();
                AdjustWindowRect(&windowRect, GetWindowLong(windowHandle, GWL_STYLE), false);

                WINDOWPLACEMENT windowPlacement;
                windowPlacement.length = sizeof(windowPlacement);
                GetWindowPlacement(windowHandle, &windowPlacement);
                windowPlacement.rcNormalPosition.left = GetSystemMetrics(SM_CXSCREEN) / 2 - image->getWidth() / 2;
                windowPlacement.rcNormalPosition.top = GetSystemMetrics(SM_CYSCREEN) / 2 - image->getHeight() / 2;
                windowPlacement.rcNormalPosition.right = windowPlacement.rcNormalPosition.left + (windowRect.right - windowRect.left);
                windowPlacement.rcNormalPosition.bottom = windowPlacement.rcNormalPosition.top + (windowRect.bottom - windowRect.top);
                SetWindowPlacement(windowHandle, &windowPlacement);

                windowRectInitialized = true;
            }

            if(fullscreen)
            {
            }
            else
            {
                disposeScreen();

                const int BPP = 32;
                memset(&bitmapInfo, 0, sizeof(BITMAPINFO));
                bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bitmapInfo.bmiHeader.biWidth = image->getWidth();
                bitmapInfo.bmiHeader.biHeight = -image->getHeight(); // To get y = 0 to be top, height must be negative.
                bitmapInfo.bmiHeader.biPlanes = 1;
                bitmapInfo.bmiHeader.biSizeImage = image->getWidth() * (BPP / 8) * image->getHeight();
                bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
                bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
                bitmapInfo.bmiHeader.biClrUsed = 0;
                bitmapInfo.bmiHeader.biClrImportant = 0;
                bitmapInfo.bmiHeader.biBitCount = BPP;
                bitmapInfo.bmiHeader.biCompression = BI_RGB;

                frontDeviceContext = GetDC(windowHandle);
                backDeviceContext = CreateCompatibleDC(frontDeviceContext);
                backSurface = CreateDIBSection(frontDeviceContext, &bitmapInfo, DIB_RGB_COLORS, &backBuffer, NULL, 0);
                if(!backSurface)
                {
                    DeleteDC(backDeviceContext);
                    return false;
                }
                backOldHandle = SelectObject(backDeviceContext, backSurface);

                long windowStyle = GetWindowLong(windowHandle, GWL_STYLE);
                windowStyle &= ~WS_POPUP;
                windowStyle |= WS_OVERLAPPEDWINDOW | WS_THICKFRAME;
                SetWindowLong(windowHandle, GWL_STYLE, windowStyle);
            }
            return true;
        }
    }

    Window::Window():
        windowHandle(0),
        frontDeviceContext(0),
        backDeviceContext(0),
        backOldHandle(0),
        backSurface(0),
        backBuffer(0),
        windowRectInitialized(false),
        image(0),
        visible(false),
        open(false),
        focused(false),
        fullscreen(false),
        mouseContained(false)
    {
        setTitle(AbstractWindow::DefaultTitle);
    }

    Window::~Window()
    {
        dispose();
    }

    LRESULT CALLBACK Window::handleEvent(UINT messageType, WPARAM wParam, LPARAM lParam)
    {
        switch(messageType)
        {
            case WM_SIZING:
            {
                if(fullscreen)
                {
                    return false;
                }
                const int SnapBoundary = 64;
                RECT* dragRect = (RECT*) lParam;
                RECT clientRect;
                RECT windowRect;
                GetClientRect(windowHandle, &clientRect);
                GetWindowRect(windowHandle, &windowRect);
                int frameWidth = (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left);
                int frameHeight = (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top);

                int xres = image->getWidth();
                int yres = image->getHeight();

                int width = dragRect->right - dragRect->left - frameWidth;
                int height = dragRect->bottom - dragRect->top - frameHeight;
                int widthRatio = xres ? (width + SnapBoundary) / xres : 0;
                int heightRatio = yres ? (height + SnapBoundary) / yres : 0;
                if(widthRatio == 0 || heightRatio == 0)
                {
                    return 0;
                }

                if(abs(width - xres * widthRatio) <= SnapBoundary)
                {
                    if(wParam==WMSZ_LEFT || wParam==WMSZ_TOPLEFT || wParam==WMSZ_BOTTOMLEFT)
                    {
                        dragRect->left = dragRect->right - widthRatio * xres - frameWidth;
                    }
                    else
                    {
                        dragRect->right = dragRect->left + widthRatio * xres + frameWidth;
                    }
                }
                if(abs(height - yres * widthRatio) <= SnapBoundary)
                {
                    if(wParam == WMSZ_TOP || wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOPRIGHT)
                    {
                        dragRect->top = dragRect->bottom - heightRatio * yres - frameHeight;
                    }
                    else
                    {
                        dragRect->bottom = dragRect->top + heightRatio * yres + frameHeight;
                    }
                }

                return true;
            }
            case WM_CLOSE:
                dispose();
                break;
            case WM_SYSCOMMAND:
                if (wParam == SC_CLOSE)
                {
                    dispose();
                }
                break;
            case WM_ACTIVATE:
                if(LOWORD(wParam) == WA_INACTIVE)
                {
                    focused = false;
                }
                else
                {
                    focused = true;
                }
                break;
            case WM_ACTIVATEAPP:
                focused = !wParam;
                break;
            case WM_KEYDOWN:
                return 0;
            case WM_KEYUP:
                return 0;
            case WM_MOUSEMOVE:
                if(!mouseContained)
                {
                    mouseContained = true;
                    ShowCursor(false);

                    TRACKMOUSEEVENT trackRequest;
                    memset(&trackRequest, 0, sizeof(trackRequest));
                    trackRequest.cbSize = sizeof(trackRequest);
                    trackRequest.dwFlags = TME_LEAVE;
                    trackRequest.hwndTrack = windowHandle;
                    TrackMouseEvent(&trackRequest);
                }
                break;
            case WM_MOUSELEAVE:
                mouseContained = false;
                ShowCursor(true);
                break;
            case WM_LBUTTONDOWN:
                break;
            case WM_LBUTTONUP:
                break;
            case WM_RBUTTONDOWN:
                break;
            case WM_RBUTTONUP:
                break;
            case WM_MBUTTONDOWN:
                break;
            case WM_MBUTTONUP:
                break;
            case WM_MOUSEWHEEL:
                break;
        }
        return DefWindowProc(windowHandle, messageType, wParam, lParam);
    }

    bool Window::isVisible() const
    {
        return this->visible;
    }
        
    void Window::setVisible(bool visible)
    {
        if(visible && !this->visible)
        {
            show();
        }
        else if(!visible && this->visible)
        {
            hide();
        }
    }

    bool Window::isOpen() const
    {
        return open;
    }

    bool Window::hasFocus() const
    {
        return focused;
    }

    void Window::refresh()
    {
        if(windowHandle)
        {
            MSG msg;
            while(PeekMessageW(&msg, windowHandle, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            if(visible)
            {
                RECT rect;
                GetClientRect(windowHandle, &rect);

                int width = rect.right - rect.left;
                int height = rect.bottom - rect.top;
                int internalWidth = width;
                int internalHeight = height;

                // Factor is used for aspect-aware integer scaling.
                int factor = width / image->getWidth();
                if(factor > height / image->getHeight())
                {
                    factor = height / image->getHeight();
                }

                // Fits on screen, multiply by scale factor.
                if(factor >= 1)
                {
                    internalWidth = image->getWidth() * factor;
                    internalHeight = image->getHeight() * factor;
                }
                // Can't fit the entire screen! Fallback on lossy downscaling.
                else
                {
                    float ratio = (float) width / (float) image->getWidth();
                    float proportionalHeight = ratio * (float) image->getHeight();
                    if((int) proportionalHeight > height)
                    {
                        ratio = (float) height / (float) image->getHeight();
                        internalHeight = height;
                        internalWidth = (int)(ratio * (float) image->getWidth());
                    }
                    else
                    {
                        internalHeight = (int) proportionalHeight;
                        internalWidth = width;
                    }
                }

                rect.left += (width - internalWidth) / 2;
                rect.top += (height - internalHeight) / 2;
                rect.right = rect.left + internalWidth;
                rect.bottom = rect.top + internalHeight;

                ClientToScreen(windowHandle, (POINT*) &rect);
                ClientToScreen(windowHandle, ((POINT*) &rect) + 1);

                memcpy(backBuffer, image->getRawData(), image->getWidth() * image->getHeight() * 4);
                StretchBlt(
                    frontDeviceContext,
                    (width - internalWidth) / 2,
                    (height - internalHeight) / 2,
                    internalWidth,
                    internalHeight,
                    backDeviceContext,
                    0,
                    0,
                    image->getWidth(),
                    image->getHeight(),
                    SRCCOPY
                );


                // Draw black letterbox bars to occupy unused window space.
                if(internalWidth != width || internalHeight != height)
                {
                    BitBlt(frontDeviceContext, 0, 0, (width - internalWidth) / 2, height, NULL, 0, 0, BLACKNESS);
                    BitBlt(frontDeviceContext, (width - internalWidth) / 2 + internalWidth, 0, (width - internalWidth) / 2, height, NULL, 0, 0, BLACKNESS);
                    BitBlt(frontDeviceContext, 0, 0, width, (height - internalHeight) / 2, NULL, 0, 0, BLACKNESS);
                    BitBlt(frontDeviceContext, 0, (height - internalHeight) / 2 + internalHeight, width, (height - internalHeight) / 2, NULL, 0, 0, BLACKNESS);
                }
            }
        }
    }

    Image* Window::getImage() const
    {
        return image;
    }

    void Window::setImage(Image* image)
    {
        this->image = image;
        if(visible && !updateResolution())
        {
            hide();
        }
    }

    bool Window::isFullscreen() const
    {
        return fullscreen;
    }

    void Window::setFullscreen(bool fullscreen)
    {
        if(fullscreen && !this->fullscreen || !fullscreen && this->fullscreen)
        {
            this->fullscreen = fullscreen;
            if(visible && !updateResolution())
            {
                hide();
            }
        }
    }

    std::string Window::getTitle() const
    {
        return title;
    }

    void Window::setTitle(std::string title)
    {
        std::wstring result(title.length(), L' ');
        std::copy(title.begin(), title.end(), result.begin());

        this->title = title;
        wideTitle = result;
        if(visible)
        {
            SetWindowText(windowHandle, wideTitle.c_str());
        }
    }
}