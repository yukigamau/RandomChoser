#include <Windows.h>
#include <commctrl.h>
import id;
import passwordSubclass;
using namespace passwordID;

void moveToPasswordRe(HWND hCur)
{
	auto hParent{ GetParent(hCur) };
	auto hPasswordRe{ GetDlgItem(hParent, idc_edit_passwordRe) };
	SetFocus(hPasswordRe);
}

LRESULT subclass::subclassEPassword(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			[[fallthrough]];
		case VK_SEPARATOR:
			moveToPasswordRe(hWnd);
			return 0;
		}

	default:
		break;
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT subclass::subclassEPasswordRe(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	case VK_RETURN:
		[[fallthrough]];
	case VK_SEPARATOR:
		return 0;
	default:
		break;
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
