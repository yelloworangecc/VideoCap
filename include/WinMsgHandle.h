#ifndef _WIN_MSG_HANDLE_
#define _WIN_MSG_HANDLE_

#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd,
                                UINT uMsg,
                                WPARAM wParam,
                                LPARAM lParam);

class WinMsgHandle
{
public:
    virtual void open(const std::wstring deviceName) = 0;
    virtual void paint() = 0;
    virtual void snap() = 0;
    virtual void capture(const bool bStart) = 0;
    virtual void reset(const std::wstring deviceName) = 0;
};

void RegistWinMsgHandle(WinMsgHandle& msgHandle);

#endif
