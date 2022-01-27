#ifndef UNICODE
#define UNICODE
#endif
#include <iostream>
#include <vector>
#include <windows.h>
#include <strmif.h>
#include <comsvcs.h>
#include <dvdmedia.h>
#include "VideoStreamRender.h"
#include "VideoFileWriter.h"

#define CHECK_HR_FAILED(statement) if(FAILED(hr)) {statement;return;}

bool VideoStreamRender::bCapStarted = false;

const VideoType VideoStreamRender::allVideoTypes[] = { 
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
    {L"MJPG",  MEDIASUBTYPE_MJPG}
};


DWORD WINAPI VideoStreamRender::CapProc(LPVOID lpParam)
{
    VideoStreamRender* pVsr = reinterpret_cast<VideoStreamRender*>(lpParam);
    const RECT videoRect = pVsr->getVideoRect();
    long frameRate = pVsr->getFrameRate();
    std::cout<<videoRect.right<<','<<videoRect.bottom<<','<<frameRate<<std::endl;
    VideoFileWriter vfw("test.avi",videoRect.right,videoRect.bottom);
    while(bCapStarted)
    {
        void* pFrame = pVsr->getImage();
        if (pFrame == nullptr) std::cout<<"GetImage failed"<<std::endl;
        vfw.writeBitmapFrame(pFrame);
        Sleep(1000/frameRate);
    }
    return 0;
}

GUID VideoStreamRender::findVideoType(const std::wstring & name)
{
    for (int i = 0; i < sizeof(allVideoTypes)/sizeof(allVideoTypes[0]); ++ i)
    {
        if (allVideoTypes[i].name.compare(name) == 0) return allVideoTypes[i].guid;
    }
    return GUID_NULL;
}

std::wstring VideoStreamRender::findVideoType(const GUID & guid)
{
    for (int i = 0; i < sizeof(allVideoTypes)/sizeof(allVideoTypes[0]); ++ i)
    {
        if (allVideoTypes[i].guid == guid) return allVideoTypes[i].name;
    }
    wchar_t buffer[GUID_STRING_SIZE] = { 0 };
    StringFromGUID2(guid, buffer, GUID_STRING_SIZE);
    return std::wstring(buffer);
}

GUID VideoStreamRender::findFormatType(const std::wstring & name)
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

std::wstring VideoStreamRender::findFormatType(const GUID & guid)
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

VideoStreamRender::VideoStreamRender():
    pEnumMoniker(NULL),pStreamConfig(NULL),bOpen(false),bRun(false)
{
    CoInitialize(0);
    createDeviceEnumerator();
}
    
VideoStreamRender::~VideoStreamRender()
{
    releaseDeviceEnumerator();
}

const std::vector<std::wstring>& VideoStreamRender::listDevice()
{
    IMoniker *pMoniker;
    IPropertyBag *pPropertyBag;
    VARIANT var;
    HRESULT hr;
    
    deviceList.clear();

    if (pEnumMoniker == NULL) createDeviceEnumerator();
    if (pEnumMoniker == NULL) return deviceList;

    //list device
    pEnumMoniker->Reset();
    while (pEnumMoniker->Next(1, &pMoniker, NULL) == S_OK)
    {
        hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropertyBag));
        if (FAILED(hr))
        {
            pMoniker->Release();
            continue;
        }
        
        VariantInit(&var);
        hr = pPropertyBag->Read(L"FriendlyName", &var, 0);
        if (FAILED(hr)) hr = pPropertyBag->Read(L"Description", &var, 0);
        if (SUCCEEDED(hr))
        {
            //std::wcout<<var.bstrVal<<std::endl;
            uniqueAppend(deviceList,var.bstrVal);
            //deviceList.emplace_back(var.bstrVal);
            VariantClear(&var); 
        }
        pPropertyBag->Release();
        pMoniker->Release();
    }
    printList(deviceList);
    return deviceList;
}

int VideoStreamRender::getDeviceIndex(std::wstring deviceName)
{
    for (int i = 0; i < deviceList.size(); ++i)
    {
        if (deviceList[i] == deviceName) return i;
    }
    return 0;
}

