module chooseWPFun;
#include <Windows.h>
import dataread;
import glob;
import id;
import window;
using dataread::data;
using window::wps;
using namespace chooseID;

LRESULT chooseOnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto &hSetting{ window::wps.hSetting };

	int id = LOWORD(wParam);
	int code = HIWORD(wParam);

	switch (id)
	{
	case IDCLOSE:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case idc_stc_settingBtn:
		if (code == BN_CLICKED)
		{
			data.getLists();	// createSettingPage需要用
			glob::createSettingPage();
			ShowWindow(hSetting, SW_SHOW);
			ShowWindow(hWnd, SW_HIDE);
			KillTimer(hWnd, idt_wait);
		}
		break;

	case idc_stc_chooseBtn:
		switch (code)
		{
		case BN_CLICKED:
			if (glob::scrollNum != glob::scrollNumMax)
				break;	// 防止重复

			// 启动计时器，开始滚动
			SetTimer(hWnd, chooseID::idt_scroll, glob::scrollInterval, nullptr);
			break;
		}
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}
