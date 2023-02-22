#ifndef _VIDEO_CAPTURE_H_
#define _VIDEO_CAPTURE_H_

#include <string>
#include <vector>
#include <functional>
#include <strmif.h>
#include <aviriff.h>
#include <dvdmedia.h>
#include <bdaiface.h>
#include <dshow.h>
#include <D3d9.h>
#include <vmr9.h>

#define WM_GRAPHNOTIFY  WM_APP + 1

#define FLIP_NONE 0
#define FLIP_HORIZONTAL 1
#define FLIP_VERTICAL 2
#define FLIP_BOTH 3

struct Format;
struct Device;
struct VmrRender;
struct GrabberNullRender;
class SampleGrabberCallback;
class TransformFilter;

extern std::string Unicode2Ansi(const wchar_t* wcharBuff);
extern std::string Unicode2Ansi(const std::wstring& wstr);
extern std::wstring Ansi2Unicode(const char* charBuff);
extern std::wstring Ansi2Unicode(const std::string& str);

class VideoCapture
{
public:
    VideoCapture();
    ~VideoCapture();

    
    const std::vector<std::wstring>& getDeviceNameList();
    const std::vector<std::wstring>& getFormatNameList(const wchar_t* pDeviceName);
    const std::vector<std::wstring>& getResolutionList(const wchar_t* pDeviceName,const wchar_t* pFormatName);

    void initialize();
    void uninitialize();
    int enumDevices();
    void enumFormats(IAMStreamConfig* pAMStreamConfig, std::vector<Format>& formatList);
    void createRender();
    void releaseRender();
    void createGraph();
    void releaseGraph();
    void setRotationDegree(int degree);
    void setGridInterval(int interval);
    
    void printDevices();
    void clean();
    void addDeviceFilter(
        const wchar_t* pDeviceName,
        const wchar_t* pFormatName,
        const wchar_t* pResolution);
    void setDeviceFilter();
    void addTransform(const wchar_t* pResolution);
    void addVmr();
    void renderVmr(HWND hWin);
    void setVmrRenderPosition(RECT& dstRect);
    void notifyWindow(HWND hWin);
    void setReferenceClock();
    void setMediaController();
    void setup(
        const wchar_t* pDeviceName,
        const wchar_t* pFormatName,
        const wchar_t* pResolution,
        HWND hWin,
        RECT& dstRect,
        HDC hMemDc);
    //void renderCapture(HWND hWin, RECT& dstRec);
    void overlapLogo(HDC hMemDc, long width, long height);
    void addGrabber();
    void renderGrabber();
    void start();
    void pause();
    void stop();
    bool isStreamStarted(){return bStreamStarted;}
    void flip(int direction);
    void snap();
    void paint(HWND hWin, HDC hdc);
    void startCapture();
    void stopCapture();
    bool isCaptureStarted(){return bCaptureStarted;}
    void freeDevices();
    Device* getDevice(const wchar_t* pName);
    Format* getFormat(
        std::vector<Format>&formatList,
        const wchar_t* pTypeName,
        const wchar_t* pResolution);
    Format* getMaxFormat(std::vector<Format>&formatList, const wchar_t* pTypeName);
    void disconnectFilter(IBaseFilter *pFilter);
    void disconnectAll();
    void removeAll();
    void* getCurrentFrame();
    const AM_MEDIA_TYPE* getCurrentMediaType();
    
    void setSaveName(const std::string& name){saveName=Ansi2Unicode(name);}
    void setSaveName(const std::wstring& name){saveName = name;}
    std::string getSaveName(){return Unicode2Ansi(saveName);}
    std::wstring getSaveNameW(){return saveName;}
    void setSavePath(const std::string& path){savePath = Ansi2Unicode(path);}
    void setSavePath(const std::wstring& path){savePath = path;}
    std::string getSavePath(){return Unicode2Ansi(savePath);}
    std::wstring getSavePathW(){return savePath;}

    long getStreamWidth(){return streamWidth;}
    long getStreamHeight(){return streamHeight;}
    long getFramRate(){return streamFrameRate;}
    IMediaEventEx* getMediaEvent(){return pIMediaEventEx;}
    bool isVrmRendered() { return bVrmRendered; }
    
private:
    std::vector<Device> deviceList;
    
    std::vector<std::wstring> deviceNameList;
    std::vector<std::wstring> formatNameList;
    std::vector<std::wstring> resolutionList;

    ICaptureGraphBuilder2* pICaptureGraphBuilder;
    IGraphBuilder* pIGraphBuilder;
    SampleGrabberCallback* pStillCapCB;
    IMediaEventEx* pIMediaEventEx;
    IMediaControl* pIMediaControl;
    
    bool bStreamStarted;
    bool bCaptureStarted;
    bool bVrmRendered;
    std::wstring saveName;
    std::wstring savePath;
    
    long streamWidth;
    long streamHeight;
    long streamFrameRate;

    Device* pCurrentDevice;
    Format* pCurrentFormat;

    TransformFilter* pTransformFilter;
    VmrRender* pVmr;
    GrabberNullRender* pGrab;
};

#endif