void VideoStreamRender::open(const std::wstring deviceName)
{
    IMoniker *pMoniker;
    IPropertyBag *pPropertyBag;
    HRESULT hr;
    VARIANT var;

    std::cout<<"IN open"<<std::endl;
    
    //if opened, close it first
    if (isOpen()) close();
    
    bool bFindMoniker = false;
    bOpen = false;
    
    //enum video devices again
    if (pEnumMoniker == NULL) createDeviceEnumerator();
    if (pEnumMoniker == NULL) return;
    pEnumMoniker->Reset();

    //find device by name or descritpion
    while (pEnumMoniker->Next(1, &pMoniker, NULL) == S_OK)
    {
        hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropertyBag));
        if (FAILED(hr))
        {
            pMoniker->Release();
            continue;
        }
        
        VariantInit(&var);
        hr = pPropertyBag->Read(L"FriendlyName", &var, 0);
        if (FAILED(hr)) hr = pPropertyBag->Read(L"Description", &var, 0);
        if (SUCCEEDED(hr))
        {
            
            std::wcout << deviceName <<L"," <<var.bstrVal<< std::endl;
            if (deviceName.compare(var.bstrVal) == 0)
            {
                pPropertyBag->Release();
                bFindMoniker = true;
                break;
            }
            VariantClear(&var); 
        }
        pPropertyBag->Release();
        pMoniker->Release();
    }

    //device not found
    if (!bFindMoniker) 
    {
        std::cout<<"Device not found"<<std::endl;
        return;
    }

    //get video source filter
    hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pCaptureFilter);
    pMoniker->Release();
    CHECK_HR_FAILED(std::cout<<"Moniker.BindToObject failed"<<std::endl)

    //create graph by CaptureGraphBuilder2
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2,
                          NULL,
                          CLSCTX_INPROC,
                          IID_ICaptureGraphBuilder2,
                          (void**)&pCaptureGraphBuilder
                          );
    CHECK_HR_FAILED(std::cout<<"Create CaptureGraphBuilder failed"<<std::endl)
        
    hr = CoCreateInstance(CLSID_FilterGraph,
                          0,
                          CLSCTX_INPROC_SERVER,
                          IID_IGraphBuilder,
                          (void**)&pGraphBuilder
                          );
    CHECK_HR_FAILED(std::cout<<"Create GraphBuilder failed"<<std::endl)
        
    hr = pCaptureGraphBuilder->SetFiltergraph(pGraphBuilder);
    CHECK_HR_FAILED(std::cout<<"CaptureGraphBuilder.SetFiltergraph failed"<<std::endl)

    //add video souce filter into graph
    hr = pGraphBuilder->AddFilter(pCaptureFilter, L"Capture Filter");
    CHECK_HR_FAILED(std::cout<<"GraphBuilder.AddFilter failed"<<std::endl)

    bOpen = true;
}

const std::vector<std::wstring>& VideoStreamRender::listFormat()
{
    HRESULT hr;
    int piCount, piSize;
    AM_MEDIA_TYPE *pMediaType;
    VIDEO_STREAM_CONFIG_CAPS scc;

    formatList.clear();
    
    if (pStreamConfig == NULL) createStreamConfiger();
    if (pStreamConfig == NULL) return formatList;

    hr = pStreamConfig->GetNumberOfCapabilities(&piCount, &piSize);
    if (FAILED(hr))
    {
        std::cout << "IAMStreamConfig.GetNumberOfCapabilities failed" << std::endl;
        return formatList;
    }

    //loop all supported media type
    for (int i = 0; i < piCount; i++)
    {
        //FrameFormat fmt;
        pStreamConfig->GetStreamCaps(i, &pMediaType, reinterpret_cast<BYTE *>(&scc));

        std::wstring videoType = findVideoType(pMediaType->subtype);
        std::wstring headerType = findFormatType(pMediaType->formattype);
        uniqueAppend(formatList, videoType+L" "+headerType);
        //std::wcout<<videoType<<L" "<<headerType<<std::endl;
    }
    printList(formatList);
    return formatList;
}

