#ifndef _VIDEO_DEVICE_H_
#define _VIDEO_DEVICE_H_

#include <dshow.h>
#include <D3d9.h>
#include <vmr9.h>

class VideoDevice
{
public:
	VideoDevice();
	~VideoDevice();
	void find(wchar_t* name = nullptr);
	void findAll() { find(nullptr); }
	void setMainWin(HWND hWin);
	void setZoomBar(HWND hBar);
	void start();
	void stop();
	void snap();
	void paint();
	void updateZoomBar(WPARAM wParam);
	//const RECT* getVideoRect(){return &videoRect;}
	//const RECT* getWinRect(){return &winRect;}
	
	IMediaEvent *pMediaEvent;
private:
	bool createDeviceEnum();
	void InitWindowlessVMR9(HWND hWin);
	HRESULT GetNextFilter(IBaseFilter *pFilter,PIN_DIRECTION Dir,IBaseFilter **ppNext, bool skipFirstPin = false);
	HRESULT GetPinCategory(IPin *pPin, GUID *pPinCategory);
	void VideoDevice::ListPin(IBaseFilter* pFilter);
	IPin *FindPin(IBaseFilter* pFilter, LPOLESTR pinName);
	void SetFormat(const GUID &subtype, long height, long width, bool isVideoInfo2);

	bool hasDevice;
	IEnumMoniker *pEnumMoniker;
	IMoniker *pMoniker;
	ICaptureGraphBuilder2 *pCaptureGraphBuilder;
	IGraphBuilder *pGraphBuilder;
	IBaseFilter *pVMRFilter;
	IBaseFilter *pCaptureFilter;
	IAMCameraControl *pCameraControl;
	IMediaControl *pMediaControl;
	IVMRWindowlessControl9 *pVMRControl;

	
	RECT videoRect, winRect, targetRect;
	HWND hwnd;
	HWND hbar;
};

#endif