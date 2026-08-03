module;
#include <Windows.h>
#include <windowsx.h>

export module settingWPFun;
import dataread;
import std;

using dataread::data;

export LRESULT CALLBACK settingOnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT settingOnCreate(HWND hWnd, HINSTANCE hInstance);

HWND listsLoadDir;
// 用于设置页面抽取名单的名单列表加载
export void loadLists(HWND hWnd, const std::vector<std::wstring> &lists)
{
	listsLoadDir = hWnd;
	for (const auto &list : lists)
	{
		int nIndex = SendMessage(hWnd, CB_ADDSTRING, 0, (LPARAM)list.c_str());
		if(list==data.defaultList)
			ComboBox_SetCurSel(hWnd, nIndex);
	}
}
// 用于设置页面抽取名单的名单的重新加载
export void reloadLists(const std::vector<std::wstring> &lists)
{
	ComboBox_ResetContent(listsLoadDir);
	loadLists(listsLoadDir, lists);
}