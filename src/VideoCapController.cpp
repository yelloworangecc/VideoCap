#ifndef UNICODE
#define UNICODE
#endif

#include <iostream>
#include <windows.h>
#include <shlobj_core.h>
#include <Shlwapi.h>
#include "VideoCapController.h"
#include "resource.h"
#include "Console.h"
#include "Dbt.h"

VideoCapController VideoCapController::instance;

VideoCapController& VideoCapController::getInstance()
{
    return instance;
}

int findWstringVector(const std::vector<std::wstring>&list,const std::wstring& item)
{
    int i;
    int size = (int)list.size();
    for (i=0;i<size;i++)
    {
        if (list[i] == item) return i;
    }
    return -1;
}

void VideoCapController::message2Title(int ids, const std::wstring& str)
{
    if(ids)
    {
        std::wstring msg = window.loadString(ids);
        msg = msg + L' ' + msg;
        window.setTitle(msg);
    }
    else window.setTitle(str);
}
void VideoCapController::message2Title(int ids, const wchar_t* str)
{
    if(ids)
    {
        std::wstring msg = window.loadString(ids);
        if (str) msg = msg + L' ' + msg;
        window.setTitle(msg);
    }
    else 
    {
        if(str) window.setTitle(str);
    }
}

void VideoCapController::message2Title(const std::wstring& str, int ids)
{
    if(ids)
    {
        std::wstring msg = str;
        msg = msg + L' ' + window.loadString(ids);
        window.setTitle(msg);
    }
    else window.setTitle(str);
}

void VideoCapController::message2Title(const wchar_t* str, int ids)
{
    if(ids)
    {
        std::wstring msg = str;
        msg = msg + L' ' + window.loadString(ids);
        window.setTitle(msg);
    }
    else window.setTitle(str);
}



void VideoCapController::initialize(HINSTANCE hApp)
{
    this->hApp = hApp;

    //Get Parameter from config file & set
    if (!settings.exist(KEY_LANGUAGE_CODE)) settings.set(KEY_LANGUAGE_CODE, "en");
    window.setLanguage(settings.get<std::string>(KEY_LANGUAGE_CODE));

    if (!settings.exist(KEY_SAVE_PATH) || settings.get<std::string>(KEY_SAVE_PATH).empty()) 
    {
        std::wstring defaultPath = Ansi2Unicode(SAVE_FILE_PATH);
        if (!PathFileExists(defaultPath.c_str())) CreateDirectory(defaultPath.c_str(), NULL);
        settings.set(KEY_SAVE_PATH, SAVE_FILE_PATH);
    }
    std::wstring savePath = Ansi2Unicode(settings.get<std::string>(KEY_SAVE_PATH));
    window.setSavePath(savePath);
    capture.setSavePath(savePath);

    //check if duplicate run
    HANDLE h_mutex = CreateMutex(NULL, FALSE, L"VideoCap");
	LONG lLastError = GetLastError();
	if (h_mutex == NULL || ERROR_ALREADY_EXISTS == lLastError)
	{
	    std::wstring msg = window.loadString(IDS_ERROR_MSGBOX_L);
        MessageBox(NULL,msg.c_str(),window.loadString(IDS_ERROR_MSGBOX_T),0);
        exit(0);
	}
    
    window.create(hApp);
    capture.initialize();

    updateDeviceCombo();
    loadLogo(L"ID_BMP_APP");
    message2Title(IDS_TITLE_HELP);

    static const GUID GUID_DEVINTERFACE_VIDEOCAMERA = 
		{ 0xE5323777, 0xF976, 0x4F5B, { 0x9B, 0x55, 0xB9, 0x46, 0x99, 0xC4, 0x6E, 0x44 } };

	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbcc_size = sizeof(NotificationFilter);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	NotificationFilter.dbcc_reserved = 0;
	NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_VIDEOCAMERA;
	hDevNotify = RegisterDeviceNotification((HWND)window, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
}

void VideoCapController::uninitialize()
{
    UnregisterDeviceNotification(hDevNotify);
    window.destory();
    capture.uninitialize();
    freeLogo();
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
        height = (int)(height * ratio);
        SetRect(&rect, 0, 0, DEFAULT_IMAGE_WIDTH, height);
    }
    return rect;
}

