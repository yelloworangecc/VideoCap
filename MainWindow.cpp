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

MainWindow::MainWindow(HINSTANCE hInstance, int _videoWidth, int _videoHeight):
    hApp(hInstance),videoWidth(_videoWidth),videoHeight(_videoHeight),addPos(10)
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

void MainWindow::addDeviceCombo()
{
    int width = 160;
    hDeviceCombo = CreateWindowEx(
        0,
        WC_COMBOBOX,
        NULL, 
        CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        addPos, 5 + videoHeight, width, 200,
        hMain,
        (HMENU)IDC_DEVICE_COMBO,
        hApp,
        NULL);
    addPos = addPos + width + 10;

    TCHAR Planets[9][10] =  
    {
        TEXT("Mercury"), TEXT("Venus"), TEXT("Terra")
    };
       
    TCHAR A[16]; 
    int  k = 0; 

    memset(&A,0,sizeof(A));       
    for (k = 0; k <= 8; k += 1)
    {
        wcscpy_s(A, sizeof(A)/sizeof(TCHAR),  (TCHAR*)Planets[k]);
        SendMessage(hDeviceCombo,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) A); 
    }
      
    SendMessage(hDeviceCombo, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);
}

void MainWindow::addFormatCombo()
{
    /*
     HWND hWndComboBox = CreateWindow(WC_COMBOBOX, TEXT(""), 
     CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
     addPos, ypos, nwidth, nheight,
     hMain, NULL, HINST_THISCOMPONENT,
     NULL);
     */
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
    addPos = addPos + width + 10;
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
    addPos = addPos + width + 10;
    //std::cout<<std::hex<<(unsigned int)hCaptureButton<<std::endl;
}

/*
void MainWindow::addTrackBar()
{
    hTrackBar = CreateWindowEx( 
        0,
        TRACKBAR_CLASS,
        NULL,
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,
        190, 5 + videoHeight, 200, 20,
        hMain,
        (HMENU)IDC_TRACKBAR,
        hApp,
        NULL); 
}
*/
