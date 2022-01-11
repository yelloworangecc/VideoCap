#include <iostream>
#include <cassert>
#include <windows.h>
#include <commctrl.h>
#include <strmif.h>
#include <dvdmedia.h>
#include <opencv2/opencv.hpp>


#include "VideoDevice.h"

VideoDevice::VideoDevice():
	hasDevice(false),pEnumMoniker(NULL),pMoniker(NULL),
	pCaptureGraphBuilder(NULL),pGraphBuilder(NULL),
	pCaptureFilter(NULL),pVMRFilter(NULL),pCameraControl(NULL),
	pMediaControl(NULL),pMediaEvent(NULL),pVMRControl(NULL)
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
}

VideoDevice::~VideoDevice()
{
	stop();
	if (pVMRControl!= NULL)           pVMRControl->Release();
	if (pMediaEvent != NULL)          pMediaEvent->Release();
	if (pMediaControl != NULL)        pMediaControl->Release();
	if (pCameraControl != NULL)       pCameraControl->Release();
	if (pVMRFilter != NULL)           pVMRFilter->Release();
	if (pCaptureFilter != NULL)       pCaptureFilter->Release();
	if (pGraphBuilder != NULL)        pGraphBuilder->Release();
	if (pCaptureGraphBuilder != NULL) pCaptureGraphBuilder->Release();
	if (pMoniker != NULL)             pMoniker->Release();
	if (pEnumMoniker != NULL)         pEnumMoniker->Release();
	
    CoUninitialize();
}

void VideoDevice::find(wchar_t * name)
{
	IMoniker *pMon;
	IPropertyBag *pPro;
	HRESULT hr;
	bool result = createDeviceEnum();
	
    while (pEnumMoniker->Next(1, &pMon, NULL) == S_OK)
    {
        hr = pMon->BindToStorage(0, 0, IID_PPV_ARGS(&pPro));
		if (FAILED(hr))
		{
			pMon->Release();
			continue;
		}
		
        VARIANT var;
        VariantInit(&var);

        hr = pPro->Read(L"Description", &var, 0);
        if (FAILED(hr)) hr = pPro->Read(L"FriendlyName", &var, 0);

        if (SUCCEEDED(hr))
        {
        	if (name == nullptr)
        	{
	            std::wcout<<var.bstrVal<<std::endl;
        	}
			else
			{
				if (wcscmp(name,var.bstrVal) == 0)
				{
					pPro->Release();
				    pMoniker = pMon;
					break;
				}
			}
			VariantClear(&var); 
        }
        pPro->Release();
        pMon->Release();
    }
}

void VideoDevice::setMainWin(HWND hWin)
{
	HRESULT hr;
	hwnd = hWin;
	hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
                           IID_ICaptureGraphBuilder2, (void**)&pCaptureGraphBuilder);
	hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER,
                          IID_IGraphBuilder, (void**)&pGraphBuilder);
	pCaptureGraphBuilder->SetFiltergraph(pGraphBuilder);

	hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pCaptureFilter);
	hr = pGraphBuilder->AddFilter(pCaptureFilter, L"Capture Filter");

	//SetFormat(MEDIASUBTYPE_YUY2,1600L,1200L,false);
