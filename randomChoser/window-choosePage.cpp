#include <Windows.h>

import dataread;
import std;
import window;

using dataread::data;
using std::wstring;
using window::WindowPages;

void window::WindowPages::chooseOnCreate(HWND hwnd)
{
	// 每秒发一次消息
	SetTimer(hwnd, IDT_WAIT, 1000, nullptr);	// 等待一定时间切换至图标模式用

	/* 标题 */
	wstring titleText = L"点名器" + VERSION;
	hTitleText = CreateWindow(
		L"STATIC",
		titleText.c_str(),
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		titleRect.left, titleRect.top,
		titleRect.right - titleRect.left, titleRect.bottom - titleRect.top,
		hwnd,
		nullptr,
		hInstance,
		nullptr
	);
#if _DEBUG

	if (!hTitleText)
		throw("抽取页面的标题文本控件创建失败！");

#endif // _DEBUG

	/* 设置按钮 */
	hSettingBtn = CreateWindow(
		L"BUTTON",
		L"…",
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		settingBtnRect.left, settingBtnRect.top,
		settingBtnRect.right - settingBtnRect.left, settingBtnRect.bottom - settingBtnRect.top,
		hwnd,
		(HMENU)IDC_BTN_SETTING,
		hInstance,
		nullptr
	);

	/* 关闭按钮 */
	hCloseBtn = CreateWindow(
		L"BUTTON",
		L"×",
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		closeBtnRect.left, closeBtnRect.top,
		closeBtnRect.right - closeBtnRect.left, closeBtnRect.bottom - closeBtnRect.top,
		hwnd,
		(HMENU)IDCLOSE,
		hInstance,
		nullptr
	);

	/* 用户区文本 */
	hTextBtn = CreateWindow(
		L"BUTTON",
		L"点击抽取",
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		hPenWidth, captionHeight, choosePageSize.cx - hPenWidth, choosePageSize.cy - hPenWidth,
		hwnd,
		(HMENU)IDC_BTN_TEXT,
		hInstance,
		nullptr
	);
}

void WindowPages::chooseOnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
	HWND ctrl = pDIS->hwndItem;
	if (ctrl == hTitleText)
	{
		/* 背景 */
		RECT rc = pDIS->rcItem;
		HBRUSH hBkg = CreateSolidBrush(data.captionBC);
		FillRect(pDIS->hDC, &rc, hBkg);
		DeleteObject(hBkg);
		SetBkMode(pDIS->hDC, TRANSPARENT);

		/* 字体 */
		hFont = CreateFont(
			(rc.bottom - rc.top) * 0.8,	// 字体高度
			0, 0, 0,
			FW_NORMAL,	// 字体粗细
			FALSE,          // 是否斜体
			FALSE,          // 是否下划线
			FALSE,          // 是否删除线
			DEFAULT_CHARSET,// 字符集
			OUT_DEFAULT_PRECIS,   // 输出精度
			CLIP_DEFAULT_PRECIS,  // 裁剪精度
			CLEARTYPE_QUALITY,      // 输出质量
			DEFAULT_PITCH | FF_SWISS,	// 字体间距和家族
			data.fontName.c_str()
		);
		SetTextColor(pDIS->hDC, data.captionFC);
		HFONT oldFont = (HFONT)SelectObject(pDIS->hDC, hFont);
		SelectObject(pDIS->hDC, hFont);
		DrawText(pDIS->hDC, titleText.c_str(), -1, &rc,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		DeleteObject(hFont);
		SelectObject(pDIS->hDC, oldFont);
	}
	else if (ctrl == hSettingBtn)
	{
		/* 背景 */
		RECT rc = pDIS->rcItem;
		HBRUSH hBkg = CreateSolidBrush(data.captionBC);
		FillRect(pDIS->hDC, &rc, hBkg);
		DeleteObject(hBkg);

		/* 字体 */
		hFont = CreateFont(
			(rc.bottom - rc.top) * 0.8,	// 字体高度
			0, 0, 0,
			FW_NORMAL,	// 字体粗细
			FALSE,          // 是否斜体
			FALSE,          // 是否下划线
			FALSE,          // 是否删除线
			DEFAULT_CHARSET,// 字符集
			OUT_DEFAULT_PRECIS,   // 输出精度
			CLIP_DEFAULT_PRECIS,  // 裁剪精度
			CLEARTYPE_QUALITY,      // 输出质量
			DEFAULT_PITCH | FF_SWISS,	// 字体间距和家族
			data.fontName.c_str()
		);
		SetTextColor(pDIS->hDC, data.captionFC);
		SelectObject(pDIS->hDC, hFont);
		DrawText(pDIS->hDC, L"…", -1, &rc,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	else if (ctrl == hCloseBtn)
	{
		/* 背景 */
		RECT rc = pDIS->rcItem;
		HBRUSH hBkg = CreateSolidBrush(data.captionBC);
		FillRect(pDIS->hDC, &rc, hBkg);
		DeleteObject(hBkg);

		/* 字体 */
		hFont = CreateFont(
			(rc.bottom - rc.top) * 0.8,	// 字体高度
			0, 0, 0,
			FW_NORMAL,	// 字体粗细
			FALSE,          // 是否斜体
			FALSE,          // 是否下划线
			FALSE,          // 是否删除线
			DEFAULT_CHARSET,// 字符集
			OUT_DEFAULT_PRECIS,   // 输出精度
			CLIP_DEFAULT_PRECIS,  // 裁剪精度
			CLEARTYPE_QUALITY,      // 输出质量
			DEFAULT_PITCH | FF_SWISS,	// 字体间距和家族
			data.fontName.c_str()
		);
		SetTextColor(pDIS->hDC, data.captionFC);
		SelectObject(pDIS->hDC, hFont);
		DrawText(pDIS->hDC, L"×", -1, &rc,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	else if (ctrl == hTextBtn)
	{
		/* 背景 */
		RECT rc = pDIS->rcItem;
		HBRUSH hBkg = CreateSolidBrush(data.clientBC);
		FillRect(pDIS->hDC, &rc, hBkg);
		DeleteObject(hBkg);

		/* 字体 */
		hFont = CreateFont(
			(rc.bottom - rc.top) * 0.8,	// 字体高度
			0, 0, 0,
			FW_NORMAL,	// 字体粗细
			FALSE,          // 是否斜体
			FALSE,          // 是否下划线
			FALSE,          // 是否删除线
			DEFAULT_CHARSET,// 字符集
			OUT_DEFAULT_PRECIS,   // 输出精度
			CLIP_DEFAULT_PRECIS,  // 裁剪精度
			CLEARTYPE_QUALITY,      // 输出质量
			DEFAULT_PITCH | FF_SWISS,	// 字体间距和家族
			data.fontName.c_str()
		);
		SetTextColor(pDIS->hDC, data.clientFC);
		SelectObject(pDIS->hDC, hFont);
		DrawText(pDIS->hDC, L"点击抽取", -1, &rc,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}