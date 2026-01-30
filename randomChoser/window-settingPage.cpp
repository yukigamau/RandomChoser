#include <Windows.h>

import dataread;
import std;
import window;
import "resource.h";

using dataread::data;
using std::vector, std::wstring;
using window::WindowPages;

void WindowPages::createSettingPage()
{
	// 获取系统主题
	getWindowStyle();

	// 注册窗口类
	const wchar_t* className = L"设置页面";
	WNDCLASS wc = {};
	wc.hbrBackground = style.backgroundBrush();
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
	HFONT font = (HFONT)SelectObject(hdc, hFont);

	DrawText(hdc, text.c_str(), -1, &rc, DT_CALCRECT);

	SelectObject(hdc, font);
	ReleaseDC(hWnd, hdc);

	*width = rc.right - rc.left;
	*height = rc.bottom - rc.top;
}

void maxSize(HWND hWnd, HFONT hFont, const vector<wstring>& texts, int* const width, int* const height)
{
	HDC hdc = GetDC(hWnd);
	HFONT font = (HFONT)SelectObject(hdc, hFont);

	for(wstring text:texts)
	{
		RECT rc = { 0, 0, 0, 0 };
		DrawText(hdc, text.c_str(), -1, &rc, DT_CALCRECT);
		*width = max(*width, rc.right - rc.left);
		*height = max(*width, rc.bottom - rc.top);
	}

	SelectObject(hdc, font);
	ReleaseDC(hWnd, hdc);
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
		welcomeX, welcomeY, welcomeW, welcomeH, hwnd, nullptr, hInstance, nullptr);
	SendMessage(welcomeStatic, WM_SETFONT, (WPARAM)style.settingStaticF, TRUE);

	int nextX = welcomeX;
	int nextY = welcomeY + welcomeH + style.textIntervalDistance;
	if (data.defaultList == L"")
	{
		wstring ifListOK = L"您尚未选择要抽取的名单。";
		int width, height;
		getSize(hSetting, style.settingStaticF, ifListOK, &width, &height);
		HWND ifListOKStatic = CreateWindow(L"STATIC", ifListOK.c_str(), WS_CHILD | WS_VISIBLE,
			nextX, nextY, width, height, hwnd, (HMENU)IDC_STATIC_RED, hInstance, nullptr);
		SendMessage(ifListOKStatic, WM_SETFONT, (WPARAM)style.settingStaticF, TRUE);

		nextY += height + style.textIntervalDistance;
	}

	if (data.lists.empty())
	{
		wstring haveNoLists = L"您还没有创建名单。";
		int width, height;
		getSize(hSetting, style.settingStaticF, haveNoLists, &width, &height);
		HWND haveNolistsStatic = CreateWindow(L"STATIC", haveNoLists.c_str(), WS_CHILD | WS_VISIBLE,
			nextX, nextY, width, height, hwnd, (HMENU)IDC_STATIC_RED, hInstance, nullptr);
		SendMessage(haveNolistsStatic, WM_SETFONT, (WPARAM)style.settingStaticF, TRUE);

		nextY += height + style.textIntervalDistance;
	}
	else
	{
		int width, height;
		maxSize(hwnd, style.settingStaticF, data.lists, &width, &height);

		HWND chooseListCombo = CreateWindow(L"COMBOBOX", nullptr,
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
			nextX, nextY, width, height, hwnd, nullptr, hInstance, nullptr);
	}

	// 同行的按钮
	wstring writeList = L"创建名单";
	int width, height;
	getSize(hSetting, style.settingStaticF, writeList, &width, &height);
	width *= style.btnOuterSizeScaleH;
	height *= style.btnOuterSizeScaleV;
	HWND writeListBtn = CreateWindowEx(0, L"BUTTON", writeList.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		nextX, nextY, width, height, hwnd, (HMENU)IDC_BTN_WRITE_LIST, hInstance, nullptr);
	SendMessage(writeListBtn, WM_SETFONT, (WPARAM)style.settingStaticF, TRUE);
	// Y值是相同的
	nextX += width + style.textIntervalDistance;

	wstring openSourceSite = L"打开源码网站";
	getSize(hSetting, style.settingStaticF, openSourceSite, &width, &height);
	width *= style.btnOuterSizeScaleH;
	height *= style.btnOuterSizeScaleV;
	HWND openSourceSiteBtn = CreateWindow(L"BUTTON", openSourceSite.c_str(), WS_CHILD | WS_VISIBLE,
		nextX, nextY, width, height, hwnd, (HMENU)IDC_BTN_OPEN_SOURCE_SITE, hInstance, nullptr);
	SendMessage(openSourceSiteBtn, WM_SETFONT, (WPARAM)style.settingStaticF, TRUE);
}

LRESULT WindowPages::settingOnCtlColorBtn(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hCtrl = (HWND)lParam;
	int id = GetDlgCtrlID(hCtrl);

	switch (id)
	{
	case IDC_BTN_OPEN_SOURCE_SITE:
		[[fallthrough]];
	case IDC_BTN_WRITE_LIST:
		return (INT_PTR)style.buttonBkBrush();

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

LRESULT WindowPages::settingOnCtlColorStatic(WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hCtrl = (HWND)lParam;
	int id = GetDlgCtrlID(hCtrl);

	if (id == IDC_STATIC_RED)
		SetTextColor(hdc, RGB(255, 0, 0));
	else
		SetTextColor(hdc, style.textColor());

	SetBkMode(hdc, TRANSPARENT);
	return (INT_PTR)GetStockObject(NULL_BRUSH);
}