/*
	std::cout<<"add smart tee"<<std::endl;
	IBaseFilter *pSmartTee;
	hr = CoCreateInstance(CLSID_SmartTee, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pSmartTee));
	hr = pGraphBuilder->AddFilter(pSmartTee, L"Smart Tee");

	std::cout<<"connect capture to smart tee"<<std::endl;
	hr = pCaptureGraphBuilder->FindPin(
	  pCaptureFilter,
	  PINDIR_OUTPUT,
	  &PIN_CATEGORY_CAPTURE,
	  &MEDIATYPE_Video,
	  true,
	  0,
	  &pPinOut
	);
	if (FAILED(hr))
	{
		std::cout<<"unable to find output pin"<<std::endl;
		return;
	}
	hr = pCaptureGraphBuilder->FindPin(
	  pSmartTee,
	  PINDIR_INPUT,
	  NULL,
	  NULL,
	  true,
	  0,
	  &pPinIn
	);
	if (FAILED(hr))
	{
		std::cout<<"unable to find input pin"<<std::endl;
		return;
	}
	pGraphBuilder->Connect(pPinOut,pPinIn);*/

	/*
	IBaseFilter* pASFWriter = 0;
	hr = pCaptureGraphBuilder->SetOutputFileName(
	  &MEDIASUBTYPE_Asf, // Create a Windows Media file.
	  L"test.wmv", // File name.
	  &pASFWriter, // Receives a pointer to the filter.
	  NULL); // Receives an IFileSinkFilter interface pointer (optional).
	hr = pCaptureGraphBuilder->RenderStream(
	  &PIN_CATEGORY_CAPTURE, // Capture pin.
	  &MEDIATYPE_Video, // Video. Use MEDIATYPE_Audio for audio.
	  pCaptureFilter, // Pointer to the capture filter. 
	  0, 
	  pASFWriter);
	  */

	InitWindowlessVMR9(hwnd);
	hr = pCaptureGraphBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pCaptureFilter, NULL, pVMRFilter);

	//IBaseFilter *pAviMuxFilter;
	//pCaptureGraphBuilder->SetOutputFileName(&MEDIASUBTYPE_Avi, L"graph.avi", &pAviMuxFilter, NULL);
	//ListPin(pAviMuxFilter);
	//pCaptureGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pCaptureFilter, NULL, pAviMuxFilter);

	//get smart tee filter
	FILTER_INFO filterInfo;
	IBaseFilter * pSmartTee;
	hr = GetNextFilter(pCaptureFilter,PINDIR_OUTPUT,&pSmartTee);
	if (FAILED(hr))
	{
		std::cout<<"can not find next filter"<<std::endl;
		return;
	}
	pSmartTee->QueryFilterInfo(&filterInfo);
	std::wcout<<"Filter:"<<filterInfo.achName<<std::endl;
	ListPin(pSmartTee);

	//create avi mux
	IBaseFilter *pAviMux;
	hr = CoCreateInstance(CLSID_AviDest, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pAviMux));
	hr = pGraphBuilder->AddFilter(pAviMux, L"Avi Mux");
	ListPin(pAviMux);

	//create file write
	IBaseFilter *pFileWriter;
	hr = CoCreateInstance(CLSID_FileWriter, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileWriter));
	hr = pGraphBuilder->AddFilter(pFileWriter, L"File writer");
	ListPin(pFileWriter);

	IPin *pFrom,*pTo;
	//connect avi mux
	pFrom = FindPin(pSmartTee, L"Capture");
	pTo = FindPin(pAviMux,L"Input 01");
	pGraphBuilder->Connect(pFrom,pTo);

	//connect file writer
	pFrom = FindPin(pAviMux, L"AVI Out");
	pTo = FindPin(pFileWriter,L"in");
	pGraphBuilder->Connect(pFrom,pTo);

	//set file name
	IFileSinkFilter2 *pFileSink;
	pFileWriter->QueryInterface(IID_IFileSinkFilter2,(void**)&pFileSink);

	IPin* capture = FindPin(pSmartTee, L"Capture");
	IAMStreamControl *pStreamControl;
	hr = capture->QueryInterface(IID_IAMStreamControl, (void**)&pStreamControl);
	if (FAILED(hr)) 
	{
		std::cout<<"unable to query IID_IAMStreamControl interface"<<std::hex<<hr<<std::endl;
		return;
	}
	REFERENCE_TIME rtStart = 1000000 , rtStop = 10000000;
	const WORD wStartCookie = 1, wStopCookie = 2; // Arbitrary values.
	pStreamControl->StartAt(&rtStart,wStartCookie);
	if (FAILED(hr)) std::cout<<"failed to start stream"<<std::hex<<hr<<std::endl;
	pStreamControl->StopAt(NULL,TRUE,wStopCookie);
	if (FAILED(hr)) std::cout<<"failed to stop stream"<<std::hex<<hr<<std::endl;


	AM_MEDIA_TYPE fmt = {0};
	fmt.majortype = MEDIATYPE_Stream;
	pFileSink->SetFileName(L"test.avi",NULL);

	IBaseFilter * pNext;
	hr = GetNextFilter(pSmartTee,PINDIR_OUTPUT,&pNext);
	pNext->QueryFilterInfo(&filterInfo);
	std::wcout<<"Filter:"<<filterInfo.achName<<std::endl;

	hr = GetNextFilter(pNext,PINDIR_OUTPUT,&pNext);
	pNext->QueryFilterInfo(&filterInfo);
	std::wcout<<"Filter:"<<filterInfo.achName<<std::endl;

	hr = GetNextFilter(pNext,PINDIR_OUTPUT,&pNext);
	pNext->QueryFilterInfo(&filterInfo);
	std::wcout<<"Filter:"<<filterInfo.achName<<std::endl;

	//hr = pCaptureGraphBuilder->ControlStream(
	//  &PIN_CATEGORY_CAPTURE, // Pin category.
	//  &MEDIATYPE_Video, // Media type.
	//  pSmartTee, // Capture filter.
	//  &rtStart, &rtStop, // Start and stop times.
	//  wStartCookie, wStopCookie // Values for the start and stop events.
	//  );
	//if (FAILED(hr)) std::cout<<"failed to control stream"<<std::hex<<hr<<std::endl;
