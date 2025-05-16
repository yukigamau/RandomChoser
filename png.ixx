module;
#include <windows.h>
#include <gdiplus.h>
#include <commctrl.h>
#include <shlwapi.h>
#include "resource.h"
#pragma comment(lib,"gdiplus.lib")
#pragma comment(lib, "shlwapi.lib")
export module png;
import data;
import std;
import value;
using namespace Gdiplus;

// 辅助函数：从资源中加载图像
Bitmap* loadPhotoFromResource(HINSTANCE hInstance, int resourceID, std::string type)
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

		// 加载资源 IDB_PNG 对应的 PNG 图像
		HINSTANCE hInstance = GetModuleHandle(nullptr);
		Gdiplus::Bitmap* pPngBitmap = loadPhotoFromResource(hInstance, IDB_PNG1, "png");
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

//Image* g_pImage;
//bool LoadMyImage() {
//	if (PathFileExistsA("png.png")) {
//		WCHAR wszPath[MAX_PATH];
//		MultiByteToWideChar(CP_ACP, 0, "png.png", -1, wszPath, MAX_PATH);
//		g_pImage = Image::FromFile(wszPath);
//		return g_pImage && g_pImage->GetLastStatus() == Ok;
//	}
//	else {
//		HINSTANCE hInst = GetModuleHandle(nullptr);
//		HRSRC hRes = FindResourceA(hInst, MAKEINTRESOURCEA(IDB_PNG2), "PNG");
//		if (!hRes) return false;
//		HGLOBAL hMem = LoadResource(hInst, hRes);
//		DWORD size = SizeofResource(hInst, hRes);
//		void* pData = LockResource(hMem);
//		IStream* pStream = nullptr;
//		CreateStreamOnHGlobal(nullptr, TRUE, &pStream);
//		ULONG written;
//		pStream->Write(pData, size, &written);
//		LARGE_INTEGER li = { 0 };
//		pStream->Seek(li, STREAM_SEEK_SET, nullptr);
//		g_pImage = Image::FromStream(pStream);
//		pStream->Release();
//		return g_pImage && g_pImage->GetLastStatus() == Ok;
//	}
//}

//export void photoTip(HWND hwnd)
//{
//	LoadMyImage();
//	if (!g_pImage) return;
//
//	int winW, winH;
//	RECT rc;
//	GetClientRect(hwnd, &rc);
//	winW = rc.right - rc.left;
//	winH = rc.bottom - rc.top;
//
//	int imgW = g_pImage->GetWidth();
//	int imgH = g_pImage->GetHeight();
//
//	double scale = min((double)winW / imgW, (double)winH / imgH);
//	int drawW = (int)(imgW * scale);
//	int drawH = (int)(imgH * scale);
//	int offsetX = (winW - drawW) / 2;
//	int offsetY = (winH - drawH) / 2;
//
//	HDC hdcScreen = GetDC(hwnd);
//	HDC hdcMem = CreateCompatibleDC(hdcScreen);
//
//	BITMAPINFO bmi = { 0 };
//	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//	bmi.bmiHeader.biWidth = winW;
//	bmi.bmiHeader.biHeight = -winH;  // top-down
//	bmi.bmiHeader.biPlanes = 1;
//	bmi.bmiHeader.biBitCount = 32;
//	bmi.bmiHeader.biCompression = BI_RGB;
//
//	void* pvBits = nullptr;
//	HBITMAP hBitmap = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pvBits, nullptr, 0);
//	HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hBitmap);
//
//	Gdiplus::Graphics graphics(hdcMem);
//	graphics.SetSmoothingMode(SmoothingModeHighQuality);
//	graphics.Clear(Color(0, 0, 0, 0));
//	graphics.DrawImage(g_pImage, offsetX, offsetY, drawW, drawH);
//
//	// 获取字体名称（ANSI → WCHAR）
//	WCHAR wszFont[LF_FACESIZE] = { 0 };
//	MultiByteToWideChar(CP_ACP, 0, data.fontName.c_str(), -1, wszFont, LF_FACESIZE);
//
//	// 创建字体
//	Gdiplus::Font font(wszFont, 72 * dpiScale, FontStyleRegular, UnitPixel);
//	Gdiplus::StringFormat format;
//	format.SetAlignment(StringAlignmentCenter);
//	format.SetLineAlignment(StringAlignmentCenter);
//
//	// 要显示的文本
//	const WCHAR* text = L"感谢使用";
//
//	// 测量文字尺寸
//	Gdiplus::RectF measuredSize;
//	graphics.MeasureString(text, -1, &font, Gdiplus::PointF(0, 0), &measuredSize);
//
//	// 卡片背景尺寸与位置（加 padding）
//	int paddingX = 40 * dpiScale;
//	int paddingY = 20 * dpiScale;
//	REAL bgWidth = measuredSize.Width + paddingX;
//	REAL bgHeight = measuredSize.Height + paddingY;
//	REAL bgX = (winW - bgWidth) / 2.0f;
//	REAL bgY = (winH - bgHeight) / 2.0f; // 垂直居中
//
//	// 圆角卡片路径
//	Gdiplus::GraphicsPath path;
//	REAL radius = 25.0f * dpiScale;
//	path.AddArc(bgX, bgY, radius, radius, 180, 90);
//	path.AddArc(bgX + bgWidth - radius, bgY, radius, radius, 270, 90);
//	path.AddArc(bgX + bgWidth - radius, bgY + bgHeight - radius, radius, radius, 0, 90);
//	path.AddArc(bgX, bgY + bgHeight - radius, radius, radius, 90, 90);
//	path.CloseFigure();
//
//	// 卡片背景（半透明深色）
//	Gdiplus::SolidBrush bgBrush(Gdiplus::Color(180, 30, 30, 30));
//	graphics.FillPath(&bgBrush, &path);
//
//	// 文本区域（使用整个窗口，但垂直居中，而且要使用偏移）
//	Gdiplus::RectF textRect(bgX, bgY + 10 * dpiScale, bgWidth, bgHeight);
//
//	// 文字画刷
//	Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 255, 255));
//	graphics.DrawString(text, -1, &font, textRect, &format, &whiteBrush);
//	SIZE sizeWin = { winW, winH };
//	POINT ptSrc = { 0, 0 };
//
//	BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
//	UpdateLayeredWindow(hwnd, hdcScreen, nullptr, &sizeWin, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
//
//	SelectObject(hdcMem, hOldBmp);
//	DeleteObject(hBitmap);
//	DeleteDC(hdcMem);
//	ReleaseDC(nullptr, hdcScreen);
//}