#ifndef _VIDEO_CAP_WIN_H_
#define _VIDEO_CAP_WIN_H_

#include<windows.h>
#include<windowsx.h>
#include<vector>

#define WINDOW_WIDTH_MARGIN 16
#define WINDOW_HEIGHT_MARGIN 39

#define DEFAULT_IMAGE_WIDTH 800
#define TOOLBAR_HEIGHT 60
#define CTRL_GAP 5
#define BUTTON_HEIGHT 40
#define BUTTON_WIDTH 100
#define BUTTON_OFFSET_Y 8
#define STATIC_HEIGHT 20
#define COMBO_HEIGHT 200
#define DEVICE_COMBO_WIDTH 160
#define FORMAT_COMBO_WIDTH 80
#define RESOLUTION_COMBO_WIDTH 100
#define TRACKBAR_WIDTH 100
#define TRACKBAR_HEIGHT 30

#define ROTATION_DEGREE_STEP 10
#define ROTATION_DEGREE_MAX 180
#define GRID_INTERVAL_STEP 10
#define GRID_INTERVAL_MAX 500

#define CONFIG_WINDOW_WIDTH 480
#define CONFIG_WINDOW_HEIGHT 320
#define CONFIG_LEFT_WIDTH 100
#define CONFIG_MID_WIDTH 220
#define CONFIG_RIGHT_WIDTH 120
#define CONFIG_LINE_HEIGHT 30

#define COMBO_TEXT_BUFFER_LEN 128
#define LANGUAGE_STRING_MAX_SIZE 1024
#define LANGUAGE_EN_INDEX 0
#define LANGUAGE_ZH_INDEX 1

class VideoCapWin
{
public:
    VideoCapWin():bCreated(false),bToolbarHide(false),bConfigHide(false){}
    ~VideoCapWin(){}

    operator HWND() const { return hWin; }

    void create(HINSTANCE hApp);
    void createConfig();
    void setLanguage(const std::string & code);
    void setSavePath(const wchar_t* path);
    void setSavePath(const std::wstring& path);
    void destory();
    HWND getHandle(){return hWin;}
    void setTitle(const std::wstring& title){SetWindowText(hWin,title.c_str());}
    void setTitle(const wchar_t* title){SetWindowText(hWin,title);}
    void setVideoSize(int width, int height);
    void setCaptureButton(int status);
    void resetTrackbarPos();
    void setRotationTrack(int pos = 0);
    void setGridTrack(int pos = 0);
    const RECT& getVideoRect();
    
    int getVideoWidth(){return videoRect.right;}
    int getVideoHeight(){return videoRect.bottom;}
    
    void redraw();

    void showMain();
    bool isCtrlsHide(){return bToolbarHide;}
    void hideToolbar();
    void showToolbar();
    bool isConfigHide(){return bConfigHide;}
    void hideConfig();
    void showConfig();

    int getLanguageComboIndex(){return ComboBox_GetCurSel(hLanguageCombo);}
    int getDeviceComboIndex(){return ComboBox_GetCurSel(hDeviceCombo);}
    int getFormatComboIndex(){return ComboBox_GetCurSel(hFormatCombo);}
    int getResolutionComboIndex(){return ComboBox_GetCurSel(hResolutionCombo);}
    
    const wchar_t* getDeviceComboText();
    const wchar_t* getFormatComboText();
    const wchar_t* getResolutionComboText();
    
    void setupDeviceCombo(const std::vector<std::wstring>& list, int index = 0);
    void setupFormatCombo(const std::vector<std::wstring>& list, int index = 0);
    void setupResolutionCombo(const std::vector<std::wstring>& list, int index = 0);
    void setupCombo(HWND hwnd, const std::vector<std::wstring>& list, int index = 0);

    const wchar_t* loadString(int ids);

    bool isWinMain(HWND handle){return handle == hWin;}
    bool isWinConfig(HWND handle){return handle == hConfig;}

    static bool useWindowsTheme;

private:
    HINSTANCE hApp;
    HWND hWin;
    HWND hDeviceStatic;
    HWND hDeviceCombo;
    HWND hRotationStatic;
    HWND hRotationTrack;
    HWND hGridIntervalStatic;
    HWND hGridIntervalTrack;
    HWND hSnapButton;
    HWND hCaptureButton;
    HWND hConfigButton;
    
    HWND hConfig;
    HWND hFormatStatic;
    HWND hResolutionStatic;
    HWND hFormatCombo;
    HWND hResolutionCombo;
    HWND hLanguageStatic;
    HWND hLanguageCombo;
    HWND hSavePathStatic;
    HWND hSavePathEdit;
    HWND hChoosePathButton;
    
    HBITMAP hBmpImage;
    HBITMAP hBmpVideo;
    HBITMAP hBmpVideoOff;
    HBITMAP hBmpConfig;
    RECT clientRect;
    RECT videoRect;
    HFONT hFont;
    wchar_t deviceTextBuffer[COMBO_TEXT_BUFFER_LEN];
    wchar_t formatTextBuffer[COMBO_TEXT_BUFFER_LEN];
    wchar_t resolutionTextBuffer[COMBO_TEXT_BUFFER_LEN];
    wchar_t rotationTextBuffer[COMBO_TEXT_BUFFER_LEN];

    bool bCreated;
    bool bToolbarHide;
    bool bConfigHide;

    std::string languageCode;
    std::wstring savePath;
    wchar_t languageStrBuff[LANGUAGE_STRING_MAX_SIZE];
};

#endif