/*
	IFileSinkFilter2 *pFileSink;
	AM_MEDIA_TYPE fmt = {0};
	fmt.majortype = MEDIATYPE_Stream
	LPOLESTR pBuffer = new OLECHAR[128];
	pNext->QueryInterface(IID_IFileSinkFilter2,(void**)&pFileSink);
	pFileSink->GetCurFile(&pBuffer,&fmt);
	std::wcout<<pBuffer<<std::endl;
	StringFromGUID2(fmt.majortype,pBuffer,128);//{E436EB83-524F-11CE-9F53-0020AF0BA770}
	std::wcout<<pBuffer<<std::endl;
	StringFromGUID2(fmt.subtype,pBuffer,128);//{00000000-0000-0000-0000-000000000000}
	std::wcout<<pBuffer<<std::endl;
	std::cout<<fmt.bFixedSizeSamples<<std::endl;//0
	std::cout<<fmt.bTemporalCompression<<std::endl;//0
	std::cout<<fmt.lSampleSize<<std::endl;//0
	StringFromGUID2(fmt.formattype,pBuffer,128);//{00000000-0000-0000-0000-000000000000}
	std::wcout<<pBuffer<<std::endl;
	std::cout<<(long)fmt.cbFormat<<std::endl;

	StringFromGUID2(FORMAT_VideoInfo,pBuffer,128);
	std::wcout<<pBuffer<<std::endl;
	StringFromGUID2(FORMAT_VideoInfo2,pBuffer,128);
	std::wcout<<pBuffer<<std::endl;
*/
	
	/*

	

	hr = GetNextFilter(pNext,PINDIR_OUTPUT,&pNext);
	pNext->QueryFilterInfo(&filterInfo);
	std::wcout<<"Filter:"<<filterInfo.achName<<std::endl;

	hr = GetNextFilter(pNext,PINDIR_OUTPUT,&pNext);
	pNext->QueryFilterInfo(&filterInfo);
	std::wcout<<"Filter:"<<filterInfo.achName<<std::endl;*/
/*
	GetNextFilter(pSmartTee,PINDIR_OUTPUT,&pNext);
	pNext->QueryFilterInfo(&filterInfo);
	std::wcout<<"Filter:"<<filterInfo.achName<<std::endl;

	GetNextFilter(pNext,PINDIR_OUTPUT,&pNext);
	pNext->QueryFilterInfo(&filterInfo);
	std::wcout<<"Filter:"<<filterInfo.achName<<std::endl;
	*/

	hr = pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID*)&pMediaControl);
	hr = pGraphBuilder->QueryInterface(IID_IMediaEvent, (LPVOID*)&pMediaEvent);
}

