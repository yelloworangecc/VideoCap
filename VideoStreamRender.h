#ifndef _VIDEO_STREAM_RENDER_H_
#define _VIDEO_STREAM_RENDER_H_

#include <string>
#include <dshow.h>
#include <D3d9.h>
#include <vmr9.h>
#include <d2d1.h>
#include <dwrite.h>
#include "WinMsgHandle.h"

struct FrameFormat
{
    GUID videoType;
    GUID formatType;
    long width;
    long height;
    long long rate;
};

class VideoStreamRender: public WinMsgHandle
{
public:

    static void listDevice();

    VideoStreamRender(const std::wstring        &_deviceName);
    VideoStreamRender(const wchar_t *pDeviceName);
    ~VideoStreamRender();

    //common functions
    void open();
    void close();
    void* getImage();
    void listFormat();
    void setFormat(const GUID &videoType,
                      const GUID &formatType,
                      long width,
                      long height);
    void render(HWND hwnd, int heightCtrlBar);
    void mixBitmap(HINSTANCE hApp, const wchar_t *bitmapName);

    //windows msg handle functions
    void paint();
    void snap();
    void capture(const bool bStart);
    bool isPointed(int xPos, int yPos);
    
    bool isOpen(){return bOpen;}
    bool isRun(){return bRun;}
    bool isCreateRes(){return bCreateRes;}

    static DWORD WINAPI CapProc(LPVOID lpParam);
    static bool bCapStarted;
private:
    IPin* getCapturePin();
    void createD2DRes();
    void printFormat(FrameFormat & fmt);

    bool bOpen,bRun,bCreateRes;
    std::wstring deviceName;
    HWND renderWin;
    RECT videoRect,winRect,targetRect;
    IBaseFilter *pCaptureFilter;
    ICaptureGraphBuilder2 *pCaptureGraphBuilder;
    IGraphBuilder *pGraphBuilder;
    IBaseFilter *pVMRFilter;
    IVMRWindowlessControl9 *pVMRControl;
    IMediaControl *pMediaControl;

    ID2D1Factory *pD2D1Factory;
    IDWriteFactory *pDWriteFactory;
    IDWriteTextFormat *pDWriteTextFormat;
    ID2D1DCRenderTarget *pRenderTarget;
    ID2D1SolidColorBrush* pBlackBrush;
};

#endif