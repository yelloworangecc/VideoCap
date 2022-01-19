#ifndef UNICODE
#define UNICODE
#endif

#include <stdio.h>  // printf, _dup2
#include <io.h>     // _open_osfhandle
#include <iostream>
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
    wchar_t* pDeviceName;

    argv = CommandLineToArgvW(pCmdLine,&argc);
    
    //AttachConsole(ATTACH_PARENT_PROCESS);
    //freopen("conout$","w",stdout);

    if (0 == wcscmp(argv[0], L"list"))
    {
        VideoStreamRender::listDevice();
        return 0;
    }
    else if (0 == wcscmp(argv[0], L"open") && argv[1] != NULL)
    {
        pDeviceName = argv[1];
    }
    else
    {
        std::wcout<<L"Usage: this.exe list             => list all video devices"<<std::endl;
        std::wcout<<L"       this.exe open device_name => open target video device"<<std::endl;
        return 0;
    }

    VideoStreamRender vsr(pDeviceName);
    vsr.open();
    vsr.listFormat();
    vsr.setFormat(MEDIASUBTYPE_YUY2,FORMAT_VideoInfo,640,480);

    MainWindow mainWindow(hInstance,640, 480);
    mainWindow.addIcon();
    mainWindow.addDeviceCombo();
    mainWindow.addSnapButton();
    mainWindow.addCapButton();

    vsr.render(mainWindow.getHandle(), 30);
    vsr.mixBitmap(hInstance, L"IDB_INFO_GRN");
    if(!vsr.isRun()) return 0;

    RegistWinMsgHandle(vsr);
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
