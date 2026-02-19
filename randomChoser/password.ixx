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
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}