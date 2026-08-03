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

LRESULT WindowPages::settingOnCtlColorBtn(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hCtrl = (HWND)lParam;
	int id = GetDlgCtrlID(hCtrl);

	switch (id)
	{
	case idc_btn_edit_list:
		[[fallthrough]];
	case idc_btn_open_source_site:
		[[fallthrough]];
	case idc_btn_write_list:
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
	case WM_COMMAND:	return settingOnCommand(hWnd, uMsg, wParam, lParam);
	case WM_CREATE:		return settingOnCreate(hWnd, wps.hInstance);

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