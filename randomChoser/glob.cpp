module glob;
#include <Windows.h>
#include "resource.h"
import window;
using window::WindowPages, window::wps;

void glob::createSettingPage()
{
	// 注册窗口类
	const wchar_t *className = L"设置页面";
	WNDCLASS wc = {};
	wc.hbrBackground = wps.style.textBkBrush();
	wc.lpfnWndProc = WindowPages::settingWP; // 设置窗口过程函数
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hInstance = GetModuleHandle(nullptr); // 获取实例句柄
	wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.lpszClassName = className;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wc))
	{
		DWORD err = GetLastError();
		if (err != ERROR_CLASS_ALREADY_EXISTS) // 可以忽略已存在
		{
			MessageBox(nullptr, L"RegisterClass failed!", L"Error", MB_ICONERROR);
			return;
		}
	}

	LPCWSTR lpWindowName = L"点名器设置";
	// 创建窗口
	wps.hSetting = CreateWindow(
		className, lpWindowName,
		WS_OVERLAPPEDWINDOW,
		// 等创建控件之后根据控件调整窗口大小和位置
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr,
		wc.hInstance,	// 实例句柄
		nullptr			// 附加数据
	);

	ShowWindow(wps.hSetting, SW_SHOW);
	SetForegroundWindow(wps.hSetting);	// 把窗口显示到最前面
	UpdateWindow(wps.hSetting);
}