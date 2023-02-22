#ifndef UNICODE
#define UNICODE
#endif
#include <iostream>
#include <Windows.h>
#include <Windowsx.h>
#include <CommCtrl.h>
#include <Shlobj_core.h>
#include "resource.h"
#include "Console.h"
#include "VideoCapWin.h"
#include "VideoCapController.h"
#include "Uxtheme.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='AMD64' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define WINDOW_CLASS_MAIN   L"Main"
#define WINDOW_CLASS_CONFIG L"Config"

LRESULT CALLBACK MainProc(HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam);

LRESULT CALLBACK ConfigProc(HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam);

bool VideoCapWin::useWindowsTheme = true;

BOOL CALLBACK SetChildThemeFont(HWND hwndChild, LPARAM lParam)
{
    HFONT hFont = (HFONT)lParam;
    SendMessage(hwndChild, WM_SETFONT, (WPARAM)hFont, TRUE);
    if (!VideoCapWin::useWindowsTheme)
    {
       // For classic theme with 3d buttons
       SetWindowTheme(hwndChild, L" ", L" ");
    }
    return TRUE;
}

void VideoCapWin::create(HINSTANCE hApp)
{
    this->hApp = hApp;
    const wchar_t CLASS_NAME[]  = WINDOW_CLASS_MAIN;

    WNDCLASSEX wc = { };
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc   = MainProc;
    wc.hInstance     = hApp;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(hApp, L"ID_ICON_APP");
    wc.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);

    RegisterClassEx(&wc);

    hBmpImage = LoadBitmap(hApp, L"ID_BMP_IMAGE");
    hBmpVideo = LoadBitmap(hApp, L"ID_BMP_VIDEO");
    hBmpVideoOff = LoadBitmap(hApp, L"ID_BMP_VIDEO_OFF");
    hBmpConfig = LoadBitmap(hApp, L"ID_BMP_CONFIG");

    hFont = CreateFont (useWindowsTheme?14:18, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
          OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
          DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));

    //Main window
    hWin = CreateWindowEx(
        0, // Optional window styles.
        CLASS_NAME, // Window class
        NULL, // Window text
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, // Window style
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL, // Parent window    
        NULL, // Menu
        hApp, // Instance handle
        NULL  // Additional application data
        );
    RegisterHotKey(hWin, 1, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'M');//CTRL+ALT+M
    SendMessage(hWin, WM_SETFONT, (WPARAM)hFont, TRUE);

    int startX = CTRL_GAP + GetSystemMetrics(SM_CXDLGFRAME);
    const RECT& videoRect = getVideoRect();

    //Device selection
    hDeviceStatic = CreateWindowEx(
        0,
        WC_STATIC,
        loadString(IDS_CAMERA),
        SS_LEFTNOWORDWRAP | WS_CHILD | WS_VISIBLE,
        startX,
        videoRect.bottom + CTRL_GAP,
        DEVICE_COMBO_WIDTH,
        STATIC_HEIGHT,
        hWin,
        (HMENU)IDC_DEVICE_STATIC,
        hApp,
        NULL);

    hDeviceCombo = CreateWindowEx(
        0,
        WC_COMBOBOX,
        NULL, 
        CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE,
        startX,
        videoRect.bottom + CTRL_GAP + STATIC_HEIGHT,
        DEVICE_COMBO_WIDTH,
        COMBO_HEIGHT,
        hWin,
        (HMENU)IDC_DEVICE_COMBO,
        hApp,
        NULL);
    SendMessage(hDeviceCombo, CB_SETDROPPEDWIDTH, (WPARAM)(DEVICE_COMBO_WIDTH*1.5), 0);
    startX += (DEVICE_COMBO_WIDTH + CTRL_GAP);

    //add trackbar for Rotation
    hRotationStatic = CreateWindowEx(
        0,
        WC_STATIC,
        loadString(IDS_ROTATION),
        SS_LEFTNOWORDWRAP | WS_CHILD | WS_VISIBLE,
        startX,
        videoRect.bottom + CTRL_GAP,
        TRACKBAR_WIDTH,
        STATIC_HEIGHT,
        hWin,
        (HMENU)IDC_ROTATION_STATIC,
        hApp,
        NULL);
    hRotationTrack = CreateWindowEx(
        0,
        TRACKBAR_CLASS,
        NULL,
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,
        startX,
        videoRect.bottom + CTRL_GAP + STATIC_HEIGHT,
        TRACKBAR_WIDTH,
        TRACKBAR_HEIGHT,
        hWin,
        (HMENU)IDC_ROTATION_TRACK,
        hApp,
        NULL);
    startX += (TRACKBAR_WIDTH + CTRL_GAP);
    
    //add trackbar for grid
    hGridIntervalStatic = CreateWindowEx(
        0,
        WC_STATIC,
        loadString(IDS_GRID_INTERVAL),
        SS_LEFTNOWORDWRAP | WS_CHILD | WS_VISIBLE,
        startX,
        videoRect.bottom + CTRL_GAP,
        TRACKBAR_WIDTH,
        STATIC_HEIGHT,
        hWin,
        (HMENU)IDC_GRID_INTERVAL_STATIC,
        hApp,
        NULL);
    hGridIntervalTrack = CreateWindowEx(
        0,
        TRACKBAR_CLASS,
        NULL,
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,
        startX,
        videoRect.bottom + CTRL_GAP + STATIC_HEIGHT,
        TRACKBAR_WIDTH,
        TRACKBAR_HEIGHT,
        hWin,
        (HMENU)IDC_GRID_INTERVAL_TRACK,
        hApp,
        NULL);
    startX += (TRACKBAR_WIDTH + CTRL_GAP);

    //Snap button
    hSnapButton = CreateWindowEx(     
        0,
        WC_BUTTON,                             
        loadString(IDS_SNAPSHOT),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        startX,
        videoRect.bottom + CTRL_GAP + BUTTON_OFFSET_Y,
        BUTTON_WIDTH,
        BUTTON_HEIGHT,                            
        hWin,                             
        (HMENU)IDC_SNAP_BUTTON,
        hApp,        
        NULL);
    startX += (BUTTON_WIDTH + CTRL_GAP);
    SendMessage(hSnapButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpImage);
    
    //Capture button
    hCaptureButton = CreateWindowEx(     
        0,
        WC_BUTTON,                             
        loadString(IDS_RECORD),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        startX,
        videoRect.bottom + CTRL_GAP + BUTTON_OFFSET_Y,
        BUTTON_WIDTH,
        BUTTON_HEIGHT,                            
        hWin,                             
        (HMENU)IDC_CAP_BUTTON,
        hApp,        
        NULL);
    SendMessage(hCaptureButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpVideo);
    startX += (BUTTON_WIDTH + CTRL_GAP);

    //Config button
    hConfigButton = CreateWindowEx(     
        0,
        WC_BUTTON,                             
        loadString(IDS_CONFIG),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        startX,
        videoRect.bottom + CTRL_GAP + BUTTON_OFFSET_Y,
        BUTTON_WIDTH,
        BUTTON_HEIGHT,                            
        hWin,                             
        (HMENU)IDC_CONFIG_BUTTON,
        hApp,        
        NULL);
    SendMessage(hConfigButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpConfig);
    startX += (BUTTON_WIDTH + CTRL_GAP);

    EnumChildWindows(hWin, SetChildThemeFont, (LPARAM)hFont);

    resetTrackbarPos();
    createConfig();
    bCreated = true;
}

