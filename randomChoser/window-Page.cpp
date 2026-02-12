#include <Windows.h>
#include "resource.h"
import std;
import window;
using std::wstring;

window::Page::Page(HINSTANCE hInstance)
	:hInstance{ hInstance }
{ }

void window::Page::createWindow(const wstring& className, const wstring& windowName, DWORD dwStyle,
	int x, int y, int width, int height)
{
	// 注册窗口类
	WNDCLASS wc = {};
	wc.hbrBackground = style.backgroundBrush();
	wc.lpfnWndProc = process; // 设置窗口过程函数
	wc.hInstance = GetModuleHandle(nullptr); // 获取实例句柄
	wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.lpszClassName = className.c_str();
	wc.style = CS_HREDRAW | CS_VREDRAW;

#if _DEBUG
	if (!RegisterClass(&wc))
	{
		DWORD err = GetLastError();
		if (err != ERROR_CLASS_ALREADY_EXISTS) // 可以忽略已存在
		{
			MessageBox(nullptr, L"窗口注册失败", L"Error", MB_ICONERROR);
			return;
		}
	}
#endif

	// 创建窗口
	hWnd = CreateWindow(
		className.c_str(), windowName.c_str(), dwStyle,
		// 等创建控件之后根据控件调整窗口大小和位置
		x, y, width, height,
		nullptr, nullptr,
		wc.hInstance,	// 实例句柄
		nullptr			// 附加数据
	);

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);	// 把窗口显示到最前面
	UpdateWindow(hWnd);
}

void window::Page::ini(HINSTANCE hInstance, Style style)
{
	this->hInstance = hInstance;
	this->style = style;
}