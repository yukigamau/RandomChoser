#include <Windows.h>
#include <gdiplus.h>

#pragma comment(lib, "Gdiplus.lib")

import window;

using window::Gdi;

Gdi::Gdi()
{
	Gdiplus::GdiplusStartupInput gdiPlusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiPlusStartupInput, NULL);
}

Gdi::~Gdi()
{
	Gdiplus::GdiplusShutdown(gdiplusToken);
}