void VideoCapWin::createConfig()
{
    const wchar_t CLASS_NAME[]  = WINDOW_CLASS_CONFIG;

    WNDCLASSEX wc = { };
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc   = ConfigProc;
    wc.hInstance     = hApp;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(hApp, L"ID_ICON_APP");
    wc.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    
    RegisterClassEx(&wc);

    int startX = CTRL_GAP;
    int startY = CTRL_GAP;
    //Config window
    hConfig = CreateWindowEx(
        0, // Optional window styles.
        CLASS_NAME, // Window class
        loadString(IDS_CONFIG), // Window text
        WS_POPUPWINDOW | WS_CAPTION , // Window style
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CONFIG_WINDOW_WIDTH,
        CONFIG_WINDOW_HEIGHT,
        NULL, // Parent window    
        NULL, // Menu
        hApp, // Instance handle
        NULL  // Additional application data
        );
    SendMessage(hConfig, WM_SETFONT, (WPARAM)hFont, TRUE);

    //Format selection
    hFormatStatic = CreateWindowEx(
        0,
        WC_STATIC,
        loadString(IDS_FORMAT),
        SS_LEFTNOWORDWRAP | WS_CHILD | WS_VISIBLE,
        startX,
        startY,
        CONFIG_LEFT_WIDTH,
        CONFIG_LINE_HEIGHT,
        hConfig,
        (HMENU)IDC_FORMAT_STATIC,
        hApp,
        NULL);
    startX += (CONFIG_LEFT_WIDTH + CTRL_GAP);
    hFormatCombo = CreateWindowEx(
        0,
        WC_COMBOBOX,
        NULL, 
        CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        startX,
        startY,
        CONFIG_MID_WIDTH,
        COMBO_HEIGHT,
        hConfig,
        (HMENU)IDC_FORMAT_COMBO,
        hApp,
        NULL);
    //SendMessage(hFormatCombo, CB_SETDROPPEDWIDTH, (WPARAM)CONFIG_MID_WIDTH*1.5, 0);
    startX = CTRL_GAP;
    startY += CONFIG_LINE_HEIGHT + CTRL_GAP;

    //Resolution selection
    hResolutionStatic = CreateWindowEx(
        0,
        WC_STATIC,
        loadString(IDS_RESOLUTION),
        SS_LEFTNOWORDWRAP | WS_CHILD | WS_VISIBLE,
        startX,
        startY,
        CONFIG_LEFT_WIDTH,
        CONFIG_LINE_HEIGHT,
        hConfig,
        (HMENU)IDC_RESOLUTION_STATIC,
        hApp,
        NULL);
    startX += (CONFIG_LEFT_WIDTH + CTRL_GAP);
    hResolutionCombo = CreateWindowEx(
        0,
        WC_COMBOBOX,
        NULL, 
        CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        startX,
        startY,
        CONFIG_MID_WIDTH,
        COMBO_HEIGHT,
        hConfig,
        (HMENU)IDC_RESOLUTION_COMBO,
        hApp,
        NULL);
    //SendMessage(hResolutionCombo, CB_SETDROPPEDWIDTH, (WPARAM)CONFIG_MID_WIDTH*1.5, 0);
    startX = CTRL_GAP;
    startY += CONFIG_LINE_HEIGHT + CTRL_GAP;

    //Language selection
    hLanguageStatic = CreateWindowEx(
        0,
        WC_STATIC,
        loadString(IDS_LANGUAGE),
        SS_LEFTNOWORDWRAP | WS_CHILD | WS_VISIBLE,
        startX,
        startY,
        CONFIG_LEFT_WIDTH,
        CONFIG_LINE_HEIGHT,
        hConfig,
        (HMENU)IDC_LANGUAGE_STATIC,
        hApp,
        NULL);
    startX += (CONFIG_LEFT_WIDTH + CTRL_GAP);
    hLanguageCombo = CreateWindowEx(
        0,
        WC_COMBOBOX,
        NULL, 
        CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        startX,
        startY,
        CONFIG_MID_WIDTH,
        COMBO_HEIGHT,
        hConfig,
        (HMENU)IDC_LANGUAGE_COMBO,
        hApp,
        NULL);
    //SendMessage(hLanguageCombo, CB_SETDROPPEDWIDTH, (WPARAM)CONFIG_MID_WIDTH*1.5, 0);
    SendMessage(hLanguageCombo,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) loadString(IDS_LANGUAGE_EN)); 
    SendMessage(hLanguageCombo,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) loadString(IDS_LANGUAGE_ZH));
    if (languageCode == "en") SendMessage(hLanguageCombo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
    else SendMessage(hLanguageCombo, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
    startX = CTRL_GAP;
    startY += CONFIG_LINE_HEIGHT + CTRL_GAP;
    
    //Save path
    hSavePathStatic = CreateWindowEx(
        0,
        WC_STATIC,
        loadString(IDS_SAVE_PATH),
        SS_LEFTNOWORDWRAP | WS_CHILD | WS_VISIBLE,
        startX,
        startY,
        CONFIG_LEFT_WIDTH,
        CONFIG_LINE_HEIGHT,
        hConfig,
        (HMENU)IDC_SAVE_PATH_STATIC,
        hApp,
        NULL);
    startX += (CONFIG_LEFT_WIDTH + CTRL_GAP);
    hSavePathEdit = CreateWindowEx(
        0,
        WC_EDIT,
        NULL, 
        WS_CHILD | WS_BORDER | WS_VISIBLE | ES_READONLY,
        startX,
        startY,
        CONFIG_MID_WIDTH,
        CONFIG_LINE_HEIGHT,
        hConfig,
        (HMENU)IDC_PATH_EDIT,
        hApp,
        NULL);
    SetWindowText(hSavePathEdit,savePath.c_str());
    startX += CONFIG_MID_WIDTH + CTRL_GAP;
    hChoosePathButton = CreateWindowEx(     
        0,
        WC_BUTTON,                             
        loadString(IDS_CHOOSE_PATH),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,
        startX,
        startY,
        CONFIG_RIGHT_WIDTH,
        CONFIG_LINE_HEIGHT,                            
        hConfig,                             
        (HMENU)IDC_CHOOSE_PATH_BUTTON,
        hApp,        
        NULL);
    startX = CTRL_GAP;
    startY += CONFIG_LINE_HEIGHT + CTRL_GAP;

    EnumChildWindows(hConfig, SetChildThemeFont, (LPARAM)hFont);
    
    ShowWindow(hConfig,SW_HIDE);
    bConfigHide = true;
}


void VideoCapWin::setLanguage(const std::string & code)
{
    this->languageCode = code;
    if (!bCreated) return;
    
    //main window
    SetWindowText(hDeviceStatic,loadString(IDS_CAMERA));
    SetWindowText(hRotationStatic,loadString(IDS_ROTATION));
    SetWindowText(hGridIntervalStatic,loadString(IDS_GRID_INTERVAL));
    SetWindowText(hSnapButton,loadString(IDS_SNAPSHOT));
    SetWindowText(hCaptureButton,loadString(IDS_RECORD));
    SetWindowText(hConfigButton,loadString(IDS_CONFIG));
    SetWindowText(hWin,loadString(IDS_TITLE_HELP));

    //config window
    SetWindowText(hFormatStatic,loadString(IDS_FORMAT));
    SetWindowText(hResolutionStatic,loadString(IDS_RESOLUTION));
    SetWindowText(hLanguageStatic,loadString(IDS_LANGUAGE));
    SetWindowText(hSavePathStatic,loadString(IDS_SAVE_PATH));
    SetWindowText(hChoosePathButton,loadString(IDS_CHOOSE_PATH));
    SetWindowText(hConfig,loadString(IDS_CONFIG));
    
    ComboBox_ResetContent(hLanguageCombo);
    SendMessage(hLanguageCombo,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) loadString(IDS_LANGUAGE_EN)); 
    SendMessage(hLanguageCombo,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) loadString(IDS_LANGUAGE_ZH));
    if (languageCode == "en") SendMessage(hLanguageCombo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
    else SendMessage(hLanguageCombo, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
}

void VideoCapWin::setSavePath(const wchar_t* path)
{
    this->savePath = path;
    if (!bCreated) return;

    SetWindowText(hSavePathEdit,savePath.c_str());
}

void VideoCapWin::setSavePath(const std::wstring& path)
{
    this->savePath = path;
    if (!bCreated) return;

    SetWindowText(hSavePathEdit,savePath.c_str());
}

void VideoCapWin::destory()
{
    UnregisterHotKey(hWin, 1);
    DeleteObject(hBmpImage);
    DeleteObject(hBmpVideo);
    DeleteObject(hBmpVideoOff);
    DeleteObject(hFont);
}

void VideoCapWin::setVideoSize(int width, int height)
{
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    std::cout<<workArea.left<<','<<workArea.top<<','<<workArea.right<<','<<workArea.bottom<<std::endl;
    if (width > workArea.right-WINDOW_WIDTH_MARGIN) width = workArea.right-WINDOW_HEIGHT_MARGIN;
    if (height > workArea.bottom-WINDOW_HEIGHT_MARGIN-TOOLBAR_HEIGHT) height = workArea.bottom-WINDOW_HEIGHT_MARGIN-TOOLBAR_HEIGHT;

    SetWindowPos(
        hWin,
        HWND_TOP,
        0,
        0,
        width+WINDOW_WIDTH_MARGIN,
        height+TOOLBAR_HEIGHT+WINDOW_HEIGHT_MARGIN,
        SWP_NOMOVE);

    int startX = CTRL_GAP + GetSystemMetrics(SM_CXDLGFRAME);

    SetWindowPos(
        hDeviceStatic,
        0,
        startX,
        height + CTRL_GAP,
        0,
        0,
        SWP_NOSIZE);

    SetWindowPos(
        hDeviceCombo,
        0,
        startX,
        height+CTRL_GAP+STATIC_HEIGHT,
        0,
        0,
        SWP_NOSIZE);
    
    startX += (DEVICE_COMBO_WIDTH + CTRL_GAP);

    SetWindowPos(
        hRotationStatic,
        0,
        startX,
        height + CTRL_GAP,
        0,
        0,
        SWP_NOSIZE);

    SetWindowPos(
        hRotationTrack,
        0,
        startX,
        height + CTRL_GAP + STATIC_HEIGHT,
        0,
        0,
        SWP_NOSIZE);

    startX += (TRACKBAR_WIDTH + CTRL_GAP);

    SetWindowPos(
        hGridIntervalStatic,
        0,
        startX,
        height + CTRL_GAP,
        0,
        0,
        SWP_NOSIZE);

    SetWindowPos(
        hGridIntervalTrack,
        0,
        startX,
        height + CTRL_GAP + STATIC_HEIGHT,
        0,
        0,
        SWP_NOSIZE);

    startX += (TRACKBAR_WIDTH + CTRL_GAP);

    SetWindowPos(
        hSnapButton,
        0,
        startX,
        height+CTRL_GAP + BUTTON_OFFSET_Y,
        0,
        0,
        SWP_NOSIZE);
   
    startX += (BUTTON_WIDTH + CTRL_GAP);
    
    SetWindowPos(
        hCaptureButton,
        0,
        startX,
        height+CTRL_GAP + BUTTON_OFFSET_Y,
        0,
        0,
        SWP_NOSIZE);
    
    startX += (BUTTON_WIDTH + CTRL_GAP);
    
    SetWindowPos(
        hConfigButton,
        0,
        startX,
        height+CTRL_GAP + BUTTON_OFFSET_Y,
        0,
        0,
        SWP_NOSIZE);

    startX += (BUTTON_WIDTH + CTRL_GAP);
}

void VideoCapWin::resetTrackbarPos()
{
    setGridTrack(0);
    setRotationTrack(ROTATION_DEGREE_MAX/ROTATION_DEGREE_STEP);
}

void VideoCapWin::setRotationTrack(int pos)
{
    int maxPos = ROTATION_DEGREE_MAX*2/ROTATION_DEGREE_STEP;
    pos = pos < 0 ? 0 : pos;
    pos = pos > maxPos ? maxPos : pos;

    SendMessage(hRotationTrack, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, maxPos));
    //SendMessage(hRotationTrack, TBM_SETPAGESIZE, 0, (LPARAM)ROTATION_DEGREE_STEP);
    //SendMessage(hRotationTrack, TBM_SETSEL, (WPARAM)FALSE, (LPARAM)MAKELONG(0, maxPos));
    SendMessage(hRotationTrack, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)(pos));
    SendMessage(hRotationTrack, TBM_SETTICFREQ, (WPARAM)maxPos/ROTATION_DEGREE_STEP, 0);
}

