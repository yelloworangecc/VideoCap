#ifndef UNICODE
#define UNICODE
#endif
#include <iostream>
#include <windows.h>
#include <strmif.h>
#include <comsvcs.h>
#include <dvdmedia.h>
#include "VideoStreamRender.h"
#include "VideoFileWriter.h"

#define CHECK_HR_FAILED(statement) if(FAILED(hr)) {statement;return;}

bool VideoStreamRender::bCapStarted = false;

DWORD WINAPI VideoStreamRender::CapProc(LPVOID lpParam)
{
    VideoStreamRender* pVsr = reinterpret_cast<VideoStreamRender*>(lpParam);
    VideoFileWriter vfw("test.avi");
    while(bCapStarted)
    {
        void* pFrame = pVsr->getImage();
        if (pFrame == nullptr) std::cout<<"GetImage failed"<<std::endl;
        vfw.writeBitmapFrame(pFrame);
        Sleep(66);//assume fps is 15
    }
    return 0;
}

void VideoStreamRender::listDevice()
{
    ICreateDevEnum *pDevEnum;
    IEnumMoniker *pEnumMoniker;
    IMoniker *pMoniker;
    IPropertyBag *pPropertyBag;
    VARIANT var;
    HRESULT hr;
    
    CoInitialize(0);
    
    hr = CoCreateInstance(CLSID_SystemDeviceEnum,
                          NULL,  
                          CLSCTX_INPROC_SERVER, 
                          IID_PPV_ARGS(&pDevEnum)
                          );
    CHECK_HR_FAILED(std::cout<<"CreateDevEnum failed "<<hr<<std::endl;CoUninitialize();)

    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
                                         &pEnumMoniker,
                                         0);
    if (hr == S_FALSE)
    {
        hr = VFW_E_NOT_FOUND;
        std::cout<<"No device found"<<std::endl;
    }
    CHECK_HR_FAILED(std::cout<<"Create CreateClassEnumerator failed"<<std::endl;CoUninitialize();)

    pDevEnum->Release();

    //list device
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
            std::wcout<<var.bstrVal<<std::endl;
            VariantClear(&var); 
        }
        pPropertyBag->Release();
        pMoniker->Release();
    }

    pEnumMoniker->Release();
    CoUninitialize();
}

VideoStreamRender::VideoStreamRender(const std::wstring        &_deviceName):
    deviceName(deviceName)
{
    CoInitialize(0);
    createD2DRes();
}
    
VideoStreamRender::VideoStreamRender(const wchar_t *pDeviceName):
    deviceName(pDeviceName)
{
    CoInitialize(0);
    createD2DRes();
}
    
VideoStreamRender::~VideoStreamRender()
{
     CoUninitialize();
}

void VideoStreamRender::open()
{
    ICreateDevEnum *pDevEnum;
    IEnumMoniker *pEnumMoniker;
    IMoniker *pMoniker;
	IPropertyBag *pPropertyBag;
    HRESULT hr;
    VARIANT var;
    bool bFindMoniker = false;
    bOpen = false;
    
    //enum video devices
    hr = CoCreateInstance(CLSID_SystemDeviceEnum,
                          NULL,  
                          CLSCTX_INPROC_SERVER, 
                          IID_PPV_ARGS(&pDevEnum)
                          );
    CHECK_HR_FAILED(std::cout<<"CreateDevEnum failed "<<hr<<std::endl)

    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
                                         &pEnumMoniker,
                                         0);
    if (hr == S_FALSE)
    {
        hr = VFW_E_NOT_FOUND;
        std::cout<<"No device found"<<std::endl;
    }
    CHECK_HR_FAILED(std::cout<<"Create CreateClassEnumerator failed"<<std::endl)

    pDevEnum->Release();

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
    if (!bFindMoniker) return;

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

