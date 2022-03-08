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

struct Format;
struct Device;
struct VmrRender;
struct GrabberNullRender;
class SampleGrabberCallback;
/**V I D E O   C A P T U R E***********************************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022211
 * Description: Video capture class using direct show
 * Members: 
   - 
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
class VideoCapture
{
public:
    VideoCapture();
    ~VideoCapture();

    
    const std::vector<std::wstring>& getDeviceNameList();
    const std::vector<std::wstring>& getFormatNameList(const wchar_t* pDeviceName);
    const std::vector<std::wstring>& getResolutionList(const wchar_t* pDeviceName,const wchar_t* pFormatName);

    void initialize(const std::string& savePath);
    void uninitialize();
    int enumDevices();
    void enumFormats(IAMStreamConfig* pAMStreamConfig, std::vector<Format>& formatList);
    void createRender();
    void releaseRender();
    void createGraph();
    void releaseGraph();

    
    void printDevices();
    void clean();
    void addDeviceFilter(
        const wchar_t* pDeviceName,
        const wchar_t* pFormatName,
        const wchar_t* pResolution);
    void setDeviceFilter();
    void addVmr();
    void renderVmr(HWND hWin);
    RECT getCurrentVideoRect();
    void setVmrRenderPosition(RECT& dstRect);
    void notifyWindow(HWND hWin);
    void setReferenceClock();
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
    void snap();
    void paint(HWND hWin, HDC hdc);
    void startCapture();
    void stopCapture();
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

    long getStreamWidth(){return streamWidth;}
    long getStreamHeight(){return streamHeight;}
    long getFramRate(){return streamFrameRate;}
    bool isCaptureStarted(){return bCaptureStarted;}
    const std::string getSavePath(){return savePath;}
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
    
    bool bStreamStarted;
    bool bCaptureStarted;
    bool bVrmRendered;
    std::string savePath;
    
    long streamWidth;
    long streamHeight;
    long streamFrameRate;

    Device* pCurrentDevice;
    Format* pCurrentFormat;

    VmrRender* pVmr;
    GrabberNullRender* pGrab;
};

#endif
