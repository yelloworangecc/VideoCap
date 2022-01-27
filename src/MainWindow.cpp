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

MainWindow::MainWindow(HINSTANCE hInstance):
    hApp(hInstance),addPos(CTRL_GRAP)
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
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        DEFAULT_IMAGE_WIDTH + WIDTH_MARGIN,
        DEFAULT_IMAGE_HEIGHT + DEFAULT_TOOLBAR_HEIGHT + HEIGHT_MARGIN,
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
    hDeviceCombo = addCombo(IDC_DEVICE_COMBO,DEVICE_COMBO_WIDTH,list,defaultIndex);
}

void MainWindow::addFormatCombo(const std::vector<std::wstring>& list, int defaultIndex)
{
    hFormatCombo = addCombo(IDC_FORMAT_COMBO,FORMAT_COMBO_WIDTH,list,defaultIndex);
}

void MainWindow::addResolutionCombo(const std::vector<std::wstring>& list, int defaultIndex)
{
    hResolutionCombo = addCombo(IDC_RESOLUTION_COMBO,RESOLUTION_COMBO_WIDTH,list,defaultIndex);
}

void MainWindow::addSnapButton()
{
    hSnapButton = CreateWindowEx(     
        0,
        WC_BUTTON,                             
        L"SNAP",                                                       
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,                            
        addPos, CTRL_GRAP + DEFAULT_IMAGE_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT,                            
        hMain,                             
        (HMENU)IDC_SNAP_BUTTON,
        hApp,        
        NULL);
    addPos = addPos + BUTTON_WIDTH + CTRL_GRAP;
    //std::cout<<std::hex<<(unsigned int)hSnapButton<<std::endl;
}

void MainWindow::addCapButton()
{
    hCaptureButton = CreateWindowEx(     
        0,
        WC_BUTTON,                             
        L"START Cap",                                                       
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,                            
        addPos, CTRL_GRAP + DEFAULT_IMAGE_HEIGHT , BUTTON_WIDTH, BUTTON_HEIGHT,                            
        hMain,                             
        (HMENU)IDC_CAP_BUTTON,
        hApp,        
        NULL);
    addPos = addPos + BUTTON_WIDTH + CTRL_GRAP;
    //std::cout<<std::hex<<(unsigned int)hCaptureButton<<std::endl;
}

void MainWindow::updateFormatCombo(const std::vector<std::wstring>& list, int defaultIndex)
{
    updateCombo(hFormatCombo,list,defaultIndex);
}

void MainWindow::updateResolutionCombo(const std::vector<std::wstring>& list, int defaultIndex)
{
    updateCombo(hResolutionCombo,list,defaultIndex);
}

void MainWindow::updateUserSettings(
    const std::vector<std::wstring>& listFormat,
    const std::vector<std::wstring>& listResolution)
{
    updateFormatCombo(listFormat);
    updateResolutionCombo(listResolution);
}

void MainWindow::updateWindowSize(int width, int height)
{
    SetWindowPos(
        hMain,
        HWND_TOPMOST,
        0,
        0,
        width,
        height,
        SWP_NOMOVE);
}

void MainWindow::updateCtrlPos(int imageHeight)
{
    std::cout << "IN updateCtrlPos " << imageHeight << std::endl;
    
    addPos = CTRL_GRAP;
    MoveWindow(hDeviceCombo,addPos,imageHeight+CTRL_GRAP,DEVICE_COMBO_WIDTH,COMBO_HEIGHT,true);

    addPos = addPos + DEVICE_COMBO_WIDTH + CTRL_GRAP;
    MoveWindow(hFormatCombo,addPos,imageHeight+CTRL_GRAP,FORMAT_COMBO_WIDTH,COMBO_HEIGHT,true);

    addPos = addPos + FORMAT_COMBO_WIDTH + CTRL_GRAP;
    MoveWindow(hResolutionCombo,addPos,imageHeight+CTRL_GRAP,RESOLUTION_COMBO_WIDTH,COMBO_HEIGHT,true);
   
    addPos = addPos + RESOLUTION_COMBO_WIDTH + CTRL_GRAP;
    MoveWindow(hSnapButton,addPos,imageHeight+CTRL_GRAP,BUTTON_WIDTH,BUTTON_HEIGHT,true);
   
    addPos = addPos + BUTTON_WIDTH + CTRL_GRAP;
    MoveWindow(hCaptureButton,addPos,imageHeight+CTRL_GRAP,BUTTON_WIDTH,BUTTON_HEIGHT,true);

    addPos = addPos + BUTTON_WIDTH + CTRL_GRAP;
}

std::vector<std::wstring> MainWindow::getUserSettings()
{
    std::vector<std::wstring> list;
    int index;
    wchar_t text[256];

    index = ComboBox_GetCurSel(hDeviceCombo);
    ComboBox_GetLBText(hDeviceCombo,index,text);
    list.push_back(text);
    
    index = ComboBox_GetCurSel(hFormatCombo);
    ComboBox_GetLBText(hFormatCombo,index,text);
    list.push_back(text);

    index = ComboBox_GetCurSel(hResolutionCombo);
    ComboBox_GetLBText(hResolutionCombo,index,text);
    list.push_back(text);
    return list;
}

HWND MainWindow::addCombo(int IDC, int width, const std::vector<std::wstring>& list, int defaultIndex)
{
    HWND hWnd = CreateWindowEx(
        0,
        WC_COMBOBOX,
        NULL, 
        CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        addPos, CTRL_GRAP + DEFAULT_IMAGE_HEIGHT, width, COMBO_HEIGHT,
        hMain,
        (HMENU)IDC,
        hApp,
        NULL);
    addPos = addPos + width + CTRL_GRAP;

    for(auto item : list)
    {
        SendMessage(hWnd,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) item.c_str()); 
    }
      
    SendMessage(hWnd, CB_SETCURSEL, (WPARAM)defaultIndex, (LPARAM)0);
    return hWnd;
}

void MainWindow::updateCombo(HWND hwnd, const std::vector<std::wstring>& list, int defaultIndex)
{
    ComboBox_ResetContent(hwnd);
    for(auto item : list)
    {
        SendMessage(hwnd,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) item.c_str()); 
    }
      
    SendMessage(hwnd, CB_SETCURSEL, (WPARAM)defaultIndex, (LPARAM)0);
}
