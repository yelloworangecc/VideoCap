#ifndef _VIDEO_CAP_CONTROLLER_H_
#define _VIDEO_CAP_CONTROLLER_H_

#include "BaseSettings.h"
#include "VideoCapWin.h"
#include "VideoCapture.h"

#define SETTING_FILE_PATH     ".\\VideoCap.json"
#define SAVE_FILE_PATH        "D:\\VideoCap\\"
#define KEY_DEVICE_NAME       "DeviceName"
#define KEY_FORMAT            "Format"
#define KEY_RESOLUTION        "Resolution"
#define KEY_SAVE_PATH         "SavePath"
#define KEY_LANGUAGE_CODE     "LanguageCode"

class VideoCapController
{
public:
    static VideoCapController& getInstance();

    void initialize(HINSTANCE hApp);
    void uninitialize();
    void message2Title(int ids, const std::wstring& str);
    void message2Title(int ids, const wchar_t* str = 0);
    void message2Title(const std::wstring& str, int ids = 0);
    void message2Title(const wchar_t* str, int ids = 0);
    
    //void fetchVideoDefault();
    void fetchUserSettings(std::string& name, std::string& format, std::string& resolution);

    void enumVideoDevices();
    void printVideoDevices();
    RECT getRectFromResolution(const std::wstring& wResolution);
    void setupVideoDevice();
    const RECT& getVideoRect(){return window.getVideoRect();}
    int getStreamWidth(){return capture.getStreamWidth();}
    int getStreamHeight(){return capture.getStreamHeight();}
    double getWidthScaleFactor(){return double(window.getVideoWidth())/capture.getStreamWidth();}
    double getHeightScaleFactor(){return double(window.getVideoHeight())/capture.getStreamHeight();}

    void showMainWindow(){window.showMain();}
    HWND getMainWindowHandle(){return window.getHandle();}
    bool isToolbarHide(){return window.isCtrlsHide();}
    void hideToolbar(){window.hideToolbar();}
    void showToolbar(){window.showToolbar();}
    void loadLogo(const wchar_t* pResourceName);
    void freeLogo();

    void reloadDeviceList();
    void updateDeviceCombo();
    void updateFormatCombo(const wchar_t* pDeviceName);
    void updateResolutionCombo(const wchar_t* pDeviceName, const wchar_t* pFormat);

    void onConfigClose(){window.hideConfig();}
    void onConfigButton(){window.showConfig();}
    void onMaximize(){hideToolbar();}
    void onRestore(){showToolbar();}

    void flipVideo(int direction){capture.flip(direction);}
    void onPaint();
    void onSize();
    void onHelp();
    void openExplorer(const wchar_t* path);
    void onOpen();
    void onSnapButton();
    void onRecordButton();
    void onPause();
    void onChoosePath();
    void onLanguageChanged();
    void onDeviceChanged();
    void onFormatChanged();
    void onResolutionChanged();
    void onRotationChanged(int degree);
    void onGridIntervalChanged(int interval);
    void onGraphEvent();

    bool isWindowMain(HWND handle){return window.isWinMain(handle);}
    bool isWindowConfig(HWND handle){return window.isWinConfig(handle);}

    static VideoCapController instance;
    
private:
    VideoCapController() :settings(SETTING_FILE_PATH) {}
    ~VideoCapController() {}
    VideoCapController(const VideoCapController& controller) :settings(SETTING_FILE_PATH) {}
    VideoCapController& operator=(const VideoCapController& controller) {}

    BaseSettings settings;
    HINSTANCE hApp;
    VideoCapWin window;
    VideoCapture capture;
    HBITMAP hBmpLogo;
    HDC hDcLogo;
    HDEVNOTIFY hDevNotify;
};

#endif
