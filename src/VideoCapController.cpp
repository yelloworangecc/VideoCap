#ifndef UNICODE
#define UNICODE
#endif
#include <iostream>
#include "VideoCapController.h"

VideoCapController VideoCapController::instance;

VideoCapController& VideoCapController::getInstance()
{
    return instance;
}

int findWstringVector(const std::vector<std::wstring>&list,const std::wstring& item)
{
    int i;
    int size = list.size();
    for (i=0;i<size;i++)
    {
        if (list[i] == item) return i;
    }
    return -1;
}

void VideoCapController::showWindow()
{
    updateDeviceCombo();
    window.show();
}

/**V I D E O   C A P   C O N T R O L L E R . F E T C H   V I D E O   D E F A U L T
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022215
 * Description: get default video parameters and update settings
 * Parameters: 
   - 
 * Return: 
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
//void VideoCapController::fetchDeviceParameters()
//{
//    capture.fetchParameters(deviceList);
//}

/**V I D E O   C A P   C O N T R O L L E R . E N U M   V I D E O   D E V I C E S
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022214
 * Description: query all video devices from system
 * Parameters: 
   - 
 * Return: 
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
void VideoCapController::initialize(HINSTANCE hApp)
{
    this->hApp = hApp;
    window.create(hApp);
    if (!settings.exist(KEY_SAVE_PATH)) settings.set(KEY_SAVE_PATH,"");
    capture.initialize(settings.get<std::string>(KEY_SAVE_PATH));
    hMemDc = loadLogo(L"ID_BMP_APP");
}

void VideoCapController::uninitialize()
{
    capture.uninitialize();
    DeleteDC(hMemDc);
}


void VideoCapController::printVideoDevices()
{
    capture.printDevices();
}

RECT VideoCapController::getRectFromResolution(const std::wstring& wResolution)
{
    RECT rect;
    int width, height;
    double ratio;
    wchar_t delimiter;
    std::wstringstream ss;

    ss << wResolution;
    ss >> width >> delimiter >> height;
    if (width >= DEFAULT_IMAGE_WIDTH)
    {
        SetRect(&rect, 0, 0, width, height);
    }
    else
    {
        ratio = double(DEFAULT_IMAGE_WIDTH) / width;
        height = height * ratio;
        SetRect(&rect, 0, 0, DEFAULT_IMAGE_WIDTH, height);
    }
    return rect;
}
/*
void VideoCapController::setupVideoAreaSize(const std::wstring& wResolution)
{
    int width, height;
    double ratio;
    wchar_t delimiter;
    std::wstringstream ss;

    ss << wResolution;
    ss >> width >> delimiter >> height;
    if (width >= DEFAULT_IMAGE_WIDTH)
    {
        window.setVideoSize(width, height);
    }
    else
    {
        ratio = double(DEFAULT_IMAGE_WIDTH) / width;
        height = height * ratio;
        window.setVideoSize(DEFAULT_IMAGE_WIDTH, height);
    }
}*/

void VideoCapController::setupVideoDevice()
{
    //check setting, if no value, use default
    std::wstring wName = Ansi2Unicode(settings.get<std::string>(KEY_DEVICE_NAME));
    std::wstring wFormat = Ansi2Unicode(settings.get<std::string>(KEY_FORMAT));
    std::wstring wResolution = Ansi2Unicode(settings.get<std::string>(KEY_RESOLUTION));

    capture.stop();
    RECT newRect = getRectFromResolution(wResolution);
    window.setVideoSize(newRect.right, newRect.bottom);

    capture.clean();
    capture.setup(wName.c_str(),wFormat.c_str(),wResolution.c_str(),window.getHandle(), newRect, hMemDc);
    capture.start();
}

HDC VideoCapController::loadLogo(const wchar_t* pResourceName)
{
    std::cout<<"IN loadLogo"<<std::endl;
    HBITMAP hBmp;
    HDC hDC,hMemDc;
    HGDIOBJ hGO;

    hBmp = LoadBitmap(hApp,pResourceName);
    hDC = GetDC(window.getHandle());
    hMemDc = CreateCompatibleDC(hDC);
    hGO = SelectObject(hMemDc,hBmp);
	
    return hMemDc;
}

void VideoCapController::updateDeviceCombo()
{
    int deviceIndex = -1;
    
    const std::vector<std::wstring>& deviceNameList = capture.getDeviceNameList();
    if (settings.exist(KEY_DEVICE_NAME))
    {
        std::wstring wDeviceName = Ansi2Unicode(settings.get<std::string>(KEY_DEVICE_NAME));
        deviceIndex = findWstringVector(deviceNameList,wDeviceName);
    }
    if (deviceIndex >= 0) 
    {
        window.setupDeviceCombo(deviceNameList,deviceIndex);
    }
    else //settrings not found, use fist value
    {
        deviceIndex = 0;
        window.setupDeviceCombo(deviceNameList);
        settings.set(KEY_DEVICE_NAME,Unicode2Ansi(deviceNameList[deviceIndex]));
    }
    updateFormatCombo(deviceNameList[deviceIndex].c_str());
}

