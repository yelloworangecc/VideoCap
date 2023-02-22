#ifndef UNICODE
#define UNICODE
#endif

#include <sstream>
#include <cstdio>
#include <vector>
#include <windows.h>
#include <CommCtrl.h>
#include <atlconv.h>
#include "resource.h"
#include "VideoCapController.h"
#include "Shortcut.h"
#include "Console.h"
#include "Uxtheme.h"
#include "dbt.h"
#include "Shapes.h"

CONSOLE_INIT();

LRESULT CALLBACK MainProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    VideoCapController& controller = VideoCapController::getInstance();
    
    int lowWP = LOWORD(wParam);
    int highWP = HIWORD(wParam);
    int xPos = GET_X_LPARAM(lParam);
    int yPos = GET_Y_LPARAM(lParam);
    int trackPos;
    const RECT& videoRect = controller.getVideoRect();
    int streamWidth = controller.getStreamWidth();
    int streamHeight = controller.getStreamHeight();
    ShapeCollection& shapes = ShapeCollection::get();
    static int cursorDisplayCount = 0;
    static bool bTrackMouse = false;
    static bool bDrawMode = false;
    static Trajectory* pTrajectory = nullptr;

    // Used with WM_CTLCOLORSTATIC
    //HDC hdc;
    //static HBRUSH hBrush = GetSysColorBrush(COLOR_BTNFACE);

    std::wstringstream wss;
    USES_CONVERSION;

    switch (uMsg)
    {
    // No need, let theme system draw these
    //case WM_CTLCOLORSTATIC:
    //    hdc = (HDC) wParam;
    //    SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
    //    return (LRESULT)hBrush;
    case WM_HOTKEY:
        CONSOLE<<"WM_HOTKEY"<<ENDL;
        controller.showMainWindow();
        break;
    case WM_KEYDOWN:
        if (wParam == VK_CONTROL)
        {
            if (!bDrawMode)
            {
                CONSOLE<<"enter draw mode"<<ENDL;
                shapes.setCrossHairColor(RED);
                bDrawMode = true;
                pTrajectory = new Trajectory(streamWidth,streamHeight);
                shapes.append(pTrajectory);
            }
        }
        break;
    case WM_KEYUP:
        if (wParam == VK_CONTROL)
        {
            if (bDrawMode)
            {
                CONSOLE<<"leave draw mode"<<ENDL;
                shapes.setCrossHairColor(GREEN);
                bDrawMode = false;
                pTrajectory = nullptr;
            }
        }
        break;
    case WM_CHAR:
        CONSOLE<<"Key '"<<char(wParam)<<"' was pressed"<<ENDL;
        switch(wParam)
        {
            case 'h':
                controller.onHelp();
                break;
            case 'o':
                controller.onOpen();
                break;
            case 's':
                controller.onSnapButton();
                break;
            case 'r':
                controller.onRecordButton();
                break;
            case ' ':
                controller.onPause();
            case char(0x08)://backspace
                shapes.back();
                break;
            case char(0x1B)://escape
                shapes.reset();
                break;
        }
        break;
    case WM_MOUSEMOVE:
        //hide cursor
        if (yPos >= 0 && yPos <= videoRect.bottom &&
            cursorDisplayCount >= 0)
        {
            cursorDisplayCount--;
            ShowCursor(FALSE);
        }
        if (yPos >= 0 && yPos <= videoRect.bottom)
        {
            double widthScaleFactor = controller.getWidthScaleFactor();
            double heightScaleFactor = controller.getHeightScaleFactor();
            int adjustX = int(xPos/widthScaleFactor);
            int adjustY = int(yPos/heightScaleFactor);
            shapes.resetCrossHair(streamWidth, streamHeight, adjustX, adjustY);
            if (bDrawMode && pTrajectory) pTrajectory->append(adjustX, adjustY);
        }
        //show cursor
        if ((yPos < 0 || yPos > videoRect.bottom) &&
            cursorDisplayCount < 0)
        {
            cursorDisplayCount++;
            ShowCursor(TRUE);
        }
        if (!bTrackMouse)
        {
            bTrackMouse = true;
            TRACKMOUSEEVENT trackEvent;
            trackEvent.cbSize = sizeof(trackEvent);
            trackEvent.dwFlags = TME_LEAVE;
            trackEvent.hwndTrack = controller.getMainWindowHandle();
            trackEvent.dwHoverTime = HOVER_DEFAULT;
            TrackMouseEvent(&trackEvent);
        }
        break;
    case WM_MOUSELEAVE:
        CONSOLE << "WM_MOUSELEAVE" << ENDL;
        bTrackMouse = false;
        //show cursor (espectially the titile bar need this)
        cursorDisplayCount++;
        ShowCursor(TRUE);
        break;
    case WM_LBUTTONDOWN:
        if (bDrawMode) break; //skip when drawing
        CONSOLE << "WM_LBUTTONDOWN" << ENDL;
        CONSOLE << xPos << ',' << yPos << ENDL;
        CONSOLE <<videoRect.top<<','<<videoRect.right<<','<<videoRect.bottom<<','<<videoRect.left<<ENDL;
        SetFocus(hwnd);
        if ((xPos < videoRect.left + 20 || xPos > videoRect.right - 20) && (yPos < videoRect.top + 20 || yPos > videoRect.bottom - 20)) controller.flipVideo(FLIP_BOTH);
        else if (xPos < videoRect.left + 20 || xPos > videoRect.right - 20) controller.flipVideo(FLIP_VERTICAL);
        else if ((yPos < videoRect.top + 20 || yPos > videoRect.bottom - 20) && yPos <= videoRect.bottom) controller.flipVideo(FLIP_HORIZONTAL);
        else {}
        break;
    case WM_GRAPHNOTIFY:
        controller.onGraphEvent();
        return 0;
    case WM_DESTROY:
        controller.uninitialize();
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        controller.onPaint();
        return 0;
    case WM_SIZE:
        controller.onSize();
        return 0;
    //handle event from sub ctrls
    case WM_COMMAND:
        CONSOLE<<"WM_COMMAND "<<lowWP<<','<<highWP<<','<<(long long)lParam<<ENDL;
        switch (lowWP)
        {
            case IDC_CONFIG_BUTTON:
                controller.onConfigButton();
                return 0;
            case IDC_SNAP_BUTTON:
                controller.onSnapButton();
                return 0;
            case IDC_CAP_BUTTON:
                controller.onRecordButton();
                return 0;
            case IDC_DEVICE_COMBO:
                if (highWP == CBN_SELCHANGE) 
                {
                    controller.onDeviceChanged();
                    shapes.resetAll();
                }
                return 0;
            
        }
        return 0;
    //handle system command
    case WM_SYSCOMMAND:
        CONSOLE<<"WM_SYSCOMMAND "<<(long long)wParam<<','<<xPos<<','<<yPos<<ENDL;
        switch(wParam)
        {
            case SC_MINIMIZE:
                break;
            case SC_MAXIMIZE:
            case SC_MAXIMIZE+2:
                controller.onMaximize();
                break;
            case SC_RESTORE:
            case SC_RESTORE+2:
                controller.onRestore();
                break;
        }
        break;
    //handle usb plug/unplug
    case WM_DEVICECHANGE:
        switch (wParam)
        {
            CONSOLE << "WM_DEVICECHANGE:" << ENDL;
            case DBT_DEVICEARRIVAL:
            case DBT_DEVICEREMOVECOMPLETE:
                controller.reloadDeviceList();
                shapes.resetAll();
                break;

            default:
                // Ignore
                break;
        }
        break;
    //handle trackbar event
    case WM_HSCROLL:
        CONSOLE << "WM_HSCROLL" << ENDL;
        CONSOLE << lowWP << ',' << highWP << ENDL;
        long windowID = GetWindowLong((HWND)lParam, GWL_ID);
        switch (lowWP) {
            case TB_LINEUP:
            case TB_LINEDOWN:
            case TB_PAGEUP:
            case TB_PAGEDOWN:
            case TB_TOP:
            case TB_BOTTOM:
                trackPos = (int)SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
                if (windowID == IDC_ROTATION_TRACK) controller.onRotationChanged(trackPos);
                if (windowID == IDC_GRID_INTERVAL_TRACK) controller.onGridIntervalChanged(trackPos);
                break;
            case TB_THUMBTRACK:
            case TB_THUMBPOSITION:
                if (windowID == IDC_ROTATION_TRACK) controller.onRotationChanged(highWP);
                if (windowID == IDC_GRID_INTERVAL_TRACK) controller.onGridIntervalChanged(highWP);
                break;
        }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


LRESULT CALLBACK ConfigProc(HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    VideoCapController& controller = VideoCapController::getInstance();
    ShapeCollection& shapes = ShapeCollection::get();
    int lowWP = LOWORD(wParam);
    int highWP = HIWORD(wParam);
    switch (uMsg)
    {   
        //case WM_CTLCOLORSTATIC:
        //    return (LRESULT)COLOR_BTNFACE;
        case WM_CLOSE:
            controller.onConfigClose();
            return 0;
        case WM_COMMAND:
            switch(lowWP)
            {
                // Change Device Format
                case IDC_FORMAT_COMBO:
                    if (highWP == CBN_SELCHANGE)
                    {
                        CONSOLE << "WM_COMMAND:IDC_FORMAT_COMBO" << ENDL;
                        controller.onFormatChanged();
                        shapes.resetAll();
                    }
                    return 0;
                
                case IDC_RESOLUTION_COMBO:
                    if (highWP == CBN_SELCHANGE)
                    {
                        CONSOLE << "WM_COMMAND:IDC_RESOLUTION_COMBO" << ENDL;
                        controller.onResolutionChanged();
                        shapes.resetAll();
                    }
                    return 0;
                case IDC_LANGUAGE_COMBO:
                    if (highWP == CBN_SELCHANGE)
                    {
                        CONSOLE << "WM_COMMAND:IDC_LANGUAGE_COMBO" << ENDL;
                        controller.onLanguageChanged();
                    }
                    return 0;
                case IDC_CHOOSE_PATH_BUTTON:
                    CONSOLE << "WM_COMMAND:IDC_CHOOSE_PATH_BUTTON" << ENDL;
                    controller.onChoosePath();
                    return 0;
            }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       PWSTR pCmdLine,
                       int nCmdShow)
{
    wchar_t* pDeviceName = NULL;
    int deviceIndex = 0;
    int formatIndex = 0;
    int resolutionIndex = 0;

    wchar_t** argv;
    int argc;
    argv = CommandLineToArgvW(pCmdLine, &argc);
    //enable console print
    if (0 == wcscmp(argv[0], L"Console"))
    {
        CONSOLE.attach();
    }
    //create shortcut
    if (0 == wcscmp(argv[0], L"Shortcut"))
    {
        CONSOLE.attach();
        Shortcut sc;
        wchar_t buf[256] = { 0 };
        int len = GetCurrentDirectory(256, buf);
        if (len > 0 &&len < 255)
        {
            buf[len] = L'\\';
            buf[len+1]=L'\0';
        }
        sc.create(buf,L"VideoCap.exe",buf);
        return 0;
    }
    if (argc >= 1 && 0 == wcscmp(argv[0], L"-DM"))
    {
        // Called from DM, use old style buttons, big font
        VideoCapWin::useWindowsTheme = false;
    }
    INITCOMMONCONTROLSEX init;
    init.dwICC = ICC_BAR_CLASSES | ICC_STANDARD_CLASSES | ICC_USEREX_CLASSES;
    init.dwSize = sizeof(init);
    InitCommonControlsEx(&init);
    
    VideoCapController& controller = VideoCapController::getInstance();
    controller.initialize(hInstance);
    controller.showMainWindow();
    controller.setupVideoDevice();

    EnableTheming(FALSE);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
