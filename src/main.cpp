#ifndef UNICODE
#define UNICODE
#endif

#include <iostream>
#include <cstdio>
#include <vector>
#include <windows.h>
#include "resource.h"
#include "VideoCapController.h"

/**W I N D O W   P R O C***************************************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022211
 * Description: win32 massage loop
 * Parameters: 
   - 
 * Return: 
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
LRESULT CALLBACK WindowProc(HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    VideoCapController& controller = VideoCapController::getInstance();
    int xPos, yPos;
    switch (uMsg)
    {
    case WM_CTLCOLORSTATIC:
        return (LRESULT)COLOR_BTNFACE;
    case WM_MOUSEMOVE:
        xPos = LOWORD(lParam);
        yPos = HIWORD(lParam);
        break;
    case WM_LBUTTONDOWN:
        std::cout << "WM_LBUTTONDOWN" << std::endl;
        xPos = LOWORD(lParam);
        yPos = HIWORD(lParam);
        std::cout << xPos << ',' << yPos << std::endl;
        break;
    //case WM_HSCROLL:
        //std::cout << "WM_HSCROLL" << std::endl;
        //return 0;
    case WM_GRAPHNOTIFY:
        std::cout << "WM_GRAPHNOTIFY" << std::endl;
        controller.onGraphEvent();
        return 0;
    case WM_DESTROY:
        std::cout << "WM_DESTROY" << std::endl;
        controller.uninitialize();
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        std::cout << "WM_PAINT" << std::endl;
        controller.onPaint();
        return 0;
    case WM_SIZE:
        std::cout << "WM_SIZE" << std::endl;
        controller.onSize();
        return 0;
    case WM_DISPLAYCHANGE:
        std::cout << "WM_DISPLAYCHANGE" << std::endl;
        return 0;
    case WM_COMMAND:
        int lowWP = LOWORD(wParam);
        int highWP = HIWORD(wParam);
        HWND hCtrl = reinterpret_cast<HWND>(lParam);
        //std::cout<<lowWP<<','<<highWP<<','<<std::hex<<(unsigned int)hCtrl<<std::endl;
        switch (lowWP)
        {
            case IDC_SNAP_BUTTON:
                std::cout << "WM_COMMAND:IDC_SNAP_BUTTON" << std::endl;
                controller.onSnap();
                return 0;
            case IDC_CAP_BUTTON:
                std::cout << "WM_COMMAND:IDC_CAP_BUTTON" << std::endl;
                controller.onCapture();
                return 0;
            case IDC_DEVICE_COMBO:
                if (highWP == CBN_SELCHANGE)
                {
                    std::cout << "WM_COMMAND:IDC_DEVICE_COMBO" << std::endl;
                    controller.onDeviceChanged();
                }
                return 0;
            case IDC_FORMAT_COMBO:
                if (highWP == CBN_SELCHANGE)
                {
                    std::cout << "WM_COMMAND:IDC_FORMAT_COMBO" << std::endl;
                    controller.onFormatChanged();
                }
                return 0;
            case IDC_RESOLUTION_COMBO:
                if (highWP == CBN_SELCHANGE)
                {
                    std::cout << "WM_COMMAND:IDC_RESOLUTION_COMBO" << std::endl;
                    controller.onResolutionChanged();
                }
                return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/**W   W I N   M A I N*****************************************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022211
 * Description: win32 entry point (unicode)
 * Parameters: 
   - 
 * Return: 
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
int WINAPI wWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       PWSTR pCmdLine,
                       int nCmdShow)
{
    wchar_t* pDeviceName = NULL;
    int deviceIndex = 0;
    int formatIndex = 0;
    int resolutionIndex = 0;

    VideoCapController& controller = VideoCapController::getInstance();
    controller.initialize(hInstance);
    controller.showWindow();
    controller.setupVideoDevice();

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
