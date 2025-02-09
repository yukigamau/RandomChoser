module;

#include <windows.h>
#include <commctrl.h>
#include <gdiplus.h>
#include "resource.h"
#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;

export module png;

export Gdiplus::Bitmap* pBitmap;	// 用于保存图标模式图片

// 从资源加载 PNG 图像
export Gdiplus::Bitmap* LoadPngFromResource(HINSTANCE hInstance, int resourceId)
{
	HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resourceId), "PNG");
	if (!hResource) return nullptr;

	DWORD imageSize = SizeofResource(hInstance, hResource);
	const void* pResourceData = LockResource(LoadResource(hInstance, hResource));
	if (!pResourceData) return nullptr;

	HGLOBAL hBuffer = GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if (!hBuffer) return nullptr;

	void* pBuffer = GlobalLock(hBuffer);
	if (!pBuffer)
	{
		GlobalFree(hBuffer);
		return nullptr;
	}

	CopyMemory(pBuffer, pResourceData, imageSize);

	IStream* pStream = nullptr;
	if (CreateStreamOnHGlobal(hBuffer, FALSE, &pStream) != S_OK)
	{
		GlobalUnlock(hBuffer);
		GlobalFree(hBuffer);
		return nullptr;
	}

	Gdiplus::Bitmap* pBitmap = new Gdiplus::Bitmap(pStream, FALSE);
	pStream->Release();
	GlobalUnlock(hBuffer);
	GlobalFree(hBuffer);

	if (pBitmap->GetLastStatus() != Gdiplus::Ok)
	{
		delete pBitmap;
		return nullptr;
	}

	return pBitmap;
}

export void pngDraw(HDC& hdc, HWND& hwnd)
{
	// 加载 PNG 图像
	if (!pBitmap)
		pBitmap = LoadPngFromResource(nullptr, IDB_PNG1);

	Gdiplus::Graphics graphics(hdc);

	// 获取窗口客户区大小
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	// 启用 GDI+ 抗锯齿模式
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);  // 启用抗锯齿
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);  // 高质量插值

	// 绘制缩放后的图像
	graphics.DrawImage(pBitmap, Gdiplus::Rect(-1, -1, clientRect.right, clientRect.bottom));
}