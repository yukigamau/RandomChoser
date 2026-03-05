#include <Windows.h>
#include <commctrl.h>
import command;
import dataread;
import id;
import listModifySubclass;
import listModifyWPFun;
import glob;
import margin;
import std;
import window;

using namespace listModifyID;

using command::Button, command::Toggle;
using dataread::data;
using margin::Margin;
using std::vector, std::wstring;
using std::any_of, std::tie;
using window::Follow, window::IfButton, window::Page, window::Style, window::WindowAdjuster;

auto& listModify{ window::wps.listModify };
auto& hSetting{ window::wps.hSetting };

bool isAllWspace(const wstring& text)
noexcept
{
	return !any_of(
		text.begin(), text.end(),
		[](wchar_t c) { return !iswspace(c); }
	);
}

bool haveText(const vector<int>& idEdits)
{
	for (auto i : idEdits)
	{
		HWND hEdit = GetDlgItem(listModify.getHWND(), i);
		auto len{ GetWindowTextLength(hEdit) };
		if (!len)
			return false;

		wstring text;
		text.resize(len);

		GetWindowText(hEdit, text.data(), len + 1);

		if (isAllWspace(text))
			return false;
		else
			continue;
	}

	return true;
}

void backSettingPage(HWND hSetting, HWND hCur)
{
	ShowWindow(hCur, SW_HIDE);
	ShowWindow(hSetting, SW_SHOW);
}

auto getWindowText(HWND hWnd)
-> wstring
{
	auto len = GetWindowTextLength(hWnd);
	wstring text;
	text.resize(len + 1);
	GetWindowText(hWnd, text.data(), len + 1);
	return text;
}

// 检查是否重名
auto checkListName(HWND hWnd)
{
	auto text{ getWindowText(hWnd) };
	for (const auto& t : data.lists)
		if (t == text)
			return false;

	return true;
}

auto openPasswordPage(HWND hWnd)
{
	if (!checkListName(hWnd))
	{
		MessageBox(hWnd, L"存在重名的名单", L"糟糕", MB_ICONERROR);
		return 0;
	}

	ShowWindow(hWnd, SW_HIDE);

	if (window::wps.password.getHWND())
	{
		ShowWindow(window::wps.password.getHWND(), SW_SHOW);
		return 0;
	}

	window::wps.password.ini(listModify.hInstance, &(window::wps.style));
	window::wps.password.createWindow(L"password", L"密码", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT);

	return 0;
};

LRESULT listModifyOnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto id{ LOWORD(wParam) };
	auto msg{ HIWORD(wParam) };

	switch (id)
	{
	case idc_btn_backSettingPage:
		switch (msg)
		{
		case BN_CLICKED:
			backSettingPage(hSetting, hWnd);
			break;

		default:
			break;
		}
		break;

	case idc_btn_ifDefaultNo:
		if (msg == STN_CLICKED)
		{
			HWND hYes = GetDlgItem(hWnd, idc_btn_ifDefaultYes);
			int len = GetWindowTextLength(hYes);
			wstring ws;
			ws.resize(len);
			GetWindowText(hYes, ws.data(), len + 1);

			ws[0] = L'○';
			SetWindowText(hYes, ws.c_str());

			HWND hNo = GetDlgItem(hWnd, idc_btn_ifDefaultNo);
			len = GetWindowTextLength(hNo);
			ws.resize(len);
			GetWindowText(hNo, ws.data(), len + 1);

			ws[0] = L'⊙';
			SetWindowText(hNo, ws.c_str());

			glob::ifNewListDefault = false;
		}
		break;

	case idc_btn_ifDefaultYes:
		if (msg == STN_CLICKED)
		{
			HWND hYes = GetDlgItem(hWnd, idc_btn_ifDefaultYes);
			int len = GetWindowTextLength(hYes);
			wstring ws;
			ws.resize(len);
			GetWindowText(hYes, ws.data(), len + 1);

			ws[0] = L'⊙';
			SetWindowText(hYes, ws.c_str());

			HWND hNo = GetDlgItem(hWnd, idc_btn_ifDefaultNo);
			len = GetWindowTextLength(hNo);
			ws.resize(len);
			GetWindowText(hNo, ws.data(), len + 1);

			ws[0] = '○';
			SetWindowText(hNo, ws.c_str());

			glob::ifNewListDefault = true;
		}
		break;

	case idc_btn_save:
		if (msg == BN_CLICKED)
			return openPasswordPage(hWnd);
		break;

	case idc_edit_listName:
		[[fallthrough]];
	case idc_edit_writeName:
		switch (msg)
		{
		case EN_CHANGE:
		{
			auto hBSave{ GetDlgItem(hWnd, idc_btn_save) };
			auto enable{ false };
			if (haveText({ idc_edit_listName,idc_edit_writeName }))
				enable = true;
			EnableWindow(hBSave, enable);
		}
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}
	return 0;
}

