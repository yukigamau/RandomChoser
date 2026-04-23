module;
#include <Windows.h>
export module window:randomChoose;
import randomChooseWPFun;

export namespace window
{
	LRESULT CALLBACK randomChooseWP(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}

LRESULT window::randomChooseWP(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		break;

	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