void VideoCapWin::setGridTrack(int pos)
{
    int maxPos = GRID_INTERVAL_MAX / GRID_INTERVAL_STEP;
	pos = pos < 0 ? 0 : pos;
    pos = pos > maxPos ? maxPos : pos;
    SendMessage(hGridIntervalTrack, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, maxPos));
    //SendMessage(hGridIntervalTrack, TBM_SETPAGESIZE, 0, (LPARAM)ROTATION_DEGREE_STEP);
    //SendMessage(hGridIntervalTrack, TBM_SETSEL, (WPARAM)FALSE, (LPARAM)MAKELONG(0, maxPos));
    SendMessage(hGridIntervalTrack, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)(pos));
    SendMessage(hGridIntervalTrack, TBM_SETTICFREQ, (WPARAM)maxPos/GRID_INTERVAL_STEP, 0);
}
void VideoCapWin::setCaptureButton(int status)
{
    if (status == 1)
    {
        SendMessage(hCaptureButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpVideo);
        SetWindowText(hCaptureButton, loadString(IDS_RECORD));
    }
    else
    {
        SendMessage(hCaptureButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpVideoOff);
        SetWindowText(hCaptureButton, loadString(IDS_STOP));
    }
}

const RECT& VideoCapWin::getVideoRect()
{
    GetClientRect(hWin, &clientRect);
    if (bToolbarHide) SetRect(&videoRect, 0, 0, clientRect.right, clientRect.bottom);
    else SetRect(&videoRect, 0, 0, clientRect.right, clientRect.bottom - TOOLBAR_HEIGHT);
    return videoRect;
}

