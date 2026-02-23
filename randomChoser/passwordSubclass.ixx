module;
#include <Windows.h>
export module passwordSubclass;

export namespace subclass
{
	LRESULT CALLBACK subclassEPassword(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	LRESULT CALLBACK subclassEPasswordRe(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
}