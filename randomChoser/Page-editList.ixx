module;
#include <Windows.h>
export module window:editList;
import editListWPFun;
import std;

export namespace window
{
	LRESULT CALLBACK editListWP(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}

LRESULT CALLBACK window::editListWP(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:			return editListOnCreate(hWnd, uMsg, wParam, lParam);
	case WM_CTLCOLORBTN:	return editListOnCtlColorButton(hWnd, uMsg, wParam, lParam);
	case WM_CTLCOLOREDIT:	return editListOnCtlColorEdit(hWnd, uMsg, wParam, lParam);
	case WM_CTLCOLORSTATIC:	return editListOnCtlColorStatic(hWnd, uMsg, wParam, lParam);

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}