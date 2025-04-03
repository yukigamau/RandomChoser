#include <windows.h>
#include <gdiplus.h>
#include <ShellScalingAPI.h>
#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "Shcore.lib")
import command;
import data;
import value;

ULONG_PTR gdiplusToken;

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
	_In_ int nCmdshow)
{
	// 设置 DPI 感知
	HRESULT hr = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	// 如果系统不支持 SetProcessDpiAwareness，回退到旧 API
	if (FAILED(hr))	BOOL success = SetProcessDPIAware();
	// 设置 DPI 缩放因子
	dpiScale = GetDPIScalingFactor() / 100.0;
	sizeByDPI();

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
	{
		choosePage(hInstance);
		readHistory();	// 读取历史记录
	}

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