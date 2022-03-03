#ifndef UNICODE
#define UNICODE
#endif
#include <iostream>
#include <Windows.h>
#include <Windowsx.h>
#include <CommCtrl.h>
#include "resource.h"
#include "VideoCapWin.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='AMD64' publicKeyToken='6595b64144ccf1df' language='*'\"")

LRESULT CALLBACK WindowProc(HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam);


void VideoCapWin::create(HINSTANCE hApp, const std::string & languageCode)
{
    this->hApp = hApp;
    this->languageCode = languageCode;

    const wchar_t CLASS_NAME[]  = L"VideoCap";

    WNDCLASSEX wc = { };
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hApp;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(hApp, L"ID_ICON_APP");

    RegisterClassEx(&wc);

    hBmpImage = LoadBitmap(hApp, L"ID_BMP_IMAGE");
    hBmpVideo = LoadBitmap(hApp, L"ID_BMP_VIDEO");
    hBmpVideoOff = LoadBitmap(hApp, L"ID_BMP_VIDEO_OFF");

    hWin = CreateWindowEx(
        0, // Optional window styles.
        CLASS_NAME, // Window class
        NULL, // Window text
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN, // Window style
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL, // Parent window    
        NULL, // Menu
        hApp, // Instance handle
        NULL  // Additional application data
        );

    int startX = CTRL_GRAP;
    const RECT& videoRect = getTargetVideoRect();

    hDeviceStatic = CreateWindowEx(
        0,
        WC_STATIC,
        loadString(IDS_CAMERA),
        SS_LEFTNOWORDWRAP | WS_CHILD | WS_VISIBLE,
        startX,
        videoRect.bottom + CTRL_GRAP,
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
        CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE,
        startX,
        videoRect.bottom + CTRL_GRAP + STATIC_HEIGHT,
        DEVICE_COMBO_WIDTH,
        COMBO_HEIGHT,
        hWin,
        (HMENU)IDC_DEVICE_COMBO,
        hApp,
        NULL);
    startX += (DEVICE_COMBO_WIDTH + CTRL_GRAP);

    hFormatStatic = CreateWindowEx(
        0,
        WC_STATIC,
        loadString(IDS_FORMAT),
        SS_LEFTNOWORDWRAP | WS_CHILD | WS_VISIBLE,
        startX,
        videoRect.bottom + CTRL_GRAP,
        FORMAT_COMBO_WIDTH,
        STATIC_HEIGHT,
        hWin,
        (HMENU)IDC_FORMAT_STATIC,
        hApp,
        NULL);

    hFormatCombo = CreateWindowEx(
        0,
        WC_COMBOBOX,
        NULL, 
        CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        startX,
        videoRect.bottom + CTRL_GRAP + STATIC_HEIGHT,
        FORMAT_COMBO_WIDTH,
        COMBO_HEIGHT,
        hWin,
        (HMENU)IDC_FORMAT_COMBO,
        hApp,
        NULL);
    startX += (FORMAT_COMBO_WIDTH + CTRL_GRAP);

    hResolutionStatic = CreateWindowEx(
        0,
        WC_STATIC,
        loadString(IDS_RESOLUTION),
        SS_LEFTNOWORDWRAP | WS_CHILD | WS_VISIBLE,
        startX,
        videoRect.bottom + CTRL_GRAP,
        RESOLUTION_COMBO_WIDTH,
        STATIC_HEIGHT,
        hWin,
        (HMENU)IDC_RESOLUTION_STATIC,
        hApp,
        NULL);

    hResolutionCombo = CreateWindowEx(
        0,
        WC_COMBOBOX,
        NULL, 
        CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        startX,
        videoRect.bottom + CTRL_GRAP,
        RESOLUTION_COMBO_WIDTH,
        COMBO_HEIGHT,
        hWin,
        (HMENU)IDC_RESOLUTION_COMBO,
        hApp,
        NULL);

    startX += (RESOLUTION_COMBO_WIDTH + CTRL_GRAP);

    hSnapButton = CreateWindowEx(     
        0,
        WC_BUTTON,                             
        loadString(IDS_SNAPSHOT),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,
        startX,
        videoRect.bottom + CTRL_GRAP + STATIC_HEIGHT,
        BUTTON_WIDTH,
        BUTTON_HEIGHT,                            
        hWin,                             
        (HMENU)IDC_SNAP_BUTTON,
        hApp,        
        NULL);
    startX += (BUTTON_WIDTH + CTRL_GRAP);
    SendMessage(hSnapButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpImage);
    
    hCaptureButton = CreateWindowEx(     
        0,
        WC_BUTTON,                             
        loadString(IDS_RECORD),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,
        startX,
        videoRect.bottom + CTRL_GRAP,
        BUTTON_WIDTH,
        BUTTON_HEIGHT,                            
        hWin,                             
        (HMENU)IDC_CAP_BUTTON,
        hApp,        
        NULL);
    startX += (BUTTON_WIDTH + CTRL_GRAP);
    SendMessage(hCaptureButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpVideo);
}