void VideoDevice::setZoomBar(HWND hBar)
{
	long min,max,step,deft,isAuto;
	HRESULT hr;

	if (pCaptureFilter == NULL) return;

	hbar = hBar;
	hr = pCaptureFilter->QueryInterface(IID_IAMCameraControl,(LPVOID*)&pCameraControl);
	pCameraControl->GetRange(CameraControl_Zoom,&min,&max,&step,&deft,&isAuto);
	SendMessage(hBar, TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG(min, max));
	SendMessage(hBar, TBM_SETPAGESIZE, 0, (LPARAM) step);
	SendMessage(hBar, TBM_SETSEL, (WPARAM) FALSE, (LPARAM) MAKELONG(min, max)); 
	SendMessage(hBar, TBM_SETPOS, (WPARAM) TRUE, (LPARAM) deft); 
}


void VideoDevice::start()
{
	HRESULT hr;
	if (pMediaControl == NULL) return;
	hr = pMediaControl->Run();
	if (FAILED(hr)) std::cout<<"MediaControl run failed"<<std::hex<<hr<<std::endl;
}

void VideoDevice::stop()
{
	HRESULT hr;
	if (pMediaControl == NULL) return;
	hr = pMediaControl->Pause();
	if (FAILED(hr)) std::cout<<"MediaControl pause failed"<<std::endl;
	hr = pMediaControl->Stop();
	if (FAILED(hr)) std::cout<<"MediaControl stop failed"<<std::endl;
}

void VideoDevice::snap()
{
	BYTE* lpCurrImage = NULL;
	DWORD dwSize, dwWritten;
	
	pVMRControl->GetCurrentImage(&lpCurrImage);
	LPBITMAPINFOHEADER pdib = (LPBITMAPINFOHEADER) lpCurrImage;
/*
DWORD 	biSize; //Specifies the number of bytes required by the structure
LONG  	biWidth; //Specifies the width of the bitmap, in pixels
LONG  	biHeight; //Specifies the height of the bitmap, in pixels
WORD 	biPlanes; //Specifies the number of planes for the target device.
WORD 	biBitCount; //Specifies the number of bits per pixel
DWORD 	biCompression; //FOURCC code or BI_RGB(0)/BI_BITFIELDS(3)
DWORD 	biSizeImage; // Specifies the size, in bytes, of the image
LONG  	biXPelsPerMeter; //Specifies the horizontal resolution, in pixels per meter, of the target device for the bitmap.
LONG  	biYPelsPerMeter; //Specifies the vertical resolution, in pixels per meter, of the target device for the bitmap.
DWORD 	biClrUsed; //Specifies the number of color indices in the color table that are actually used by the bitmap
DWORD 	biClrImportant; //pecifies the number of color indices that are considered important for displaying the bitmap
*/
	std::cout<<pdib->biSize<<std::endl; //40
	std::cout<<pdib->biWidth<<std::endl; //320
	std::cout<<pdib->biHeight<<std::endl; //240
	std::cout<<pdib->biPlanes<<std::endl; //1
	std::cout<<pdib->biBitCount<<std::endl; //32
	std::cout<<pdib->biCompression<<std::endl; //0
	std::cout<<pdib->biSizeImage<<std::endl; //307200
	std::cout<<pdib->biXPelsPerMeter<<std::endl; //0
	std::cout<<pdib->biYPelsPerMeter<<std::endl; //0
	std::cout<<pdib->biClrUsed<<std::endl; //0
	std::cout<<pdib->biClrImportant<<std::endl; //0

    cv::Mat image(cv::Size(pdib->biWidth, pdib->biHeight), CV_8UC4, (void*)(pdib+1), cv::Mat::AUTO_STEP);
	
  	HANDLE hFile = CreateFile("test.bmp",
		                      GENERIC_WRITE,
		                      FILE_SHARE_READ,
		                      NULL,
                              CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL,
                              0);

  	if (hFile == INVALID_HANDLE_VALUE) return;

  	// Initialize the bitmap header
  	dwSize = pdib->biSizeImage;
  	BITMAPFILEHEADER      hdr;
  	hdr.bfType            = 0x4d42;
  	hdr.bfSize            = dwSize + pdib->biSize;
  	hdr.bfReserved1       = 0;
  	hdr.bfReserved2       = 0;
  	hdr.bfOffBits         = (DWORD)sizeof(BITMAPFILEHEADER) + pdib->biSize;// + DibPaletteSize(pdib);

  	// Write the bitmap header and bitmap bits to the file
  	WriteFile(hFile, (LPCVOID) &hdr, sizeof(BITMAPFILEHEADER), &dwWritten, 0);
  	WriteFile(hFile, (LPCVOID) pdib, dwSize + pdib->biSize, &dwWritten, 0);

  	CloseHandle(hFile);
	CoTaskMemFree(lpCurrImage);

	//cv::Mat image;
    //image = cv::imread("test.bmp", 1 );
	cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
    cv::imshow("Display Image", image);
	cv::waitKey(0);
	
	return;
}
void VideoDevice::paint()
{
	PAINTSTRUCT ps; 
    HDC         hdc; 
    hdc = BeginPaint(hwnd, &ps);
	
    if (pVMRControl != NULL) 
    { 
		HRGN rgnClient = CreateRectRgnIndirect(&winRect); 
		HRGN rgnVideo  = CreateRectRgnIndirect(&targetRect);  
		CombineRgn(rgnClient, rgnClient, rgnVideo, RGN_DIFF);  

		HBRUSH hbr = GetSysColorBrush(COLOR_BTNFACE); 
		FillRgn(hdc, rgnClient, hbr); 

		DeleteObject(hbr); 
		DeleteObject(rgnClient); 
		DeleteObject(rgnVideo); 

		HRESULT hr = pVMRControl->RepaintVideo(hwnd, hdc);  
    } 
    else
    { 
	    FillRect(hdc, &winRect, (HBRUSH)(COLOR_BTNFACE + 1));
    } 
    EndPaint(hwnd, &ps); 
}

