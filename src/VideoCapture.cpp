#ifndef UNICODE
#define UNICODE
#endif
#include <iostream>
#include <sstream>
#include <ctime>
#include <cassert>
#include <strmif.h>
#include <winnt.h>
#include <opencv2/opencv.hpp>

#include "streams.h"
#include "VideoFileWriter.h"
#include "VideoCapture.h"
#include "Shapes.h"

std::string getLocalTimeString();
void FreeMediaType(AM_MEDIA_TYPE *pMediaType);
GUID findVideoType(const std::wstring & name);
std::wstring findVideoType(const GUID & guid);
GUID findFormatType(const std::wstring & name);
std::wstring findFormatType(const GUID & guid);
void uniqueAppend(std::vector<std::wstring>& list, const std::wstring newItem);
void uniqueAppend(std::vector<std::wstring>& list, const wchar_t* newItem);
IPin* getFilterPin(IBaseFilter* pFilter, wchar_t * pPinName);
void printPinInfo(IBaseFilter* pSourceFilter, wchar_t* pPinName);
void printDeviceInfo(wchar_t* pDeviceName,IBaseFilter* pSourceFilter);
int compareResolutionString(const wchar_t* r1, const wchar_t* r2);

#ifndef GUID_STRING_SIZE
#define GUID_STRING_SIZE 40
#endif

#include <initguid.h>
DEFINE_GUID(CLSID_NullRenderer, 0xc1f400a4, 0x3f08, 0x11d3, 0x9f, 0x0b, 0x00, 0x60, 0x08, 0x03, 0x9e, 0x37);
DEFINE_GUID(CLSID_SampleGrabber, 0xc1f400a0, 0x3f08, 0x11d3, 0x9f, 0x0b, 0x00, 0x60, 0x08, 0x03, 0x9e, 0x37);
DEFINE_GUID(IID_ISampleGrabber, 0x6b652fff, 0x11fe, 0x4fce, 0x92, 0xad, 0x02, 0x66, 0xb5, 0xd7, 0xc7, 0x8f);
DEFINE_GUID(CLSID_TransformFilter, 0x1915c5c7, 0x2aa, 0x415f, 0x89, 0xf, 0x76, 0xd9, 0x4c, 0x85, 0xaa, 0xf1);
DEFINE_GUID(MEDIASUBTYPE_I420, 0x30323449, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

//directshow filter for rotation transforming
class TransformFilter: public CTransInPlaceFilter
{
public:
    TransformFilter(int _width = 0, int _height = 0)
        :CTransInPlaceFilter(L"Transform Filter", 0, CLSID_TransformFilter, 0), size(_width, _height),
        rotationdegree(0), gridInterval(0),flipStatus(0)
    {
        
    }
        
    ~TransformFilter()
    {
        std::cout<<"TransformFilter destructed"<<std::endl;
    }
    
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT * pHr)
    {
        TransformFilter* pFilter = new TransformFilter();
        if (pFilter == NULL)
        {
            *pHr = E_OUTOFMEMORY;
        }
        return pFilter;
    }
    
    HRESULT CheckInputType(const CMediaType * pMediaType)
    {
        if ((pMediaType->majortype != MEDIATYPE_Video) ||
            (pMediaType->subtype != MEDIASUBTYPE_RGB32) ||
            (pMediaType->formattype != FORMAT_VideoInfo) //|| 
            //(pMediaType->cbFormat < sizeof(VIDEOINFOHEADER))
            )
        {
            return VFW_E_TYPE_NOT_ACCEPTED;
        }

        VIDEOINFOHEADER* pVih =
            reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
        if (//(pVih->bmiHeader.biBitCount != 8) ||
            (pVih->bmiHeader.biCompression != BI_RGB))
        {
            return VFW_E_TYPE_NOT_ACCEPTED;
        }

        /* Check the palette table.
        if (pVih->bmiHeader.biClrUsed > PALETTE_ENTRIES(pVih))
        {
            return VFW_E_TYPE_NOT_ACCEPTED;
        }
        DWORD cbPalette = pVih->bmiHeader.biClrUsed * sizeof(RGBQUAD);
        if (BITMAPINFOHEADER->cbFormat < sizeof(VIDEOINFOHEADER) + cbPalette)
        {
            return VFW_E_TYPE_NOT_ACCEPTED;
        }*/

        return S_OK;
    }
    
    HRESULT Transform(IMediaSample * pSample)
    {
        // Get pointers to the underlying buffers.
        BYTE* pBuffer;
        //long dataLen;
        HRESULT hr = pSample->GetPointer(&pBuffer);
        if (FAILED(hr))
        {
            std::cout << "get sample pointer failed" << std::endl;
            return hr;
        }
        //create Mat
        cv::Mat image(size,
                CV_8UC4,
                (void*)(pBuffer),
                cv::Mat::AUTO_STEP
            );
        int width = image.size().width;
        int height = image.size().height;
        
        //flip
        switch(flipStatus)
        {
            case FLIP_HORIZONTAL:
                cv::flip(image,image,0);
                break;
            case FLIP_VERTICAL:
                cv::flip(image,image,1);
                break;
            case FLIP_BOTH:
                cv::flip(image,image,-1);
                break;
            default:
                break;
        }
        
        // rotate
        if (rotationdegree && size.width>0 && size.height>0)
        {
            cv::Mat ratationM = cv::getRotationMatrix2D(cv::Point2f((float)size.width / 2, (float)size.height / 2), (double)rotationdegree, 1.0);
            cv::warpAffine(image, image, ratationM, size);
        }

        /*
        // add grid
        if (gridInterval > 0)
        {
            // horizontal line
            int pos = height/2;
            while(pos < height)
            {
                cv::line(image,cv::Point(0,pos),cv::Point(width,pos),cv::Scalar(255,255,255),1);
                pos += gridInterval;
            }
            pos = image.size().height/2-1;
            while(pos >= 0)
            {
                cv::line(image,cv::Point(0,pos),cv::Point(width,pos),cv::Scalar(255,255,255),1);
                pos -= gridInterval;
            }
            // vertical line
            pos = width/2;
            while(pos  <width)
            {
                cv::line(image,cv::Point(pos,0),cv::Point(pos,height),cv::Scalar(255,255,255),1);
                pos += gridInterval;
            }
            pos = image.size().width/2-1;
            while(pos >= 0)
            {
                cv::line(image,cv::Point(pos,0),cv::Point(pos,height),cv::Scalar(255,255,255),1);
                pos -= gridInterval;
            }
        }*/
        ShapeCollection& shapes = ShapeCollection::get();
        shapes.draw(image);

        return S_OK;
    }
    
    void setRotationDegree(int degree)
    {
        this->rotationdegree = degree;
    }

    void setGridInterval(int interval)
    {
        ShapeCollection& shapes = ShapeCollection::get();
        shapes.resetGrid(size.width,size.height,interval);
    }

    void updateFlipStatus(int direction)
    {
        flipStatus = flipStatus ^ direction;
    }
private:
    cv::Size size;
    int rotationdegree;
    int gridInterval;
    int flipStatus;
    
};

