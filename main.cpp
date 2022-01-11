#ifndef UNICODE
#define UNICODE
#endif 
#include <iostream>
#include <assert.h>
#include <windows.h>
#include <commctrl.h>
#include "VideoDevice.h"

#define WM_GRAPHNOTIFY  WM_APP + 1

VideoDevice* vdev;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance,
	                   HINSTANCE hPrevInstance,
	                   PWSTR pCmdLine,
	                   int nCmdShow)
{
	wchar_t ** argv;
	int argc;
	
	argv = CommandLineToArgvW(pCmdLine,&argc);
    vdev = new VideoDevice();

	if (0 == wcscmp(argv[0], L"list"))
	{
		vdev->findAll();
		return 0;
	}
	else if (0 == wcscmp(argv[0], L"open") && argv[1] != NULL)
	{
		vdev->find(argv[1]);
	}
	else
	{
		std::cout<<"Usage: this.exe list             => list all vedio devices"<<std::endl;
		std::cout<<"       this.exe open device_name => open target device"<<std::endl;
		return 0;
	}

	// icon 
	BYTE ANDmaskIcon[] = {0xFF, 0xFF, 0xFF, 0xFF,   // line 1 
	                      0xFF, 0xFF, 0xC3, 0xFF,   // line 2 
	                      0xFF, 0xFF, 0x00, 0xFF,   // line 3 
	                      0xFF, 0xFE, 0x00, 0x7F,   // line 4 
	 
	                      0xFF, 0xFC, 0x00, 0x1F,   // line 5 
	                      0xFF, 0xF8, 0x00, 0x0F,   // line 6 
	                      0xFF, 0xF8, 0x00, 0x0F,   // line 7 
	                      0xFF, 0xF0, 0x00, 0x07,   // line 8 
	 
	                      0xFF, 0xF0, 0x00, 0x03,   // line 9 
	                      0xFF, 0xE0, 0x00, 0x03,   // line 10 
	                      0xFF, 0xE0, 0x00, 0x01,   // line 11 
	                      0xFF, 0xE0, 0x00, 0x01,   // line 12 
	 
	                      0xFF, 0xF0, 0x00, 0x01,   // line 13 
	                      0xFF, 0xF0, 0x00, 0x00,   // line 14 
	                      0xFF, 0xF8, 0x00, 0x00,   // line 15 
	                      0xFF, 0xFC, 0x00, 0x00,   // line 16 
	 
	                      0xFF, 0xFF, 0x00, 0x00,   // line 17 
	                      0xFF, 0xFF, 0x80, 0x00,   // line 18 
	                      0xFF, 0xFF, 0xE0, 0x00,   // line 19 
	                      0xFF, 0xFF, 0xE0, 0x01,   // line 20 
	 
	                      0xFF, 0xFF, 0xF0, 0x01,   // line 21 
	                      0xFF, 0xFF, 0xF0, 0x01,   // line 22 
	                      0xFF, 0xFF, 0xF0, 0x03,   // line 23 
	                      0xFF, 0xFF, 0xE0, 0x03,   // line 24 
	 
	                      0xFF, 0xFF, 0xE0, 0x07,   // line 25 
	                      0xFF, 0xFF, 0xC0, 0x0F,   // line 26 
	                      0xFF, 0xFF, 0xC0, 0x0F,   // line 27 
	                      0xFF, 0xFF, 0x80, 0x1F,   // line 28 
	 
	                      0xFF, 0xFF, 0x00, 0x7F,   // line 29 
	                      0xFF, 0xFC, 0x00, 0xFF,   // line 30 
	                      0xFF, 0xF8, 0x03, 0xFF,   // line 31 
	                      0xFF, 0xFC, 0x3F, 0xFF};  // line 32 
	 
	BYTE XORmaskIcon[] = {0x00, 0x00, 0x00, 0x00,   // line 1 
	                      0x00, 0x00, 0x00, 0x00,   // line 2 
	                      0x00, 0x00, 0x00, 0x00,   // line 3 
	                      0x00, 0x00, 0x00, 0x00,   // line 4 
	 
	                      0x00, 0x00, 0x00, 0x00,   // line 5 
	                      0x00, 0x00, 0x00, 0x00,   // line 6 
	                      0x00, 0x00, 0x00, 0x00,   // line 7 
	                      0x00, 0x00, 0x38, 0x00,   // line 8 
	 
	                      0x00, 0x00, 0x7C, 0x00,   // line 9 
	                      0x00, 0x00, 0x7C, 0x00,   // line 10 
	                      0x00, 0x00, 0x7C, 0x00,   // line 11 
	                      0x00, 0x00, 0x38, 0x00,   // line 12 
	 
	                      0x00, 0x00, 0x00, 0x00,   // line 13 
	                      0x00, 0x00, 0x00, 0x00,   // line 14 
	                      0x00, 0x00, 0x00, 0x00,   // line 15 
	                      0x00, 0x00, 0x00, 0x00,   // line 16 
	 
	                      0x00, 0x00, 0x00, 0x00,   // line 17 
	                      0x00, 0x00, 0x00, 0x00,   // line 18 
	                      0x00, 0x00, 0x00, 0x00,   // line 19 
	                      0x00, 0x00, 0x00, 0x00,   // line 20 
	 
	                      0x00, 0x00, 0x00, 0x00,   // line 21 
	                      0x00, 0x00, 0x00, 0x00,   // line 22 
	                      0x00, 0x00, 0x00, 0x00,   // line 23 
	                      0x00, 0x00, 0x00, 0x00,   // line 24 
	 
	                      0x00, 0x00, 0x00, 0x00,   // line 25 
	                      0x00, 0x00, 0x00, 0x00,   // line 26 
	                      0x00, 0x00, 0x00, 0x00,   // line 27 
	                      0x00, 0x00, 0x00, 0x00,   // line 28 
	 
	                      0x00, 0x00, 0x00, 0x00,   // line 29 
	                      0x00, 0x00, 0x00, 0x00,   // line 30 
	                      0x00, 0x00, 0x00, 0x00,   // line 31 
	                      0x00, 0x00, 0x00, 0x00};  // line 32 
 
	HICON hIcon = CreateIcon(hInstance,    // application instance  
				             32,              // icon width 
				             32,              // icon height 
				             1,               // number of XOR planes 
				             1,               // number of bits per pixel 
				             ANDmaskIcon,     // AND bitmask  
				             XORmaskIcon);    // XOR bitmask 

    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"Sample Window Class";
    
    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.
    HWND hwndMain = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        NULL,    // Window text
        WS_OVERLAPPEDWINDOW^WS_THICKFRAME^WS_MAXIMIZEBOX^WS_MINIMIZEBOX,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 656, 549,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

	assert(hwndMain!=0);
	
	SetClassLongPtr(hwndMain,          // window handle 
				    GCLP_HICON,              // changes icon 
				    (LONG_PTR) hIcon
				   ); 
	
	//add button
    HWND hwndPushButton = CreateWindowEx(     
		0,
        L"button",                             
        L"SAVE",                                                       
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,                            
        10, 485, 80, 20,                            
        hwndMain,                             
        (HMENU)1001,
        hInstance,        
        NULL);
	assert(hwndPushButton!=0);
	
	//add trackbar
	HWND hwndTrack = CreateWindowEx( 
        0,
        TRACKBAR_CLASS,
        NULL,
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,
        100, 480, 200, 30,
        hwndMain,
        (HMENU)1002,
        hInstance,
        NULL); 
	assert(hwndTrack!=0);
	
    vdev->setMainWin(hwndMain);
	vdev->setZoomBar(hwndTrack);
	vdev->start();

    ShowWindow(hwndMain, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

HRESULT HandleGraphEvent(void)
{
    LONG evCode;
    LONG_PTR evParam1, evParam2;
    HRESULT hr = S_OK;

    if (!vdev->pMediaEvent)
        return E_POINTER;

    while (SUCCEEDED(vdev->pMediaEvent->GetEvent(&evCode, &evParam1, &evParam2, 0)))
    {
        // Insert event processing code here, if desired
        switch (evCode)
        {
        case EC_PALETTE_CHANGED:
            std::cout << "EC_PALETTE_CHANGED" << std::endl;
            break;
        case EC_VIDEO_SIZE_CHANGED:
            std::cout << "EC_VIDEO_SIZE_CHANGED" << std::endl;
            break;
        case EC_CLOCK_CHANGED:
            std::cout << "EC_CLOCK_CHANGED" << std::endl;
            break;
        case EC_PAUSED:
            std::cout << "EC_PAUSED" << std::endl;
            break;
		case EC_STREAM_CONTROL_STARTED: 
	    	std::cout << "EC_STREAM_CONTROL_STARTED" << std::endl;
	    	break;

	    case EC_STREAM_CONTROL_STOPPED: 
	    	std::cout << "EC_STREAM_CONTROL_STOPPED" << std::endl;
	    	break;
        }
    }
	hr = vdev->pMediaEvent->FreeEventParams(evCode, evParam1, evParam2);
    return hr;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_HSCROLL:
		//TBNotifications(wParam,hwndTrack,100u,400u);
		vdev->updateZoomBar(wParam);
		return 0;
    case WM_GRAPHNOTIFY:
        HandleGraphEvent();
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_WINDOWPOSCHANGING:
		RECT rcWin;
		GetWindowRect(hwnd,&rcWin);
		SetWindowPos(hwnd, HWND_TOPMOST, rcWin.left, rcWin.top, rcWin.right - rcWin.left, rcWin.bottom - rcWin.top, SWP_NOSIZE|SWP_NOMOVE);
		return 0;
    case WM_PAINT:
        vdev->paint();
        return 0;
	case WM_COMMAND:
		switch(LOWORD(wParam))                            
    	{                            
        	case 1001:                        
            	vdev->snap();
				return 0; 
		}
		return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
