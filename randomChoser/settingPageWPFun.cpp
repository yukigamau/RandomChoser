#include <Windows.h>
#include <windowsx.h>
#include "resource.h"

import dataread;
import glob;
import id;
import settingWPFun;
import std;
import website;
import window;

using dataread::data;
using std::vector, std::wstring;
using website::openWebsite;
using window::WindowAdjuster, window::WindowPages;
using namespace settingID;

// 同时求最大的宽度和高度
void maxSize(HWND hWnd, HFONT hFont, const vector<wstring> &texts, int *const width, int *const height)
{
	// 初始化，防出错。
	*width = 0;
	*height = 0;

	HDC hdc = GetDC(hWnd);
	HFONT font = (HFONT)SelectObject(hdc, hFont);

	for (const wstring &text : texts)
	{
		RECT rc = { 0, 0, 0, 0 };
		DrawText(hdc, text.c_str(), -1, &rc, DT_CALCRECT);
		*width = max(*width, rc.right - rc.left);
		*height = max(*height, rc.bottom - rc.top);
	}

	SelectObject(hdc, font);
	ReleaseDC(hWnd, hdc);
}

void WindowPages::settingOnCreate(HWND hWnd)
{
	constexpr auto pBegin{ POINT{10,10} };
	WindowAdjuster wa(hWnd, style.hFStatic, style.interval, pBegin);

	const wstring welcome = L"欢迎使用本程序！当前版本：" + glob::VERSION + L"\n"
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
		wstring wsDefalutList = L"抽取名单：";
		int width, height;
		std::tie(width, height) = wa.getCtlSize(wsDefalutList);
		HWND hDefalutList = CreateWindow(L"STATIC", wsDefalutList.c_str(), WS_CHILD | WS_VISIBLE,
			wa.x, wa.y, width, height, hWnd, (HMENU)idc_ccb_default_list, hInstance, nullptr);
		SendMessage(hDefalutList, WM_SETFONT, (WPARAM)style.hFStatic, TRUE);

		wa.ctlBeside(width);

		maxSize(hWnd, style.hFStatic, data.lists, &width, &height);
		width += 50;	// 补足下拉键的宽度

		HWND chooseListCombo = CreateWindow(L"COMBOBOX", nullptr,
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
			wa.x, wa.y, width, height, hWnd, nullptr, hInstance, nullptr);

		for (auto a : data.lists)
		{
			int nIndex = SendMessage(chooseListCombo, CB_ADDSTRING, 0, (LPARAM)a.c_str());
			if (nIndex == CB_ERR)
				throw std::runtime_error("插入名单选项失败");
			else if (nIndex == CB_ERRSPACE)
				throw std::runtime_error("chooseListCombo CB_ERRSPACE");
			else if (a == data.defaultList)
				ComboBox_SetCurSel(chooseListCombo, nIndex);
		}

		SendMessage(chooseListCombo, WM_SETFONT, (WPARAM)style.hFStatic, TRUE);

		wa.ctlLeft(pBegin.x);
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

	wstring editList = L"修改当前名单";
	wa.getCtlSize(editList, &width, &height);
	width *= style.btnOuterSizeScaleH;
	height *= style.btnOuterSizeScaleV;
	HWND editListBtn = CreateWindow(L"BUTTON", editList.c_str(), WS_CHILD | WS_VISIBLE,
		wa.x, wa.y, width, height, hWnd, (HMENU)idc_btn_edit_list, hInstance, nullptr);
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
	case idc_btn_edit_list:
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
		return settingOnCommand(hWnd, uMsg, wParam, lParam);

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