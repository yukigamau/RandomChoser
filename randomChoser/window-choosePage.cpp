#include <Windows.h>

import dataread;
import glob;
import id;
import std;
import window;

using dataread::data;
using std::wstring;
using window::WindowPages;

using namespace chooseID;

void window::WindowPages::chooseOnCreate(HWND hwnd)
{
	// 每秒发一次消息
	SetTimer(hwnd, IDT_WAIT, 1000, nullptr);	// 等待一定时间切换至图标模式用

	/* 标题 */
	int titleWidth = titleRect.right - titleRect.left;
	int titleHeight = titleRect.bottom - titleRect.top;
	wstring titleText = L"点名器" + VERSION;
	hTitleText = CreateWindow(
		L"STATIC",
		titleText.c_str(),
		WS_CHILD | WS_VISIBLE,
		titleRect.left, titleRect.top,
		titleWidth, titleHeight,
		hwnd,
		nullptr,
		hInstance,
		nullptr
	);

	glob::title.ini(titleHeight * 0.8, data.fontName);

	SendMessage(hTitleText, WM_SETFONT, glob::title.send(), true);

	/* 设置按钮 */
	hSettingBtn = CreateWindow(
		L"STATIC",
		L"…",
		WS_CHILD | WS_VISIBLE | SS_NOTIFY,
		settingBtnRect.left, settingBtnRect.top,
		settingBtnRect.right - settingBtnRect.left, settingBtnRect.bottom - settingBtnRect.top,
		hwnd,
		(HMENU)idc_stc_settingBtn,
		hInstance,
		nullptr
	);
	SendMessage(hSettingBtn, WM_SETFONT, glob::title.send(), true);

	/* 关闭按钮 */
	hCloseBtn = CreateWindow(
		L"STATIC",
		L"×",
		WS_CHILD | WS_VISIBLE | SS_NOTIFY,
		closeBtnRect.left, closeBtnRect.top,
		closeBtnRect.right - closeBtnRect.left, closeBtnRect.bottom - closeBtnRect.top,
		hwnd,
		(HMENU)IDCLOSE,
		hInstance,
		nullptr
	);
	SendMessage(hCloseBtn, WM_SETFONT, glob::title.send(), true);

	/* 用户区文本 */
	int chooseBtnWidth = choosePageSize.cx - hPenWidth;
	int chooseBtnHeight = choosePageSize.cy - hPenWidth - captionHeight;
	hTextBtn = CreateWindow(
		L"STATIC",
		L"点击抽取",
		WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_CENTER | SS_CENTERIMAGE,
		hPenWidth, captionHeight, chooseBtnWidth, chooseBtnHeight,
		hwnd,
		(HMENU)idc_stc_chooseBtn,
		hInstance,
		nullptr
	);
	glob::chooseBtn.ini(chooseBtnHeight * 0.8, data.fontName);
	SendMessage(hTextBtn, WM_SETFONT, glob::chooseBtn.send(), true);
}