void VideoDevice::updateZoomBar(WPARAM wParam)
{
	LRESULT pos;

    switch (LOWORD(wParam)) { 
        case TB_ENDTRACK:
          
			pos = SendMessage(hbar, TBM_GETPOS, 0, 0);
            pCameraControl->Set(CameraControl_Zoom, long(pos)/10*10,0);
            break; 

        default:
			
            break; 
        } 
}

bool VideoDevice::createDeviceEnum()
{
	ICreateDevEnum *pDevEnum;
    HRESULT hr;
	
	hr = CoCreateInstance(CLSID_SystemDeviceEnum,
                          NULL,  
                          CLSCTX_INPROC_SERVER, 
                          IID_PPV_ARGS(&pDevEnum)
                          );
	assert(SUCCEEDED(hr));
	if (pEnumMoniker != NULL) 
	{
		pEnumMoniker->Reset();
	}
	else
	{
		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
		                                     &pEnumMoniker,
		                                     0);
		if (hr == S_FALSE)
    	{
    		std::cout<<"No Device Found"<<std::endl;
        	hr = VFW_E_NOT_FOUND; 
    	}
	}
	
    pDevEnum->Release();
	
	if(SUCCEEDED(hr)) return true;
	return false;
}

void VideoDevice::InitWindowlessVMR9(HWND hwnd) 
{
	HRESULT hr;
	long lWidth, lHeight;
		
    if (pGraphBuilder == NULL) return;

    hr = CoCreateInstance(CLSID_VideoMixingRenderer9,
                                  NULL, 
								  CLSCTX_INPROC,
								  IID_IBaseFilter,
								  (void**)&pVMRFilter); 
    if (FAILED(hr)) return;

    hr = pGraphBuilder->AddFilter(pVMRFilter, L"Video Mixing Renderer");
    if (FAILED(hr)) 
    {
        pVMRFilter->Release();
		pVMRFilter = NULL;
		return;
    }

	//Set VMR mode
    IVMRFilterConfig9* pVMRFilterConfig; 
    hr = pVMRFilter->QueryInterface(IID_IVMRFilterConfig9, (void**)&pVMRFilterConfig);
    if (SUCCEEDED(hr)) 
    { 
		hr = pVMRFilterConfig->SetRenderingMode(VMR9Mode_Windowless); 
		pVMRFilterConfig->Release(); 
    }
	
    if (SUCCEEDED(hr))
    {
		hr = pVMRFilter->QueryInterface(IID_IVMRWindowlessControl9, (void**)&pVMRControl);
		if( SUCCEEDED(hr)) 
		{ 
	    	hr = pVMRControl->SetVideoClippingWindow(hwnd); 
	    	if (FAILED(hr))
	    	{
				pVMRControl->Release();
				pVMRControl = NULL;
				return;
	    	}
		} 
    }

	hr = pVMRControl->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL); 
	if (SUCCEEDED(hr))
	{
	    SetRect(&videoRect, 0, 0, lWidth, lHeight); 
	    GetClientRect(hwnd, &winRect);
		SetRect(&targetRect, 0, 0, winRect.right, winRect.bottom - 30); 
	    hr = pVMRControl->SetVideoPosition(&videoRect, &targetRect); 
	}
}

