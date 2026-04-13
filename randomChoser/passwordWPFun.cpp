#include <Windows.h>
#include <commCtrl.h>

import command;
import dataread;
import id;
import margin;
import match;
import glob;
import passwordSubclass;
import passwordWPFun;
import rgb;
import std;
import window;

using namespace passwordID;

using command::Button, command::Edit, command::Static, command::WinStyle;
using margin::Margin;
using match::Match, match::MatchDegree;
using rgb::Color, rgb::Rgb;
using std::wstring;
using std::tie;
using window::Follow, window::IfButton, window::WindowAdjuster;

auto& password{ window::wps.password };

#pragma region Command消息

bool ifNeedPassword{ true };

const wstring needPasswordNoOff = L"○ 不需要";
const wstring needPasswordNoOn = L"⊙ 不需要";
const wstring needPasswordYesOff = L"○ 需要";
const wstring needPasswordYesOn = L"⊙ 需要";

Match mt;

void changeNeedPasswordGroup()
{
	auto hBIfNeedPasswordNo{ GetDlgItem(password.getHWND(), idc_btn_needPasswordNo) };
	auto hBIfNeedPasswordYes{ GetDlgItem(password.getHWND(), idc_btn_needPasswordYes) };

	if (ifNeedPassword)
	{
		SetWindowText(hBIfNeedPasswordNo, needPasswordNoOff.c_str());
		SetWindowText(hBIfNeedPasswordYes, needPasswordYesOn.c_str());
	}
	else
	{
		SetWindowText(hBIfNeedPasswordNo, needPasswordNoOn.c_str());
		SetWindowText(hBIfNeedPasswordYes, needPasswordYesOff.c_str());
	}

	// 设置密码作用提示颜色
	HWND hSPasswordTip{ GetDlgItem(password.getHWND(), idc_static_passwordTip) };
	InvalidateRect(hSPasswordTip, NULL, TRUE);
}

bool ifSetMatch(HWND hWnd)
{
	static auto hPasswordRe = GetDlgItem(hWnd, idc_edit_passwordRe);

	auto passwordReLen = GetWindowTextLength(hPasswordRe);
	if (!passwordReLen)
		return mt.change(MatchDegree::none);

	static auto hPassword = GetDlgItem(hWnd, idc_edit_password);
	auto passwordLen = GetWindowTextLength(hPassword);
	if (!passwordLen)
		return mt.change(MatchDegree::diff);

	const int maxLen = 32;	// 应该绰绰有余了
	wstring password;
	password.resize(maxLen);
	GetWindowText(hPassword, password.data(), maxLen);
	wstring passwordRe;
	passwordRe.resize(maxLen);
	GetWindowText(hPasswordRe, passwordRe.data(), maxLen);

	if (password == passwordRe)
		return mt.change(MatchDegree::same);
	else
		return mt.change(MatchDegree::diff);
}

void invalidatePassword(HWND hParent)
{
	static auto h1 = GetDlgItem(hParent, idc_edit_password);
	static auto h2 = GetDlgItem(hParent, idc_edit_passwordRe);

	InvalidateRect(h1, NULL, TRUE);
	InvalidateRect(h2, NULL, TRUE);
}


#pragma region passwordOnCommand

/*
* 函数：	saveList
* 作用：	保存列表
*/
void saveList()
{
	auto hTitle = GetDlgItem(window::wps.listModify.getHWND(), listModifyID::idc_edit_listName);
	auto nTitle = GetWindowTextLength(hTitle);
	wstring wsTitle;
	wsTitle.resize(nTitle);
	GetWindowText(hTitle, wsTitle.data(), nTitle + 1);

	auto hText = GetDlgItem(window::wps.listModify.getHWND(), listModifyID::idc_edit_writeName);
	auto nText = GetWindowTextLength(hText);
	wstring wsText;
	wsText.resize(nText);
	GetWindowText(hText, wsText.data(), nText + 1);

	dataread::data.saveLists(wsTitle, glob::ifNewListDefault);

	if (!ifNeedPassword)
	{
		dataread::data.saveListText(wsTitle, wsText);
		return;
	}

	auto hPassword = GetDlgItem(window::wps.password.getHWND(), passwordID::idc_edit_password);
	auto nPassword = GetWindowTextLength(hPassword);
	wstring wsPassword;
	wsPassword.resize(nPassword);
	GetWindowText(hPassword, wsPassword.data(), nPassword + 1);

	dataread::data.saveListText(wsTitle, wsText, wsPassword);
}

