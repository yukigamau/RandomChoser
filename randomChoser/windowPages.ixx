module;

#include <Windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <ShellScalingAPI.h>

#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "Shcore.lib")

export module window:windowPages;

import "resource.h";

import dataread;
import std;

using dataread::data, dataread::ifDataExists;
using Gdiplus::Bitmap, Gdiplus::Color, Gdiplus::Graphics, Gdiplus::SmoothingModeAntiAlias;
using std::wstring;

export const wstring VERSION = L"2.0.0";

export namespace window
{
	constexpr int IDT_SCROLL = 1;
	constexpr int IDT_SCROLL_INTERVAL = 50;
	constexpr int IDT_WAIT = 2;
	constexpr int IDT_TRANSPARENCY = 3;
	constexpr int IDC_BTN_EDIT_LIST = 1501;
	constexpr int IDC_BTN_OPEN_SOURCE_SITE = 1502;
	constexpr int IDC_BTN_SETTING = 1503;
	constexpr int IDC_BTN_TEXT = 1504;
	constexpr int IDC_BTN_WRITE_LIST = 1505;
	constexpr int IDC_COMBO_LISTS = 1551;
	constexpr int IDC_STATIC_RED = 1601;

	void moreControlTurnOn();

	enum ColorStyle
	{
		light,
		dark
	};

	enum Mode
	{
		choose,
		icon
	};

	class Gdi
	{
	private:
		ULONG_PTR gdiplusToken;

	public:
		Gdi();
		~Gdi();
	};

	class Style
	{
		// 按钮的比例调整
	public:
		// 横向
		double btnOuterSizeScaleH = 1.6;
		// 垂直
		double btnOuterSizeScaleV = 1.8;
		double dpiScale = 1.0;

		// 颜色
	private:
		ColorStyle color = dark;	// 大部分电脑的默认设置应该
	public:
		int interval = 15;

	public:
		~Style();

	public:
		void dpi(double dpiScale);
		void getWindowStyle();
		void ini(DWORD styleValue);
	private:
		void iniFont();

	public:
		HBRUSH buttonBkBrush();
		HBRUSH textBkBrush();
		COLORREF textBkColor();
		COLORREF textColor();

		// 字体
	public:
		HFONT hFStatic = nullptr;
	};

	class Page
	{
	public:
		WNDPROC process;
		Style* style;
		HINSTANCE hInstance = nullptr;

	private:
		HWND hWnd = nullptr;

	public:
		Page() = default;
		Page(HINSTANCE hInstance);

	public:
		void createWindow(const wstring& className, const wstring& windowName, DWORD dwstyle,
			int x, int y, int width, int height);
		void ini(HINSTANCE hInstance, Style* style);
	};

	class WindowPages
	{
	public:
		// 自动关闭一些需要关闭的资源，比如GDI+
		~WindowPages();

	private:
		UINT getDPIScalingFactor();
		SIZE screenSize;
	private:
		HINSTANCE hInstance = nullptr;
	private:
		void iniDpi();
	public:
		void ini(HINSTANCE hInstance);
		void createWindow(HINSTANCE hInstance);

	public:
		Style style;
		// 新的页面规则
		Page listModify;

	private:
		int waitNum = 0;
		const int WAIT_NUM_MAX = 8;

	private:
		SIZE choosePageSize = { 200,90 };
		HWND hChoose = nullptr;
		HWND hTitleText = nullptr;
		HWND hSettingBtn = nullptr;
		HWND hCloseBtn = nullptr;
		HWND hTextBtn = nullptr;
		int captionHeight = GetSystemMetrics(SM_CYCAPTION);
		int hPenWidth = 2;
		HFONT hFont = nullptr;
		RECT titleRect;	// 在ini函数中赋值
		wstring titleText = L"点名器" + VERSION;
		RECT settingBtnRect;	// 在ini函数中赋值
		RECT closeBtnRect;	// 在ini函数中赋值
		/* 用于给抽取名字时滚动 */
		int scrollNumMax = 10;	// 滚动数字上限
		int scrollNum = scrollNumMax;	// 当前滚动名字剩余数

