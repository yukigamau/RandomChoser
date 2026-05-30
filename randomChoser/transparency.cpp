module transparency;
#include <Windows.h>
import id;	// 注意这里会有两个窗口（抽取和图标，保险起见，不要用id的命名空间）
import glob;
import window;

using window::wps;

// glob::Mode
using _Mode = glob::Mode;

/* 透明变换动画 */
bool changeAlpha = false;	// 标志更改透明度是增加还是减少，true为增加
BYTE currentAlpha = 255; // 当前透明度，初始为完全不透明
const int transparencyChange = 15;	// 透明度变化

void transparency::handleSwitchMode(_Mode m)
{
	const HWND &hChoose = wps.getHChoose();

	if (m == _Mode::choose)
	{
		wps.inIconPage();
		SetTimer(wps.getHIcon(), chooseID::idt_transparency, glob::TRANSPARENCY_INTERVAL, nullptr);
	}
	else if (m == _Mode::icon)
	{
		wps.outIconPage();
		// 显式确保 hChoose 的分层属性正确
		SetLayeredWindowAttributes(hChoose, 0, currentAlpha, LWA_ALPHA);

		SetTimer(hChoose, chooseID::idt_transparency, glob::TRANSPARENCY_INTERVAL, nullptr);
		SetTimer(hChoose, chooseID::idt_scroll, glob::scrollInterval, nullptr);
		SetTimer(hChoose, chooseID::idt_wait, 1000, nullptr);
	}
}

void transparency::updateWindowAlpha(HWND hwnd, _Mode m)
{
	if (m == _Mode::choose)
	{
		SetLayeredWindowAttributes(hwnd, 0, currentAlpha, LWA_ALPHA);
	}
	else if (m == _Mode::icon)
	{
		BLENDFUNCTION blend = { 0 };
		blend.BlendOp = AC_SRC_OVER;
		blend.SourceConstantAlpha = (BYTE)currentAlpha;
		blend.AlphaFormat = AC_SRC_ALPHA;

		const HDC hdcMem = wps.getHdcMem();
		UpdateLayeredWindow(hwnd, nullptr, nullptr, nullptr, hdcMem, nullptr, 0, &blend, ULW_ALPHA);
	}
}

void transparency::transparency(HWND hwnd, _Mode m)
{
	if (changeAlpha) // 增加透明度（淡入）
	{
		if (currentAlpha + transparencyChange >= 255)
		{
			currentAlpha = 255;
			KillTimer(hwnd, chooseID::idt_transparency);
			changeAlpha = false;
			glob::transparencyTimerActive = false;
		}
		else
		{
			currentAlpha += transparencyChange;
		}
	}
	else // 减少透明度（淡出）
	{
		if (currentAlpha - transparencyChange <= transparencyChange)
		{
			currentAlpha = 0;
			KillTimer(hwnd, chooseID::idt_transparency);
			changeAlpha = true;

			// 抽取出去的状态切换逻辑
			transparency::handleSwitchMode(m);
			return;
		}
		currentAlpha -= transparencyChange;
	}

	// 抽取出去的渲染逻辑
	transparency::updateWindowAlpha(hwnd, m);
}