void VideoCapController::updateFormatCombo(const wchar_t* pDeviceName)
{
    int formatIndex = -1;
    const std::vector<std::wstring>& formatNameList = capture.getFormatNameList(pDeviceName);
    if (settings.exist(KEY_FORMAT))
    {
        std::wstring wFormat = Ansi2Unicode(settings.get<std::string>(KEY_FORMAT));
        formatIndex = findWstringVector(formatNameList,wFormat);
    }
    if (formatIndex >= 0) 
    {
        window.setupFormatCombo(formatNameList,formatIndex);
    }
    else //settrings not found, use fist value
    {
        formatIndex = 0;
        window.setupFormatCombo(formatNameList);
        settings.set(KEY_FORMAT,Unicode2Ansi(formatNameList[formatIndex]));
    }
    updateResolutionCombo(pDeviceName, formatNameList[formatIndex].c_str());
}

void VideoCapController::updateResolutionCombo(const wchar_t* pDeviceName, const wchar_t* pFormat)
{
    int resolutionindex = -1;
    const std::vector<std::wstring>& resolutonList = capture.getResolutionList(pDeviceName,pFormat);
    
    std::wcout<<pDeviceName<< L'.'<<pFormat<<L'.'<< resolutonList.size() <<std::endl;
    if (settings.exist(KEY_RESOLUTION))
    {
        std::wstring wResolution = Ansi2Unicode(settings.get<std::string>(KEY_RESOLUTION));
        resolutionindex = findWstringVector(resolutonList,wResolution);
    }
    if (resolutionindex >= 0) 
    {
        window.setupResolutionCombo(resolutonList,resolutionindex);
    }
    else //settrings not found, use fist value
    {
        resolutionindex = 0;
        window.setupResolutionCombo(resolutonList);
        settings.set(KEY_RESOLUTION,Unicode2Ansi(resolutonList[resolutionindex]));
    }
}

void VideoCapController::onPaint()
{
    PAINTSTRUCT ps; 
    HDC         hdc; 
    RECT        client; 
    HWND hWin = window.getHandle();
    GetClientRect(hWin, &client);
    hdc = BeginPaint(hWin, &ps);
    FillRect(hdc, &client, (HBRUSH)(COLOR_BTNFACE)); 
    RECT rect = window.getTargetVideoRect();
    capture.setRenderPosition(rect);
    capture.paint(hWin, hdc);
    EndPaint(hWin, &ps); 
}

void VideoCapController::onSize()
{
    RECT rect = window.getTargetVideoRect();
    capture.setRenderPosition(rect);
    RedrawWindow(window.getHandle(), 0, 0, RDW_ERASE | RDW_INVALIDATE);
}

void VideoCapController::onSnap()
{
    capture.snap();
}

void VideoCapController::onCapture()
{
    static bool startCap = false;
    if (!startCap)
    {
        window.setCaptureButtonText(L"STOP  Cap");
        capture.startCapture();
        startCap = true;
    }
    else 
    {
        window.setCaptureButtonText(L"START Cap");
        capture.stopCapture();
        startCap = false;
    }
    
}

void VideoCapController::onDeviceChanged()
{
    int resolutionindex = -1;
    const wchar_t* pDeviceName = window.getDeviceComboText();
    settings.set(KEY_DEVICE_NAME, Unicode2Ansi(pDeviceName));
    updateFormatCombo(pDeviceName);
    
    setupVideoDevice();
}

void VideoCapController::onFormatChanged()
{
    const wchar_t* pDeviceName = window.getDeviceComboText();
    const wchar_t* pFormat = window.getFormatComboText();
    settings.set(KEY_FORMAT, Unicode2Ansi(pFormat));
    
    std::wcout<<pDeviceName<< L'.'<<pFormat<<L'.'<<std::endl;
    updateResolutionCombo(pDeviceName,pFormat);
    
    setupVideoDevice();
}

void VideoCapController::onResolutionChanged()
{
    const wchar_t* pResolution = window.getResolutionComboText();
    settings.set(KEY_RESOLUTION, Unicode2Ansi(pResolution));
    
    setupVideoDevice();
}

void VideoCapController::onGraphEvent()
{
    LONG evCode;
    LONG_PTR evParam1, evParam2;
    HRESULT hr = S_OK;

    IMediaEventEx* pIMediaEventEx = capture.getMediaEvent();
    if (!pIMediaEventEx)
    {
        std::cout<<"IMediaEventEx is not valid"<<std::endl;
        return;
    }

    while (SUCCEEDED(pIMediaEventEx->GetEvent(&evCode, &evParam1, &evParam2, 0)))
    {
        // Insert event processing code here, if desired
        switch (evCode)
        {
            case EC_PALETTE_CHANGED:
                std::cout << "EC_PALETTE_CHANGED" << std::endl;
                break;
            case EC_VIDEO_SIZE_CHANGED:
                std::cout << "EC_VIDEO_SIZE_CHANGED" << std::endl;
                break;
            case EC_CLOCK_CHANGED:
                std::cout << "EC_CLOCK_CHANGED" << std::endl;
                break;
            case EC_PAUSED:
                std::cout << "EC_PAUSED" << std::endl;
                break;
            case EC_STREAM_CONTROL_STARTED:
                std::cout << "EC_STREAM_CONTROL_STARTED" << std::endl;
                break;
            case EC_STREAM_CONTROL_STOPPED:
                std::cout << "EC_STREAM_CONTROL_STOPPED" << std::endl;
                break;
            case EC_QUALITY_CHANGE:
                std::cout << "EC_QUALITY_CHANGE" << std::endl;
                break;
            case EC_VMR_RENDERDEVICE_SET:
                std::cout << "EC_VMR_RENDERDEVICE_SET" << std::endl;
                break;
            default:
                std::cout << "Unknow media evet " << evCode << std::endl;
                break;
        }
    }
    hr = pIMediaEventEx->FreeEventParams(evCode, evParam1, evParam2);
    return;
}

