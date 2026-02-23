module;
#include <Windows.h>
export module window:password;
import passwordWPFun;

export namespace window
{
	LRESULT CALLBACK passwordWP(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}

LRESULT CALLBACK window::passwordWP(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:		return passwordOnCommand		(hWnd, uMsg, wParam, lParam);
	case WM_CREATE:			return passwordOnCreate			(hWnd, uMsg, wParam, lParam);
	case WM_CTLCOLORBTN:	return passwordOnCtlColorBtn	(hWnd, uMsg, wParam, lParam);
	case WM_CTLCOLOREDIT:	return passwordOnCtlColorEdit	(hWnd, uMsg, wParam, lParam);
	case WM_CTLCOLORSTATIC:	return passwordOnCtlColorStatic	(hWnd, uMsg, wParam, lParam);

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}