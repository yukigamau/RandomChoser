/*
* subclass中的subclassWP需要使用到可能的id之类的独存在于模块中的数据
*/
#include <Windows.h>
#include <commctrl.h>
import id;
import subclass;

enum class SpecialKey :int
{
	ctrl_backspace = 127
};

auto deleteLineBefore(HWND hWnd) -> int
{
	// 光标当前位置与当前行末
	DWORD curPos, endPos;
	SendMessage(hWnd, EM_GETSEL, (WPARAM)&curPos, (LPARAM)&endPos);

	// 光标起始行开头
	int lineIndex = (int)SendMessage(hWnd, EM_LINEFROMCHAR, curPos, 0);
	int lineStart = (int)SendMessage(hWnd, EM_LINEINDEX, lineIndex, 0);

	// 选中当前行中光标前面的内容
	SendMessage(hWnd, EM_SETSEL, lineStart, curPos);

	// 删除
	SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)L"");

	return 0;
}

LRESULT CALLBACK subclass::subclassEListName(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	case WM_CHAR:
		switch ((wchar_t)wParam)
		{
		case (int)SpecialKey::ctrl_backspace:
			return deleteLineBefore(hWnd);

		default:
			break;
		}

		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_TAB:
		{
			auto hParent{ GetParent(hWnd) };
			auto hEWriteNames{ GetDlgItem(hParent, idc_edit_writeName) };
			SetFocus(hEWriteNames);
			return 0;
		}
		}
		break;

	default:
		break;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK subclass::subclassEWriteNames(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	case WM_CHAR:
		switch ((wchar_t)wParam)
		{
		case (int)SpecialKey::ctrl_backspace:
			return deleteLineBefore(hWnd);

		default:
			break;
		}
		break;

	default:
		break;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