const std::vector<std::wstring>& VideoStreamRender::listResolution()
{
    HRESULT hr;
    int piCount, piSize;
    AM_MEDIA_TYPE *pMediaType;
    VIDEO_STREAM_CONFIG_CAPS scc;
    
    resolutionList.clear();
    
    if (pStreamConfig == NULL) createStreamConfiger();
    if (pStreamConfig == NULL) return resolutionList;

    //loop all supported media type
    hr = pStreamConfig->GetNumberOfCapabilities(&piCount, &piSize);
    if (FAILED(hr))
    {
        std::cout<<"IAMStreamConfig.GetNumberOfCapabilities failed"<<std::endl;
        return resolutionList;
    }
    
    for (int i = 0; i < piCount; i++)
    {
        //FrameFormat fmt;
        pStreamConfig->GetStreamCaps(i, &pMediaType, reinterpret_cast<BYTE *>(&scc));
        if (FORMAT_VideoInfo == pMediaType->formattype)
        {

            VIDEOINFOHEADER *pvideoInfo = (VIDEOINFOHEADER *)pMediaType->pbFormat;
            std::wstringstream sstream;
            sstream<<pvideoInfo->bmiHeader.biWidth<<L" "<<pvideoInfo->bmiHeader.biHeight;
            uniqueAppend(resolutionList, sstream.str());
            //std::wcout<<sstream.str()<<std::endl;
        }
        else if (FORMAT_VideoInfo2 == pMediaType->formattype)
        {
            VIDEOINFOHEADER2 *pvideoInfo = (VIDEOINFOHEADER2 *)pMediaType->pbFormat;
            std::wstringstream sstream;
            sstream<<pvideoInfo->bmiHeader.biWidth<<L" "<<pvideoInfo->bmiHeader.biHeight;
            uniqueAppend(resolutionList, sstream.str());
            //std::wcout<<sstream.str()<<std::endl;
        }
    }
    printList(resolutionList);
    return resolutionList;
}

void VideoStreamRender::close()
{
    std::cout<<"IN close"<<std::endl;
    
    //stage 1, stop and release render related members
    if (isRun())
    {
        bRun = false;
        //stop video stream
        pMediaControl->Stop();
        //release render related members
        pMediaControl->Release();
        pVMRControl->Release();
        pMediaControl = NULL;
        pVMRControl = NULL;
        std::cout<<"stage 1"<<std::endl;
    }
    
    //stage 2, release all filter and device related members
    if (isOpen())
    {
        bOpen = false;
        releaseStreamConfiger();
        //release filters
        IEnumFilters *pEnum = NULL;
        HRESULT hr = pGraphBuilder->EnumFilters(&pEnum);
        if (SUCCEEDED(hr))
        {
            IBaseFilter *pFilter = NULL;
            while (S_OK == pEnum->Next(1, &pFilter, NULL))
             {
                 // remove and release one filter
                 pGraphBuilder->RemoveFilter(pFilter);
                 pFilter->Release();
                 
                 pEnum->Reset();
            }
            pEnum->Release();
        }
        pCaptureFilter = NULL;
        pVMRFilter = NULL;
        //release device related members
        pGraphBuilder->Release();
        pCaptureGraphBuilder->Release();
        std::cout<<"stage 2"<<std::endl;
    }
    
}

void* VideoStreamRender::getImage()
{
    BYTE* lpCurrImage;
    HRESULT hr;

    if (!isOpen()) return nullptr;
    
    hr = pVMRControl->GetCurrentImage(&lpCurrImage);
    if (FAILED(hr))
    {
        return nullptr;
    }
    
    return reinterpret_cast<void*>(lpCurrImage);
}

/*
 * set the render window and stream format
 */