void VideoCapController::setupVideoDevice()
{
    //check setting, if no value, use default
     std::wstring wName,wFormat,wResolution;
    if (settings.exist(KEY_DEVICE_NAME)) wName = Ansi2Unicode(settings.get<std::string>(KEY_DEVICE_NAME));
    if (wName.empty()) return;
    std::wcout<<L"Device:"<<wName<<std::endl;
    if (settings.exist(KEY_FORMAT)) wFormat = Ansi2Unicode(settings.get<std::string>(KEY_FORMAT));
    else wFormat = L"YUY2";
    std::wcout<<L"Format:"<<wFormat<<std::endl;
    if (settings.exist(KEY_RESOLUTION)) wResolution = Ansi2Unicode(settings.get<std::string>(KEY_RESOLUTION));
    else wResolution = L"800 x 600";
    std::wcout<<L"Resolution:"<<wResolution<<std::endl;

    capture.stop();
    RECT newRect = getRectFromResolution(wResolution);
    window.setVideoSize(newRect.right, newRect.bottom);

    capture.clean();
    capture.setup(wName.c_str(),wFormat.c_str(),wResolution.c_str(),window.getHandle(), newRect, hDcLogo);
    capture.start();
}

void VideoCapController::loadLogo(const wchar_t* pResourceName)
{
    HDC hDC;
    HGDIOBJ hGO;

    hBmpLogo = LoadBitmap(hApp,pResourceName);
    hDC = GetDC(window.getHandle());
    hDcLogo = CreateCompatibleDC(hDC);
    hGO = SelectObject(hDcLogo, hBmpLogo);
}

void VideoCapController::freeLogo()
{
    DeleteObject(hBmpLogo);
    DeleteDC(hDcLogo);
}

void VideoCapController::reloadDeviceList()
{
	// Not elegant, but works
	capture.uninitialize();
	capture.initialize();

    //const wchar_t* pOldDeviceName = window.getDeviceComboText();
	updateDeviceCombo();
    setupVideoDevice();
}

/*
 * capture should be initilized first
 */
void VideoCapController::updateDeviceCombo()
{
    int deviceIndex = -1;
    
    const std::vector<std::wstring>& deviceNameList = capture.getDeviceNameList();
    if (deviceNameList.empty()) return;
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
    if (formatNameList.empty()) return;
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
    if (resolutonList.empty()) return;
    //std::wcout<<pDeviceName<< L'.'<<pFormat<<L'.'<< resolutonList.size() <<std::endl;
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
    //FillRect(hdc, &client, GetSysColorBrush(COLOR_BTNFACE)); // not needed since we have wc.hbrBackground
    RECT rect = window.getVideoRect();
    capture.setVmrRenderPosition(rect);
    capture.paint(hWin, hdc);
    EndPaint(hWin, &ps); 
}

void VideoCapController::onSize()
{
    if (!IsIconic(window.getHandle()))
    {
        RECT rect = window.getVideoRect();
        capture.setVmrRenderPosition(rect);
        window.redraw();
    }
}

void VideoCapController::onHelp()
{
    std::wstring savePath = capture.getSavePathW();
    if (savePath == L"./")
    {
        wchar_t currentPath[256] = { 0 };
        GetCurrentDirectory(256, currentPath);
        savePath = currentPath;
    }
    
    std::wstring msg(window.loadString(IDS_HELP_MSGBOX_L1));
    msg += savePath + window.loadString(IDS_HELP_MSGBOX_L2);
    MessageBox(window.getHandle(),msg.c_str(),window.loadString(IDS_HELP_MSGBOX_T),0);
}
//C:\Windows\explorer.exe
void VideoCapController::openExplorer(const wchar_t* path)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
    std::wstring cmd = L"C:\\Windows\\explorer.exe ";
    cmd += path;
    CreateProcess(
        NULL,
        const_cast<wchar_t*>(cmd.c_str()),
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi );
        
    // Wait until child process exits.    
    //WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}

