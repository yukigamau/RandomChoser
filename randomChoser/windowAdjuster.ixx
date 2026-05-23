module;

#include <Windows.h>

export module window:windowAdjuster;
import command;
import std;
import glob;
using command::Toggle;
using std::tuple, std::wstring;
using std::max, std::tie;

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
		const int left = 0;
		POINT pt;
		int interval = 0;

		// 这两个本来是用来兼容之前的pt的改动的，结果发现这样更好用
		const LONG& x = pt.x;
		const LONG& y = pt.y;

	private:
		int maxX = 0;
		int maxY = 0;

	private:
		HWND hWnd = nullptr;
		HFONT hFont = nullptr;

	public:
		[[deprecated("这个函数中可能会出现interval与xBegin和yBegin的位置问题")]]
		WindowAdjuster(HWND hWnd, HFONT hFont, int interval, int xBegin, int yBegin);
		WindowAdjuster(HWND hWnd, HFONT hFont, int interval, POINT pt);

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
		tuple<int, int> getCtlSize(Toggle& tg);
		int getMultipleLinesHeight(int n);

		// 获取maxX值
		int outMaxWidth();
	};
}

window::WindowAdjuster::WindowAdjuster(HWND hWnd, HFONT hFont, int interval, int xBegin, int yBegin)
	:hWnd{ hWnd }, hFont{ hFont }, interval{ interval }, left{ xBegin }
{
	pt.x = xBegin;
	pt.y = yBegin;
}
window::WindowAdjuster::WindowAdjuster(HWND hWnd, HFONT hFont, int interval, POINT pt)
	:hWnd{ hWnd }, hFont{ hFont }, interval{ interval }, pt{ pt }, left{ pt.x }
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
	pt.x += width + interval;
}

void window::WindowAdjuster::ctlLeft(int xLeft)
{
	pt.x = xLeft;
}

void window::WindowAdjuster::ctlNext(int height)
{
	pt.y += height + interval;
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
		width *= glob::btnOuterSizeScaleH;
		height *= glob::btnOuterSizeScaleV;
	}

	if (follow == window::Follow::next)
		adjust(width, height);
	else
	{
		// 这里是因为每次adjust的时候，会出现interval的纵向增加，需要抵消掉。
		const auto no_add_height{ -interval };
		adjust(width, no_add_height);
	}

	return { width,height };
}
tuple<int, int> window::WindowAdjuster::getCtlSize(Toggle& tg)
{
	auto allWidth{ 0 }, allHeight{ 0 };

	// 标识语
	auto [width, height] = calcRect(tg.text);
	allWidth += width;
	allHeight += height;	// 只有一行
	
	// 间隔
	allWidth += interval;

	// 第一个选项
	tie(width, height) = calcRect(tg.first);
	allWidth += width;

	// 间隔
	allWidth += interval;
	
	// 第二个选项
	tie(width, height) = calcRect(tg.second);
	allWidth += width;

	adjust(allWidth, allHeight);
	
	return { allWidth, allHeight };
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