void VideoStreamRender::set(
    RenderWin* pRenderWin,
    const std::wstring& format,
    const std::wstring& resolution)
{
    HRESULT hr;
    int piCount, piSize;
    AM_MEDIA_TYPE *pMediaType;
    VIDEO_STREAM_CONFIG_CAPS scc;
    int imageHeight;
    double radio;

    std::cout<<"IN set"<<std::endl;
    bool bFind = false;
    if (!isOpen()) return;

    this->pRenderWin = pRenderWin;
    std::wstringstream formatStream(format);
    std::wstringstream resolutionStream(resolution);
    std::wstring videoType;
    std::wstring formatType;
    int width;
    int height;
    formatStream>>videoType>>formatType;
    resolutionStream>>width>>height;

    if (double(width)/DEFAULT_IMAGE_WIDTH >= 1)
    {
        //use the image size
        pRenderWin->updateWindowSize(width+WIDTH_MARGIN,
            height+DEFAULT_TOOLBAR_HEIGHT+HEIGHT_MARGIN);
        pRenderWin->updateCtrlPos(height);
   }
    else
    {
        //fit the window size
        radio = double(DEFAULT_IMAGE_WIDTH)/width;
        imageHeight = (int)(height*radio);
        pRenderWin->updateWindowSize(DEFAULT_IMAGE_WIDTH+WIDTH_MARGIN,
            imageHeight+DEFAULT_TOOLBAR_HEIGHT+HEIGHT_MARGIN);
        pRenderWin->updateCtrlPos(imageHeight);
    }
    
    if (pStreamConfig == NULL) createStreamConfiger();
    if (pStreamConfig == NULL) return;

    //loop all supported media type
    hr = pStreamConfig->GetNumberOfCapabilities(&piCount, &piSize);
    CHECK_HR_FAILED(std::cout<<"IAMStreamConfig.GetNumberOfCapabilities failed"<<std::endl)
    for (int i = 0; i < piCount; i++)
    {
        pStreamConfig->GetStreamCaps(i, &pMediaType, reinterpret_cast<BYTE *>(&scc));
        if (FORMAT_VideoInfo == findFormatType(formatType) && FORMAT_VideoInfo == pMediaType->formattype)
        {

            VIDEOINFOHEADER *pvideoInfo = (VIDEOINFOHEADER *)pMediaType->pbFormat;
            if (findVideoType(videoType) == pMediaType->subtype && 
                width == pvideoInfo->bmiHeader.biWidth && 
                height == pvideoInfo->bmiHeader.biHeight)
            {
                frameRate = 10000000/pvideoInfo->AvgTimePerFrame;
                bFind = true;
                break;
            }
        }
        else if (FORMAT_VideoInfo2 == findFormatType(formatType) && FORMAT_VideoInfo2 == pMediaType->formattype)
        {
            VIDEOINFOHEADER2 *pvideoInfo = (VIDEOINFOHEADER2 *)pMediaType->pbFormat;
            if (findVideoType(videoType) == pMediaType->subtype && 
                width == pvideoInfo->bmiHeader.biWidth && 
                height == pvideoInfo->bmiHeader.biHeight)
            {
                frameRate = 10000000/pvideoInfo->AvgTimePerFrame;
                bFind = true;
                break;
            }
        }
    }
    
    // set format
    if (bFind) pStreamConfig->SetFormat(pMediaType);
}

/*
 * render stream
 */
void VideoStreamRender::render(int heightCtrlBar)
{
    long lWidth, lHeight;
    HRESULT hr;

    std::cout<<"IN render"<<std::endl;
    bRun = false;
    if (!isOpen()) return;

    //create VMR9 filter
    hr = CoCreateInstance(CLSID_VideoMixingRenderer9,
                          NULL, 
                          CLSCTX_INPROC,
                          IID_IBaseFilter,
                          (void**)&pVMRFilter); 
    CHECK_HR_FAILED(std::cout<<"Create VideoMixingRenderer9 failed"<<std::endl)

    hr = pGraphBuilder->AddFilter(pVMRFilter, L"Video Mixing Renderer");
    CHECK_HR_FAILED(std::cout<<"GraphBuilder.AddFilter failed"<<std::endl)

    //Set VMR Windowless mode
    IVMRFilterConfig9* pVMRFilterConfig; 
    hr = pVMRFilter->QueryInterface(IID_IVMRFilterConfig9, (void**)&pVMRFilterConfig);
    CHECK_HR_FAILED(std::cout<<"IBaseFilter.QueryInterface failed"<<std::endl)
    
    hr = pVMRFilterConfig->SetRenderingMode(VMR9Mode_Windowless);
    pVMRFilterConfig->Release(); 
    CHECK_HR_FAILED(std::cout<<"IVMRFilterConfig9.SetRenderingMode failed"<<std::endl)

    hr = pVMRFilter->QueryInterface(IID_IVMRWindowlessControl9, (void**)&pVMRControl);
    CHECK_HR_FAILED(std::cout<<"IBaseFilter.QueryInterface IVMRWindowlessControl9 failed"<<std::endl)

    hr = pVMRControl->SetVideoClippingWindow(pRenderWin->getHandle());
    CHECK_HR_FAILED(std::cout<<"IVMRWindowlessControl9.SetVideoClippingWindow failed"<<std::endl)

    //add VMRFilter into graph
    hr = pCaptureGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE,
                                            &MEDIATYPE_Video,
                                            pCaptureFilter,
                                            NULL,
                                            pVMRFilter);
    CHECK_HR_FAILED(std::cout<<"CaptureGraphBuilder.RenderStream failed"<<std::endl)

    
    hr = pVMRControl->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL);
    CHECK_HR_FAILED(std::cout<<"IVMRWindowlessControl9.GetNativeVideoSize failed"<<std::endl)

    SetRect(&videoRect, 0, 0, lWidth, lHeight); 
    GetClientRect(pRenderWin->getHandle(), &winRect);
    SetRect(&targetRect, 0, 0, winRect.right, winRect.bottom - heightCtrlBar); 
    hr = pVMRControl->SetVideoPosition(&videoRect, &targetRect);
    CHECK_HR_FAILED(std::cout<<"IVMRWindowlessControl9.SetVideoPosition failed"<<std::endl)

    //get media control and run
    hr = pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID*)&pMediaControl);
    CHECK_HR_FAILED(std::cout<<"GraphBuilder.QueryInterface IMediaControl failed"<<std::endl)
    hr = pMediaControl->Run();
    CHECK_HR_FAILED(std::cout<<"IMediaControl.Run failed"<<std::endl)

    this->pRenderWin = pRenderWin;
    bRun = true;

    //createRenderTarget(renderWin);
}

