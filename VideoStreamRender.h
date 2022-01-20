#ifndef _VIDEO_STREAM_RENDER_H_
#define _VIDEO_STREAM_RENDER_H_

#include <string>
#include <vector>
#include <dshow.h>
#include <D3d9.h>
#include <vmr9.h>
#include <d2d1.h>
#include <dwrite.h>
#include "WinMsgHandle.h"

//video type name and it's GUID
struct VideoType
{
    std::wstring name;
    GUID guid;
};

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
    //static function members
    static DWORD WINAPI CapProc(LPVOID lpParam);
    static GUID findVideoType(const std::wstring & name);
    static std::wstring findVideoType(const GUID & guid);
    GUID VideoStreamRender::findFormatType(const std::wstring& name);
    std::wstring VideoStreamRender::findFormatType(const GUID& guid);

    //constructions and destructions
    VideoStreamRender();
    ~VideoStreamRender();

    //common functions
    const std::vector<std::wstring> &listDevice();
    int getDeviceIndex(std::wstring deviceName);
    void open(const std::wstring deviceName);
    const std::vector<std::wstring> &listFormat();
    const std::vector<std::wstring> &listResolution();
    void close();
    void* getImage();
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
    //bool isPointed(int xPos, int yPos);

    //status check functions
    bool isOpen(){return bOpen;}
    bool isRun(){return bRun;}
    bool isCreateRes(){return bCreateRes;}

    //static data members
    static bool bCapStarted;
    static const VideoType allVideoTypes[];
private:
    void createDeviceEnumerator();
    void releaseDeviceEnumerator();
    void createStreamConfiger();
    void releaseStreamConfiger();
    IPin* getCapturePin();
    //void createD2DRes();
    void printFormat(FrameFormat & fmt);
    void uniqueAppend(std::vector<std::wstring>& list, const std::wstring newItem);

    bool bOpen,bRun,bCreateRes;
    HWND renderWin;
    RECT videoRect,winRect,targetRect;
    //device open related members
    IBaseFilter *pCaptureFilter;
    ICaptureGraphBuilder2 *pCaptureGraphBuilder;
    IGraphBuilder *pGraphBuilder;
    //render related members
    IBaseFilter *pVMRFilter;
    IVMRWindowlessControl9 *pVMRControl;
    IMediaControl *pMediaControl;
    //config related numbers
    IEnumMoniker *pEnumMoniker;
    IAMStreamConfig *pStreamConfig;

    std::vector<std::wstring> deviceList;
    std::vector<std::wstring> formatList;
    std::vector<std::wstring> resolutionList;

    //ID2D1Factory *pD2D1Factory;
    //IDWriteFactory *pDWriteFactory;
    //IDWriteTextFormat *pDWriteTextFormat;
    //ID2D1DCRenderTarget *pRenderTarget;
    //ID2D1SolidColorBrush* pBlackBrush;
};

#endif