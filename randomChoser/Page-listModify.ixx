module;
#include <Windows.h>
export module window:listModify;
import listModifyWPFun;

export namespace window
{
	LRESULT CALLBACK listModifyWP(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}

LRESULT CALLBACK window::listModifyWP(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		return listModifyOnCommand(hWnd, uMsg, wParam, lParam);

	case WM_CREATE:
		return listModifyOnCreate(hWnd, uMsg, wParam, lParam);

	case WM_CTLCOLORBTN:
		return listModifyOnCtlColorButton(hWnd, uMsg, wParam, lParam);

	case WM_CTLCOLOREDIT:
		return listModifyOnCtlColorEdit(hWnd, uMsg, wParam, lParam);

	case WM_CTLCOLORSTATIC:
		return listModifyOnCtlColorStatic(hWnd, uMsg, wParam, lParam);

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}