//global variable used by Directshow base classes
CFactoryTemplate g_Templates[] =
{
  {
    L"Transform Filter",
    &CLSID_TransformFilter,
    TransformFilter::CreateInstance,
    NULL,
    NULL
  }
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

//sample grabber callback interface
interface ISampleGrabberCB : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE SampleCB(
        double SampleTime,
        IMediaSample* pSample) = 0;

    virtual HRESULT STDMETHODCALLTYPE BufferCB(
        double SampleTime,
        BYTE* pBuffer,
        LONG BufferLen) = 0;
};

//sample grabber setup interface
interface ISampleGrabber : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE SetOneShot(
        BOOL OneShot) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetMediaType(
        const AM_MEDIA_TYPE* pType) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType(
        AM_MEDIA_TYPE* pType) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetBufferSamples(
        BOOL BufferThem) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer(
        LONG* pBufferSize,
        LONG* pBuffer) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetCurrentSample(
        IMediaSample** ppSample) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetCallback(
        ISampleGrabberCB* pCallback,
        LONG WhichMethodToCallback) = 0;
};

//implementation of sample grabber callback 
class SampleGrabberCallback : public ISampleGrabberCB
{
public:
    SampleGrabberCallback(VideoCapture* _pVideoCapture)
        :pVideoCapture(_pVideoCapture)
    {}
    STDMETHODIMP_(ULONG) AddRef() { return 1; }
    STDMETHODIMP_(ULONG) Release() { return 2; }

    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
    {
        std::cout<<"IN QueryInterface"<<std::endl;
        
        *ppvObject = static_cast<ISampleGrabberCB*>(this);
        return S_OK;
    }

    STDMETHODIMP SampleCB(double Time, IMediaSample *pSample)
    {
        std::cout<<"IN SampleCB"<<std::endl;
        return E_NOTIMPL;
    }

    STDMETHODIMP BufferCB(double Time, BYTE *pBuffer, long BufferLen)
    {
        const AM_MEDIA_TYPE* pMediaType = pVideoCapture->getCurrentMediaType();
        if (!pMediaType)
        {
            std::cout << "No valid media type found" << std::endl;
            return S_OK;
        }
        const VIDEOINFOHEADER* pVideoInfo = (VIDEOINFOHEADER*)pMediaType->pbFormat;
        std::string fileName = "still-" + getLocalTimeString() + ".bmp";
        pVideoCapture->setSaveName(fileName);
        std::string path = pVideoCapture->getSavePath() + fileName;
        long width = pVideoInfo->bmiHeader.biWidth;
        long height = pVideoInfo->bmiHeader.biHeight;
        std::cout << BufferLen << ',' << path << ',' << width << ',' << height << std::endl;
        VideoFileWriter::WriteImageFile(
            path,
            width,
            height,
            pBuffer);
        return S_OK;
    }
private:
    VideoCapture* pVideoCapture;
};

//video type name and it's GUID
struct VideoType
{
    std::wstring name;
    GUID guid;
};


const VideoType allVideoTypes[] = { 
    {L"IYUV",  MEDIASUBTYPE_IYUV} , 
    {L"RGB1",  MEDIASUBTYPE_RGB1} , 
    {L"RGB24", MEDIASUBTYPE_RGB24} , 
    {L"RGB32", MEDIASUBTYPE_RGB32} , 
    {L"RGB4",  MEDIASUBTYPE_RGB4} , 
    {L"RGB555",MEDIASUBTYPE_RGB555} , 
    {L"RGB565",MEDIASUBTYPE_RGB565} , 
    {L"RGB8",  MEDIASUBTYPE_RGB8} , 
    {L"YUY2",  MEDIASUBTYPE_YUY2} , 
    {L"YV12",  MEDIASUBTYPE_YV12} , 
    {L"YVU9",  MEDIASUBTYPE_YVU9} , 
    {L"YVYU",  MEDIASUBTYPE_YVYU} ,
    {L"MJPG",  MEDIASUBTYPE_MJPG} ,
    {L"I420",  MEDIASUBTYPE_I420}
};

///////////////////////////  HANDY FUNCTIONS  /////////////////////////////
std::string getLocalTimeString()
{
    time_t t = time(NULL);
    struct tm currentTime;
    localtime_s(&currentTime, &t);
    std::stringstream ss;
    ss<<currentTime.tm_year + 1900
      <<std::setw(2)<<std::setfill('0')
      <<currentTime.tm_mon + 1
      <<currentTime.tm_mday
      <<currentTime.tm_hour
      <<currentTime.tm_min
      <<currentTime.tm_sec;
    return ss.str();
}

void FreeMediaType(AM_MEDIA_TYPE *pMediaType)
{
    if (pMediaType)
    {
        if (pMediaType->cbFormat != 0 && pMediaType->pbFormat)
        {
            CoTaskMemFree((PVOID)pMediaType->pbFormat);
            pMediaType->cbFormat = 0;
            pMediaType->pbFormat = nullptr;
        }
        if (pMediaType->pUnk)
        {
            pMediaType->pUnk->Release();
            pMediaType->pUnk = nullptr;
        }
        CoTaskMemFree((pMediaType));
    	pMediaType = nullptr;
    }
}

GUID findVideoType(const std::wstring & name)
{
    for (int i = 0; i < sizeof(allVideoTypes)/sizeof(allVideoTypes[0]); ++ i)
    {
        if (allVideoTypes[i].name.compare(name) == 0) return allVideoTypes[i].guid;
    }
    return GUID_NULL;
}

std::wstring findVideoType(const GUID & guid)
{
    for (int i = 0; i < sizeof(allVideoTypes)/sizeof(allVideoTypes[0]); ++ i)
    {
        if (allVideoTypes[i].guid == guid) return allVideoTypes[i].name;
    }
    wchar_t buffer[GUID_STRING_SIZE] = { 0 };
    StringFromGUID2(guid, buffer, GUID_STRING_SIZE);
    return std::wstring(buffer);
}

GUID findFormatType(const std::wstring & name)
{
    if (name == std::wstring(L"FORMAT_VideoInfo"))
    {
        return FORMAT_VideoInfo;
    }
    if (name == std::wstring(L"FORMAT_VideoInfo"))
    {
        return FORMAT_VideoInfo2;
    }
    return GUID_NULL;
}

std::wstring findFormatType(const GUID & guid)
{
    if (guid == FORMAT_VideoInfo)
    {
        return std::wstring(L"FORMAT_VideoInfo");
    }
    if (guid == FORMAT_VideoInfo2)
    {
        return std::wstring(L"FORMAT_VideoInfo2");
    }
    wchar_t buffer[GUID_STRING_SIZE] = { 0 };
    StringFromGUID2(guid, buffer, GUID_STRING_SIZE);
    return std::wstring(buffer);
}

