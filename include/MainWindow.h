#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <vector>
#include "RenderWin.h"

class MainWindow:public RenderWin
{
public:
    MainWindow(HINSTANCE hInstance);
    ~MainWindow(){}

    void addIcon();
    void addDeviceCombo(const std::vector<std::wstring>& list, int defaultIndex = 0);
    void addFormatCombo(const std::vector<std::wstring>& list, int defaultIndex = 0);
    void addResolutionCombo(const std::vector <std::wstring >& list, int defaultIndex = 0);
    void addSnapButton();
    void addCapButton();

    void updateFormatCombo(const std::vector<std::wstring>& list, int defaultIndex = 0);
    void updateResolutionCombo(const std::vector <std::wstring >& list, int defaultIndex = 0);
    void updateUserSettings(
        const std::vector<std::wstring>& listFormat,
        const std::vector<std::wstring>& listResolution);
    void updateWindowSize(int width, int height);
    void updateCtrlPos(int imageHeight);
    
    HINSTANCE getApp(){return hApp;}
    HWND getHandle(){return hMain;}
    std::vector<std::wstring> getUserSettings();
private:
    MainWindow(const MainWindow& instance){}
    const MainWindow &operator=(const MainWindow& instance){}
    
    HWND addCombo(
        int IDC,
        int width,
        const std::vector<std::wstring>& list,
        int defaultIndex);

    
    void updateCombo(
        HWND hwnd,
        const std::vector<std::wstring>& list,
        int defaultIndex);

    HINSTANCE hApp;
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