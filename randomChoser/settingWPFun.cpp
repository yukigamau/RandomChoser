module settingWPFun;
import <Windows.h>;
import <windowsx.h>;
import dataread;
import id;
import glob;
import std;
import website;
import window;
using dataread::data;
using std::wstring;
using window::WindowAdjuster;
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

// 同时求最大的宽度和高度
void maxSize(HWND hWnd, HFONT hFont, const std::vector<wstring> &texts, int *const width, int *const height)
{
	// 初始化，防出错。
	*width = 0;
	*height = 0;

	HDC hdc = GetDC(hWnd);
	HFONT font = (HFONT)SelectObject(hdc, hFont);

	for (const wstring &text : texts)
	{
		RECT rc = { 0, 0, 0, 0 };
		DrawText(hdc, text.c_str(), -1, &rc, DT_CALCRECT);
		*width = max(*width, rc.right - rc.left);
		*height = max(*height, rc.bottom - rc.top);
	}

	SelectObject(hdc, font);
	ReleaseDC(hWnd, hdc);
}

LRESULT settingOnCreate(HWND hWnd, HINSTANCE hInstance)
{
	auto &style{ wps.style };

	constexpr auto pBegin{ POINT{10,10} };
	WindowAdjuster wa(hWnd, style.hFStatic, style.interval, pBegin);

	const wstring welcome = L"欢迎使用本程序！当前版本：" + glob::VERSION + L"\n"
		L"如果您在使用本产品时遇到程序漏洞，请发邮件至yvehuanghun@outlook.com" + L"\n"
		L"本项目己经在github上开源：https://github.com/yukigamau/RandomChoser";
	int width, height;
	wa.getCtlSize(welcome, &width, &height);
	HWND welcomeStatic = CreateWindow(L"STATIC", welcome.c_str(), WS_CHILD | WS_VISIBLE,
		wa.x, wa.y, width, height, hWnd, nullptr, hInstance, nullptr);
	SendMessage(welcomeStatic, WM_SETFONT, (WPARAM)style.hFStatic, TRUE);

	wa.ctlNext(height);

	if (data.defaultList == L"")
	{
		wstring ifListOK = L"您尚未选择要抽取的名单。";
		int width, height;
		wa.getCtlSize(ifListOK, &width, &height);
		HWND ifListOKStatic = CreateWindow(L"STATIC", ifListOK.c_str(), WS_CHILD | WS_VISIBLE,
			wa.x, wa.y, width, height, hWnd, (HMENU)IDC_STATIC_RED, hInstance, nullptr);
		SendMessage(ifListOKStatic, WM_SETFONT, (WPARAM)style.hFStatic, TRUE);

		wa.ctlNext(height);
	}

	if (data.lists.empty())
	{
		wstring haveNoLists = L"您尚未创建名单。";
		int width, height;
		wa.getCtlSize(haveNoLists, &width, &height);
		HWND haveNolistsStatic = CreateWindow(L"STATIC", haveNoLists.c_str(), WS_CHILD | WS_VISIBLE,
			wa.x, wa.y, width, height, hWnd, (HMENU)IDC_STATIC_RED, hInstance, nullptr);
		SendMessage(haveNolistsStatic, WM_SETFONT, (WPARAM)style.hFStatic, TRUE);

		wa.ctlNext(height);
	}
	else
	{
		wstring wsDefalutList = L"抽取名单：";
		int width, height;
		std::tie(width, height) = wa.getCtlSize(wsDefalutList);
		HWND hDefalutList = CreateWindow(L"STATIC", wsDefalutList.c_str(), WS_CHILD | WS_VISIBLE,
			wa.x, wa.y, width, height, hWnd, nullptr, hInstance, nullptr);
		SendMessage(hDefalutList, WM_SETFONT, (WPARAM)style.hFStatic, TRUE);

		wa.ctlBeside(width);

		maxSize(hWnd, style.hFStatic, data.lists, &width, &height);
		width += 50;	// 补足下拉键的宽度

		HWND chooseListCombo = CreateWindow(L"COMBOBOX", nullptr,
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
			wa.x, wa.y, width, height, hWnd, (HMENU)idc_ccb_default_list, hInstance, nullptr);

		for (auto a : data.lists)
		{
			int nIndex = SendMessage(chooseListCombo, CB_ADDSTRING, 0, (LPARAM)a.c_str());
			if (nIndex == CB_ERR)
				throw std::runtime_error("插入名单选项失败");
			else if (nIndex == CB_ERRSPACE)
				throw std::runtime_error("chooseListCombo CB_ERRSPACE");
			else if (a == data.defaultList)
				ComboBox_SetCurSel(chooseListCombo, nIndex);
		}

		SendMessage(chooseListCombo, WM_SETFONT, (WPARAM)style.hFStatic, TRUE);

		wa.ctlLeft(pBegin.x);
		wa.ctlNext(height);
	}

	// 同行的按钮
	wstring writeList = L"创建名单";
	wa.getCtlSize(writeList, &width, &height);
	width *= style.btnOuterSizeScaleH;
	height *= style.btnOuterSizeScaleV;
	HWND writeListBtn = CreateWindowEx(0, L"BUTTON", writeList.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		wa.x, wa.y, width, height, hWnd, (HMENU)idc_btn_write_list, hInstance, nullptr);
	SendMessage(writeListBtn, WM_SETFONT, (WPARAM)style.hFStatic, TRUE);

	wa.ctlBeside(width);

	wstring openSourceSite = L"打开源码网站";
	wa.getCtlSize(openSourceSite, &width, &height);
	width *= style.btnOuterSizeScaleH;
	height *= style.btnOuterSizeScaleV;
	HWND openSourceSiteBtn = CreateWindow(L"BUTTON", openSourceSite.c_str(), WS_CHILD | WS_VISIBLE,
		wa.x, wa.y, width, height, hWnd, (HMENU)idc_btn_open_source_site, hInstance, nullptr);
	SendMessage(openSourceSiteBtn, WM_SETFONT, (WPARAM)style.hFStatic, TRUE);

	wa.ctlBeside(width);

	wstring editList = L"修改当前名单";
	wa.getCtlSize(editList, &width, &height);
	width *= style.btnOuterSizeScaleH;
	height *= style.btnOuterSizeScaleV;
	HWND editListBtn = CreateWindow(L"BUTTON", editList.c_str(), WS_CHILD | WS_VISIBLE,
		wa.x, wa.y, width, height, hWnd, (HMENU)idc_btn_edit_list, hInstance, nullptr);
	SendMessage(editListBtn, WM_SETFONT, (WPARAM)style.hFStatic, TRUE);

	// 如果没有名单，那么就不会启用【修改已有名单】
	if (data.lists.empty())
		EnableWindow(editListBtn, false);

	wa.apply();

	return 0;
}
