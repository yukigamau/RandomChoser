module;
#include <Windows.h>
export module transparency;
import glob;

export namespace transparency
{
	void handleSwitchMode(glob::Mode m);	// 状态切换函数
	void updateWindowAlpha(HWND hwnd, glob::Mode m);	// 应用透明度渲染
	void transparency(HWND hwnd, glob::Mode m);
}