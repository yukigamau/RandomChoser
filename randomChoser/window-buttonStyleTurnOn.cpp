#include <Windows.h>;
#include <commctrl.h>

import window;

void window::buttonStyleTurnOn()
{
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_STANDARD_CLASSES;
	InitCommonControlsEx(&icex);
}