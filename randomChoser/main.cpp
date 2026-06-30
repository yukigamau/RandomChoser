#include <Windows.h>;

import std;
import window;

using std::exception, std::string, std::wstring;
using window::moreControlTurnOn, window::Gdi, window::wps;

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdshow)
{
	moreControlTurnOn();
	
	// 启用GDI+
	Gdi gdi;

	wps.ini(hInstance);
	wps.createWindow(hInstance);

	// 消息循环
	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}