void VideoCapWin::redraw()
{
    //RedrawWindow(hDeviceStatic, 0, 0, RDW_ERASE | RDW_INVALIDATE);
    //RedrawWindow(hFormatStatic, 0, 0, RDW_ERASE | RDW_INVALIDATE);
    //RedrawWindow(hResolutionStatic, 0, 0, RDW_ERASE | RDW_INVALIDATE);
    RedrawWindow(hWin, 0, 0, RDW_ERASE | RDW_INVALIDATE);
}

void VideoCapWin::showMain()
{
    if (IsIconic(hWin))
    {
        SendMessage(hWin,WM_SYSCOMMAND,SC_RESTORE,0);
    }
    ShowWindow(hWin, SW_SHOW);
    SetForegroundWindow(hWin);
}


void VideoCapWin::hideToolbar()
{
    ShowWindow(hDeviceStatic,SW_HIDE);
    ShowWindow(hDeviceCombo,SW_HIDE);
    ShowWindow(hRotationStatic,SW_HIDE);
    ShowWindow(hRotationTrack,SW_HIDE);
    ShowWindow(hGridIntervalStatic,SW_HIDE);
    ShowWindow(hGridIntervalTrack,SW_HIDE);
    ShowWindow(hSnapButton,SW_HIDE);
    ShowWindow(hCaptureButton,SW_HIDE);
    ShowWindow(hConfigButton,SW_HIDE);
    bToolbarHide = true;
}

