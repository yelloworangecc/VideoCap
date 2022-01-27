#ifndef _RENDER_WIN_
#define _RENDER_WIN_

#define DEFAULT_IMAGE_WIDTH 640
#define DEFAULT_IMAGE_HEIGHT 480
#define DEFAULT_TOOLBAR_HEIGHT 30
#define WIDTH_MARGIN 16
#define HEIGHT_MARGIN 39
#define CTRL_GRAP 5
#define BUTTON_HEIGHT 20
#define BUTTON_WIDTH 80
#define COMBO_HEIGHT 200
#define DEVICE_COMBO_WIDTH 140
#define FORMAT_COMBO_WIDTH 200
#define RESOLUTION_COMBO_WIDTH 80

class RenderWin
{
public:
    virtual HINSTANCE getApp() = 0;
    virtual HWND getHandle() = 0;
    virtual void updateUserSettings(
        const std::vector<std::wstring>& listFormat,
        const std::vector<std::wstring>& listResolution) = 0;
    virtual std::vector<std::wstring> getUserSettings() = 0;
    virtual void updateWindowSize(int width, int height) = 0;
    virtual void updateCtrlPos(int imageHeight) = 0;
};

#endif
