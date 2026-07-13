module settingWPFun;
#include <Windows.h>
#include <windowsx.h>
import dataread;
import id;
import std;
import website;
import window;
using std::wstring;
using window::wps;
using namespace settingID;

LRESULT CALLBACK settingOnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto &listModify{ wps.listModify };
	auto &editList{ wps.editList };

	int id = LOWORD(wParam);
	
	switch (id)
	{
	case idc_btn_edit_list:
		if (editList.hInstance)
			ShowWindow(editList.getHWND(), SW_SHOW);
		else
		{
			HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
			editList.ini(hInstance, &wps.style);
			editList.createWindow(L"editList", L"列表修改", WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT);
		}

		ShowWindow(hWnd, SW_HIDE);
		return 0;

	case idc_btn_open_source_site:
		website::openWebsite(L"https://github.com/yukigamau/RandomChoser");
		return 0;

	case idc_btn_write_list:
		if (listModify.hInstance)
			ShowWindow(listModify.getHWND(), SW_SHOW);
		else
		{
			HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
			listModify.ini(hInstance, &wps.style);
			listModify.createWindow(L"listModify", L"创建名单", WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT);
		}

		ShowWindow(hWnd, SW_HIDE);
		return 0;

	case idc_ccb_default_list:
	{
		WORD notificationCode = HIWORD(wParam);
		if (notificationCode == CBN_SELCHANGE)
		{
			HWND hComboBox = (HWND)lParam;
			int currentIndex = ComboBox_GetCurSel(hComboBox);
			if(currentIndex!=CB_ERR)
			{
				wstring title;
				title.resize(256);
				ComboBox_GetLBText(hComboBox, currentIndex, title.data());
				dataread::data.changeDefaultList(title);
			}
		}
		break;
	}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}