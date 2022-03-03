#ifndef _VIDEO_CAP_WIN_H_
#define _VIDEO_CAP_WIN_H_

#include<windows.h>
#include<vector>

#define WINDOW_WIDTH_MARGIN 16
#define WINDOW_HEIGHT_MARGIN 39

#define DEFAULT_IMAGE_WIDTH 570
#define TOOLBAR_HEIGHT 50
#define CTRL_GRAP 5
#define BUTTON_HEIGHT 40
#define BUTTON_WIDTH 100
#define STATIC_HEIGHT 20
#define COMBO_HEIGHT 200
#define DEVICE_COMBO_WIDTH 160
#define FORMAT_COMBO_WIDTH 80
#define RESOLUTION_COMBO_WIDTH 100

#define COMBO_TEXT_BUFFER_LEN 128

/**V I D E O   C A P   W I N***********************************************
 * Create: BY Huang Cheng(chhuang@kns.com) ON 2022211
 * Description: A window for video capture application
 * Members: 
   - 
----------------------------C H A N G E   L O G----------------------------
 * 
**************************************************************************/
class VideoCapWin
{
public:
    VideoCapWin(){}
    ~VideoCapWin(){}

    void create(HINSTANCE hApp);
    void destory();
    HWND getHandle(){return hWin;}
    void show();
    void setVideoSize(int width, int height);
    void setCaptureButton(int status);
    const RECT& getTargetVideoRect();
    void redraw();

    int getDeviceComboIndex();
    int getFormatComboIndex();
    int getResolutionComboIndex();
    const wchar_t* getDeviceComboText();
    const wchar_t* getFormatComboText();
    const wchar_t* getResolutionComboText();
    void setupDeviceCombo(const std::vector<std::wstring>& list, int index = 0);
    void setupFormatCombo(const std::vector<std::wstring>& list, int index = 0);
    void setupResolutionCombo(const std::vector<std::wstring>& list, int index = 0);
    void setupCombo(HWND hwnd, const std::vector<std::wstring>& list, int index = 0);

private:
    HINSTANCE hApp;
    HWND hWin;
    HWND hDeviceStatic;
    HWND hFormatStatic;
    HWND hResolutionStatic;
    HWND hDeviceCombo;
    HWND hFormatCombo;
    HWND hResolutionCombo;
    HWND hSnapButton;
    HWND hCaptureButton;
    HBITMAP hBmpImage;
    HBITMAP hBmpVideo;
    HBITMAP hBmpVideoOff;
    RECT clientRect;
    RECT videoRect;
    wchar_t deviceTextBuffer[COMBO_TEXT_BUFFER_LEN];
    wchar_t formatTextBuffer[COMBO_TEXT_BUFFER_LEN];
    wchar_t resolutionTextBuffer[COMBO_TEXT_BUFFER_LEN];
};

#endif
