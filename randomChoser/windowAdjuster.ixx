module;

#include <Windows.h>

export module window:windowAdjuster;
import std;
using std::wstring;
using std::max;

export namespace window
{
	class WindowAdjuster
	{
	public:
		// 用于控件的位置确定
		const int left;
		int x = 0;
		int y = 0;

	private:
		int maxX = 0;
		int maxY = 0;
		int interval = 0;

	private:
		HWND hWnd = nullptr;
		HFONT hFont = nullptr;

	public:
		WindowAdjuster(HWND hWnd, HFONT hFont, int interval, int xBegin, int yBegin);

	public:
		void adjust(int width, int height);
		void apply();
		void ctlBeside(int width);
		void ctlNext(int height);
		void getCtlSize(const wstring& text, int* const width, int* const height);
	};
}

window::WindowAdjuster::WindowAdjuster(HWND hWnd, HFONT hFont, int interval, int xBegin, int yBegin)
	:hWnd{ hWnd }, hFont{ hFont }, interval{ interval }, x{ xBegin }, y{ yBegin }, left{ xBegin }
{ }

void window::WindowAdjuster::adjust(int width, int height)
{
	maxX = max(width, maxX);
	// Y值是积累的
	maxY += height + interval;
}

void window::WindowAdjuster::apply()
{
	// 宽度要加上边缘，但这里不得不增加更多的宽度来让文字全部显示出来
	maxX += 5 * left;

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int x = (screenWidth - maxX) / 2;
	int y = (screenHeight - maxY) / 2;
	SetWindowPos(hWnd, nullptr, x, y, maxX, maxY, SWP_NOZORDER);
}

void window::WindowAdjuster::ctlBeside(int width)
{
	x += width + interval;
}

void window::WindowAdjuster::ctlNext(int height)
{
	y += height + interval;
}

void window::WindowAdjuster::getCtlSize(const wstring& text, int* const width, int* const height)
{
	RECT rc = { 0, 0, 0, 0 };

	HDC hdc = GetDC(hWnd);
	HFONT font = (HFONT)SelectObject(hdc, hFont);

	DrawText(hdc, text.c_str(), -1, &rc, DT_CALCRECT);

	SelectObject(hdc, font);
	ReleaseDC(hWnd, hdc);

	*width = rc.right - rc.left;
	*height = rc.bottom - rc.top;

	adjust(*width, *height);
}