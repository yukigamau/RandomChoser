#include <Windows.h>

import dataread;
import window;

using dataread::data;
using window::Style;

Style::~Style()
{
	DeleteObject(hFStatic);
}

void Style::dpi(double dpiSacle)
{
	this->dpiScale = dpiSacle;

	interval *= dpiSacle;
}

void Style::getWindowStyle()
{
	// 系统的默认设定
	const DWORD dark = 0;

	DWORD value = 1;
	DWORD size = sizeof(value);

	if (RegGetValue(
		HKEY_CURRENT_USER,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
		L"AppUsesLightTheme",
		RRF_RT_REG_DWORD,
		nullptr,
		&value,
		&size
	) == ERROR_SUCCESS)
	{
		ini(value);
	}
	else if (RegGetValue(
		HKEY_CURRENT_USER,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
		L"SystemUsesLightTheme",
		RRF_RT_REG_DWORD,
		nullptr,
		&value,
		&size
	) == ERROR_SUCCESS
		)
	{
		ini(value);
	}
	else
		ini(value);
}

void Style::ini(DWORD styleValue)
{
	const DWORD DARK = 0;

	if (styleValue == DARK)
		color = ColorStyle::dark;
	else
		color = ColorStyle::light;

	iniFont();
}

void Style::iniFont()
{
	hFStatic = CreateFont(
		18 * dpiScale,			// 字体高度
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
}

HBRUSH Style::buttonBkBrush()
{
	return textBkBrush();
}

HBRUSH Style::textBkBrush()
{
	if (color == dark)
		return CreateSolidBrush(RGB(30, 30, 30));
	else
		return (HBRUSH)(COLOR_WINDOW + 1);
}

COLORREF Style::textBkColor()
{
	if (color == dark)
		return RGB(30, 30, 30);
	else
		return RGB(255, 255, 255);
}

COLORREF Style::textColor()
{
	if (color == dark)
		return RGB(255, 255, 255);
	else
		return RGB(0, 0, 0);
}