/*
 * set the render parameter and start render
 */
void VideoStreamRender::reset(const std::wstring deviceName)
{
    std::cout<<"IN reset"<<std::endl;
    std::vector<std::wstring> settings = pRenderWin->getUserSettings();

    if (deviceName.empty())
    {
        //device not changed
        open(settings[0]);
        if (!isOpen()) return;
    }
    else
    {
        //device changed
        open(deviceName);
        if (!isOpen()) return;
        listFormat();
        listResolution();
        pRenderWin->updateUserSettings(formatList, resolutionList);
        settings = pRenderWin->getUserSettings();
    }
    
    //set and render
    set(pRenderWin,settings[1],settings[2]);
    render();
    mixBitmap(L"IDI_ICON_APP");
}

void VideoStreamRender::paint()
{
    PAINTSTRUCT ps; 
    HDC hdc;
    hdc = BeginPaint(pRenderWin->getHandle(), &ps);
    
        FillRect(hdc, &winRect, (HBRUSH)(COLOR_WINDOW));
    if (isRun() && pVMRControl != NULL) 
    {   
        std::cout<<winRect.left <<' '
                 <<winRect.top  <<' '
                 <<winRect.right  <<' '
                 <<winRect.bottom  <<' ' <<std::endl;
        std::cout<<targetRect.left <<' '
                 <<targetRect.top  <<' '
                 <<targetRect.right  <<' '
                 <<targetRect.bottom  <<' ' <<std::endl;
        HRGN rgnClient = CreateRectRgnIndirect(&winRect); 
        HRGN rgnVideo  = CreateRectRgnIndirect(&targetRect);  
        CombineRgn(rgnClient, rgnClient, rgnVideo, RGN_DIFF);  

        HBRUSH hbr = GetSysColorBrush(COLOR_WINDOW); 
        FillRgn(hdc, rgnClient, hbr); 

        DeleteObject(hbr); 
        DeleteObject(rgnClient); 
        DeleteObject(rgnVideo); 

        pVMRControl->RepaintVideo(pRenderWin->getHandle(), hdc);
    } 
    EndPaint(pRenderWin->getHandle(), &ps);

}

void VideoStreamRender::snap()
{
    void* pFrame = getImage();
    if (pFrame == nullptr) std::cout<<"GetImage failed"<<std::endl;
    VideoFileWriter::WriteImageFile("test.jpg",pFrame);
}

void VideoStreamRender::capture(const bool bStart)
{
    if (bStart)
    {
        DWORD threadId;
        HANDLE hThread = CreateThread( 
            0,                   // default security attributes
            0,                      // use default stack size  
            CapProc,       // thread function name
            (void*)this,          // argument to thread function 
            0,                      // use default creation flags 
            &threadId);   // returns the thread identifier 
        bCapStarted = true;
    }
    else
    {
        bCapStarted = false;
    }
}