// Get the first upstream or downstream filter
HRESULT VideoDevice::GetNextFilter(
    IBaseFilter *pFilter, // Pointer to the starting filter
    PIN_DIRECTION Dir,    // Direction to search (upstream or downstream)
    IBaseFilter **ppNext, // Receives a pointer to the next filter.
    bool skipFirstPin) 
{
    if (!pFilter || !ppNext) return E_POINTER;

    IEnumPins *pEnum = 0;
    IPin *pPin = 0;
	PIN_INFO PinInfo;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr)) return hr;
	std::cout<<"before loop"<<std::endl;
    while (S_OK == pEnum->Next(1, &pPin, 0))
    {
        std::cout<<"before query directory"<<std::endl;
        PIN_DIRECTION ThisPinDir;
        hr = pPin->QueryDirection(&ThisPinDir);
        if (FAILED(hr))
        {
            // Something strange happened.
            hr = E_UNEXPECTED;
            pPin->Release();
            break;
        }
        if (ThisPinDir == Dir)
        {
    		if (skipFirstPin) 
			{
				pPin->Release();
				continue;
			}
            std::cout<<"before get next pin"<<std::endl;
            IPin *pPinNext = 0;
            hr = pPin->ConnectedTo(&pPinNext);
            if (SUCCEEDED(hr))
            {
                // Get the filter that owns that pin.
	        	hr = pPin->QueryPinInfo(&PinInfo);
				std::wcout<<"This:"<<PinInfo.achName<<std::endl;
                hr = pPinNext->QueryPinInfo(&PinInfo);
				std::wcout<<"Next:"<<PinInfo.achName<<std::endl;
                pPinNext->Release();
                pPin->Release();
                pEnum->Release();
                if (FAILED(hr) || (PinInfo.pFilter == NULL))
                {
                    // Something strange happened.
                    return E_UNEXPECTED;
                }
                // This is the filter we're looking for.
                *ppNext = PinInfo.pFilter; // Client must release.
                return S_OK;
            }
        }
        pPin->Release();
    }
    pEnum->Release();
    // Did not find a matching filter.
    return E_FAIL;
}
	
HRESULT VideoDevice::GetPinCategory(IPin *pPin, GUID *pPinCategory)
{
    HRESULT hr;
    IKsPropertySet *pKs;
    hr = pPin->QueryInterface(IID_IKsPropertySet, (void **)&pKs);
    if (FAILED(hr))
    {
        // The pin does not support IKsPropertySet.
        return hr;
    }
    // Try to retrieve the pin category.
    DWORD cbReturned;
    hr = pKs->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0, 
        pPinCategory, sizeof(GUID), &cbReturned);

    // If this succeeded, pPinCategory now contains the category GUID.

    pKs->Release();
    return hr;
}

