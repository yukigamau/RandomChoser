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
	// ���� DPI ��֪
	HRESULT hr = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	// ���ϵͳ��֧�� SetProcessDpiAwareness�����˵��� API
	if (FAILED(hr))	BOOL success = SetProcessDPIAware();
	// ���� DPI ��������
	dpiScale = GetDPIScalingFactor() / 100.0;
	sizeByDPI();

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
	{
		choosePage(hInstance);
		readHistory();	// ��ȡ��ʷ��¼
	}

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