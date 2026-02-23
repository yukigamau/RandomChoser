module;

#include <Windows.h>

export module window:windowAdjuster;
import std;
using std::tuple, std::wstring;
using std::max;

export namespace window
{
	// 控件是在上一个控件的哪个方向
	enum class Follow
	{
		next,
		beside
	};

	enum class IfButton
	{
		button,
		others
	};

	class WindowAdjuster
	{
	public:
		// 用于控件的位置确定
		const int left;
		int x = 0;
		int y = 0;
		int interval = 0;

	private:
		int maxX = 0;
		int maxY = 0;

	private:
		HWND hWnd = nullptr;
		HFONT hFont = nullptr;

	public:
		WindowAdjuster(HWND hWnd, HFONT hFont, int interval, int xBegin, int yBegin);

	public:
		// 用于判断窗口的尺寸，每次增加新的控件都需要重新调整
		void adjust(int width, int height);
		// 需要用控件的右x值来调整应该的maxX
		void adjustMaxXChange(int w);
		void adjustMaxYAddon(int h);
		void apply();
		void ctlBeside(int width);
		void ctlLeft(int xLeft);
		void ctlNext(int height);

	private:
		tuple<int, int> calcRect(const wstring& text);
	public:
		void getCtlSize(const wstring& text, int* const width, int* const height);
		tuple<int, int> getCtlSize(const wstring& text,
			IfButton ifButton = IfButton::others, Follow follow = Follow::next);
		int getMultipleLinesHeight(int n);

		// 获取maxX值
		int outMaxWidth();
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

void window::WindowAdjuster::adjustMaxXChange(int w)
{
	maxX = max(maxX, w);
}

void window::WindowAdjuster::adjustMaxYAddon(int h)
{
	maxY += h + interval;
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

void window::WindowAdjuster::ctlLeft(int xLeft)
{
	x = xLeft;
}

void window::WindowAdjuster::ctlNext(int height)
{
	y += height + interval;
}

tuple<int, int> window::WindowAdjuster::calcRect(const wstring& text)
{
	RECT rc{ 0,0,0,0 };

	HDC hdc = GetDC(hWnd);
	HFONT font = (HFONT)SelectObject(hdc, hFont);

	DrawText(hdc, text.c_str(), -1, &rc, DT_CALCRECT);

	SelectObject(hdc, font);
	ReleaseDC(hWnd, hdc);

	auto width{ rc.right - rc.left };
	auto height{ rc.bottom - rc.top };

	return { width,height };
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
tuple<int, int> window::WindowAdjuster::getCtlSize(const wstring& text, IfButton ifButton, Follow follow)
{
	auto [width, height] = calcRect(text);

	if (ifButton == IfButton::button)
	{
		// 横向
		constexpr double btnOuterSizeScaleH = 1.6;
		// 垂直
		constexpr double btnOuterSizeScaleV = 1.8;

		width *= btnOuterSizeScaleH;
		height *= btnOuterSizeScaleV;
	}

	switch (follow)
	{
	case window::Follow::next:
		adjust(width, height);
		break;

	case window::Follow::beside:
	{
		const auto no_add_height{ -interval };
		adjust(width, no_add_height);
		break;
	}
	}

	return { width,height };
}

int window::WindowAdjuster::getMultipleLinesHeight(int n)
{
	auto [_, height] = calcRect(L"正");
	height *= n;
	return height;
}

int window::WindowAdjuster::outMaxWidth()
{
	return maxX;
}