#include <Windows.h>
#include <ShellScalingAPI.h>

#pragma comment(lib, "Shcore.lib")

import window;


void window::WindowPages::iniDpi()
{
	// 设置 DPI 感知
	HRESULT hr = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	// 如果系统不支持 SetProcessDpiAwareness，回退到旧 API
	if (FAILED(hr))	SetProcessDPIAware();

	// DPI调整
	const double defaultDPI = 96.0;
	double dpiScale = getDPIScalingFactor() / defaultDPI;

	captionHeight = GetSystemMetrics(SM_CYCAPTION);

	choosePageSize.cx *= dpiScale;
	choosePageSize.cy *= dpiScale;

	iconPageSize.cx *= dpiScale;
	iconPageSize.cy *= dpiScale;

	hPenWidth *= dpiScale;

	int btnSize = captionHeight;

	titleRect = { 0,0,choosePageSize.cx,captionHeight };

	settingBtnRect = { choosePageSize.cx - 2 * btnSize,0,choosePageSize.cx - btnSize,captionHeight };
	closeBtnRect = { choosePageSize.cx - btnSize,0,choosePageSize.cx,captionHeight };

	settingFontHeight *= dpiScale;

	// 屏幕
	screenSize.cx = GetSystemMetrics(SM_CXSCREEN);
	screenSize.cy = GetSystemMetrics(SM_CYSCREEN);

	style.dpi(dpiScale);
}

void window::WindowPages::ini(HINSTANCE hInstance)
{
	iniDpi();
	this->hInstance = hInstance;
	style.getWindowStyle();

	// 配置页面的过程处理函数
	listModify.process = window::listModifyWP;
	password.process = window::passwordWP;
	editList.process = window::editListWP;
}