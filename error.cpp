#include <windows.h>

module error;

import std;

void ShowError(const char* msg)
{
	MessageBox(NULL, msg, "´íÎó", MB_OK | MB_ICONERROR);
}