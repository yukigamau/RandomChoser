module;
#include <windows.h>
#include <commctrl.h>
#include <gdiplus.h>
#include "resource.h"
#pragma comment(lib,"gdiplus.lib")
export module png;
import value;
using namespace Gdiplus;

// ��������������Դ�м��� PNG ͼ��
Bitmap* LoadPNGFromResource(HINSTANCE hInstance, int resourceID)
{
	// ���� PNG ��Դ����Դ����Ϊ "PNG"
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

	// ����ȫ���ڴ汣����Դ����
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

	// ����һ���������ڴ����ݷ�װ������
	IStream* pStream = nullptr;
	if (CreateStreamOnHGlobal(hBuffer, TRUE, &pStream) != S_OK)
	{
		GlobalFree(hBuffer);
		return nullptr;
	}

	// �����д��� Bitmap ����
	Bitmap* pBitmap = Bitmap::FromStream(pStream);
	pStream->Release();

	return pBitmap;
}

// ���� PNG ͼ���ڷֲ�ͼ�괰����
export void iconPng(HWND hwnd)
{
	// ��ȡ��Ļ DC �������ڴ� DC
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

	// ʹ�� GDI+ ���ƣ�����Դ�м��� PNG ͼ�񲢻��Ƶ��ڴ� DC
	{
		Gdiplus::Graphics graphics(g_hdcMem);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		// ��ձ�����ȷ��͸��
		graphics.Clear(Gdiplus::Color(0, 0, 0, 0));

		// ������Դ IDB_PNG1 ��Ӧ�� PNG ͼ��
		HINSTANCE hInstance = GetModuleHandle(nullptr);
		Gdiplus::Bitmap* pPngBitmap = LoadPNGFromResource(hInstance, IDB_PNG1);
		if (pPngBitmap)
		{
			// ���� PNG ͼ�񵽴��ڣ���ͼ��ߴ��봰�ڲ�һ�£����Զ����죩
			graphics.DrawImage(pPngBitmap, 0, 0, g_sizeWnd.cx, g_sizeWnd.cy);
			delete pPngBitmap;
		}
	}

	// ��ʼ���·ֲ㴰����ʾ����
	BLENDFUNCTION blend = { 0 };
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = (BYTE)0;
	blend.AlphaFormat = AC_SRC_ALPHA;
	RECT rcClient;
	GetWindowRect(hwnd, &rcClient);
	POINT ptWnd = { rcClient.left, rcClient.top };
	POINT ptSrc = { 0, 0 };
	UpdateLayeredWindow(hwnd, hdcScreen, &ptWnd, &g_sizeWnd, g_hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

	// ������Դ
	SelectObject(g_hdcMem, hOldBitmap);
	DeleteObject(hBitmap);
	ReleaseDC(hwnd, hdcScreen);
}