void uniqueAppend(std::vector<std::wstring>& list, const std::wstring newItem)
{
    for(auto item: list)
    {
        if (item.compare(newItem) == 0) return;
    }
    list.emplace_back(newItem);
}

void uniqueAppend(std::vector<std::wstring>& list, const wchar_t* newItem)
{
    for(auto item: list)
    {
        if (item.compare(newItem) == 0) return;
    }
    list.emplace_back(newItem);
}


IPin* getFilterPin(IBaseFilter* pFilter, wchar_t * pPinName)
{
    IEnumPins *pEnumPins;
    IPin *pPin;
    PIN_INFO pinInfo;
    HRESULT hr;
    
    //if (!isOpen()) return nullptr;
    hr = pFilter->EnumPins(&pEnumPins);
    if (FAILED(hr))
    {
        std::cout<<"IBaseFilter.EnumPins failed"<<std::endl;
        return nullptr;
    }

    while (pEnumPins->Next(1, &pPin, NULL) == S_OK)
    {
        hr = pPin->QueryPinInfo(&pinInfo);
        if (SUCCEEDED(hr) && wcscmp(pPinName,pinInfo.achName) == 0)
        {
            pEnumPins->Release();
            return pPin;
        }
        pPin->Release();
    }
    pEnumPins->Release();
    return nullptr;
}

void printPinInfo(IBaseFilter* pSourceFilter, wchar_t* pPinName)
{
    HRESULT hr;
    int piCount, piSize;
    IAMStreamConfig* pAMStreamConfig;
    AM_MEDIA_TYPE* pMediaType;
    VIDEO_STREAM_CONFIG_CAPS scc;
    IPin* pCapPin;
    std::vector<std::wstring> typeList;

    //get capture pin
    pCapPin = getFilterPin(pSourceFilter, pPinName);
    if (pCapPin == nullptr)
    {
        std::cout << "find pin failed" << std::endl;
        return;
    }

    //query stream config interface
    hr = pCapPin->QueryInterface(IID_IAMStreamConfig, (void**)&pAMStreamConfig);
    if (FAILED(hr))
    {
        std::cout << "IPin.QueryInterface IAMStreamConfig failed" << std::endl;
        return;
    }
    pCapPin->Release();

    //get capability count
    hr = pAMStreamConfig->GetNumberOfCapabilities(&piCount, &piSize);
    if (FAILED(hr))
    {
        std::cout << "IAMStreamConfig.GetNumberOfCapabilities failed" << std::endl;
        return;
    }

    //loop all capabilities and get video type
    for (int i = 0; i < piCount; i++)
    {
        pAMStreamConfig->GetStreamCaps(i, &pMediaType, reinterpret_cast<BYTE*>(&scc));
        if (FORMAT_VideoInfo == pMediaType->formattype)
        {
            std::wstring videoType = findVideoType(pMediaType->subtype);
            uniqueAppend(typeList, videoType);
        }
        FreeMediaType(pMediaType);
    }

    //loop all video type
    for (auto item : typeList)
    {
        std::wcout << "    " << item << std::endl;
        //loop all capabilities again and get resolutions
        for (int i = 0; i < piCount; i++)
        {
            pAMStreamConfig->GetStreamCaps(i, &pMediaType, reinterpret_cast<BYTE*>(&scc));
            if (FORMAT_VideoInfo == pMediaType->formattype && findVideoType(pMediaType->subtype) == item)
            {
                VIDEOINFOHEADER* pvideoInfo = (VIDEOINFOHEADER*)pMediaType->pbFormat;
                std::cout << "      " << pvideoInfo->bmiHeader.biWidth << " x " << pvideoInfo->bmiHeader.biHeight << std::endl;
            }
            FreeMediaType(pMediaType);
        }
    }
    pAMStreamConfig->Release();
}

void printDeviceInfo(wchar_t* pDeviceName,IBaseFilter* pSourceFilter)
{
    
    std::wcout<<L"\nDEVICE: "<< pDeviceName <<L"\n  PORT: Capture (for video image)"<<std::endl;
    printPinInfo(pSourceFilter,L"Capture");
    std::wcout<<L"\nDEVICE: "<< pDeviceName <<L"\n  PORT: Still (for still image)"<<std::endl;
    printPinInfo(pSourceFilter,L"Still");

    pSourceFilter->Release();
}

