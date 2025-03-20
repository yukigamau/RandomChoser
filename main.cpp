#include <windows.h>
#include <gdiplus.h>
import command;
import data;
#pragma comment(lib, "Gdiplus.lib")

ULONG_PTR gdiplusToken;

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
	_In_ int nCmdshow)
{
	// gdi+初始化
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiPlusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiPlusStartupInput, NULL);

	// 数据初始化
	if (!initializeData())
	{
		store.ifRight = false;
		MessageBox(nullptr, "找不到数据了诶。那我们一起重新设置一遍吧！", "要说嗯", MB_ICONQUESTION);
		settingPage(hInstance);
	}
	else
		choosePage(hInstance);

	// 消息循环
	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);

	return 0;
}