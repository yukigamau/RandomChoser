module;
#include <Windows.h>
export module listModifySubclass;
import std;
using std::function;

export namespace subclass
{
	LRESULT CALLBACK subclassEListName(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	LRESULT CALLBACK subclassEWriteNames(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
}