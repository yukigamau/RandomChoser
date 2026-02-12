module;
#include <Windows.h>
export module window:listModify;

export namespace window
{
	LRESULT CALLBACK listModifyWP(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}

LRESULT CALLBACK window::listModifyWP(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		break;

	case WM_CREATE:
		break;

	case WM_CTLCOLORBTN:
		break;

	case WM_CTLCOLORSTATIC:
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}