int compareResolutionString(const wchar_t* r1, const wchar_t* r2)
{
    std::wstringstream ss1,ss2;
    long width1,width2;
    
    ss1<<r1;
    ss1>>width1;
    
    ss2<<r2;
    ss2>>width2;

    if (width1 == width2)
    {
        return 0;
    }
    else if (width1>width2)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

//thread entry point for capturing video to mp4 file
DWORD WINAPI CaptureProc(LPVOID lpParam)
{
    VideoCapture* pVideoCapture = reinterpret_cast<VideoCapture*>(lpParam);
    std::string fileName = "video-" + getLocalTimeString() + ".mp4";
    pVideoCapture->setSaveName(fileName);
    std::string path = pVideoCapture->getSavePath() + fileName;
    long frameRate = pVideoCapture->getFramRate();
    long width = pVideoCapture->getStreamWidth();
    long height = pVideoCapture->getStreamHeight();
    std::cout << path << ',' << frameRate << ',' << width << ',' << height << std::endl;

    VideoFileWriter vfw(
        path,
        frameRate,
        width,
        height);

    while (pVideoCapture->isCaptureStarted())
    {
        void* pFrame = pVideoCapture->getCurrentFrame();
        if (pFrame) 
        {
            vfw.writeBitmapFrame(pFrame);
            CoTaskMemFree(pFrame);
        }
        else
        {
            std::cout << "GetImage failed" << std::endl;
        }
        Sleep(1000/frameRate);
    }
    return 0;
}

//video format struct
struct Format
{
    Format(const wchar_t* _pTypeName, const wchar_t* _pResolution, long _frameRate, AM_MEDIA_TYPE* _pMediaType)
        :typeName(_pTypeName),resolution(_pResolution),frameRate(_frameRate),pMediaType(_pMediaType)
    {
    }
    Format(const Format& copy)
    {
        typeName = copy.typeName;
        resolution = copy.resolution;
        pMediaType = copy.pMediaType;
        frameRate = copy.frameRate;
    }
    std::wstring typeName;
    std::wstring resolution;
    long frameRate;
    AM_MEDIA_TYPE* pMediaType;
};

//video device struct which support a list of format
struct Device
{
    Device(const wchar_t* _pName, IBaseFilter* _pFilter):
        name(_pName),pFilter(_pFilter), pAMVideoControl(nullptr), 
        pAMCameraControl(nullptr),pCapturePin(nullptr), pStillPin(nullptr),
        pCaptureStreamConfig(nullptr), pStillStreamConfig(nullptr)
    {
        HRESULT hr;
        hr = _pFilter->QueryInterface(IID_IAMVideoControl, (void**)&pAMVideoControl);
        if (FAILED(hr)) std::cout << "IBaseFilter.QueryInterface AMVideoControl failed" << std::endl;

        hr = _pFilter->QueryInterface(IID_IAMCameraControl, (void**)&pAMCameraControl);
        if (FAILED(hr)) std::cout << "IBaseFilter.QueryInterface AMCameraControl failed" << std::endl;

        pCapturePin = getFilterPin(_pFilter,L"Capture");
        if (!pCapturePin) 
        {
            pCapturePin = getFilterPin(_pFilter, L"Output");
            if (!pCapturePin) std::cout<<"No Capture/Output pin found" << std::endl;
        }
        else
        {
            hr = pCapturePin->QueryInterface(IID_IAMStreamConfig, (void**)&pCaptureStreamConfig);
            if (FAILED(hr)) std::cout<<"IPin.QueryInterface AMStreamConfig failed" << std::endl;
        }

        pStillPin = getFilterPin(_pFilter, L"Still");
        if (!pStillPin)
        {
            std::cout << "No still pin found" << std::endl;
        }
        else
        {
            hr = pStillPin->QueryInterface(IID_IAMStreamConfig, (void**)&pStillStreamConfig);
            if (FAILED(hr)) std::cout << "IPin.QueryInterface AMStreamConfig failed" << std::endl;
        }
    }
    Device(const Device& copy)
    {
        name = copy.name;
        pFilter = copy.pFilter;
        pAMVideoControl = copy.pAMVideoControl;
        pAMCameraControl = copy.pAMCameraControl;
        pCapturePin = copy.pCapturePin;
        pStillPin = copy.pStillPin;
        pCaptureStreamConfig = copy.pCaptureStreamConfig;
        pStillStreamConfig = copy.pStillStreamConfig;
        formatList = copy.formatList;
    }
    std::wstring name;
    IBaseFilter* pFilter;
    IAMVideoControl* pAMVideoControl;
    IAMCameraControl* pAMCameraControl;
    IPin* pCapturePin;
    IPin* pStillPin;
    IAMStreamConfig* pCaptureStreamConfig;
    IAMStreamConfig* pStillStreamConfig;
    std::vector<Format> formatList;
};

//directshow vmr render struct
struct VmrRender
{
    VmrRender()
        :pFilter(nullptr), pVMRFilterConfig(nullptr),
        pVMRWindowlessControl9(nullptr), pVMRMixerBitmap9(nullptr)
    {
        HRESULT hr;
        hr = CoCreateInstance(CLSID_VideoMixingRenderer9,
            NULL,
            CLSCTX_INPROC,
            IID_IBaseFilter,
            (void**)&pFilter);
        if (FAILED(hr))
        {
            std::cout << "Create VideoMixingRenderer9 failed" << std::endl;
            return;
        }
        hr = pFilter->QueryInterface(IID_IVMRFilterConfig9, (void**)&pVMRFilterConfig);
        if (FAILED(hr)) std::cout << "IBaseFilter.QueryInterface IVMRFilterConfig9 failed" << std::endl;

        hr = pFilter->QueryInterface(IID_IVMRMixerBitmap9, (void**)&pVMRMixerBitmap9);
        if (FAILED(hr)) std::cout << "IBaseFilter.QueryInterface IVMRMixerBitmap9 failed" << std::endl;

        hr = pVMRFilterConfig->SetRenderingMode(VMR9Mode_Windowless);
        if (FAILED(hr)) std::cout << "VMRFilterConfig.SetRenderingMode failed" << std::endl;

        hr = pFilter->QueryInterface(IID_IVMRWindowlessControl9, (void**)&pVMRWindowlessControl9);
        if (FAILED(hr)) std::cout << "IBaseFilter.QueryInterface IVMRWindowlessControl9 failed" << std::endl;
    }
    ~VmrRender()
    {
        if (pVMRMixerBitmap9) pVMRMixerBitmap9->Release();
        if (pVMRFilterConfig) pVMRFilterConfig->Release();
        if (pFilter) pFilter->Release();
    }
    IBaseFilter* pFilter;
    IVMRFilterConfig9* pVMRFilterConfig;
    IVMRWindowlessControl9* pVMRWindowlessControl9;
    IVMRMixerBitmap9* pVMRMixerBitmap9;
};

//sample grabber and null render struct for supporting hardware snapshot button
struct GrabberNullRender
{
    GrabberNullRender() :
        pGrabberFilter(nullptr), pNullFilter(nullptr), pSampleGrabber(nullptr)
    {
        HRESULT hr;
        hr = CoCreateInstance(
            CLSID_SampleGrabber,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IBaseFilter,
            (void**)&pGrabberFilter
        );
        if (FAILED(hr))
        {
            std::cout << "Create SampleGrabber failed" << std::endl;
            return;
        }
        hr = CoCreateInstance(
            CLSID_NullRenderer,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IBaseFilter,
            (void**)&pNullFilter
        );
        if (FAILED(hr))
        {
            std::cout << "Create NullRenderer failed" << std::endl;
            return;
        }
        hr = pGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&pSampleGrabber);
        if (FAILED(hr)) std::cout << "SampleGrabber.QueryInterface failed" << std::endl;

        hr = pSampleGrabber->SetOneShot(false);
        if (FAILED(hr)) std::cout << "SampleGrabber.SetOneShot failed" << std::endl;

        hr = pSampleGrabber->SetBufferSamples(true);
        if (FAILED(hr)) std::cout << "SampleGrabber.SetBufferSamples failed" << std::endl;

        AM_MEDIA_TYPE mt;
        ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
        mt.majortype = MEDIATYPE_Video;
        mt.subtype = MEDIASUBTYPE_RGB24;
        mt.formattype = FORMAT_VideoInfo;
        hr = pSampleGrabber->SetMediaType(&mt);
        if (FAILED(hr)) std::cout << "SampleGrabber.SetMediaType failed" << std::endl;
    }
    ~GrabberNullRender()
    {
        if (pSampleGrabber) pSampleGrabber->Release();
        if (pNullFilter) pNullFilter->Release();
        if (pGrabberFilter) pGrabberFilter->Release();
    }
    IBaseFilter* pGrabberFilter;
    IBaseFilter* pNullFilter;
    ISampleGrabber* pSampleGrabber;
};

VideoCapture::VideoCapture():
    pICaptureGraphBuilder(nullptr), pIGraphBuilder(nullptr), pStillCapCB(nullptr), pIMediaEventEx(nullptr),pIMediaControl(nullptr),
    bStreamStarted(false), bCaptureStarted(false), bVrmRendered(false),
    pCurrentDevice(nullptr), pCurrentFormat(nullptr), pTransformFilter(nullptr), pVmr(nullptr), pGrab(nullptr)
{
    CoInitialize(0);
}
VideoCapture::~VideoCapture()
{
    CoUninitialize();
}

