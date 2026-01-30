#include <Windows.h>

import dataread;
import std;
import window;
import "resource.h";

using dataread::data;
using std::wstring;
using window::WindowPages;

void WindowPages::createSettingPage()
{
	// 获取系统主题
	getWindowStyle();

	// 注册窗口类
	const wchar_t* className = L"设置页面";
	WNDCLASS wc = {};
	wc.hbrBackground = style.backgroundColor();
	wc.lpfnWndProc = settingWP; // 设置窗口过程函数
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

	int x = (screenSize.cx - settingPageSize.cx) / 2;
	int y = (screenSize.cy - settingPageSize.cy) / 2;
	hSetting = CreateWindow(
		className, lpWindowName,
		WS_OVERLAPPEDWINDOW,
		x, y,
		settingPageSize.cx, settingPageSize.cy,
		nullptr, nullptr,
		wc.hInstance,	// 实例句柄
		nullptr			// 附加数据
	);

	ShowWindow(hSetting, SW_SHOW);
	SetForegroundWindow(hSetting);	// 把窗口显示到最前面
	UpdateWindow(hSetting);
}

void getSize(HWND hWnd, HFONT hFont, const wstring& text, int* const width, int* const height)
{
	RECT rc = { 0, 0, 0, 0 };

	HDC hdc = GetDC(hWnd);
	HFONT old = (HFONT)SelectObject(hdc, hFont);

	DrawText(hdc, text.c_str(), -1, &rc,
		DT_CALCRECT);

	SelectObject(hdc, old);
	ReleaseDC(hWnd, hdc);

	*width = rc.right - rc.left;
	*height = rc.bottom - rc.top;
}

void WindowPages::settingOnCreate(HWND hwnd)
{
	const wstring welcome = L"欢迎使用本程序！当前版本：" + VERSION + L"\n"
		L"如果您在使用本产品时遇到程序漏洞，请发邮件至yvehuanghun@outlook.com" + L"\n"
		L"本项目己经在github上开源：https://github.com/yukigamau/RandomChoser";
	const int welcomeX = 10, welcomeY = 10;
	int welcomeW, welcomeH;
	getSize(hSetting, style.settingStaticF, welcome, &welcomeW, &welcomeH);
	HWND welcomeStatic = CreateWindow(L"STATIC", welcome.c_str(), WS_CHILD | WS_VISIBLE,
		welcomeX, welcomeY, welcomeW + 4, welcomeH + 2, hwnd, nullptr, hInstance, nullptr);
	SendMessage(welcomeStatic, WM_SETFONT, (WPARAM)style.settingStaticF, TRUE);

	int nextX = welcomeX;
	int nextY = welcomeY + welcomeH + 2 + style.textIntervalDistance;
	if (data.defaultList == L"")
	{
		wstring ifListOK = L"您尚未选择要抽取的名单。";
		int width, height;
		getSize(hSetting, style.settingStaticF, ifListOK, &width, &height);
		HWND ifListOKStatic = CreateWindow(L"STATIC", ifListOK.c_str(), WS_CHILD | WS_VISIBLE,
			nextX, nextY, width, height, hwnd, (HMENU)IDC_IFLISTOK, hInstance, nullptr);
		SendMessage(ifListOKStatic, WM_SETFONT, (WPARAM)style.settingStaticF, TRUE);
	}
}

LRESULT WindowPages::settingOnCtlColorStatic(WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hCtrl = (HWND)lParam;
	int id = GetDlgCtrlID(hCtrl);

	if (id == IDC_IFLISTOK)
		SetTextColor(hdc, RGB(255, 0, 0));
	else
	SetTextColor(hdc, style.textColor());

	SetBkMode(hdc, TRANSPARENT);
	return (INT_PTR)GetStockObject(NULL_BRUSH);
}