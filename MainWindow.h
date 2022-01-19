#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

class MainWindow
{
public:
    MainWindow(HINSTANCE hInstance, int videoWidth = 640, int videoHeight = 480);
    ~MainWindow(){}

    HWND getHandle(){return hMain;}
    void addIcon();
    void addDeviceCombo();
    void addFormatCombo();
    void addSnapButton();
    void addCapButton();
    void addTrackBar();
private:
    MainWindow(const MainWindow& instance){}
    const MainWindow &operator=(const MainWindow& instance){}

    HINSTANCE hApp;
    int videoWidth;
    int videoHeight;
    HWND hMain;
    HICON hIcon;
    HWND hDeviceCombo;
    HWND hFormatCombo;
    HWND hSnapButton;
    HWND hCaptureButton;
    //HWND hTrackBar;
    int addPos;
};

#endif