	private:
		void createChoosePage();
	public:	// 消息处理
		void chooseOnCommand(WPARAM wParam);
		void chooseOnCreate();
		LRESULT chooseOnCtlColorStatic(WPARAM wParam, LPARAM lParam);
		void chooseOnDestory();
		void chooseOnDrawItem(WPARAM wParam, LPARAM lParam);
		LRESULT chooseOnNcHitTest(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		void chooseOnPaint(HDC& hdc);
		void chooseOnTimer(WPARAM wParam);

	public:
		static LRESULT CALLBACK chooseWP(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


	public:
		void inIconPage();
		void outIconPage();

	private:
		SIZE iconPageSize = { 50,50 };
		HWND hIcon = nullptr;
		HDC hdcMem;
		// 用于处理窗口拖动逻辑
		POINT ptOld{};
		bool isDragging = false;	// 默认为不处于拖动状态
	private:
		bool ifLeft(RECT& originalRect) const;
		bool ifUp(RECT& originalRect) const;
		void offset(HWND& hwnd, RECT& newPosition, SIZE windowSize);
		void createIconPage();
	public:
		void iconOnLButtonDown(LPARAM lParam);
		void iconOnLButtonUp();
		void iconOnMouseMove(WPARAM wParam, LPARAM lParam);
		void iconOnPaint();
		void iconOnTimer(WPARAM wParam);
	private:
		static LRESULT CALLBACK iconWP(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		Bitmap* loadPhotoFromResource(HINSTANCE hInstance, int resourceID, wstring type);

		/* 设置窗口 */
	private:
		int settingFontHeight = 20;
		HWND hSetting;
	private:
		void createSettingPage();
	public:
		LRESULT settingOnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		void settingOnCreate(HWND hwnd);
		LRESULT settingOnCtlColorBtn(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT settingOnCtlColorStatic(WPARAM wParam, LPARAM lParam);
	private:
		static LRESULT CALLBACK settingWP(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		/* 透明变换动画 */
	private:
		bool changeAlpha = false;	// 标志更改透明度是增加还是减少，true为增加
		BYTE currentAlpha = 255; // 当前透明度，初始为完全不透明
	private:
		const int TRANSPARENCY_INTERVAL = 5;
		bool transparencyTimerActive = false;	// 透明度计时器是否处于活动状态，如果是是，需要在销毁窗口时杀掉
		const int transparencyChange = 15;	// 透明度变化
		void transparency(HWND hwnd, Mode m);
	} wps;
}

// 自动关闭一些需要关闭的资源，比如GDI+
window::WindowPages::~WindowPages()
{
	// 关闭可能的计时器
	KillTimer(hChoose, IDT_SCROLL);			// 可能还在抽取就关了
	KillTimer(hChoose, IDT_TRANSPARENCY);	// 可能还在透明度变幻关了
	KillTimer(hChoose, IDT_WAIT);			// 一定存在于使用抽取窗口时
}

UINT window::WindowPages::getDPIScalingFactor()
{
	HDC hdc = GetDC(NULL);
	UINT dpi = GetDeviceCaps(hdc, LOGPIXELSX); // 96 DPI 为 100% 缩放
	ReleaseDC(NULL, hdc);
	return MulDiv(dpi, 100, 96); // 返回百分比（如 200 表示 200%）
}

void window::WindowPages::iniDpi()
{
	// 设置 DPI 感知
	HRESULT hr = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	// 如果系统不支持 SetProcessDpiAwareness，回退到旧 API
	if (FAILED(hr))	BOOL success = SetProcessDPIAware();

	// DPI调整
	const double defaultDPI = 96.0;
	double dpiScale = getDPIScalingFactor() / defaultDPI;

	choosePageSize.cx *= dpiScale;
	choosePageSize.cy *= dpiScale;

	iconPageSize.cx *= dpiScale;
	iconPageSize.cy *= dpiScale;

	hPenWidth *= dpiScale;

	int btnSize = 50 * dpiScale;

	titleRect = { 0,0,choosePageSize.cx,captionHeight };
	titleRect.left += 10 * dpiScale;	// 给文字留点空间
	titleRect.right -= 2 * btnSize;	// 避开关闭、设置按钮

	settingBtnRect = { choosePageSize.cx - 2 * btnSize,0,choosePageSize.cx - btnSize,captionHeight };
	closeBtnRect = { choosePageSize.cx - btnSize,0,choosePageSize.cx,captionHeight };

	settingFontHeight *= dpiScale;

	// 屏幕
	screenSize.cx = GetSystemMetrics(SM_CXSCREEN);
	screenSize.cy = GetSystemMetrics(SM_CYSCREEN);

	style.dpi(dpiScale);
}

void window::WindowPages::chooseOnCommand(WPARAM wParam)
{
	int id = LOWORD(wParam);
	int code = HIWORD(wParam);

	switch (id)
	{
	case IDC_BTN_TEXT:
		switch(code)
		{
		case BN_CLICKED:
			if (scrollNum != scrollNumMax)
				break;	// 防止重复

			// 启动计时器，开始滚动
			SetTimer(hChoose, IDT_SCROLL, IDT_SCROLL_INTERVAL, nullptr);
			break;
		}
		break;
	}
}

void window::WindowPages::chooseOnCreate()
{
	// 每秒发一次消息
	SetTimer(hChoose, IDT_WAIT, 1000, nullptr);	// 等待一定时间切换至图标模式用

	/* 标题 */
	wstring titleText = L"点名器" + VERSION;
	hTitleText = CreateWindow(
		L"STATIC",
		titleText.c_str(),
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		titleRect.left, titleRect.top,
		titleRect.right - titleRect.left, titleRect.bottom - titleRect.top,
		hChoose,
		nullptr,
		hInstance,
		nullptr
	);

	/* 设置按钮 */
	hSettingBtn = CreateWindow(
		L"BUTTON",
		L"…",
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		settingBtnRect.left, settingBtnRect.top,
		settingBtnRect.right - settingBtnRect.left, settingBtnRect.bottom - settingBtnRect.top,
		hChoose,
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
		hChoose,
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
		hChoose,
		(HMENU)IDC_BTN_TEXT,
		hInstance,
		nullptr
	);
}

LRESULT window::WindowPages::chooseOnCtlColorStatic(WPARAM wParam, LPARAM lParam)
{
	HWND hParam = (HWND)lParam;
	if (hParam == hTitleText)
	{
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, data.captionFC);
		SetBkMode(hdcStatic, OPAQUE);
		return (LRESULT)(HBRUSH)CreateSolidBrush(data.captionBC);
	}
	else
		return 0;	// 这个是默认的windows api返回值
}

void window::WindowPages::chooseOnDestory()
{
	if (transparencyTimerActive)
		KillTimer(nullptr, IDT_TRANSPARENCY);  // 完全透明后停止定时器

	if(hFont)
		DeleteObject(hFont);

	// 帮图标窗口清理资源
	if (hdcMem)
		DeleteDC(hdcMem);

	PostQuitMessage(0);
}

LRESULT window::WindowPages::chooseOnNcHitTest(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT hit = DefWindowProc(hwnd, uMsg, wParam, lParam);

	if (hit != HTCLIENT)
		return hit;

	// 拦截并判断是否需要设定为返回标题栏可拖动
	POINT pt
	{
		GET_X_LPARAM(lParam),
		GET_Y_LPARAM(lParam)
	};

	ScreenToClient(hwnd, &pt);

	RECT rc{};
	GetWindowRect(hTitleText, &rc);
	ScreenToClient(hwnd, (POINT*)&rc);
	ScreenToClient(hwnd, (POINT*)&rc + 1);

	if (PtInRect(&rc, pt))
		return HTCAPTION;
	else
		return hit;
}

void window::WindowPages::chooseOnPaint(HDC& hdc)
{
	/* 用户区背景 */
	// 创建画笔和画刷
	HBRUSH hBrushBk = CreateSolidBrush(data.clientBC);
	SelectObject(hdc, hBrushBk);
	// 绘制底色矩形
	Rectangle(hdc, 0, captionHeight - 1, choosePageSize.cx, choosePageSize.cy);
	// 删除 GDI 对象
	DeleteObject(hBrushBk);

	/* 标题栏 */
	// 创建画笔和画刷
	HPEN hPen = CreatePen(PS_SOLID, hPenWidth, data.captionBC); // 边框画笔
	HBRUSH hBrush = CreateSolidBrush(data.captionBC);   // 背景画刷
	SelectObject(hdc, hPen);
	SelectObject(hdc, hBrush);
	// 绘制标题栏矩形
	Rectangle(hdc, 0, 0, choosePageSize.cx, captionHeight);
	// 删除 GDI 对象
	DeleteObject(hBrush);
	DeleteObject(hPen);
}

void window::WindowPages::chooseOnTimer(WPARAM wParam)
{
	switch (wParam)
	{
	case IDT_SCROLL:
		scrollNum--;
		LPCWSTR nameOut;
		if (scrollNum)	// 随机滚动没有结束
			nameOut = data.nameRandom().c_str();
		else
		{
			nameOut = data.nameOut().c_str();

			// 终止文本滚动
			KillTimer(hChoose, IDT_SCROLL);
		}

		// 设置文本
		SetWindowText(hTextBtn, nameOut);
		break;

	case IDT_TRANSPARENCY:
		transparency(hChoose, choose);
		break;

	case IDT_WAIT:
		waitNum++;
		if (waitNum != WAIT_NUM_MAX)
			break;

		waitNum = 0;	// 清零waitNum
		SetTimer(hChoose, IDT_TRANSPARENCY, TRANSPARENCY_INTERVAL, nullptr);	// 用于设置透明度修改时间
		transparencyTimerActive = true;
		break;
	}
}

LRESULT CALLBACK window::WindowPages::chooseWP(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		wps.chooseOnCreate();
		break;

	case WM_COMMAND:
		wps.chooseOnCommand(wParam);
		break;

	case WM_CTLCOLORSTATIC:
		return wps.chooseOnCtlColorStatic(wParam, lParam);

	case WM_DESTROY:
		wps.chooseOnDestory();
		break;

	case WM_DRAWITEM:
		wps.chooseOnDrawItem(wParam, lParam);
		return true;
		break;

	case WM_NCHITTEST:
		return wps.chooseOnNcHitTest(hwnd, uMsg, wParam, lParam);

	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hwnd, &ps);
		wps.chooseOnPaint(hdc);
		EndPaint(hwnd, &ps);
		break;

	case WM_TIMER:
		wps.chooseOnTimer(wParam);
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

void window::WindowPages::createChoosePage()
{
	if (hChoose == NULL)// 创建窗口
	{
		// 注册窗口类
		const wchar_t* className = L"抽取页面";
		WNDCLASS wc = {};
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpfnWndProc = chooseWP;	// 设置窗口过程函数
		wc.hInstance = GetModuleHandle(nullptr);	// 获取实例句柄
		wc.hIcon = LoadIcon(hInstance, (wchar_t*)IDI_ICON1);
		wc.lpszClassName = className;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClass(&wc);

		LPCWSTR lpWindowName = L"点名器";
		hChoose = CreateWindowEx(
			WS_EX_LAYERED | WS_EX_TOPMOST,
			className, lpWindowName,
			WS_POPUP,
			(screenSize.cx - choosePageSize.cx) / 2,
			(screenSize.cy - choosePageSize.cy) / 2,   // 窗口位置
			choosePageSize.cx, choosePageSize.cy,                // 窗口大小
			nullptr,                        // 父窗口句柄
			nullptr,                        // 菜单句柄
			wc.hInstance,                   // 实例句柄
			nullptr                         // 附加数据
		);
		SetLayeredWindowAttributes(hChoose, 0, 255, LWA_ALPHA); // 设置窗口为支持透明

		if (!data.ifTop)	// 置顶设置处理
			SetWindowPos(hChoose, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	ShowWindow(hChoose, SW_SHOW); // 显示窗口
	UpdateWindow(hChoose);
}

void window::WindowPages::inIconPage()
{
	// 加载窗口
	if (!hIcon)
		createIconPage();

	ShowWindow(hChoose, SW_HIDE);
	ShowWindow(hIcon, SW_SHOW);
}

void window::WindowPages::outIconPage()
{
	ShowWindow(hIcon, SW_HIDE);
	ShowWindow(hChoose, SW_SHOW);
}

// 检查窗口的位置偏好
bool window::WindowPages::ifLeft(RECT& originalRect) const
{
	// 检查窗口是否偏右
	return (originalRect.right + originalRect.left) / 2 < screenSize.cx / 2;
}

bool window::WindowPages::ifUp(RECT& originalRect) const
{
	// 检查窗口是否偏上
	return (originalRect.bottom + originalRect.top) / 2 > screenSize.cy / 2;
}

void window::WindowPages::offset(HWND& hwnd, RECT& newPosition, SIZE windowSize)
{
	// 1. 获取原始窗口的大小和位置
	RECT originalRect;
	GetWindowRect(hwnd, &originalRect);

	// 2. 修改位置参数
	// 水平
	if (ifLeft(originalRect))
	{
		if (originalRect.left < 0)
			newPosition.left = 0;
		else
			newPosition.left = originalRect.left;
	}
	else
	{
		if (originalRect.right > screenSize.cx)
			newPosition.left = screenSize.cx - windowSize.cx;
		else
			newPosition.left = originalRect.right - windowSize.cx;
	}
	// 垂直
	if (ifUp(originalRect))
	{
		if (originalRect.top < 0)
			newPosition.top = 0;
		else
			newPosition.top = originalRect.top;
	}
	else
	{
		if (originalRect.bottom > screenSize.cy)
			newPosition.top = screenSize.cy - windowSize.cy;
		else
			newPosition.top = originalRect.bottom - windowSize.cy;
	}
}

void window::WindowPages::createIconPage()
{
	RECT newPosition;
	offset(hChoose, newPosition, iconPageSize);

	const wchar_t* iconWndName = L"icon";
	// 创建窗口
	// 注册新窗口类型
	WNDCLASS iconWc = {};
	iconWc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	iconWc.hInstance = GetModuleHandle(nullptr); // 获取实例句柄
	iconWc.lpfnWndProc = iconWP;
	iconWc.lpszClassName = iconWndName;
	iconWc.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&iconWc);

	// 设置新窗口
	hIcon = CreateWindowEx(
		WS_EX_LAYERED,
		iconWndName,	// 窗口类名
		L"点名器图标模式",		// 窗口标题
		WS_POPUP,	// 窗口风格
		newPosition.left, newPosition.top,	// 窗口位置
		iconPageSize.cx, iconPageSize.cy,			// 窗口大小
		nullptr,			// 父窗口句柄
		nullptr,			// 菜单句柄
		iconWc.hInstance,	// 实例句柄
		nullptr				// 附加数据
	);

	// 显示窗口
	ShowWindow(hChoose, SW_HIDE);
	ShowWindow(hIcon, SW_SHOWNOACTIVATE);
	UpdateWindow(hIcon);

	// 关闭抽取窗口计时
	KillTimer(hChoose, IDT_WAIT);

	// 调整位置
	HWND set = data.ifTop ? HWND_TOPMOST : HWND_NOTOPMOST;
	SetWindowPos(hIcon, set, newPosition.left, newPosition.top, 0, 0, SWP_NOSIZE);
	iconOnPaint();
}

// 辅助函数：从资源中加载图像
Bitmap* window::WindowPages::loadPhotoFromResource(HINSTANCE hInstance, int resourceID, wstring type)
{
	// 查找 PNG 资源，资源类型为 "PNG"
	HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resourceID), type.c_str());
	if (!hResource)
		return nullptr;

	DWORD imageSize = SizeofResource(hInstance, hResource);
	if (imageSize == 0)
		return nullptr;

	HGLOBAL hResData = LoadResource(hInstance, hResource);
	if (!hResData)
		return nullptr;

	void* pResourceData = LockResource(hResData);
	if (!pResourceData)
		return nullptr;

	// 分配全局内存保存资源数据
	HGLOBAL hBuffer = GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if (!hBuffer)
		return nullptr;

	void* pBuffer = GlobalLock(hBuffer);
	if (!pBuffer)
	{
		GlobalFree(hBuffer);
		return nullptr;
	}

	memcpy(pBuffer, pResourceData, imageSize);
	GlobalUnlock(hBuffer);

	// 创建一个流，将内存数据封装到流中
	IStream* pStream = nullptr;
	if (CreateStreamOnHGlobal(hBuffer, TRUE, &pStream) != S_OK)
	{
		GlobalFree(hBuffer);
		return nullptr;
	}

	// 从流中创建 Bitmap 对象
	Bitmap* pBitmap = Bitmap::FromStream(pStream);
	pStream->Release();

	return pBitmap;
}

void window::WindowPages::iconOnLButtonDown(LPARAM lParam)
{
	ptOld.x = GET_X_LPARAM(lParam);
	ptOld.y = GET_Y_LPARAM(lParam);
	// 转换为屏幕坐标（因为窗口可能被移动，局部坐标不足以处理）
	ClientToScreen(hIcon, &ptOld);
	isDragging = false;	// 重置isDragging
	// 捕获鼠标事件，确保拖动不会因鼠标移出窗口而中断
	SetCapture(hIcon);
}

void window::WindowPages::iconOnLButtonUp()
{
	if (!isDragging)
	{
		SetTimer(hIcon, IDT_TRANSPARENCY, TRANSPARENCY_INTERVAL, nullptr);	// 动态回复抽取窗口
		transparencyTimerActive = true;
	}
	ReleaseCapture();
}

void window::WindowPages::iconOnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (!(wParam & MK_LBUTTON))
		return;	// 不是处于左键按下的状态

	// 如果鼠标左键按下并且发生了移动，则开始拖动
	POINT ptNew;
	ptNew.x = GET_X_LPARAM(lParam);
	ptNew.y = GET_Y_LPARAM(lParam);
	ClientToScreen(hIcon, &ptNew);
	// 判断鼠标是否移动了，如果移动了，则认为是拖动
	int dx = ptNew.x - ptOld.x;
	int dy = ptNew.y - ptOld.y;

	if (abs(dx) > 5 || abs(dy) > 5)
	{  // 如果移动的距离大于阈值，认为是拖动
		isDragging = true;
	}

	if (!isDragging)
		return;

	// 移动窗口
	RECT rect;
	GetWindowRect(hIcon, &rect);
	SetWindowPos(hIcon, NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	ptOld = ptNew;  // 更新鼠标位置
}

void window::WindowPages::iconOnPaint()
{
	// 获取屏幕 DC 及创建内存 DC
	HDC hdcScreen = GetDC(hIcon);
	hdcMem = CreateCompatibleDC(hdcScreen);

	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = iconPageSize.cx;
	bmi.bmiHeader.biHeight = -iconPageSize.cy; // top-down DIB
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	void* pvBits = nullptr;
	HBITMAP hBitmap = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

	// 使用 GDI+ 绘制：从资源中加载 PNG 图像并绘制到内存 DC
	{
		Graphics graphics(hdcMem);
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		// 清空背景，确保透明
		graphics.Clear(Color(0, 0, 0, 0));

		// 加载资源 IDB_PNG 对应的 PNG 图像
		HINSTANCE hInstance = GetModuleHandle(nullptr);
		Bitmap* pPngBitmap = loadPhotoFromResource(hInstance, IDB_PNG1, L"png");
		if (pPngBitmap)
		{
			// 绘制 PNG 图像到窗口（若图像尺寸与窗口不一致，可自动拉伸）
			graphics.DrawImage(pPngBitmap, 0, 0, iconPageSize.cx, iconPageSize.cy);
			delete pPngBitmap;
		}
	}

	// 初始更新分层窗口显示内容
	BLENDFUNCTION blend = { 0 };
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = (BYTE)0;
	blend.AlphaFormat = AC_SRC_ALPHA;
	RECT rcClient;
	GetWindowRect(hIcon, &rcClient);
	POINT ptWnd = { rcClient.left, rcClient.top };
	POINT ptSrc = { 0, 0 };
	UpdateLayeredWindow(hIcon, hdcScreen, &ptWnd, &iconPageSize, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

	// 清理资源
	SelectObject(hdcMem, hOldBitmap);	// 将位图从内存DC中清出
	DeleteObject(hBitmap);
	ReleaseDC(hIcon, hdcScreen);
}

void window::WindowPages::iconOnTimer(WPARAM wParam)
{
	switch (wParam)
	{
	case IDT_TRANSPARENCY:
		transparency(hIcon, icon);
		break;
	}
}

LRESULT CALLBACK window::WindowPages::iconWP(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_LBUTTONDOWN:
		wps.iconOnLButtonDown(lParam);
		break;

	case WM_LBUTTONUP:
		wps.iconOnLButtonUp();
		break;

	case WM_TIMER:
		wps.iconOnTimer(wParam);
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

void window::WindowPages::createWindow(HINSTANCE hInstance)
{
	this->hInstance = hInstance;
	if (ifDataExists())
		createChoosePage();
	else
		createSettingPage();
}

void window::WindowPages::transparency(HWND hwnd, Mode m)
{
	if (changeAlpha)	// 增加透明度
	{
		if (currentAlpha + transparencyChange >= 255)
		{
			KillTimer(hwnd, IDT_TRANSPARENCY);
			changeAlpha = false;
			transparencyTimerActive = false;
		}
		currentAlpha += transparencyChange;
	}
	else // 减少透明度
	{
		if (currentAlpha - transparencyChange <= transparencyChange)
		{
			KillTimer(hwnd, IDT_TRANSPARENCY);
			changeAlpha = true;
			if (m == choose)
			{
				m = icon;	// 切换模式标记为图标
				inIconPage();
				SetTimer(hIcon, IDT_TRANSPARENCY, TRANSPARENCY_INTERVAL, nullptr);
			}
			else if (m == icon)
			{
				m = choose;	// 切换模式标记为抽取
				outIconPage();
				SetTimer(hChoose, IDT_TRANSPARENCY, TRANSPARENCY_INTERVAL, nullptr);
				SetTimer(hChoose, IDT_SCROLL, IDT_SCROLL_INTERVAL, nullptr);	// 同时开始滚动以优化使用
			}
		}
		currentAlpha -= transparencyChange;  // 每次减少透明度
	}

	// 更新透明度
	if (m == choose)
		SetLayeredWindowAttributes(hwnd, 0, currentAlpha, LWA_ALPHA);
	else if (m == icon)
	{
		BLENDFUNCTION blend = { 0 };
		blend.BlendOp = AC_SRC_OVER;
		blend.SourceConstantAlpha = (BYTE)currentAlpha;
		blend.AlphaFormat = AC_SRC_ALPHA;
		UpdateLayeredWindow(
			hwnd,			// 窗口句柄
			NULL,			// 目标 HDC（设为 NULL）
			NULL,			// pptDst = NULL（位置不变）
			NULL,			// psize = NULL（尺寸不变）
			hdcMem,			// 源 HDC（含带透明度的图像）
			NULL,			// pptSrc = (0,0)
			RGB(0, 0, 0),	// 颜色键（未使用）
			&blend,			// 混合函数（启用 Alpha）
			ULW_ALPHA		// 使用 Alpha 混合
		);
	}
}