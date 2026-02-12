#include <Windows.h>;

import std;
import window;

using std::exception, std::string, std::wstring;
using window::moreControlTurnOn, window::Gdi, window::wps;

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdshow)
try
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

	return 0;
}
catch (const exception& e)
{
	string errorMessage = "发生了错误：\n";
	errorMessage += e.what();
	// exception只支持ANSI
	MessageBoxA(nullptr, errorMessage.c_str(), "错误X﹏X", MB_ICONERROR);
	return 1;
}
catch (...)
{
	MessageBox(nullptr, L"出现了未知的错误！", L"X﹏X", MB_ICONERROR);
	return 1;
}