void VideoCapture::initialize()
{
    if (enumDevices())
    {
        createGraph();
        createRender();
    }
}

void VideoCapture::uninitialize()
{
    stop();
    clean();
    releaseRender();
    releaseGraph();
    freeDevices();
}

int VideoCapture::enumDevices()
{
    ICreateDevEnum *pDevEnum;
    IEnumMoniker *pEnumMoniker;
    IMoniker *pMoniker;
    IBaseFilter* pSourceFilter;
    IPropertyBag *pPropertyBag;
    VARIANT var;
    HRESULT hr;

    hr = CoCreateInstance(CLSID_SystemDeviceEnum,
                          NULL,  
                          CLSCTX_INPROC_SERVER, 
                          IID_PPV_ARGS(&pDevEnum)
                          );
    if (FAILED(hr))
    {
        std::cout<<"CreateDevEnum failed "<<hr<<std::endl;
        return 0;
    }

    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
                                         &pEnumMoniker,
                                         0);
    if (hr == S_FALSE)
    {
        hr = VFW_E_NOT_FOUND;
        std::cout<<"No device found"<<std::endl;
    }
    
    if (FAILED(hr))
    {
        std::cout<<"Create CreateClassEnumerator failed"<<std::endl;
        return 0;
    }

    pDevEnum->Release();

    //deviceList.clear();
    while (pEnumMoniker->Next(1, &pMoniker, NULL) == S_OK)
    {
        pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropertyBag));
        VariantInit(&var);
        hr = pPropertyBag->Read(L"FriendlyName", &var, 0);
        if (FAILED(hr)) hr = pPropertyBag->Read(L"Description", &var, 0);
        if (SUCCEEDED(hr))
        {
            hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pSourceFilter);
            if (FAILED(hr))
            {
                std::cout<<"IMoniker.BindToObject failed"<<std::endl;
            }
            else
            {   
                deviceList.emplace_back(var.bstrVal, pSourceFilter);
                if (deviceList.back().pCaptureStreamConfig) enumFormats(deviceList.back().pCaptureStreamConfig, deviceList.back().formatList);
            }
        }
        VariantClear(&var);
        pPropertyBag->Release();
        pMoniker->Release();
    }
    pEnumMoniker->Release();
    return (int)deviceList.size();
}

void VideoCapture::enumFormats(IAMStreamConfig* pAMStreamConfig, std::vector<Format>& formatList)
{
    int piCount, piSize;
    AM_MEDIA_TYPE* pMediaType;
    VIDEO_STREAM_CONFIG_CAPS scc;
    HRESULT hr;
    std::vector<std::wstring> typeList;
    
    hr = pAMStreamConfig->GetNumberOfCapabilities(&piCount, &piSize);
    if (FAILED(hr))
    {
        std::cout << "IAMStreamConfig.GetNumberOfCapabilities failed" << std::endl;
        return;
    }

    for (int i = 0; i < piCount; i++)
    {
        pAMStreamConfig->GetStreamCaps(i, &pMediaType, reinterpret_cast<BYTE*>(&scc));
        if (FORMAT_VideoInfo == pMediaType->formattype)
        {
            VIDEOINFOHEADER* pVideoInfo = (VIDEOINFOHEADER*)pMediaType->pbFormat;
            if (pVideoInfo->bmiHeader.biHeight < 480) continue;//too low resolution will be skipped

            std::wstringstream ss;
            ss<<(pVideoInfo->bmiHeader.biWidth)<<L" x "<<(pVideoInfo->bmiHeader.biHeight);

            long frameRate = 10000000/(long)pVideoInfo->AvgTimePerFrame;

            formatList.emplace_back(
                findVideoType(pMediaType->subtype).c_str(),
                ss.str().c_str(),
                frameRate,
                pMediaType);
        }
    }
}

void VideoCapture::createRender()
{
    pVmr = new VmrRender();
    pGrab = new GrabberNullRender();
    pStillCapCB = new SampleGrabberCallback(this);
    HRESULT hr = pGrab->pSampleGrabber->SetCallback(pStillCapCB, 1);
    if (FAILED(hr)) std::cout << "SampleGrabber.SetCallback failed" << std::endl;
}

void VideoCapture::releaseRender()
{
    delete pVmr;
    delete pGrab;
    delete pStillCapCB;
}

void VideoCapture::createGraph()
{
    HRESULT hr;
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2,
        NULL,
        CLSCTX_INPROC,
        IID_ICaptureGraphBuilder2,
        (void**)&pICaptureGraphBuilder
    );
    if (FAILED(hr)) std::cout << "create CaptureGraphBuilder2 failed" << std::endl;
    hr = CoCreateInstance(CLSID_FilterGraph,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IGraphBuilder,
        (void**)&pIGraphBuilder
    );
    if (FAILED(hr)) std::cout << "create FilterGraph failed" << std::endl;
    hr = pICaptureGraphBuilder->SetFiltergraph(pIGraphBuilder);
    if (FAILED(hr)) std::cout << "CaptureGraphBuilder2.SetFiltergraph failed" << std::endl;
}

void VideoCapture::releaseGraph()
{
    if (pIGraphBuilder)
    {
        pIGraphBuilder->Release();
        pIGraphBuilder = nullptr;
    }

    if (pICaptureGraphBuilder)
    {
        pICaptureGraphBuilder->Release();
        pICaptureGraphBuilder = nullptr;
    }
}

void VideoCapture::setRotationDegree(int degree)
{
    if (pTransformFilter)
    { 
        pTransformFilter->setRotationDegree(degree);
    }
}

void VideoCapture::setGridInterval(int interval)
{
    if (pTransformFilter)
    {
        pTransformFilter->setGridInterval(interval);
    }
}

void VideoCapture::printDevices()
{
    for (auto& device : deviceList)
    {
        std::wcout<<L"Device Name:"<<device.name<<std::endl;
        for (auto& format : device.formatList)
        {
            std::wcout<<L"    "<<format.typeName<<L' '<<format.resolution<<std::endl;
        }
    }
}

const std::vector<std::wstring>& VideoCapture::getDeviceNameList()
{
    deviceNameList.clear();
    for (auto& device : deviceList)
    {
        //std::wcout<<device.name<<std::endl;
        deviceNameList.emplace_back(device.name);
    }
    return deviceNameList;
}

const std::vector<std::wstring>& VideoCapture::getFormatNameList(const wchar_t* pDeviceName)
{
    formatNameList.clear();
    for (auto& device : deviceList)
    {
        if (device.name.compare(pDeviceName) == 0)
        {
            for (auto& format : device.formatList)
            {
                //std::wcout<<format.typeName<<std::endl;
                uniqueAppend(formatNameList,format.typeName);
            }
        }
    }
    return formatNameList;
}