void VideoDevice::ListPin(IBaseFilter* pFilter)
{
	IEnumPins *pEnumPins;
	IPin *pPin;
	PIN_INFO pinInfo;
	HRESULT hr;

	if (pFilter == NULL) return;
    hr = pFilter->EnumPins(&pEnumPins);
	assert(SUCCEEDED(hr));
	
	while (pEnumPins->Next(1, &pPin, NULL) == S_OK)
    {
		hr = pPin->QueryPinInfo(&pinInfo);
		if (SUCCEEDED(hr))
		{
			std::wcout<<pinInfo.achName<<std::endl;
		}
        pPin->Release();
    }
	pEnumPins->Release();
	return;
}


IPin *VideoDevice::FindPin(IBaseFilter* pFilter, LPOLESTR pinName)
{
	IEnumPins *pEnumPins;
	IPin *pPin;
	PIN_INFO pinInfo;
	HRESULT hr;

	if (pFilter == NULL) return NULL;
    hr = pFilter->EnumPins(&pEnumPins);
	assert(SUCCEEDED(hr));
	
	while (pEnumPins->Next(1, &pPin, NULL) == S_OK)
    {
		hr = pPin->QueryPinInfo(&pinInfo);
		if (SUCCEEDED(hr) && wcscmp(pinName,pinInfo.achName) == 0)
		{
			pEnumPins->Release();
			return pPin;
		}
        pPin->Release();
    }
	pEnumPins->Release();
	return NULL;
}

void VideoDevice::SetFormat(const GUID &subtype, long height, long width, bool isVideoInfo2)
{
	IPin *captureOut;
	IAMStreamConfig *pStreamConfig;
	HRESULT hr;
    int piCount, piSize;
	AM_MEDIA_TYPE *pMediaType;
	VIDEO_STREAM_CONFIG_CAPS scc;
	
	bool isFind = false;
	LPOLESTR pBuffer = new OLECHAR[1024];
	
	captureOut = FindPin(pCaptureFilter,L"Capture");
	if (captureOut == NULL)
	{
		std::cout<<"unable to find output pin Capture"<<std::endl;
		return;
	}
	hr = captureOut->QueryInterface(IID_IAMStreamConfig, (void**)&pStreamConfig);
	if (FAILED(hr)) 
	{
		std::cout<<"unable to query IAMStreamConfig interface"<<std::endl;
		return;
	}
	
    hr = pStreamConfig->GetNumberOfCapabilities(&piCount, &piSize);
    for (int i = 0; i < piCount; i++)
    {
        pStreamConfig->GetStreamCaps(i, &pMediaType, reinterpret_cast<BYTE *>(&scc));
        if (isVideoInfo2 == false && FORMAT_VideoInfo == pMediaType->formattype)
        {

            VIDEOINFOHEADER *pvideoInfo = (VIDEOINFOHEADER *)pMediaType->pbFormat;
            if (subtype == pMediaType->subtype && height == pvideoInfo->bmiHeader.biHeight && width == pvideoInfo->bmiHeader.biWidth)
            {
            	isFind = true;
                break;
            }
            //std::cout << " height " << std::dec << pvideoInfo->bmiHeader.biHeight << " width " << pvideoInfo->bmiHeader.biWidth;
            //std::cout << ", frame rate " << std::dec << 10000000 / pvideoInfo->AvgTimePerFrame << std::endl;
        }
		else if (isVideoInfo2 == true && FORMAT_VideoInfo2 == pMediaType->formattype)
        {
            VIDEOINFOHEADER2 *pvideoInfo = (VIDEOINFOHEADER2 *)pMediaType->pbFormat;
            if (subtype == pMediaType->subtype && height == pvideoInfo->bmiHeader.biHeight && width == pvideoInfo->bmiHeader.biWidth)
            {
            	isFind = true;
                break;
            }
            //std::cout << " height " << std::dec << pvideoInfo->bmiHeader.biHeight << " width " << pvideoInfo->bmiHeader.biWidth;
            //std::cout << ", frame rate " << std::dec << 10000000 / pvideoInfo->AvgTimePerFrame << std::endl;
        }
    }
	if (isFind) pStreamConfig->SetFormat(pMediaType);
	pStreamConfig->Release();
    return;
}


