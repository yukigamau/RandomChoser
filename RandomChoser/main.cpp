#include <windows.h>
#include <gdiplus.h>
import command;
import data;
#pragma comment(lib, "Gdiplus.lib")

ULONG_PTR gdiplusToken;

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
	_In_ int nCmdshow)
{
	// gdi+��ʼ��
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiPlusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiPlusStartupInput, NULL);

	// ���ݳ�ʼ��
	if (!initializeData())
	{
		store.ifRight = false;
		MessageBox(nullptr, "�Ҳ�������������������һ����������һ��ɣ�", "Ҫ˵��", MB_ICONQUESTION);
		settingPage(hInstance);
	}
	else
		choosePage(hInstance);

	// ��Ϣѭ��
	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);

	return 0;
}