const std::vector<std::wstring>& VideoCapture::getResolutionList(const wchar_t* pDeviceName,const wchar_t* pFormatName)
{
    resolutionList.clear();
    for (auto& device : deviceList)
    {
        if (device.name.compare(pDeviceName) == 0)
        {
            for (auto& format : device.formatList)
            {
                if (format.typeName.compare(pFormatName) == 0)
                {
                    //std::wcout<<format.resolution<<std::endl;
                    resolutionList.emplace_back(format.resolution);
                }
            }
        }
    }
    return resolutionList;
}


void VideoCapture::clean()
{
    bStreamStarted = false;
    bCaptureStarted = false;
    bVrmRendered = false;

    if (pIMediaEventEx)
    {
        pIMediaEventEx->SetNotifyWindow(NULL, 0, 0);
        pIMediaEventEx->Release();
        pIMediaEventEx = nullptr;
    }

    if (pIMediaControl)
    {
        pIMediaControl->Release();
        pIMediaControl = nullptr;
    }

    //disconnect and remove filters connected with device source filter
    if (pCurrentDevice)
    {
        disconnectAll();
        pCurrentDevice = nullptr;
    }

    if (pIGraphBuilder)
    {
        //also remove device source filter
        removeAll();
        pCurrentFormat = nullptr;
    }
}

void VideoCapture::addDeviceFilter(
    const wchar_t* pDeviceName,
    const wchar_t* pFormatName,
    const wchar_t* pResolution )
{
    std::wcout<< pDeviceName << L',' << pFormatName << L',' << pResolution << std::endl;
    //create source filter and set format
    pCurrentDevice = getDevice(pDeviceName);
    if (!pCurrentDevice)
    {
        std::wcout << L"IN addDeviceFilter: " << pDeviceName << L" is not found" << std::endl;
        return;
    }

    HRESULT hr = pIGraphBuilder->AddFilter(pCurrentDevice->pFilter, pCurrentDevice->name.c_str());
    if (FAILED(hr)) std::cout << "GraphBuilder AddFilter failed" << std::endl;

    pCurrentFormat = getFormat(pCurrentDevice->formatList, pFormatName, pResolution);
    if (!pCurrentFormat) std::wcout << pFormatName << L',' << pResolution << L" is not found" << std::endl;
}

void VideoCapture::setDeviceFilter()
{
    HRESULT hr;
    if (!pCurrentDevice || !pCurrentFormat)
    {
        std::cout << "IN setDeviceFilter: No valid device or format is found" << std::endl;
        return;
    }

    if (pCurrentDevice->pCaptureStreamConfig)
    {
        hr = pCurrentDevice->pCaptureStreamConfig->SetFormat(pCurrentFormat->pMediaType);
        if (FAILED(hr)) std::cout << "AMStreamConfig.SetFormat failed" << std::endl;
    }

    if (pCurrentDevice->pStillPin && pCurrentDevice->pAMVideoControl)
    {
        hr = pCurrentDevice->pAMVideoControl->SetMode(pCurrentDevice->pStillPin, VideoControlFlag_Trigger || VideoControlFlag_ExternalTriggerEnable);
        if (FAILED(hr)) std::cout << "AMVideoControl.SetMode failed" << std::endl;
    }

    if (pCurrentDevice->pAMCameraControl)
    {
        //TODO: camera setup
        //long value, flag;
        //hr = pCurrentDevice->pAMCameraControl->Get(CameraControl_Roll, &value, &flag);
        //std::cout << value << ',' << flag << std::endl;

        /*
        long min, max, step, default, flag;
        hr = pCurrentDevice->pAMCameraControl->GetRange(
            CameraControl_Roll,
            &min, //0
            &max, //3
            &step, //1
            &default, //0
            &flag //2
            );
        std::cout << min << ',' << max << ',' << step << ',' << default << ',' << flag << std::endl;
        */

        //hr = pCurrentDevice->pAMCameraControl->Set(CameraControl_Roll, 0, CameraControl_Flags_Manual);
        //if (FAILED(hr)) std::cout << "AMCameraControl.Set CameraControl_Roll failed" << std::endl;
    }

    if (pCurrentDevice->pStillStreamConfig)
    { 
        hr = pCurrentDevice->pStillStreamConfig->SetFormat(pCurrentFormat->pMediaType);
        if (FAILED(hr)) std::cout << "AMStreamConfig.SetFormat failed" << std::endl;
    }

    streamFrameRate = pCurrentFormat->frameRate;
}

void VideoCapture::addTransform(const wchar_t* pResolution)
{
    int width, height;
    wchar_t delimiter;
    std::wstringstream ss;
    HRESULT hr;

    ss << pResolution;
    ss >> width >> delimiter >> height;

    pTransformFilter = new TransformFilter(width,height);//auto deleted when cleaning the Graph
    hr = pIGraphBuilder->AddFilter(pTransformFilter, L"Transfrom Filter");
    if (FAILED(hr)) std::cout << "GraphBuilder.AddFilter transform failed" << std::endl;
}

void VideoCapture::addVmr()
{
    HRESULT hr;
    if (!pIGraphBuilder || !pVmr)
    {
        std::cout << "IN addVmr: Initialization not completed" << std::endl;
        return;
    }

    hr = pIGraphBuilder->AddFilter(pVmr->pFilter, L"Video Mixing Renderer");
    if (FAILED(hr)) std::cout << "GraphBuilder.AddFilter vmr failed"<< std::endl;
}

void VideoCapture::renderVmr(HWND hWin)
{
    if (!pCurrentDevice)
    {
        std::cout << "IN renderVmr: Device not found" << std::endl;
        return;
    }
    std::cout << "Before SetVideoClippingWindow" << std::endl;
    HRESULT hr = pVmr->pVMRWindowlessControl9->SetVideoClippingWindow(hWin);
    if (FAILED(hr)) std::cout << "VMRWindowlessControl9.SetVideoClippingWindow failed" << std::endl;
    std::cout << "After SetVideoClippingWindow" << std::endl;
    hr = pICaptureGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE,
        &MEDIATYPE_Video,
        pCurrentDevice->pFilter,
        pTransformFilter,
        pVmr->pFilter);
    if (FAILED(hr))
    {
        std::cout << "CaptureGraphBuilder.RenderStream failed" << std::endl;
        return;
    }
    bVrmRendered = true;
}