//bool VideoStreamRender::isPointed(int xPos, int yPos)
//{
//    return true;
//}

void VideoStreamRender::createDeviceEnumerator()
{
    std::cout<<"IN createDeviceEnumerator"<<std::endl;
    HRESULT hr;
    ICreateDevEnum *pDevEnum;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum,
                          NULL,  
                          CLSCTX_INPROC_SERVER, 
                          IID_PPV_ARGS(&pDevEnum)
                          );
    if (FAILED(hr))
    {
        std::cout<<"CreateDevEnum failed "<<hr<<std::endl;
        pEnumMoniker = NULL;
        return;
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
        pEnumMoniker = NULL;
    }

    pDevEnum->Release();
}

void VideoStreamRender::releaseDeviceEnumerator()
{
    if (pEnumMoniker != NULL) pEnumMoniker->Release();
    pEnumMoniker = NULL;
}

void VideoStreamRender::createStreamConfiger()
{
    std::cout<<"IN createStreamConfiger"<<std::endl;
    IPin *pCapPin;
    HRESULT hr;

    if (!isOpen()) 
    {
        pStreamConfig = NULL;
        return;
    }
    
    //get capture pin
    pCapPin = getCapturePin();
    if (pCapPin == nullptr)
    {
        std::cout<<"find Capture pin failed"<<std::endl;
        pStreamConfig = NULL;
        return;
    }

    //query stream config interface
    hr = pCapPin->QueryInterface(IID_IAMStreamConfig, (void**)&pStreamConfig);
    if (FAILED(hr))
    {
        std::cout<<"IPin.QueryInterface IAMStreamConfig failed"<<std::endl;
        pStreamConfig = NULL;
    }
}

void VideoStreamRender::releaseStreamConfiger()
{
    if (pStreamConfig != NULL) pStreamConfig->Release();
    pStreamConfig = NULL;
}

                             
IPin* VideoStreamRender::getCapturePin()
{
	IEnumPins *pEnumPins;
	IPin *pPin;
	PIN_INFO pinInfo;
	HRESULT hr;
    
    if (!isOpen()) return nullptr;

    hr = pCaptureFilter->EnumPins(&pEnumPins);
    if (FAILED(hr))
    {
        std::cout<<"IBaseFilter.EnumPins failed"<<std::endl;
        return nullptr;
    }

    while (pEnumPins->Next(1, &pPin, NULL) == S_OK)
    {
        hr = pPin->QueryPinInfo(&pinInfo);
        if (SUCCEEDED(hr) && wcscmp(L"Capture",pinInfo.achName) == 0)
        {
            pEnumPins->Release();
            return pPin;
        }
        pPin->Release();
    }
    pEnumPins->Release();
    return nullptr;
}
/*
void VideoStreamRender::createD2DRes()
{
    static const WCHAR msc_fontName[] = L"Verdana";
    static const FLOAT msc_fontSize = 50;
    HRESULT hr;
    bCreateRes = false;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2D1Factory);
    CHECK_HR_FAILED(std::cout<<"D2D1CreateFactory failed "<<hr<<std::endl;)
   
    // Create a DirectWrite factory.
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown **>(&pDWriteFactory)
        );
    CHECK_HR_FAILED(std::cout<<"DWriteCreateFactory failed "<<hr<<std::endl;)

    // Create a DirectWrite text format object.
    hr = pDWriteFactory->CreateTextFormat(
        msc_fontName,
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        msc_fontSize,
        L"", //locale
        &pDWriteTextFormat
        );
    CHECK_HR_FAILED(std::cout<<"DWriteFactory.CreateTextFormat failed "<<hr<<std::endl;)

    // Center the text horizontally and vertically.
    pDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    pDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);


    // Create a DC render target.
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(
            DXGI_FORMAT_B8G8R8A8_UNORM,
            D2D1_ALPHA_MODE_IGNORE),
        0,
        0,
        D2D1_RENDER_TARGET_USAGE_NONE,
        D2D1_FEATURE_LEVEL_DEFAULT
    );

    hr = pD2D1Factory->CreateDCRenderTarget(&props, &pRenderTarget);
    CHECK_HR_FAILED(std::cout<<"D2D1Factory.CreateDCRenderTarget failed "<<hr<<std::endl;)
      
    hr = pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black),
        &pBlackBrush
        ); 
    CHECK_HR_FAILED(std::cout<<"DCRenderTarget.CreateSolidColorBrush failed "<<hr<<std::endl;)

    if (SUCCEEDED(hr)) bCreateRes = true;
}
*/

