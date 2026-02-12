#include <Windows.h>;
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

import window;

void window::moreControlTurnOn()
{
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_STANDARD_CLASSES;
	InitCommonControlsEx(&icex);
}