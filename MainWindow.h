#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <vector>

class MainWindow
{
public:
    MainWindow(HINSTANCE hInstance, int videoWidth = 640, int videoHeight = 480);
    ~MainWindow(){}

    HWND getHandle(){return hMain;}
    void addIcon();
    void addDeviceCombo(const std::vector<std::wstring>& list, int defaultIndex = 0);
    void addFormatCombo(const std::vector<std::wstring>& list, int defaultIndex = 0);
    void addResolutionCombo(const std::vector <std::wstring >& list, int defaultIndex = 0);
    void addSnapButton();
    void addCapButton();
    //void addTrackBar();
private:
    MainWindow(const MainWindow& instance){}
    const MainWindow &operator=(const MainWindow& instance){}
    
    HWND MainWindow::addCombo(
        int IDC,
        int width,
        const std::vector<std::wstring>& list,
        int defaultIndex);

    HINSTANCE hApp;
    int videoWidth;
    int videoHeight;
    HWND hMain;
    HICON hIcon;
    HWND hDeviceCombo;
    HWND hFormatCombo;
    HWND hResolutionCombo;
    HWND hSnapButton;
    HWND hCaptureButton;
    //HWND hTrackBar;
    int addPos;
};

#endif