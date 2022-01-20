#ifndef UNICODE
#define UNICODE
#endif 

#include <iostream>
#include <windows.h>
#include <commctrl.h>
#include <Windowsx.h>
#include "MainWindow.h"
#include "WinMsgHandle.h"
#include "resource.h"

#define POS_GAP 5

MainWindow::MainWindow(HINSTANCE hInstance, int _videoWidth, int _videoHeight):
    hApp(hInstance),videoWidth(_videoWidth),videoHeight(_videoHeight),addPos(5)
{
    const wchar_t CLASS_NAME[]  = L"VideoCap";

    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hApp;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    hMain = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        NULL,    // Window text
        WS_OVERLAPPEDWINDOW^WS_THICKFRAME^WS_MAXIMIZEBOX^WS_MINIMIZEBOX,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 16 + videoWidth, 69 + videoHeight,

        NULL,       // Parent window    
        NULL,       // Menu
        hApp,  // Instance handle
        NULL        // Additional application data
        );
}

void MainWindow::addIcon()
{
    hIcon = LoadIcon(hApp, L"IDI_ICON_APP");
    SetClassLongPtr(hMain, // window handle 
                    GCLP_HICON, // changes icon 
                    (LONG_PTR) hIcon); 
}

void MainWindow::addDeviceCombo(const std::vector<std::wstring>& list, int defaultIndex)
{
    hDeviceCombo = addCombo(IDC_DEVICE_COMBO,140,list,defaultIndex);
}

void MainWindow::addFormatCombo(const std::vector<std::wstring>& list, int defaultIndex)
{
    hFormatCombo = addCombo(IDC_FORMAT_COMBO,200,list,defaultIndex);
}

void MainWindow::addResolutionCombo(const std::vector<std::wstring>& list, int defaultIndex)
{
    hResolutionCombo = addCombo(IDC_RESOLUTION_COMBO,80,list,defaultIndex);
}

void MainWindow::addSnapButton()
{
    int width = 80;
    hSnapButton = CreateWindowEx(     
        0,
        WC_BUTTON,                             
        L"SNAP",                                                       
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,                            
        addPos, 5 + videoHeight, width, 20,                            
        hMain,                             
        (HMENU)IDC_SNAP_BUTTON,
        hApp,        
        NULL);
    addPos = addPos + width + POS_GAP;
    //std::cout<<std::hex<<(unsigned int)hSnapButton<<std::endl;
}

void MainWindow::addCapButton()
{
    int width = 80;
    hCaptureButton = CreateWindowEx(     
        0,
        WC_BUTTON,                             
        L"START Cap",                                                       
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,                            
        addPos, 5 + videoHeight , width, 20,                            
        hMain,                             
        (HMENU)IDC_CAP_BUTTON,
        hApp,        
        NULL);
    addPos = addPos + width + POS_GAP;
    //std::cout<<std::hex<<(unsigned int)hCaptureButton<<std::endl;
}

HWND MainWindow::addCombo(int IDC, int width, const std::vector<std::wstring>& list, int defaultIndex)
{
    HWND hWnd = CreateWindowEx(
        0,
        WC_COMBOBOX,
        NULL, 
        CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        addPos, 5 + videoHeight, width, 200,
        hMain,
        (HMENU)IDC,
        hApp,
        NULL);
    addPos = addPos + width + POS_GAP;

    for(auto item : list)
    {
        SendMessage(hWnd,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) item.c_str()); 
    }
      
    SendMessage(hWnd, CB_SETCURSEL, (WPARAM)defaultIndex, (LPARAM)0);
    return hWnd;
}