void VideoStreamRender::mixBitmap(const wchar_t *bitmapName)
{
    /*
    IDirect3D9* pDirect3D9;
    IDirect3DDevice9* pDirect3DDevice9;
    IDirect3DSurface9* pDirect3DSurface9;
    D3DPRESENT_PARAMETERS d3dpp = { 0 };
    
    d3dpp.Windowed   = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
    
    pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
    pDirect3D9->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        renderWin,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp,
        &pDirect3DDevice9);

    pDirect3DDevice9->CreateOffscreenPlainSurface(
        720,
        480,
        D3DFMT_A8R8G8B8,
        D3DPOOL_DEFAULT,
        &pDirect3DSurface9, 
        NULL);


    HDC hDC = NULL;
    pDirect3DSurface9->GetDC(hDC);
    */
    HDC hDC,hMemDc;
    HGDIOBJ hGO;
    HBITMAP hBmp;
    HRESULT hr;

    if (!isRun()) return;
        
    hDC = GetDC(pRenderWin->getHandle());
    hMemDc = CreateCompatibleDC(hDC);
    hBmp = LoadBitmap(pRenderWin->getApp(),bitmapName);
    hGO = SelectObject(hMemDc,hBmp);
    
    //RECT rSrc = ;
    //RECT rDst = ;
    VMR9AlphaBitmap infoBmp;
    infoBmp.dwFlags = VMR9AlphaBitmap_hDC;
    infoBmp.hdc = hMemDc;
    infoBmp.pDDS = 0;
    infoBmp.rSrc = { 0,0,32,32 };
    infoBmp.rDest.left     = 0.0f;
    infoBmp.rDest.top      = 1.0f - ((float)32 / (float)winRect.bottom);
    infoBmp.rDest.right    = ((float)32 / (float)winRect.right);
    infoBmp.rDest.bottom   = 1.0f;
    infoBmp.fAlpha = 0.25;
    infoBmp.clrSrcKey = 0;
    infoBmp.dwFilterMode = 0;

    IVMRMixerBitmap9* pVMRMixerBitmap9; 
    hr = pVMRFilter->QueryInterface(IID_IVMRMixerBitmap9, (void**)&pVMRMixerBitmap9);
    CHECK_HR_FAILED(std::cout<<"IBaseFilter.QueryInterface failed"<<std::endl)

    hr = pVMRMixerBitmap9->SetAlphaBitmap(&infoBmp);
    CHECK_HR_FAILED(std::cout<<"IVMRMixerBitmap9.SetAlphaBitmap failed"<<std::endl)
}

void VideoStreamRender::printFormat(FrameFormat & fmt)
{
    wchar_t buffer[GUID_STRING_SIZE] = { 0 };
    std::wstring name = findVideoType(fmt.videoType);
    std::wcout <<L"video type:"<<name;
   
    if (fmt.formatType == FORMAT_VideoInfo)
    {
        std::wcout<<L", formate type:FORMAT_VideoInfo";
    }
    else if (fmt.formatType == FORMAT_VideoInfo2)
    {
        std::wcout<<L", formate type:FORMAT_VideoInfo2";
    }
    else
    {
        StringFromGUID2(fmt.formatType, buffer, GUID_STRING_SIZE);
        std::wcout<<L", format type:"<<buffer;
    }
    
    std::wcout<<L", width:"<<fmt.width;
    std::wcout<<L", height:"<<fmt.height;
    std::wcout<<L", frame rate:"<<10000000/fmt.rate<< std::endl;
}

void VideoStreamRender::uniqueAppend(std::vector<std::wstring>& list, const std::wstring newItem)
{
    for(auto item: list)
    {
        if (item == newItem) return;
    }
    list.emplace_back(newItem);
}

void VideoStreamRender::printList(std::vector<std::wstring>& list)
{
    for(auto item: list)
    {
        std::wcout<<item<<std::endl;
    }
}