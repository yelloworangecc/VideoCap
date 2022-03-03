#ifndef _VIDEO_CAP_CONTROLLER_H_
#define _VIDEO_CAP_CONTROLLER_H_

#include "BaseSettings.h"
#include "VideoCapWin.h"
#include "VideoCapture.h"

#define SETTING_FILE_PATH     ".\\VideoCap.json"
#define KEY_DEVICE_NAME       "DeviceName"
#define KEY_FORMAT            "Format"
#define KEY_RESOLUTION        "Resolution"
#define KEY_SAVE_PATH         "SavePath"
#define KEY_LANGUAGE_CODE     "LanguageCode"

/**V I D E O   C A P   C O N T R O L L E R . H*****************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022211
 * Description: video capture app controller class
 * Members: 
   - 
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
class VideoCapController
{
public:
    static VideoCapController& getInstance();

    void initialize(HINSTANCE hApp);
    void uninitialize();
    void showWindow();

    
    //void fetchVideoDefault();
    void fetchUserSettings(std::string& name, std::string& format, std::string& resolution);

    void enumVideoDevices();
    void printVideoDevices();
    RECT getRectFromResolution(const std::wstring& wResolution);
    void setupVideoDevice();
    void loadLogo(const wchar_t* pResourceName);
    void freeLogo();

    void updateDeviceCombo();
    void updateFormatCombo(const wchar_t* pDeviceName);
    void updateResolutionCombo(const wchar_t* pDeviceName, const wchar_t* pFormat);

    void onPaint();
    void onSize();
    void onSnap();
    void onCapture();
    void onDeviceChanged();
    void onFormatChanged();
    void onResolutionChanged();
    void onGraphEvent();

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
};

#endif
