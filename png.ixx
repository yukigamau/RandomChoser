module;
#include <windows.h>
#include <commctrl.h>
#include <gdiplus.h>
#include "resource.h"
#pragma comment(lib,"gdiplus.lib")
export module png;
import value;
using namespace Gdiplus;

// 辅助函数：从资源中加载 PNG 图像
Bitmap* LoadPNGFromResource(HINSTANCE hInstance, int resourceID)
{
	// 查找 PNG 资源，资源类型为 "PNG"
	HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resourceID), "PNG");
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

// 绘制 PNG 图像在分层图标窗口上
export void iconPng(HWND hwnd)
{
	// 获取屏幕 DC 及创建内存 DC
	HDC hdcScreen = GetDC(hwnd);
	g_hdcMem = CreateCompatibleDC(hdcScreen);

	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = g_sizeWnd.cx;
	bmi.bmiHeader.biHeight = -g_sizeWnd.cy; // top-down DIB
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	void* pvBits = nullptr;
	HBITMAP hBitmap = CreateDIBSection(g_hdcMem, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(g_hdcMem, hBitmap);

	// 使用 GDI+ 绘制：从资源中加载 PNG 图像并绘制到内存 DC
	{
		Gdiplus::Graphics graphics(g_hdcMem);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		// 清空背景，确保透明
		graphics.Clear(Gdiplus::Color(0, 0, 0, 0));

		// 加载资源 IDB_PNG1 对应的 PNG 图像
		HINSTANCE hInstance = GetModuleHandle(nullptr);
		Gdiplus::Bitmap* pPngBitmap = LoadPNGFromResource(hInstance, IDB_PNG1);
		if (pPngBitmap)
		{
			// 绘制 PNG 图像到窗口（若图像尺寸与窗口不一致，可自动拉伸）
			graphics.DrawImage(pPngBitmap, 0, 0, g_sizeWnd.cx, g_sizeWnd.cy);
			delete pPngBitmap;
		}
	}

	// 初始更新分层窗口显示内容
	BLENDFUNCTION blend = { 0 };
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = (BYTE)0;
	blend.AlphaFormat = AC_SRC_ALPHA;
	RECT rcClient;
	GetWindowRect(hwnd, &rcClient);
	POINT ptWnd = { rcClient.left, rcClient.top };
	POINT ptSrc = { 0, 0 };
	UpdateLayeredWindow(hwnd, hdcScreen, &ptWnd, &g_sizeWnd, g_hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

	// 清理资源
	SelectObject(g_hdcMem, hOldBitmap);
	DeleteObject(hBitmap);
	ReleaseDC(hwnd, hdcScreen);
}