void VideoStreamRender::close()
{
    if (isOpen())
    {
        pCaptureFilter->Release();
        pGraphBuilder->Release();
        pCaptureGraphBuilder->Release();
    }
    if (isRun())
    {
        pMediaControl->Stop();
        pMediaControl->Release();
        pVMRControl->Release();
        pVMRFilter->Release();
    }
    bOpen = false;
    bRun = false;
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

void VideoStreamRender::listFormat()
{
    IPin *pCapPin;
    IAMStreamConfig *pStreamConfig;
    HRESULT hr;
    int piCount, piSize;
    AM_MEDIA_TYPE *pMediaType;
    VIDEO_STREAM_CONFIG_CAPS scc;
    bool bFind = false;

    if (!isOpen()) return;
    
    //get capture pin
    pCapPin = getCapturePin();
    if (pCapPin == nullptr)
    {
        std::cout<<"find Capture pin failed"<<std::endl;
        return;
    }

    //query stream config interface
    hr = pCapPin->QueryInterface(IID_IAMStreamConfig, (void**)&pStreamConfig);
    CHECK_HR_FAILED(std::cout<<"IPin.QueryInterface IAMStreamConfig failed"<<std::endl)

    //loop all supported media type
    hr = pStreamConfig->GetNumberOfCapabilities(&piCount, &piSize);
    CHECK_HR_FAILED(std::cout<<"IAMStreamConfig.GetNumberOfCapabilities failed"<<std::endl)
    for (int i = 0; i < piCount; i++)
    {
        FrameFormat fmt;
        pStreamConfig->GetStreamCaps(i, &pMediaType, reinterpret_cast<BYTE *>(&scc));
        if (FORMAT_VideoInfo == pMediaType->formattype)
        {

            VIDEOINFOHEADER *pvideoInfo = (VIDEOINFOHEADER *)pMediaType->pbFormat;
            fmt.videoType = pMediaType->subtype;
            fmt.formatType = FORMAT_VideoInfo;
            fmt.width = pvideoInfo->bmiHeader.biWidth;
            fmt.height = pvideoInfo->bmiHeader.biHeight;
            fmt.rate = pvideoInfo->AvgTimePerFrame;
        }
        else if (FORMAT_VideoInfo2 == pMediaType->formattype)
        {
            VIDEOINFOHEADER2 *pvideoInfo = (VIDEOINFOHEADER2 *)pMediaType->pbFormat;
            fmt.videoType = pMediaType->subtype;
            fmt.formatType = FORMAT_VideoInfo2;
            fmt.width = pvideoInfo->bmiHeader.biWidth;
            fmt.height = pvideoInfo->bmiHeader.biHeight;
            fmt.rate = pvideoInfo->AvgTimePerFrame;
        }
        printFormat(fmt);
    }
    
    pStreamConfig->Release();
}

void VideoStreamRender::setFormat(const GUID &videoType,
                                      const GUID &formatType,
                                      long width,
                                      long height)
{
    IPin *pCapPin;
    IAMStreamConfig *pStreamConfig;
    HRESULT hr;
    int piCount, piSize;
    AM_MEDIA_TYPE *pMediaType;
    VIDEO_STREAM_CONFIG_CAPS scc;
    bool bFind = false;

    if (!isOpen()) return;
    
    //get capture pin
    pCapPin = getCapturePin();
    if (pCapPin == nullptr)
    {
        std::cout<<"find Capture pin failed"<<std::endl;
        return;
    }

    //query stream config interface
    hr = pCapPin->QueryInterface(IID_IAMStreamConfig, (void**)&pStreamConfig);
    CHECK_HR_FAILED(std::cout<<"IPin.QueryInterface IAMStreamConfig failed"<<std::endl)

    //loop all supported media type
    hr = pStreamConfig->GetNumberOfCapabilities(&piCount, &piSize);
    CHECK_HR_FAILED(std::cout<<"IAMStreamConfig.GetNumberOfCapabilities failed"<<std::endl)
    for (int i = 0; i < piCount; i++)
    {
        pStreamConfig->GetStreamCaps(i, &pMediaType, reinterpret_cast<BYTE *>(&scc));
        if (FORMAT_VideoInfo == formatType && FORMAT_VideoInfo == pMediaType->formattype)
        {

            VIDEOINFOHEADER *pvideoInfo = (VIDEOINFOHEADER *)pMediaType->pbFormat;
            if (videoType == pMediaType->subtype && 
                width == pvideoInfo->bmiHeader.biWidth && 
                height == pvideoInfo->bmiHeader.biHeight)
            {
            	bFind = true;
                break;
            }
        }
		else if (FORMAT_VideoInfo2 == formatType && FORMAT_VideoInfo2 == pMediaType->formattype)
        {
            VIDEOINFOHEADER2 *pvideoInfo = (VIDEOINFOHEADER2 *)pMediaType->pbFormat;
            if (videoType == pMediaType->subtype && 
                width == pvideoInfo->bmiHeader.biWidth && 
                height == pvideoInfo->bmiHeader.biHeight)
            {
            	bFind = true;
                break;
            }
        }
    }
    
    // set format
    if (bFind) pStreamConfig->SetFormat(pMediaType);
    pStreamConfig->Release();
}

void VideoStreamRender::render(HWND hwnd, int heightCtrlBar)
{
    long lWidth, lHeight;
    HRESULT hr;

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

    hr = pVMRControl->SetVideoClippingWindow(hwnd);
    CHECK_HR_FAILED(std::cout<<"IVMRWindowlessControl9.SetVideoClippingWindow failed"<<std::endl)

    hr = pVMRControl->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL);
    CHECK_HR_FAILED(std::cout<<"IVMRWindowlessControl9.SetVideoClippingWindow failed"<<std::endl)

    SetRect(&videoRect, 0, 0, lWidth, lHeight); 
    GetClientRect(hwnd, &winRect);
    SetRect(&targetRect, 0, 0, winRect.right, winRect.bottom - heightCtrlBar); 
    hr = pVMRControl->SetVideoPosition(&videoRect, &targetRect);
    CHECK_HR_FAILED(std::cout<<"IVMRWindowlessControl9.SetVideoPosition failed"<<std::endl)

    //add VMRFilter into graph
    hr = pCaptureGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE,
                                            &MEDIATYPE_Video,
                                            pCaptureFilter,
                                            NULL,
                                            pVMRFilter);
    CHECK_HR_FAILED(std::cout<<"CaptureGraphBuilder.RenderStream failed"<<std::endl)

    //get media control and run
    hr = pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID*)&pMediaControl);
    CHECK_HR_FAILED(std::cout<<"GraphBuilder.QueryInterface IMediaControl failed"<<std::endl)
    hr = pMediaControl->Run();
    CHECK_HR_FAILED(std::cout<<"IMediaControl.Run failed"<<std::endl)

    renderWin = hwnd;
    bRun = true;

    //createRenderTarget(renderWin);
}

