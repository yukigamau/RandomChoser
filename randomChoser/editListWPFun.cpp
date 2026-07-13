#include <Windows.h>
import command;
import editListWPFun;
import std;
import window;

using std::wstring;
using std::tie;
using window::WindowAdjuster;

auto &editList{ window::wps.editList };

#pragma region Create

LRESULT editListOnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	constexpr POINT pBegin{ 10,10 };
	WindowAdjuster wa(hWnd, window::wps.style.hFStatic, window::wps.style.interval, pBegin);

	const wstring wsTitle{ L"当前处于列表编辑状态" };
	int width, height;
	tie(width, height) = wa.getCtlSize(wsTitle);
	command::Static title(hWnd, editList.hInstance, wsTitle);
	title.x = wa.x;
	title.y = wa.y;
	title.w = width;
	title.h = height;
	title.hFont = editList.style->hFStatic;
	title.create();

	return 0;
}

#pragma endregion

LRESULT editListOnCtlColorStatic(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;
	SetTextColor(hdc, editList.style->textColor());
	SetBkColor(hdc, editList.style->textBkColor());
	return (LRESULT)editList.style->textBkBrush();
}