void VideoCapture::setVmrRenderPosition(RECT& dstRect)
{
    if (!pVmr || !bVrmRendered)
    {
        std::cout << "IN setVmrRenderPosition: Vmr is not rendering" << std::endl;
        return;
    }
    RECT srcRect;
    HRESULT hr = pVmr->pVMRWindowlessControl9->GetNativeVideoSize(&streamWidth, &streamHeight, NULL, NULL);
    if (FAILED(hr)) std::cout << "IVMRWindowlessControl9.GetNativeVideoSize failed" << std::endl;

    SetRect(&srcRect, 0, 0, streamWidth, streamHeight);
    std::cout << srcRect.right<< ',' << srcRect.bottom << " -> " << dstRect.right<< ',' << dstRect.bottom << std::endl;
    hr = pVmr->pVMRWindowlessControl9->SetVideoPosition(&srcRect, &dstRect);
    if (FAILED(hr)) std::cout << "IVMRWindowlessControl9.SetVideoPosition failed" << std::endl;
}

void VideoCapture::notifyWindow(HWND hWin)
{
    if (!pIGraphBuilder)
    {
        std::cout << "IN notifyWindow: Initialization not completed" << std::endl;
        return;
    }
    HRESULT hr = pIGraphBuilder->QueryInterface(IID_IMediaEventEx, (void**)&pIMediaEventEx);
    if (FAILED(hr)) std::cout << "GraphBuilder.QueryInterface IMediaEventEx failed" << std::endl;

    hr = pIMediaEventEx->SetNotifyWindow((OAHWND)hWin, WM_GRAPHNOTIFY, 0);
    if (FAILED(hr)) std::cout << "MediaEventEx.SetNotifyWindow failed" << std::endl;
}

void VideoCapture::setReferenceClock()
{
    if (!pIGraphBuilder)
    {
        std::cout << "IN setReferenceClock: Initialization not completed" << std::endl;
        return;
    }
    IMediaFilter* pMediaFilter = nullptr;
    pIGraphBuilder->QueryInterface(IID_IMediaFilter, (void**)&pMediaFilter);
    pMediaFilter->SetSyncSource(0);//no reference clock, run as soon as possible
    pMediaFilter->Release();
    pMediaFilter = nullptr;
}

void VideoCapture::setMediaController()
{
    HRESULT hr = pIGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID*)&pIMediaControl);
    if (FAILED(hr)) pIMediaControl = nullptr;
}

void VideoCapture::setup(
    const wchar_t* pDeviceName,
    const wchar_t* pFormatName,
    const wchar_t* pResolution,
    HWND hWin,
    RECT& dstRect,
    HDC hMemDc)
{
    addDeviceFilter(pDeviceName, pFormatName, pResolution);
    std::cout << "addDeviceFilter OK" << std::endl;
    setDeviceFilter();
    std::cout << "setDeviceFilter OK" << std::endl;
    addTransform(pResolution);
    std::cout << "addTransform OK" << std::endl;
    addVmr();
    std::cout << "addVmr OK" << std::endl;
    renderVmr(hWin);
    std::cout << "renderVmr OK" << std::endl;
    setVmrRenderPosition(dstRect);
    std::cout << "setVmrRenderPosition OK" << std::endl;
    overlapLogo(hMemDc, dstRect.right, dstRect.bottom);
    std::cout << "overlapLogo OK" << std::endl;
    addGrabber();
    std::cout << "addGrabber OK" << std::endl;
    renderGrabber();
    std::cout << "renderGrabber OK" << std::endl;
    notifyWindow(hWin);
    setReferenceClock();
    setMediaController();
}

void VideoCapture::overlapLogo(HDC hMemDc, long width, long height)
{
    HRESULT hr;

    if (!pVmr || !bVrmRendered) return;

    VMR9AlphaBitmap infoBmp;
    infoBmp.dwFlags = VMR9AlphaBitmap_hDC;
    infoBmp.hdc = hMemDc;
    infoBmp.pDDS = 0;
    infoBmp.rSrc = { 0,0,32,32 };
    infoBmp.rDest.left = 0.0f;
    infoBmp.rDest.top = 1.0f - ((float)32 / (float)height);
    infoBmp.rDest.right = ((float)32 / (float)width);
    infoBmp.rDest.bottom = 1.0f;
    infoBmp.fAlpha = (float)0.2;
    infoBmp.clrSrcKey = 0;
    infoBmp.dwFilterMode = 0;

    hr = pVmr->pVMRMixerBitmap9->SetAlphaBitmap(&infoBmp);
    if (FAILED(hr)) std::cout<<"IVMRMixerBitmap9.SetAlphaBitmap failed"<<std::endl;
}

void VideoCapture :: addGrabber()
{
    HRESULT hr;

    if (!pIGraphBuilder || !pGrab)
    {
        std::cout << "IN addGrabber: Initialization not completed" << std::endl;
        return;
    }
    hr = pIGraphBuilder->AddFilter(pGrab->pGrabberFilter, L"Sample Grabber");
    if (FAILED(hr)) std::cout << "GraphBuilder.AddFilter failed" << std::endl;


    hr = pIGraphBuilder->AddFilter(pGrab->pNullFilter, L"NullRender");
    if (FAILED(hr)) std::cout << "GraphBuilder.AddFilter failed" << std::endl;
}

void VideoCapture::renderGrabber()
{
    HRESULT hr;

    if (!pCurrentDevice || !pCurrentDevice->pStillPin)
    {
        std::cout << "IN renderGrabber: Device still pin not found" << std::endl;
        return;
    }

    hr = pICaptureGraphBuilder->RenderStream(
        &PIN_CATEGORY_STILL, // Connect this pin ...
        &MEDIATYPE_Video,    // with this media type ...
        pCurrentDevice->pFilter,                // on this filter ...
        pGrab->pGrabberFilter,          // to the Sample Grabber ...
        pGrab->pNullFilter);              // ... and finally to the Null Renderer.
    if (FAILED(hr)) std::cout << "CaptureGraphBuilder.RenderStream failed" << std::endl;
}

void VideoCapture::start()
{
    if (!pIMediaControl) return;
    HRESULT hr = pIMediaControl->Run();
    if (SUCCEEDED(hr)) bStreamStarted = true;
}

void VideoCapture::pause()
{
    if (!pIMediaControl || !bStreamStarted) return;
    HRESULT hr = pIMediaControl->Pause();
    if (SUCCEEDED(hr)) bStreamStarted = false;
}

void VideoCapture::stop()
{
    if (!pIMediaControl || !bStreamStarted) return;
    if (bCaptureStarted) stopCapture();
    HRESULT hr = pIMediaControl->Stop();
    if (SUCCEEDED(hr)) bStreamStarted = false;

}

void VideoCapture::flip(int direction)
{
    if (pTransformFilter) pTransformFilter->updateFlipStatus(direction);
}


void VideoCapture::snap()
{
    void* pFrame = getCurrentFrame();
    std::string fileName;
    if (pFrame)
    {
        fileName = "frame-" + getLocalTimeString() + ".jpg";
        setSaveName(fileName);
        VideoFileWriter::WriteImageFile(getSavePath() + fileName, pFrame);
        CoTaskMemFree(pFrame);
    }
}