LRESULT listModifyOnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	constexpr auto xBegin{ 10 };
	constexpr auto yBegin{ 10 };
	WindowAdjuster wa(hWnd, listModify.style->hFStatic, listModify.style->interval, xBegin, yBegin);

	// 用于对齐下面的名字输入编辑框和名单名字框
	auto maxX{ 0 };

	wstring wsListName{ L"名单名字：" };
	auto [width, height] = wa.getCtlSize(wsListName);
	HWND hSListName = CreateWindow(L"STATIC", wsListName.c_str(), WS_CHILD | WS_VISIBLE,
		wa.x, wa.y, width, height, hWnd, nullptr, listModify.hInstance, nullptr);
	listModify.style->setFont(hSListName);

	wa.ctlBeside(width);
	maxX += width + wa.interval;

	width *= 9;
	Margin margin(listModify.style->dpiScale);
	HWND hEListName = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		wa.x, wa.y - margin.getHalfMargin(), width, height + margin.getHalfMargin(), hWnd,
		(HMENU)idc_edit_listName, listModify.hInstance, nullptr);
	listModify.style->setFont(hEListName);
	margin.apply(hEListName);
	SetWindowSubclass(hEListName, subclass::subclassEListName, (UINT_PTR)idc_edit_listName, 0);

	maxX += width;
	wa.adjustMaxXChange(maxX);

	wa.ctlNext(height + margin.getHalfMargin());
	wa.ctlLeft(xBegin);

	wstring wsWriteNamesTip{ L"在下方写下名字，用换行或tab区分各个名字。\n不支持用空格！！！" };
	tie(width, height) = wa.getCtlSize(wsWriteNamesTip);
	HWND hSWriteNamesTip = CreateWindow(L"STATIC", wsWriteNamesTip.c_str(), WS_CHILD | WS_VISIBLE,
		wa.x, wa.y, width, height, hWnd, (HMENU)idc_static_red, listModify.hInstance, nullptr);
	listModify.style->setFont(hSWriteNamesTip);

	wa.ctlNext(height);

	wstring wsIfDefault{ L"是否设置为默认名单：" };
	wstring wsIfDefaultYes{ L"是" };
	wstring wsIfDefaultNO{ L"否" };
	Toggle ifDefault(hWnd, listModify.hInstance, wsIfDefault, wsIfDefaultYes, wsIfDefaultNO,
		listModify.style->interval, idc_btn_ifDefaultYes, idc_btn_ifDefaultNo);
	tie(width, height) = wa.getCtlSize(ifDefault);
	ifDefault.x = wa.x;
	ifDefault.y = wa.y;
	ifDefault.hFont = listModify.style->hFStatic;
	ifDefault.create();
	glob::ifNewListDefault = true;

	wa.ctlNext(height);

	wstring wsSaveBtnText{ L"保存名单进入下一步" };
	tie(width, height) = wa.getCtlSize(wsSaveBtnText, IfButton::button);
	Button save(hWnd, listModify.hInstance, wsSaveBtnText);
	save.x = wa.x;
	save.y = wa.y;
	save.w = width;
	save.h = height;
	save.id = idc_btn_save;
	save.hFont = listModify.style->hFStatic;
	save.create();

	wa.ctlBeside(width);

	wstring wsBackSettingPageBtnText{ L"返回设置页面" };
	tie(width, height) = wa.getCtlSize(wsBackSettingPageBtnText, IfButton::button, Follow::beside);
	HWND hBBackSettingPage = CreateWindow(L"BUTTON", wsBackSettingPageBtnText.c_str(), WS_CHILD | WS_VISIBLE,
		wa.x, wa.y, width, height, hWnd, (HMENU)idc_btn_backSettingPage, listModify.hInstance, nullptr);
	SendMessage(hBBackSettingPage, WM_SETFONT, (WPARAM)listModify.style->hFStatic, TRUE);

	wa.ctlLeft(xBegin);
	wa.ctlNext(height);
	
	height = wa.getMultipleLinesHeight(32);	// 这里是32行内容，对于大部分情况下应该是够用的
	width = maxX;
	HWND hEWriteName = CreateWindowEx(0, L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_VSCROLL,
		wa.x, wa.y, width, height, hWnd, (HMENU)idc_edit_writeName, listModify.hInstance, nullptr);
	SendMessage(hEWriteName, WM_SETFONT, (WPARAM)listModify.style->hFStatic, TRUE);
	margin.apply(hEWriteName);
	SetWindowSubclass(hEWriteName, subclass::subclassEWriteNames, (UINT_PTR)idc_edit_writeName, 0);
	Style::setTapStops(hEWriteName, 64);

	wa.ctlNext(height);

	// 这里的多余的值用于处理不知道为什么出现的编辑框无法完整出现的问题
	wa.adjustMaxYAddon(height + margin.getHalfMargin() * 4 + wa.interval);

	wa.apply();

	return 0;
}

LRESULT listModifyOnCtlColorButton(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hCtrl = (HWND)lParam;
	int id = GetDlgCtrlID(hCtrl);

	switch (id)
	{
	case idc_btn_backSettingPage:
		[[fallthrough]];
	case idc_btn_save:
		return (INT_PTR)listModify.style->buttonBkBrush();

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT listModifyOnCtlColorEdit(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hEdit = (HWND)lParam;

	SetTextColor(hdc, listModify.style->textColor());   // 字体颜色
	SetBkColor(hdc, listModify.style->textBkColor());	// 背景颜色，同时也改变边框的颜色

	return (INT_PTR)listModify.style->textBkBrush();
}

LRESULT listModifyOnCtlColorStatic(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hCtrl = (HWND)lParam;
	auto id{ GetDlgCtrlID(hCtrl) };
	
	switch (id)
	{
	case idc_static_red:
		SetTextColor(hdc, RGB(255, 0, 0));
		break;

	default:
		SetTextColor(hdc, listModify.style->textColor());
		break;
	}

	SetBkColor(hdc, listModify.style->textBkColor());
	return (LRESULT)listModify.style->textBkBrush();
}