void VideoCapWin::showToolbar()
{
    ShowWindow(hDeviceStatic,SW_SHOW);
    ShowWindow(hDeviceCombo,SW_SHOW);
    ShowWindow(hRotationStatic,SW_SHOW);
    ShowWindow(hRotationTrack,SW_SHOW);
    ShowWindow(hGridIntervalStatic,SW_SHOW);
    ShowWindow(hGridIntervalTrack,SW_SHOW);
    ShowWindow(hSnapButton,SW_SHOW);
    ShowWindow(hCaptureButton,SW_SHOW);
    ShowWindow(hConfigButton,SW_SHOW);
    bToolbarHide = false;
}

void VideoCapWin::hideConfig()
{   
    ShowWindow(hConfig,SW_HIDE);
    bConfigHide = true;
}

void VideoCapWin::showConfig()
{
    SetWindowPos(hConfig, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    bConfigHide = false;
}

const wchar_t* VideoCapWin::getDeviceComboText()
{
    int index = ComboBox_GetCurSel(hDeviceCombo);
    ComboBox_GetLBText(hDeviceCombo, index, deviceTextBuffer);
    return deviceTextBuffer;
}

const wchar_t* VideoCapWin::getFormatComboText()
{
    int index = ComboBox_GetCurSel(hFormatCombo);
    ComboBox_GetLBText(hFormatCombo, index, formatTextBuffer);
    return formatTextBuffer;
}

const wchar_t* VideoCapWin::getResolutionComboText()
{
    int index = ComboBox_GetCurSel(hResolutionCombo);
    ComboBox_GetLBText(hResolutionCombo, index, resolutionTextBuffer);
    return resolutionTextBuffer;
}

void VideoCapWin::setupDeviceCombo(const std::vector<std::wstring>& list, int index)
{
    setupCombo(hDeviceCombo,list,index);
}

void VideoCapWin::setupFormatCombo(const std::vector<std::wstring>& list, int index)
{
    setupCombo(hFormatCombo,list,index);
}

void VideoCapWin::setupResolutionCombo(const std::vector<std::wstring>& list, int index)
{
    setupCombo(hResolutionCombo,list,index);
}

void VideoCapWin::setupCombo(HWND hwnd, const std::vector<std::wstring>& list, int index)
{
    ComboBox_ResetContent(hwnd);
    for(auto item : list)
    {
        SendMessage(hwnd,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) item.c_str()); 
    }
      
    SendMessage(hwnd, CB_SETCURSEL, (WPARAM)index, (LPARAM)0);
}

const wchar_t* VideoCapWin::loadString(int ids)
{
    if (languageCode.compare("zh") == 0)
    {
        CONSOLE<< "zh" << ENDL;
        LoadString(hApp, ids + ID_LANGUAGE_OFFSET_zh, languageStrBuff, LANGUAGE_STRING_MAX_SIZE);
    }
    else
    {
        CONSOLE<< "en" << ENDL;
        LoadString(hApp, ids + ID_LANGUAGE_OFFSET_en, languageStrBuff, LANGUAGE_STRING_MAX_SIZE);
    }
    return languageStrBuff;
}
