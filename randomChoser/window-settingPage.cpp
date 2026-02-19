#include <Windows.h>

import dataread;
import std;
import website;
import window;
import "resource.h";

using dataread::data;
using std::vector, std::wstring;
using website::openWebsite;
using window::WindowAdjuster, window::WindowPages;

void WindowPages::createSettingPage()
{
	// 注册窗口类
	const wchar_t* className = L"设置页面";
	WNDCLASS wc = {};
	wc.hbrBackground = style.textBkBrush();
	wc.lpfnWndProc = settingWP; // 设置窗口过程函数
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
	hSetting = CreateWindow(
		className, lpWindowName,
		WS_OVERLAPPEDWINDOW,
		// 等创建控件之后根据控件调整窗口大小和位置
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr,
		wc.hInstance,	// 实例句柄
		nullptr			// 附加数据
	);

	ShowWindow(hSetting, SW_SHOW);
	SetForegroundWindow(hSetting);	// 把窗口显示到最前面
	UpdateWindow(hSetting);
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

LRESULT WindowPages::settingOnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);
	switch (id)
	{
	case IDC_BTN_EDIT_LIST:

		return 0;

	case IDC_BTN_OPEN_SOURCE_SITE:
		openWebsite(L"https://github.com/yukigamau/RandomChoser");
		return 0;

	case IDC_BTN_WRITE_LIST:
		listModify.ini(hInstance, &style);
		listModify.createWindow(L"listModify", L"创建名单", WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT);
		ShowWindow(hWnd, SW_HIDE);
		return 0;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

void WindowPages::settingOnCreate(HWND hWnd)
{
	const int xBegin = 10;
	const int yBegin = 10;
	WindowAdjuster wa(hWnd, style.hFStatic, style.interval, xBegin, yBegin);

	const wstring welcome = L"欢迎使用本程序！当前版本：" + VERSION + L"\n"
		L"如果您在使用本产品时遇到程序漏洞，请发邮件至yvehuanghun@outlook.com" + L"\n"
		L"本项目己经在github上开源：https://github.com/yukigamau/RandomChoser";
	int width, height;
	wa.getCtlSize(welcome, &width, &height);
	HWND welcomeStatic = CreateWindow(L"STATIC", welcome.c_str(), WS_CHILD | WS_VISIBLE,
		wa.x, wa.y, width, height, hWnd, nullptr, hInstance, nullptr);
	SendMessage(welcomeStatic, WM_SETFONT, (WPARAM)style.hFStatic, TRUE);

	wa.ctlNext(height);

	if (data.defaultList == L"")
	{
		wstring ifListOK = L"您尚未选择要抽取的名单。";
		int width, height;
		wa.getCtlSize(ifListOK, &width, &height);
		HWND ifListOKStatic = CreateWindow(L"STATIC", ifListOK.c_str(), WS_CHILD | WS_VISIBLE,
			wa.x, wa.y, width, height, hWnd, (HMENU)IDC_STATIC_RED, hInstance, nullptr);
		SendMessage(ifListOKStatic, WM_SETFONT, (WPARAM)style.hFStatic, TRUE);

		wa.ctlNext(height);
	}

	if (data.lists.empty())
	{
		wstring haveNoLists = L"您尚未创建名单。";
		int width, height;
		wa.getCtlSize(haveNoLists, &width, &height);
		HWND haveNolistsStatic = CreateWindow(L"STATIC", haveNoLists.c_str(), WS_CHILD | WS_VISIBLE,
			wa.x, wa.y, width, height, hWnd, (HMENU)IDC_STATIC_RED, hInstance, nullptr);
		SendMessage(haveNolistsStatic, WM_SETFONT, (WPARAM)style.hFStatic, TRUE);

		wa.ctlNext(height);
	}
	else
	{
		int width, height;
		maxSize(hWnd, style.hFStatic, data.lists, &width, &height);

		HWND chooseListCombo = CreateWindow(L"COMBOBOX", nullptr,
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
			wa.x, wa.y, width, height, hWnd, nullptr, hInstance, nullptr);

		wa.ctlNext(height);
	}

	// 同行的按钮
	wstring writeList = L"创建名单";
	wa.getCtlSize(writeList, &width, &height);
	width *= style.btnOuterSizeScaleH;
	height *= style.btnOuterSizeScaleV;
	HWND writeListBtn = CreateWindowEx(0, L"BUTTON", writeList.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		wa.x, wa.y, width, height, hWnd, (HMENU)IDC_BTN_WRITE_LIST, hInstance, nullptr);
	SendMessage(writeListBtn, WM_SETFONT, (WPARAM)style.hFStatic, TRUE);
	
	wa.ctlBeside(width);

	wstring openSourceSite = L"打开源码网站";
	wa.getCtlSize(openSourceSite, &width, &height);
	width *= style.btnOuterSizeScaleH;
	height *= style.btnOuterSizeScaleV;
	HWND openSourceSiteBtn = CreateWindow(L"BUTTON", openSourceSite.c_str(), WS_CHILD | WS_VISIBLE,
		wa.x, wa.y, width, height, hWnd, (HMENU)IDC_BTN_OPEN_SOURCE_SITE, hInstance, nullptr);
	SendMessage(openSourceSiteBtn, WM_SETFONT, (WPARAM)style.hFStatic, TRUE);

	wa.ctlBeside(width);

	wstring editList = L"修改已有名单";
	wa.getCtlSize(editList, &width, &height);
	width *= style.btnOuterSizeScaleH;
	height *= style.btnOuterSizeScaleV;
	HWND editListBtn = CreateWindow(L"BUTTON", editList.c_str(), WS_CHILD | WS_VISIBLE,
		wa.x, wa.y, width, height, hWnd, (HMENU)IDC_BTN_EDIT_LIST, hInstance, nullptr);
	SendMessage(editListBtn, WM_SETFONT, (WPARAM)style.hFStatic, TRUE);

	// 如果没有名单，那么就不会启用【修改已有名单】
	if (data.lists.empty())
		EnableWindow(editListBtn, false);

	wa.apply();
}

LRESULT WindowPages::settingOnCtlColorBtn(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hCtrl = (HWND)lParam;
	int id = GetDlgCtrlID(hCtrl);

	switch (id)
	{
	case IDC_BTN_EDIT_LIST:
		[[fallthrough]];
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

LRESULT CALLBACK window::WindowPages::settingWP(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		return wps.settingOnCommand(hWnd, uMsg, wParam, lParam);

	case WM_CREATE:
		wps.settingOnCreate(hWnd);
		break;

	case WM_CTLCOLORBTN:
		return wps.settingOnCtlColorBtn(hWnd, uMsg, wParam, lParam);

	case WM_CTLCOLORSTATIC:
		return wps.settingOnCtlColorStatic(wParam, lParam);

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}