void VideoCapWin::destory()
{
    DeleteObject(hBmpImage);
    DeleteObject(hBmpVideo);
    DeleteObject(hBmpVideoOff);
}

void VideoCapWin::show()
{
    ShowWindow(hWin, SW_SHOW);
}

void VideoCapWin::setVideoSize(int width, int height)
{
    SetWindowPos(
        hWin,
        HWND_TOPMOST,
        0,
        0,
        width+WINDOW_WIDTH_MARGIN,
        height+TOOLBAR_HEIGHT+WINDOW_HEIGHT_MARGIN,
        SWP_NOMOVE);

    int startX = CTRL_GRAP;

    SetWindowPos(
        hDeviceStatic,
        0,
        startX,
        height + CTRL_GRAP,
        0,
        0,
        SWP_NOSIZE);

    SetWindowPos(
        hDeviceCombo,
        0,
        startX,
        height+CTRL_GRAP+STATIC_HEIGHT,
        0,
        0,
        SWP_NOSIZE);
    
    startX += (DEVICE_COMBO_WIDTH + CTRL_GRAP);

    SetWindowPos(
        hFormatStatic,
        0,
        startX,
        height + CTRL_GRAP,
        0,
        0,
        SWP_NOSIZE);

    SetWindowPos(
        hFormatCombo,
        0,
        startX,
        height+CTRL_GRAP+STATIC_HEIGHT,
        0,
        0,
        SWP_NOSIZE);

    startX += (FORMAT_COMBO_WIDTH + CTRL_GRAP);

    SetWindowPos(
        hResolutionStatic,
        0,
        startX,
        height + CTRL_GRAP,
        0,
        0,
        SWP_NOSIZE);

    SetWindowPos(
        hResolutionCombo,
        0,
        startX,
        height+CTRL_GRAP+STATIC_HEIGHT,
        0,
        0,
        SWP_NOSIZE);
   
    startX += (RESOLUTION_COMBO_WIDTH + CTRL_GRAP);
    SetWindowPos(
        hSnapButton,
        0,
        startX,
        height+CTRL_GRAP,
        0,
        0,
        SWP_NOSIZE);
   
    startX += (BUTTON_WIDTH + CTRL_GRAP);
    SetWindowPos(
        hCaptureButton,
        0,
        startX,
        height+CTRL_GRAP,
        0,
        0,
        SWP_NOSIZE);

    startX += (BUTTON_WIDTH + CTRL_GRAP);

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

const RECT& VideoCapWin::getTargetVideoRect()
{
    GetClientRect(hWin, &clientRect);
    SetRect(&videoRect, 0, 0, clientRect.right, clientRect.bottom - TOOLBAR_HEIGHT);
    return videoRect;
}

void VideoCapWin::redraw()
{
    //RedrawWindow(hDeviceStatic, 0, 0, RDW_ERASE | RDW_INVALIDATE);
    //RedrawWindow(hFormatStatic, 0, 0, RDW_ERASE | RDW_INVALIDATE);
    //RedrawWindow(hResolutionStatic, 0, 0, RDW_ERASE | RDW_INVALIDATE);
    RedrawWindow(hWin, 0, 0, RDW_ERASE | RDW_INVALIDATE);
}

int VideoCapWin::getDeviceComboIndex()
{
    return ComboBox_GetCurSel(hDeviceCombo);
}

int VideoCapWin::getFormatComboIndex()
{
    return ComboBox_GetCurSel(hFormatCombo);
}

int VideoCapWin::getResolutionComboIndex()
{
    return ComboBox_GetCurSel(hResolutionCombo);
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
        std::cout << "zh" << std::endl;
        LoadString(hApp, ids + ID_LANGUAGE_OFFSET_zh, languageStrBuff, LANGUAGE_STRING_MAX_SIZE);
    }
    else
    {
        std::cout << "en" << std::endl;
        LoadString(hApp, ids + ID_LANGUAGE_OFFSET_en, languageStrBuff, LANGUAGE_STRING_MAX_SIZE);
    }
    return languageStrBuff;
}
