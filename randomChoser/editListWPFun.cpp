#include <Windows.h>
#include <commctrl.h>
import command;
import dataread;
import editListWPFun;
import id;
import margin;
import settingWPFun;
import std;
import window;

using command::WinStyle;
using dataread::data;
using margin::Margin;
using std::wstring;
using std::tie;
using window::WindowAdjuster;
using window::restart;

using namespace editListID;

auto &editList{ window::wps.editList };

#pragma region Command

LRESULT backSetting()
{
	HWND hCur = editList.getHWND();
	HWND hSetting = window::wps.hSetting;

	ShowWindow(hCur, SW_HIDE);
	ShowWindow(hSetting, SW_SHOW);

	return 0;
}

inline LRESULT switchBackSetting(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto msg{ HIWORD(wParam) };

	switch (msg)
	{
	case STN_CLICKED:
		return backSetting();

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

wstring getInpassword()
{
	wchar_t buffer[257]{};
	HWND hPassword = GetDlgItem(editList.getHWND(), idc_edit_password);
	GetWindowText(hPassword, buffer, 257);
	wstring inPassword{ buffer };
	return inPassword;
}

// 获取idc_edit_list中被修改过的文本
wstring getModifiedText()
{
	HWND hEdit = GetDlgItem(editList.getHWND(), idc_edit_list);
	int length = GetWindowTextLength(hEdit);

	wstring text;
	text.resize(length + 1);
	GetWindowText(hEdit, text.data(), length + 1);

	// 去除最后的\0
	text.pop_back();

	// 把CRLF改成LF
	text.erase(std::remove(text.begin(), text.end(), L'\r'), text.end());

	return text;
}

bool checkPassword()
{
	// 获取输入的密码
	wstring inPassword = getInpassword();

	// 获取对应的名单的密码
	wstring truePassword = data.getPassword(data.defaultList);

	return inPassword == truePassword;
}

void replaceList(const wstring &listName, const wstring &listText, const wstring &password)
{
	data.saveListText(listName, listText, password);
}

LRESULT switchConfirm(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto msg{ HIWORD(wParam) };

	if (msg != STN_CLICKED)
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	// 处理STN_CLICKED消息
	if (!checkPassword())
	{
		MessageBox(hWnd, L"密码错误", L"密码错误", MB_OK);
		return 0;
	}

	switch (MessageBox(hWnd, L"确定修改吗？", L"密码一致", MB_YESNOCANCEL))
	{
	case IDYES:
		replaceList(data.defaultList, getModifiedText(), getInpassword());
		return 0;

	case IDNO:
		backSetting();
		return 0;

	case IDCANCEL:
		return 0;
	}

	return 0;
}

void deleteList()
{
	if (data.deleteList(data.defaultList))
		reloadLists(data.lists);
	else
		restart();
}

LRESULT switchDelete(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto msg{ HIWORD(wParam) };
	
	if (msg != STN_CLICKED)
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	// 处理STN_CLICKED
	switch (MessageBox(hWnd, L"确定删除吗？\n本次操作不可撤回。", L"警告", MB_YESNOCANCEL))
	{
	case IDYES:
		deleteList();
		backSetting();
		return 0;

	case IDNO:
		backSetting();
		return 0;

	case IDCANCEL:
		return 0;
	}

	return 0;
}

LRESULT editListOnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto id{ LOWORD(wParam) };

	switch (id)
	{
	case idc_btn_backSetting:
		return switchBackSetting(hWnd, uMsg, wParam, lParam);

	case idc_btn_confirm:
		return switchConfirm(hWnd, uMsg, wParam, lParam);

	case idc_btn_delete:
		return switchDelete(hWnd, uMsg, wParam, lParam);

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Command
#pragma endregion

#pragma region Create

enum class SpecialKey :int
{
	ctrl_backspace = 127
};

// 修改ctrl+backspace以免输入del
LRESULT CALLBACK subclassECtrlBackspace(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
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
		password.setSubclass(subclassECtrlBackspace);
		password.create();

		wa.ctlLeft(pBegin.x);
		wa.ctlNext(height);
	}

	{
		int maxWidth = wa.outMaxWidth();
		width = maxWidth;
		height = width;

		Margin margin{ editList.style->dpiScale };
		command::Edit list(hWnd, editList.hInstance, &margin);
		list.x = wa.x;
		list.y = wa.y;
		list.w = width;
		list.h = height;
		list.id = idc_edit_list;
		list.hFont = editList.style->hFStatic;
		list.setSubclass(subclassECtrlBackspace);
		list.addWinStyle(WinStyle::multiLine);
		list.create();

		wa.adjustMaxYAddon(height);
		wa.ctlNext(height);

		// 设置文本
		wstring listText;
		for (const auto &dn : data.defaultNames)
			listText += dn + L"\r\n";
		// 去除最后的\r\n
		listText.pop_back();
		listText.pop_back();

		SetWindowText(list.getHWnd(), listText.c_str());
	}

	{
		wstring wsBackSetting = L"返回设置页面";
		tie(width, height) = wa.getCtlSize(wsBackSetting, window::IfButton::button);

		command::Button backSetting(hWnd, editList.hInstance, wsBackSetting);
		backSetting.x = wa.x;
		backSetting.y = wa.y;
		backSetting.w = width;
		backSetting.h = height;
		backSetting.id = idc_btn_backSetting;
		backSetting.hFont = editList.style->hFStatic;
		backSetting.create();

		wa.ctlBeside(width);
	}

	{
		wstring wsConfirm = L"确认修改";
		tie(width, height) = wa.getCtlSize(wsConfirm, window::IfButton::button, window::Follow::beside);

		command::Button confirm(hWnd, editList.hInstance, wsConfirm);
		confirm.x = wa.x;
		confirm.y = wa.y;
		confirm.w = width;
		confirm.h = height;
		confirm.id = idc_btn_confirm;
		confirm.hFont = editList.style->hFStatic;
		confirm.create();

		wa.ctlBeside(width);
	}

	{
		wstring wsDelete = L"删除名单";
		tie(width, height) = wa.getCtlSize(wsDelete, window::IfButton::button, window::Follow::beside);

		command::Button deleteBtn(hWnd, editList.hInstance, wsDelete);
		deleteBtn.x = wa.x;
		deleteBtn.y = wa.y;
		deleteBtn.w = width;
		deleteBtn.h = height;
		deleteBtn.id = idc_btn_delete;
		deleteBtn.hFont = editList.style->hFStatic;
		deleteBtn.create();

		wa.ctlLeft(pBegin.x);
		wa.ctlNext(height);
	}

	wa.apply();

	return 0;
}

// Create
#pragma endregion

LRESULT editListOnCtlColorButton(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (INT_PTR)editList.style->buttonBkBrush();
}

LRESULT editListOnCtlColorEdit(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hEdit = (HWND)lParam;

	SetTextColor(hdc, editList.style->textColor());   // 字体颜色
	SetBkColor(hdc, editList.style->textBkColor());	// 背景颜色，同时也改变边框的颜色

	return (INT_PTR)editList.style->textBkBrush();
}

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