void VideoCapture::paint(HWND hWin, HDC hdc)
{
    if (!pVmr || !bStreamStarted)
    {
        std::cout << "IN paint: Vmr is not rendering" << std::endl;
        return;
    }
    pVmr->pVMRWindowlessControl9->RepaintVideo(hWin, hdc);
}


void VideoCapture::startCapture()
{
    DWORD threadId;
    if (!bVrmRendered) return;
    HANDLE hThread = CreateThread( 
        0,                   // default security attributes
        0,                      // use default stack size  
        CaptureProc,       // thread function name
        (void*)this,          // argument to thread function 
        0,                      // use default creation flags 
        &threadId);   // returns the thread identifier 
    bCaptureStarted = true;
}

void VideoCapture::stopCapture()
{
    bCaptureStarted = false;
}

void VideoCapture::freeDevices()
{
    for (auto& device: deviceList)
    {
        for (auto& format : device.formatList)
        {
            FreeMediaType(format.pMediaType);
        }
        device.formatList.clear();
        
        if (device.pCaptureStreamConfig)
        {
            device.pCaptureStreamConfig->Release();
            device.pCaptureStreamConfig = nullptr;
        }
        if (device.pStillStreamConfig)
        {
            device.pStillStreamConfig->Release();
            device.pStillStreamConfig = nullptr;
        }
        if (device.pCapturePin)
        {
            device.pCapturePin->Release();
            device.pCapturePin = nullptr;
        }
        if (device.pStillPin)
        {
            device.pStillPin->Release();
            device.pStillPin = nullptr;
        }
        if (device.pAMVideoControl)
        {
            device.pAMVideoControl->Release();
            device.pAMVideoControl = nullptr;
        }
        if (device.pAMCameraControl)
        {
            device.pAMCameraControl->Release();
            device.pAMCameraControl = nullptr;
        }
        if (device.pFilter)
        {
            device.pFilter->Release();
            device.pFilter = nullptr;
        }
    }
    deviceList.clear();
}

Device* VideoCapture::getDevice(const wchar_t* pName)
{
    for (auto& device : deviceList)
    {
        if (device.name.compare(pName) == 0)
        {
            return &device;
        }
    }
    return nullptr;
}

Format* VideoCapture::getFormat(std::vector<Format>&formatList, const wchar_t* pTypeName, const wchar_t* pResolution)
{
    for (auto& format : formatList)
    {
        if (format.typeName.compare(pTypeName) == 0 && format.resolution.compare(pResolution) == 0)
        {
            return &format;
        }
    }
    return nullptr;
}

Format* VideoCapture::getMaxFormat(std::vector<Format>&formatList, const wchar_t* pTypeName)
{
    const wchar_t* pMaxValue = L"0 x 0";
    Format* pMaxFormat = nullptr;
    for (auto& format : formatList)
    {
        if (format.typeName.compare(pTypeName) == 0)
        {
            if (compareResolutionString(format.resolution.c_str(), pMaxValue) > 0)
            {
                pMaxValue = format.resolution.c_str();
                pMaxFormat = &format;
            }
        }
    }
    return pMaxFormat;
}


void VideoCapture::disconnectAll()
{
    disconnectFilter(pCurrentDevice->pFilter);
}

void VideoCapture::disconnectFilter(IBaseFilter *pBF)
{
    IPin *pP, *pTo;
    ULONG u;
    IEnumPins *pins = NULL;
    PIN_INFO pininfo;
    
    HRESULT hr = pBF->EnumPins(&pins);
    if (hr != S_OK || !pins)
        return;
    pins->Reset();
    while (hr == NOERROR)
    {
        hr = pins->Next(1, &pP, &u);
        if (hr == S_OK && pP)
        {
            pP->ConnectedTo(&pTo);
            if (pTo)
            {
                hr = pTo->QueryPinInfo(&pininfo);
                if (hr == NOERROR)
                {
                    if (pininfo.dir == PINDIR_INPUT)
                    {
                        disconnectFilter(pininfo.pFilter);
                        std::wcout<<L"disconnect pin:"<<pininfo.achName<<std::endl;
                        pIGraphBuilder->Disconnect(pTo);
                        pIGraphBuilder->Disconnect(pP);
                        pIGraphBuilder->RemoveFilter(pininfo.pFilter);
                    }
                    pininfo.pFilter->Release();
                    pininfo.pFilter = NULL;
                }
                pTo->Release();
            }
            pP->Release();
        }
    }
    pins->Release();
}

void VideoCapture::removeAll()
{
    HRESULT hr = 0;

    int i = 0;
    while (hr == NOERROR)
    {
        IEnumFilters * pEnum = 0;
        ULONG cFetched = 0;

        // We must get the enumerator again every time because removing a filter from the graph
        // invalidates the enumerator. We always get only the first filter from each enumerator.
        hr = pIGraphBuilder->EnumFilters(&pEnum);
        if (FAILED(hr)) 
        { 
            std::cout << "GraphBuilder.EnumFilters failed" << std::endl;
            return;
        }

        IBaseFilter * pFilter = NULL;
        if (pEnum->Next(1, &pFilter, &cFetched) == S_OK)
        {
            FILTER_INFO FilterInfo;
            memset(&FilterInfo, 0, sizeof(FilterInfo));
            hr = pFilter->QueryFilterInfo(&FilterInfo);
            FilterInfo.pGraph->Release();

            int count = 0;
            char buffer[255];
            memset(buffer, 0, 255 * sizeof(char));

            while( FilterInfo.achName[count] != 0x00 )
            {
                buffer[count] = (char)FilterInfo.achName[count];
                count++;
            }

            hr = pIGraphBuilder->RemoveFilter(pFilter);
            if (FAILED(hr)) 
            { 
                std::cout << "GraphBuilder.RemoveFilter failed" << std::endl;
                return;
            }
            std::cout<< "remove filter: " << buffer << std::endl;

            pFilter->Release();
            pFilter = nullptr;
        }
        pEnum->Release();
        pEnum = nullptr;

        if (cFetched == 0) break;
        i++;
    }

 return;
}

void* VideoCapture::getCurrentFrame()
{
    BYTE* lpCurrImage = nullptr;
    HRESULT hr;

    if (!pVmr || !bVrmRendered)
    {
        std::cout << "IN getCurrentFrame: Vmr is not in rendering" << std::endl;
        return nullptr;
    }
    
    hr = pVmr->pVMRWindowlessControl9->GetCurrentImage(&lpCurrImage);
    if (FAILED(hr))
    {
        std::cout<< "IVMRWindowlessControl9.GetCurrentImage failed" <<std::endl;
        return nullptr;
    }
    
    return reinterpret_cast<void*>(lpCurrImage);
}

const AM_MEDIA_TYPE* VideoCapture::getCurrentMediaType()
{ 
    if (pCurrentFormat) return pCurrentFormat->pMediaType;
    else return nullptr; 
}