void VideoCapController::onOpen()
{
    std::wstring path = capture.getSavePathW();
    //remove last slash
    path[path.size() - 1] = L'\0';
    CONSOLE<<path<<ENDL;
    openExplorer(path.c_str());
}

void VideoCapController::onSnapButton()
{
    capture.snap();
    std::wstring msg = capture.getSaveNameW();
    msg += window.loadString(IDS_TITLE_SAVED) + capture.getSavePathW();
    message2Title(msg);
}

void VideoCapController::onRecordButton()
{
    static bool startCap = false;
    if (!startCap)
    {
        window.setCaptureButton(0);
        capture.startCapture();
        startCap = true;

        message2Title(window.loadString(IDS_TITLE_RECORDING));
    }
    else 
    {
        window.setCaptureButton(1);
        capture.stopCapture();
        startCap = false;
        
        std::wstring msg = capture.getSaveNameW();
        msg += window.loadString(IDS_TITLE_SAVED) + capture.getSavePathW();
        message2Title(msg);
    }
}

void VideoCapController::onPause()
{
    if (capture.isStreamStarted()) capture.pause();
    else capture.start();
}

void VideoCapController::onChoosePath()
{
    wchar_t buff[256];
    BROWSEINFO browseInfo = {0};
    browseInfo.hwndOwner = GetForegroundWindow();
    browseInfo.lpszTitle = window.loadString(IDS_CHOOSE_PATH);
    browseInfo.ulFlags = BIF_NEWDIALOGSTYLE;
    PCIDLIST_ABSOLUTE list = SHBrowseForFolder(&browseInfo);
    SHGetPathFromIDList(list,buff);

    std::wstring savePathW = buff;
    if (savePathW.empty()) return;
    savePathW += L'\\';
    
    settings.set(KEY_SAVE_PATH,Unicode2Ansi(savePathW));
    capture.setSavePath(savePathW);
    window.setSavePath(savePathW);
}

void VideoCapController::onLanguageChanged()
{
    //update settings
    if (window.getLanguageComboIndex() == LANGUAGE_EN_INDEX) settings.set(KEY_LANGUAGE_CODE, "en");
    else settings.set(KEY_LANGUAGE_CODE, "zh");
    window.setLanguage(settings.get<std::string>(KEY_LANGUAGE_CODE));
}


void VideoCapController::onDeviceChanged()
{
    int resolutionindex = -1;
    const wchar_t* pDeviceName = window.getDeviceComboText();
    settings.set(KEY_DEVICE_NAME, Unicode2Ansi(pDeviceName));

    updateFormatCombo(pDeviceName);
    setupVideoDevice();
    
    window.resetTrackbarPos();
}

void VideoCapController::onFormatChanged()
{
    const wchar_t* pDeviceName = window.getDeviceComboText();
    const wchar_t* pFormat = window.getFormatComboText();
    settings.set(KEY_FORMAT, Unicode2Ansi(pFormat));

    updateResolutionCombo(pDeviceName,pFormat);
    setupVideoDevice();
    
    window.resetTrackbarPos();
}

void VideoCapController::onResolutionChanged()
{
    const wchar_t* pResolution = window.getResolutionComboText();
    settings.set(KEY_RESOLUTION, Unicode2Ansi(pResolution));

    setupVideoDevice();

    window.resetTrackbarPos();
}

void VideoCapController::onRotationChanged(int pos)
{
    int degree = pos * ROTATION_DEGREE_STEP - ROTATION_DEGREE_MAX;
    std::cout << "Rotation degree: " << degree << std::endl;
    capture.setRotationDegree(degree);
}

void VideoCapController::onGridIntervalChanged(int pos)
{
    int interval = pos * GRID_INTERVAL_STEP;
    std::cout << "Grid interval: " << interval << std::endl;
    capture.setGridInterval(interval);
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
