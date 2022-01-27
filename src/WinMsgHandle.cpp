#ifndef UNICODE
#define UNICODE
#endif 

#include <iostream>
#include <windowsx.h>
#include "WinMsgHandle.h"
#include "resource.h"

//DirectShow graph event
#define WM_GRAPHNOTIFY  WM_APP + 1

static WinMsgHandle* pMsgHandle = nullptr;

void RegistWinMsgHandle(WinMsgHandle& msgHandle)
{
    pMsgHandle = &msgHandle;
}

/*

VideoDevice* vdev;

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
*/
LRESULT CALLBACK WindowProc(HWND hwnd,
                                UINT uMsg,
                                WPARAM wParam,
                                LPARAM lParam)
{
    int xPos,yPos;
    switch (uMsg)
    {
        case WM_MOUSEMOVE:
        
            xPos = LOWORD(lParam); 
            yPos = HIWORD(lParam);

            break;
            
        case WM_LBUTTONDOWN:
            std::cout<<"WM_LBUTTONDOWN"<<std::endl;

            xPos = LOWORD(lParam); 
            yPos = HIWORD(lParam);
            std::cout<<xPos<<','<<yPos<<std::endl;
            //MessageBox(hwnd,L"Content",L"Help",MB_OK);

            break;
        case WM_HSCROLL:
            std::cout<<"WM_HSCROLL"<<std::endl;
            //TBNotifications(wParam,hwndTrack,100u,400u);
            //vdev->updateZoomBar(wParam);
            return 0;
        case WM_GRAPHNOTIFY:
            std::cout<<"WM_GRAPHNOTIFY"<<std::endl;
            //HandleGraphEvent();
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        /*
        case WM_WINDOWPOSCHANGING:

            RECT rcWin;
            GetWindowRect(hwnd,&rcWin);
            SetWindowPos(hwnd,
                HWND_TOP,
                rcWin.left,
                rcWin.top,
                rcWin.right - rcWin.left,
                rcWin.bottom - rcWin.top,
                SWP_NOSIZE|SWP_NOMOVE); //keep current size & pos

            return 0;
        */
        case WM_PAINT:
            if (pMsgHandle) pMsgHandle->paint();
            return 0;
        case WM_COMMAND:
            int lowWP  = LOWORD(wParam);
            int highWP = HIWORD(wParam);
            HWND hCtrl = reinterpret_cast<HWND>(lParam);
            //std::cout<<lowWP<<','<<highWP<<','<<std::hex<<(unsigned int)hCtrl<<std::endl;
            switch(lowWP)                            
            {                            
                case IDC_SNAP_BUTTON:
                    std::cout<<"WM_COMMAND:IDC_SNAP_BUTTON"<<std::endl;
                    if (pMsgHandle) pMsgHandle->snap();
                    return 0; 
                case IDC_CAP_BUTTON:
                    std::cout<<"WM_COMMAND:IDC_CAP_BUTTON"<<std::endl;
                    if (pMsgHandle)
                    {
                        static bool bStarted = false;
                        if (!bStarted)
                        {
                            SetWindowText(hCtrl,L"STOP  Cap");
                            pMsgHandle->capture(true);
                            bStarted = true;
                        }
                        else
                        {
                            SetWindowText(hCtrl,L"START Cap");
                            pMsgHandle->capture(false);
                            bStarted = true;
                        }
                    }
                    //pMsgHandle->paint();
                    return 0;
                case ID_SHORTCUT_SWITCH_DEVICE:
                    std::cout<<"WM_COMMAND:ID_SHORTCUT_SWITCH_DEVICE"<<std::endl;
                    return 0;
                case ID_SHORTCUT_SWITCH_FORMAT:
                    std::cout<<"WM_COMMAND:ID_SHORTCUT_SWITCH_FORMAT"<<std::endl;
                    return 0;
                case ID_SHORTCUT_SAVE_IMAGE:
                    std::cout<<"WM_COMMAND:ID_SHORTCUT_SAVE_IMAGE"<<std::endl;
                    return 0;
                case ID_SHORTCUT_CAPTURE:
                    std::cout<<"WM_COMMAND:ID_SHORTCUT_CAPTURE"<<std::endl;
                    return 0;
                case IDC_DEVICE_COMBO:
                    if(highWP == CBN_SELCHANGE)
                    {
                        //get select device
                        wchar_t devideName[256];
                        int index = ComboBox_GetCurSel(hCtrl);
                        ComboBox_GetLBText(hCtrl,index,devideName);
                        pMsgHandle->reset(devideName);
                    }
                    return 0;
                case IDC_FORMAT_COMBO:
                case IDC_RESOLUTION_COMBO:
                    if(highWP == CBN_SELCHANGE)
                    {
                        //open device and reset
                        pMsgHandle->reset(L"");
                    }
                    return 0;
            }
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