/*
* 函数：	passwordOnCommand
* 作用：	用于密码页面的控件消息处理
*/
LRESULT passwordOnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto id{ LOWORD(wParam) };
	auto msg{ HIWORD(wParam) };

	switch (id)
	{
	case idc_btn_backListModify:
		if (msg == STN_CLICKED)
		{
			ShowWindow(window::wps.listModify.getHWND(), SW_SHOW);
			ShowWindow(window::wps.password.getHWND(), SW_HIDE);
		}
		break;

	case idc_btn_needPasswordNo:
		switch (msg)
		{
		case STN_CLICKED:
			if (!ifNeedPassword)
				break;
			ifNeedPassword = false;
			changeNeedPasswordGroup();
			return 0;

		default:
			break;
		}

	case idc_btn_needPasswordYes:
		switch (msg)
		{
		case STN_CLICKED:
			if (ifNeedPassword)
				break;
			ifNeedPassword = true;
			changeNeedPasswordGroup();
			return 0;

		default:
			break;
		}

	case idc_btn_yes:
		if (msg == BN_CLICKED)
			saveList();
		break;

	case idc_edit_password:
		[[fallthrough]];
	case idc_edit_passwordRe:
		switch (msg)
		{
		case EN_CHANGE:
			if (ifSetMatch(hWnd))
				invalidatePassword(hWnd);
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#pragma endregion
// passwordOnCommand

LRESULT passwordOnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	constexpr auto xBegin{ 10 }, yBegin{ 10 };
	WindowAdjuster wa(hWnd, password.style->hFStatic, password.style->interval, xBegin, yBegin);

	wstring ifNeedPassword{ L"请选择是否要密码：" };
	auto [width, height] = wa.getCtlSize(ifNeedPassword);
	Static stcIfNeedPassword(hWnd, password.hInstance, ifNeedPassword);
	stcIfNeedPassword.x = wa.x;
	stcIfNeedPassword.y = wa.y;
	stcIfNeedPassword.w = width;
	stcIfNeedPassword.h = height;
	stcIfNeedPassword.id = idc_static_ifNeedPassword;
	stcIfNeedPassword.hFont = password.style->hFStatic;
	stcIfNeedPassword.create();

	wa.ctlBeside(width);

	/* 这里开关按钮用○和⊙来代替状态 */
	tie(width, height) = wa.getCtlSize(needPasswordYesOn);
	HWND hBIfNeedPasswordYes = CreateWindow(L"STATIC", needPasswordYesOn.c_str(),
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | SS_NOTIFY,
		wa.x, wa.y, width, height, hWnd, (HMENU)idc_btn_needPasswordYes, password.hInstance, nullptr);
	password.style->setFont(hBIfNeedPasswordYes);
	wa.ctlBeside(width);
	// 否
	tie(width, height) = wa.getCtlSize(needPasswordNoOff);
	HWND hBIfNeedPasswordNo = CreateWindow(L"STATIC", needPasswordNoOff.c_str(),
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | SS_NOTIFY,
		wa.x, wa.y, width, height, hWnd, (HMENU)idc_btn_needPasswordNo, password.hInstance, nullptr);
	password.style->setFont(hBIfNeedPasswordNo);

	wa.ctlNext(height);
	wa.ctlLeft(xBegin);

	wstring passwordTip{ L"没有密码的名单将自动设置为不可更改" };
	tie(width, height) = wa.getCtlSize(passwordTip);
	HWND hSPasswordTip = CreateWindow(L"STATIC", passwordTip.c_str(), WS_CHILD | WS_VISIBLE,
		wa.x, wa.y, width, height, hWnd, (HMENU)idc_static_passwordTip, password.hInstance, nullptr);
	password.style->setFont(hSPasswordTip);

	wa.ctlNext(height);

#pragma region 两个密码输入

	wstring passwordText{ L"密码：" };
	wstring passwordReText{ L"确认密码:" };
	// 为了对齐，以最长的为准
	tie(width, height) = wa.getCtlSize(passwordReText);

	Static stcPassword(hWnd, password.hInstance, passwordText);
	stcPassword.x = wa.x;
	stcPassword.y = wa.y;
	stcPassword.w = width;
	stcPassword.h = height;
	stcPassword.id = idc_stc_password;
	stcPassword.hFont = password.style->hFStatic;
	stcPassword.create();

	wa.ctlBeside(width);

	Margin margin(password.style->dpiScale);
	Edit ePassword(hWnd, password.hInstance, &margin);
	ePassword.x = wa.x;
	ePassword.y = wa.y;
	width *= 9;
	ePassword.w = width;
	ePassword.h = height;
	ePassword.id = idc_edit_password;
	ePassword.hFont = password.style->hFStatic;
	ePassword.addWinStyle(WinStyle::password);
	ePassword.setSubclass(subclass::subclassEPassword);
	ePassword.create();

	wa.adjustMaxXChange(width);
	wa.ctlLeft(xBegin);
	wa.ctlNext(height);

	auto farX{ 0 };

	tie(width, height) = wa.getCtlSize(passwordReText);
	farX += width;
	Static stcPasswordReText(hWnd, password.hInstance, passwordReText);
	stcPasswordReText.x = wa.x;
	stcPasswordReText.y = wa.y;
	stcPasswordReText.w = width;
	stcPasswordReText.h = height;
	stcPasswordReText.hFont = password.style->hFStatic;
	stcPasswordReText.create();

	wa.ctlBeside(width);

	width *= 9;
	farX += width;
	Edit ePasswordRe(hWnd, password.hInstance, &margin);
	ePasswordRe.x = wa.x;
	ePasswordRe.y = wa.y;
	ePasswordRe.w = width;
	ePasswordRe.h = height;
	ePasswordRe.id = idc_edit_passwordRe;
	ePasswordRe.hFont = password.style->hFStatic;
	ePasswordRe.addWinStyle(WinStyle::password);
	ePasswordRe.setSubclass(subclass::subclassEPasswordRe);
	ePasswordRe.create();

	farX += xBegin + wa.interval;

	wa.adjustMaxXChange(farX);
	wa.ctlLeft(xBegin);
	wa.ctlNext(height);

	// 重置密码匹配程度
	mt.change(MatchDegree::none);

#pragma endregion

	wstring yes = L"确认";
	tie(width, height) = wa.getCtlSize(yes, IfButton::button);
	Button btnYes(hWnd, password.hInstance, yes);
	btnYes.x = wa.x;
	btnYes.y = wa.y;
	btnYes.w = width;
	btnYes.h = height;
	btnYes.id = idc_btn_yes;
	btnYes.hFont = password.style->hFStatic;
	btnYes.create();

	wa.ctlBeside(width);

	wstring backListModify = L"返回名单页面";
	tie(width, height) = wa.getCtlSize(backListModify, IfButton::button, Follow::beside);
	Button btnBackListModify(hWnd, password.hInstance, backListModify);
	btnBackListModify.x = wa.x;
	btnBackListModify.y = wa.y;
	btnBackListModify.w = width;
	btnBackListModify.h = height;
	btnBackListModify.id = idc_btn_backListModify;
	btnBackListModify.hFont = password.style->hFStatic;
	btnBackListModify.create();

	wa.apply();

	return 0;
}

LRESULT passwordOnCtlColorBtn(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (INT_PTR)password.style->buttonBkBrush();
}

#pragma region 编辑框颜色

LRESULT defaultColor(HDC hdc)
{
	SetTextColor(hdc, password.style->textColor());   // 字体颜色
	SetBkColor(hdc, password.style->textBkColor());	// 背景颜色，同时也改变边框的颜色

	return (INT_PTR)password.style->textBkBrush();
}

LRESULT setColor(HDC hdc, const Rgb& rgb)
{
	SetTextColor(hdc, rgb.cur());
	SetBkColor(hdc, password.style->textBkColor());

	return (INT_PTR)password.style->textBkBrush();
}

LRESULT passwordOnCtlColorEdit(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hCtrl = (HWND)lParam;
	auto id{ GetDlgCtrlID(hCtrl) };
	switch (id)
	{
	case idc_edit_password:
		[[fallthrough]];
	case idc_edit_passwordRe:
		switch (mt.degree())
		{
		case MatchDegree::diff:
			// 红色
			return setColor(hdc, Color::grape);

		case MatchDegree::none:
			return defaultColor(hdc);

		case MatchDegree::same:
			// 绿色
			return setColor(hdc, Color::jade);
		}

	default:
		return defaultColor(hdc);
	}
}

#pragma endregion

LRESULT passwordOnCtlColorStatic(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hCtrl = (HWND)lParam;
	auto id{ GetDlgCtrlID(hCtrl) };

	switch (id)
	{
	case idc_static_passwordTip:
		if (ifNeedPassword)
			SetTextColor(hdc, password.style->textColor());
		else
			SetTextColor(hdc, RGB(255, 0, 0));
		break;

	default:
		SetTextColor(hdc, password.style->textColor());
		break;
	}

	SetBkColor(hdc, password.style->textBkColor());
	return (LRESULT)password.style->textBkBrush();
}