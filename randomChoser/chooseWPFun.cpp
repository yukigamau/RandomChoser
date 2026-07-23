module chooseWPFun;
#include <Windows.h>
import dataread;
import glob;
import id;
import std;
import transparency;
import window;
using dataread::data;
using std::wstring;
using window::wps;
using namespace chooseID;

int waitNum = 0;
const int WAIT_NUM_MAX = 8;

LRESULT chooseOnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto &hSetting{ window::wps.hSetting };

	int id = LOWORD(wParam);
	int code = HIWORD(wParam);

	switch (id)
	{
	case IDCLOSE:
		DestroyWindow(hWnd);
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
				break;	// 说明正在滚动，取消再次滚动

			// 启动计时器，开始滚动
			SetTimer(hWnd, chooseID::idt_scroll, glob::scrollInterval, nullptr);

			// 等待的计数归零
			waitNum = 0;

			break;
		}
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

void timer_idt_scroll(HWND hWnd)
{
	glob::scrollNum--;
	wstring nameOut;
	if (glob::scrollNum)	// 随机滚动没有结束
		nameOut = data.nameRandom();
	else
	{
		nameOut = data.nameOut();

		// 终止文本滚动
		KillTimer(hWnd, chooseID::idt_scroll);

		// 重置scrollNum
		glob::scrollNum = glob::scrollNumMax;
	}

	// 设置文本
	SetWindowText(GetDlgItem(hWnd, idc_stc_chooseBtn), nameOut.c_str());

#if _DEBUG
	if (nameOut == L"")
		MessageBox(nullptr, L"没有内容在nameOut里面", L"报错", MB_ICONWARNING);
#endif
}

LRESULT chooseOnTimer(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case chooseID::idt_scroll:
		timer_idt_scroll(hWnd);
		break;

	case idt_transparency:
		transparency::transparency(hWnd, glob::Mode::choose);
		break;

	case chooseID::idt_wait:
		waitNum++;
		if (waitNum != WAIT_NUM_MAX)
			break;

		waitNum = 0;	// 清零waitNum
		SetTimer(hWnd, idt_transparency, glob::TRANSPARENCY_INTERVAL, nullptr);	// 用于设置透明度修改时间
		glob::transparencyTimerActive = true;
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}
