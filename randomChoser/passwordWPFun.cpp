#include <Windows.h>
#include <commCtrl.h>

import command;
import id;
import margin;
import passwordSubclass;
import passwordWPFun;
import std;
import window;

using namespace passwordID;

using command::Edit, command::Static, command::WinStyle;
using margin::Margin;
using std::wstring;
using std::tie;
using window::WindowAdjuster;

auto& password{ window::wps.password };

auto ifNeedPassword{ true };
const wstring needPasswordNoOff = L"○ 不需要";
const wstring needPasswordNoOn = L"⊙ 不需要";
const wstring needPasswordYesOff = L"○ 需要";
const wstring needPasswordYesOn = L"⊙ 需要";
void changeNeedPasswordGroup()
{
	auto hBIfNeedPasswordNo{ GetDlgItem(password.getHWND(), idc_btn_needPasswordNo)};
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
LRESULT passwordOnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto id{ LOWORD(wParam) };
	auto msg{ HIWORD(wParam) };

	switch (id)
	{
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
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

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
	ePassword.hFont = password.style->hFStatic;
	ePassword.addWinStyle(WinStyle::password);
	ePassword.setSubclass(subclass::subclassEPassword);
	ePassword.create();

	wa.adjustMaxXChange(width);
	wa.ctlLeft(xBegin);
	wa.ctlNext(height);

	tie(width, height) = wa.getCtlSize(passwordReText);
	Static stcPasswordReText(hWnd, password.hInstance, passwordReText);
	stcPasswordReText.x = wa.x;
	stcPasswordReText.y = wa.y;
	stcPasswordReText.w = width;
	stcPasswordReText.h = height;
	stcPasswordReText.hFont = password.style->hFStatic;
	stcPasswordReText.create();

	wa.ctlBeside(width);

	width *= 9;
	Edit ePasswordRe(hWnd, password.hInstance, &margin);
	ePasswordRe.x = wa.x;
	ePasswordRe.y = wa.y;
	ePasswordRe.w = width;
	ePasswordRe.h = height;
	ePasswordRe.hFont = password.style->hFStatic;
	ePasswordRe.addWinStyle(WinStyle::password);
	ePasswordRe.setSubclass(subclass::subclassEPasswordRe);
	ePasswordRe.create();

	wa.adjustMaxXChange(width);
	wa.ctlLeft(xBegin);
	wa.ctlNext(height);

	return 0;
}

LRESULT passwordOnCtlColorBtn(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hCtrl = (HWND)lParam;
	auto id{ GetDlgCtrlID(hCtrl) };

	switch (id)
	{
	default:
		SetTextColor(hdc, password.style->textColor());
		break;
	}

	SetBkMode(hdc, TRANSPARENT);
	return (INT_PTR)GetStockObject(NULL_BRUSH);
}

LRESULT passwordOnCtlColorEdit(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	HWND hEdit = (HWND)lParam;

	SetTextColor(hdc, password.style->textColor());   // 字体颜色
	SetBkColor(hdc, password.style->textBkColor());	// 背景颜色，同时也改变边框的颜色

	return (INT_PTR)password.style->textBkBrush();
}

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
