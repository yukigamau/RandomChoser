#include <Windows.h>
import dataread;
import listModifyWPFun;
import std;
import window;

using dataread::data;
using std::wstring;
using std::tie;
using window::Page, window::WindowAdjuster;

auto& listModify{ window::wps.listModify };
const auto idc_static_red{ 1501 };

LRESULT listModifyOnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	constexpr auto xBegin{ 10 };
	constexpr auto yBegin{ 10 };
	WindowAdjuster wa(hWnd, listModify.style->hFStatic, listModify.style->interval, xBegin, yBegin);

	// 用于对齐下面的名字输入编辑框和名单名字框
	auto maxX{ 0 };

	wstring wsListName{ L"名单名字：" };
	auto [width, height] = wa.getCtlSize(wsListName);
	HWND hSListName = CreateWindow(L"STATIC", wsListName.c_str(), WS_CHILD | WS_VISIBLE,
		wa.x, wa.y, width, height, hWnd, nullptr, listModify.hInstance, nullptr);
	SendMessage(hSListName, WM_SETFONT, (WPARAM)listModify.style->hFStatic, TRUE);

	wa.ctlBeside(width);
	maxX += width + wa.interval;

	width *= 9;
	const auto halfMargin{ 5 * listModify.style->dpiScale };
	HWND hEListName = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		wa.x, wa.y - halfMargin, width, height + halfMargin, hWnd, nullptr, listModify.hInstance, nullptr);
	SendMessage(hEListName, WM_SETFONT, (WPARAM)listModify.style->hFStatic, TRUE);
	SendMessage(hEListName, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN,
		MAKELPARAM(halfMargin, halfMargin));
	
	maxX += width;
	wa.adjustMaxXChange(maxX);

	wa.ctlNext(height + halfMargin);
	wa.ctlLeft(xBegin);

	wstring wsWriteNamesTip{ L"在下方写下名字，一行一个。\n不支持用空格和制表符！！！" };
	tie(width, height) = wa.getCtlSize(wsWriteNamesTip);
	HWND hSWriteNamesTip = CreateWindow(L"STATIC", wsWriteNamesTip.c_str(), WS_CHILD | WS_VISIBLE,
		wa.x, wa.y, width, height, hWnd, (HMENU)idc_static_red, listModify.hInstance, nullptr);
	SendMessage(hSWriteNamesTip, WM_SETFONT, (WPARAM)listModify.style->hFStatic, TRUE);

	wa.ctlNext(height);

	height *= 16;	// 这里是2*32行内容，对于大部分情况下应该是够用的
	width = maxX;
	HWND hEWriteName = CreateWindowEx(0, L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_VSCROLL,
		wa.x, wa.y, width, height, hWnd, nullptr, listModify.hInstance, nullptr);
	SendMessage(hEWriteName, WM_SETFONT, (WPARAM)listModify.style->hFStatic, TRUE);
	SendMessage(hEWriteName, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN,
		MAKELPARAM(halfMargin, halfMargin));

	// 这里的多余的值用于处理不知道为什么出现的编辑框无法完整出现的问题
	wa.adjustMaxYAddon(height + halfMargin * 4 + wa.interval);

	wa.apply();

	return 0;
}

LRESULT listModifyOnCtlColorEdit(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hEdit = (HWND)lParam;

	SetTextColor(hdc, listModify.style->textColor());   // 字体颜色
	SetBkColor(hdc, listModify.style->textBkColor());	// 背景颜色，同时也改变边框的颜色

	return (INT_PTR)listModify.style->textBkBrush();
}

LRESULT listModifyOnCtlColorStatic(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hCtrl = (HWND)lParam;
	auto id{ GetDlgCtrlID(hCtrl) };
	
	switch (id)
	{
	case idc_static_red:
		SetTextColor(hdc, RGB(255, 0, 0));
		break;

	default:
		SetTextColor(hdc, listModify.style->textColor());
		break;
	}

	SetBkMode(hdc, TRANSPARENT);
	return (INT_PTR)GetStockObject(NULL_BRUSH);
}