void VideoStreamRender::paint()
{
    PAINTSTRUCT ps; 
    HDC hdc;
    hdc = BeginPaint(renderWin, &ps);
    
    if (isRun()) 
    {   
        HRGN rgnClient = CreateRectRgnIndirect(&winRect); 
        HRGN rgnVideo  = CreateRectRgnIndirect(&targetRect);  
        CombineRgn(rgnClient, rgnClient, rgnVideo, RGN_DIFF);  

        HBRUSH hbr = GetSysColorBrush(COLOR_BTNFACE); 
        FillRgn(hdc, rgnClient, hbr); 

        DeleteObject(hbr); 
        DeleteObject(rgnClient); 
        DeleteObject(rgnVideo); 

        pVMRControl->RepaintVideo(renderWin, hdc);
    } 
    else
    { 
        FillRect(hdc, &targetRect, (HBRUSH)(COLOR_WINDOW));
    } 
    
    EndPaint(renderWin, &ps);

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

bool VideoStreamRender::isPointed(int xPos, int yPos)
{
    return true;
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


void VideoStreamRender::mixBitmap(HINSTANCE hApp, const wchar_t *bitmapName)
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
        
    hDC = GetDC(renderWin);
    hMemDc = CreateCompatibleDC(hDC);
    hBmp = LoadBitmap(hApp,bitmapName);
    hGO = SelectObject(hMemDc,hBmp);
    
    //RECT rSrc = ;
    //RECT rDst = ;
    VMR9AlphaBitmap infoBmp;
    infoBmp.dwFlags = VMR9AlphaBitmap_hDC;
    infoBmp.hdc = hMemDc;
    infoBmp.pDDS = 0;
    infoBmp.rSrc = { 0,0,32,32 };
    infoBmp.rDest.left     = 0.0f;
    infoBmp.rDest.top      = 1.0f - ((float)32 / (float)480);
    infoBmp.rDest.right    = ((float)32 / (float) 640);
    infoBmp.rDest.bottom   = 1.0f;
    infoBmp.fAlpha = 0.5;
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

    if (fmt.videoType == MEDIASUBTYPE_YUY2)
    {
        std::wcout<<L"video type:YUY2";
    }
    else if (fmt.videoType == MEDIASUBTYPE_MJPG)
    {
        std::wcout<<L"video type:MJPG";
    }
    else
    {
        StringFromGUID2(fmt.videoType,buffer,GUID_STRING_SIZE);
        std::wcout<<L"video type:"<<buffer;
    }

    if (fmt.formatType == FORMAT_VideoInfo)
    {
        std::wcout<<L", video type:FORMAT_VideoInfo";
    }
    else if (fmt.formatType == FORMAT_VideoInfo2)
    {
        std::wcout<<L", video type:FORMAT_VideoInfo2";
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

