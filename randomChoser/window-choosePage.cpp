#include <Windows.h>

import dataread;
import window;

using dataread::data;
using window::WindowPages;

void WindowPages::chooseOnDrawItem(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;

	HWND ctrl = (HWND)lParam;
	if (ctrl == hTitleText)
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
		DrawText(pDIS->hDC, titleText.c_str(), -1, &rc,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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