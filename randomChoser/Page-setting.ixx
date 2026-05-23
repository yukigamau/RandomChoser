//module;
//#include <Windows.h>
//export module window:setting;
//import settingWPFun;
//
//export namespace window
//{
//	LRESULT CALLBACK settingWP(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//}
//
//LRESULT CALLBACK window::settingWP(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	switch (uMsg)
//	{
//	case WM_COMMAND:		return settingOnCommand(hWnd, uMsg, wParam, lParam);
//	case WM_CREATE:			return settingOnCreate(hWnd, uMsg, wParam, lParam);
//	case WM_CTLCOLORBTN:	return settingOnCtlColorBtn(hWnd, uMsg, wParam, lParam);
//	case WM_CTLCOLOREDIT:	return settingOnCtlColorEdit(hWnd, uMsg, wParam, lParam);
//	case WM_CTLCOLORSTATIC:	return settingOnCtlColorStatic(hWnd, uMsg, wParam, lParam);
//
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		break;
//	}
//
//	return DefWindowProc(hWnd, uMsg, wParam, lParam);
//}