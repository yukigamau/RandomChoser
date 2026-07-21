#include <Windows.h>
#include <commctrl.h>
import command;
import dataread;
import editListWPFun;
import id;
import margin;
import std;
import window;

using dataread::data;
using margin::Margin;
using std::wstring;
using std::tie;
using window::WindowAdjuster;

using namespace editListID;

auto &editList{ window::wps.editList };

#pragma region Create

enum class SpecialKey :int
{
	ctrl_backspace = 127
};

LRESULT CALLBACK subclassEPassword(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	case WM_CHAR:
		switch ((wchar_t)wParam)
		{
		case (int)SpecialKey::ctrl_backspace:
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

		default:
			break;
		}

		break;

	default:
		break;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT editListOnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	constexpr POINT pBegin{ 10,10 };
	WindowAdjuster wa(hWnd, window::wps.style.hFStatic, window::wps.style.interval, pBegin);
	int width, height;

	{
		const wstring wsTitle{ L"当前处于列表编辑状态" };
		tie(width, height) = wa.getCtlSize(wsTitle);
		command::Static title(hWnd, editList.hInstance, wsTitle);
		title.x = wa.x;
		title.y = wa.y;
		title.w = width;
		title.h = height;
		title.hFont = editList.style->hFStatic;
		title.create();

		wa.ctlNext(height);
	}

	{
		const wstring wsCur{ L"当前列表：" };
		tie(width, height) = wa.getCtlSize(wsCur);
		command::Static cur(hWnd, editList.hInstance, wsCur);
		cur.x = wa.x;
		cur.y = wa.y;
		cur.w = width;
		cur.h = height;
		cur.hFont = editList.style->hFStatic;
		cur.create();

		wa.ctlBeside(width);
	}

	{
		const wstring wsListName{ data.defaultList };
		tie(width, height) = wa.getCtlSize(wsListName);
		command::Static listName(hWnd, editList.hInstance, wsListName);
		listName.x = wa.x;
		listName.y = wa.y;
		listName.w = width;
		listName.h = height;
		listName.hFont = editList.style->hFStatic;
		listName.id = idc_stc_blue;
		listName.create();

		wa.ctlLeft(pBegin.x);
		wa.ctlNext(height);
	}

	{
		const wstring wsPassword{ L"密码：" };
		tie(width, height) = wa.getCtlSize(wsPassword);
		command::Static password(hWnd, editList.hInstance, wsPassword);
		password.x = wa.x;
		password.y = wa.y;
		password.w = width;
		password.h = height;
		password.hFont = editList.style->hFStatic;
		password.create();

		wa.ctlBeside(width);
	}

	{
		Margin margin{ editList.style->dpiScale };
		command::Edit password(hWnd, editList.hInstance, &margin);
		password.x = wa.x;
		password.y = wa.y;

		wa.adjustMaxXChange(width * 10 + editList.style->interval + pBegin.x);
		width *= 9;
		password.w = width;

		password.h = height;
		password.id = idc_edit_password;
		password.hFont = editList.style->hFStatic;
		password.addWinStyle(command::WinStyle::password);
		password.setSubclass(subclassEPassword);
		password.create();

		wa.ctlLeft(pBegin.x);
		wa.ctlNext(height);
	}

	wa.apply();

	return 0;
}

LRESULT editListOnCtlColorEdit(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}

#pragma endregion

LRESULT editListOnCtlColorStatic(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hCtrl = (HWND)lParam;
	auto id{ GetDlgCtrlID(hCtrl) };

	switch (id)
	{
	case idc_stc_blue:
		SetTextColor(hdc, RGB(102, 204, 255));
		break;

	default:
		SetTextColor(hdc, editList.style->textColor());
		break;
	}

	SetBkColor(hdc, editList.style->textBkColor());
	return (LRESULT)editList.style->textBkBrush();
}