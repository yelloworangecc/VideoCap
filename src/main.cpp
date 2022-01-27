#ifndef UNICODE
#define UNICODE
#endif

#include <iostream>
#include <cstdio>
#include <vector>
#include <windows.h>
#include "MainWindow.h"
#include "VideoStreamRender.h"
#include "WinMsgHandle.h"
#include "resource.h"

int WINAPI wWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       PWSTR pCmdLine,
                       int nCmdShow)
{
    wchar_t ** argv;
    int argc;
    VideoStreamRender vsr;
    wchar_t* pDeviceName = NULL;
    int deviceIndex = 0;
    int formatIndex = 0;
    int resolutionIndex = 0;
    
    //parse command line
    argv = CommandLineToArgvW(pCmdLine,&argc);
    if (0 == wcscmp(argv[0], L"list"))
    {
        AllocConsole();
        freopen("conout$","w",stdout);
        vsr.listDevice();
        system("pause");
        FreeConsole();
        return 0;
    }
    //user specified device
    if (argc == 2 && 0 == wcscmp(argv[0], L"open"))
    {
        pDeviceName = argv[1];
    }
    
    RegistWinMsgHandle(vsr);
    
    std::vector<std::wstring> deviceList = vsr.listDevice();
    if (deviceList.empty()) return -1;

    //is user specified device
    if (pDeviceName != NULL) deviceIndex = vsr.getDeviceIndex(pDeviceName);
    vsr.open(pDeviceName != NULL  ? pDeviceName:deviceList[0]);
    if(!vsr.isOpen()) return -1;
    
    std::vector<std::wstring> formatList = vsr.listFormat();
    if (formatList.empty()) return -1;
    
    std::vector<std::wstring> resolutionList = vsr.listResolution();
    if (resolutionList.empty()) return -1;


    MainWindow mainWindow(hInstance);
    mainWindow.addIcon();
    mainWindow.addDeviceCombo(deviceList, deviceIndex);
    mainWindow.addFormatCombo(formatList, formatIndex);
    mainWindow.addResolutionCombo(resolutionList,resolutionIndex);
    mainWindow.addSnapButton();
    mainWindow.addCapButton();

    vsr.set(&mainWindow,formatList[formatIndex],resolutionList[resolutionIndex]);
    vsr.render();
    if(!vsr.isRun()) return 0;
    vsr.mixBitmap(L"IDB_INFO_KNS");

    
    HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR_TALBE));

    ShowWindow(mainWindow.getHandle(), SW_SHOW);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(mainWindow.getHandle(), hAccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}
