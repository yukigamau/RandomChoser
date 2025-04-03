module;

#include <ctime>
#include <Windows.h>
#include <gdiplus.h>
#include "resource.h"
#pragma comment(lib,"gdiplus.lib")
export module command;
import std;
import data;
import png;
import value;
using namespace Gdiplus;

// WindowProc的函数声明
export LRESULT CALLBACK WPsetting(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT CALLBACK WPchoose(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT CALLBACK WPicon(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 设置页面
export void settingPage(_In_ HINSTANCE hInstance)
{
	if (store.ifRight)
		initializeStore();	// 用于初始化存储数据，如果数据正常则重新设置，不使用默认值
	settingInstance = hInstance;	// 用于后续创建按钮

	// 注册窗口类
	const char* className = "设置页面";
	WNDCLASS wc = {};
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpfnWndProc = WPsetting; // 设置窗口过程函数
	wc.hInstance = GetModuleHandle(nullptr); // 获取实例句柄
	wc.hIcon = LoadIconA(hInstance, (char*)IDI_ICON1);
	wc.lpszClassName = className;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wc);

	int nWidth = 742 * 1.5 * dpiScale;
	int nHeight = 550 * dpiScale;
	LPCSTR lpWindowName = "点名器设置";
	// 创建窗口
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int x = (screenWidth - nWidth) / 2;
	int y = (screenHeight - nHeight) / 2;
	HWND hwnd = CreateWindow(
		className, lpWindowName,
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,	// 禁止窗口改变大小
		(GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2, (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2,
		nWidth, nHeight,
		nullptr, nullptr,
		wc.hInstance,	// 实例句柄
		nullptr			// 附加数据
	);

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);	// 把窗口显示到最前面
	UpdateWindow(hwnd);
}

void AddControl(HWND hwnd, HWND& control, LPCSTR text, DWORD style, int x, int y, int width,
	int height,	HMENU id)
{
	control = CreateWindow("BUTTON", text, style, x, y, width, height, hwnd, id, NULL, NULL);
}

void AddEdit(HWND hwnd, HWND& control, std::string text,
	int x, int y, int width, int height, HMENU id)
{
	control = CreateWindow("EDIT", text.c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER,
		x, y, width, height, hwnd, id, NULL, NULL);
}

void AddStatic(HWND& hwnd, LPCSTR text, short& x, short& y, short& sizeY)
{
	CreateWindow("STATIC", text, WS_CHILD | WS_VISIBLE, x, y, 150 * dpiScale, sizeY, hwnd, NULL, NULL, NULL);
}

// 回调函数，用于遍历所有子窗口
BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
	// 发送 WM_SETFONT 消息，lParam=TRUE 让控件立即重绘
	SendMessage(hwnd, WM_SETFONT, (WPARAM)lParam, TRUE);
	return TRUE;
}

// 修改指定窗口及其子控件的字体
void SetFontForAllControls(HWND hWndParent, HFONT hFont)
{
	// 枚举所有子控件
	EnumChildWindows(hWndParent, EnumChildProc, (LPARAM)hFont);
}

// 回调函数：用于枚举字体
int CALLBACK EnumFontFamExProc(const LOGFONT* lpelfe, const TEXTMETRIC* lpntme, DWORD FontType,
	LPARAM lParam)
{
	if (lpelfe->lfCharSet != GB2312_CHARSET)
	{
		return 1; // 过滤非中文字体
	}

	HWND hComboBox = (HWND)lParam;
	char fontName[LF_FACESIZE];
	int count = SendMessage(hComboBox, CB_GETCOUNT, 0, 0);

	// 避免重复添加
	for (int i = 0; i < count; i++)
	{
		SendMessage(hComboBox, CB_GETLBTEXT, i, (LPARAM)fontName);
		if (strcmp(fontName, lpelfe->lfFaceName) == 0)
		{
			return 1;
		}
	}

	// 添加到列表
	SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)lpelfe->lfFaceName);

	return 1;
}

// 填充字体列表，并为每个字体创建句柄
void PopulateFontList(HWND hComboBox, std::map<std::string, HFONT>& fontMap)
{
	HDC hdc = GetDC(NULL);
	LOGFONT logFont = { 0 };
	logFont.lfCharSet = DEFAULT_CHARSET;
	EnumFontFamiliesEx(hdc, &logFont,
		[](const LOGFONT* lpelfe, const TEXTMETRIC* lptm, DWORD FontType, LPARAM lParam) -> int
		{
			HWND hComboBox = (HWND)lParam;
			std::string fontName = lpelfe->lfFaceName;

			// 过滤掉 @ 开头的字体
			if (fontName[0] != '@')
			{
				// 确保不重复添加
				if (SendMessage(hComboBox, CB_FINDSTRINGEXACT, -1, (LPARAM)fontName.c_str()) == CB_ERR)
				{
					SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)fontName.c_str());
				}
			}
			return 1; // 继续枚举
		}, (LPARAM)hComboBox, 0);
	ReleaseDC(NULL, hdc);

	int count = SendMessage(hComboBox, CB_GETCOUNT, 0, 0);
	for (int i = 0; i < count; i++)
	{
		char fontName[LF_FACESIZE];
		SendMessage(hComboBox, CB_GETLBTEXT, i, (LPARAM)fontName);

		// 创建字体对象
		HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontName);
		fontMap[fontName] = hFont;
	}

	// 选中数据中的字体
	int index = SendMessage(hComboBox, CB_FINDSTRINGEXACT, -1, (LPARAM)store.fontName.c_str());
	SendMessage(hComboBox, CB_SETCURSEL, index, 0);
}

// 简化颜色设置控件代码
void addColor(HWND& hWnd, short& width, std::string staticText, COLORREF& color,
	HWND child, int id, short& x, short& y, short& sizeY, short xCommand)
{	// xCommand不引用，以防修改初始值
	// 颜色数据处理
	int red = GetRValue(color);
	int green = GetGValue(color);
	int blue = GetBValue(color);
	// 控件创建
	AddStatic(hWnd, staticText.c_str(), x, y, sizeY);
	AddStatic(hWnd, "16进制", xCommand, y, sizeY);
	xCommand += width * 2.5;

	// 确保16位值正确
	std::string color16 = std::format("{:06x}", color);
	colorCorrect(color16);
	AddEdit(hWnd, child, color16, xCommand, y, width * 3, sizeY, (HMENU)id);
	y += yAdd;
}

// 句柄
HWND hTopMostYes, hTopMostNo;

export void settingDraw(HWND& hwnd, HDC& hdc, LPARAM& lParam)
{
	// 坐标用
	short sizeY = 20 * dpiScale;
	short x = 10 * dpiScale;
	short y = 10 * dpiScale;
	short xCommandFirst = 160 * dpiScale;
	short xCommand = xCommandFirst;

	// 设置字体
	if (!hFSetting)
	{
		hFSetting = CreateFont(sizeY, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, store.fontName.c_str());
	}
	SelectObject(hdc, hFSetting);
	SIZE sizeOfFont{};
	GetTextExtentPoint32(hdc, "一", 3, &sizeOfFont);
	short width = sizeOfFont.cx;

	/* 置顶选项 */
	AddStatic(hwnd, "是否置顶", x, y, sizeY);
	AddControl(hwnd, hTopMostYes, "是", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		xCommand, y, 40 * dpiScale, 25 * dpiScale, (HMENU)IDB_topYes);
	AddControl(hwnd, hTopMostNo, "否", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		xCommand + 50 * dpiScale, 10 * dpiScale, 40 * dpiScale, 25 * dpiScale, (HMENU)IDB_topNo);
	// 设置初始值
	if (store.ifRight && !store.ifTop)
		SendMessage(hTopMostNo, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(hTopMostYes, BM_SETCHECK, BST_CHECKED, 0);
	y += yAdd;

	addColor(hwnd, width, "标题栏背景颜色", store.captionBC, hCaptionBC16, IDE_captionBC16,
		x, y, sizeY, xCommand);

	addColor(hwnd, width, "标题栏字体颜色", store.captionFC, hCaptionFC16, IDE_captionFC16,
		x, y, sizeY, xCommand);

	addColor(hwnd, width, "抽取区背景颜色", store.clientBC, hClientBC16, IDE_clientBC16,
		x, y, sizeY, xCommand);

	addColor(hwnd, width, "抽取区字体颜色", store.clientFC, hClientFC16, IDE_clientFC16,
		x, y, sizeY, xCommand);

	AddStatic(hwnd, "选择字体", x, y, sizeY);
	hFontName = CreateWindow("COMBOBOX", "",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN,
		xCommand, y, 200 * dpiScale, sizeY * 10, hwnd, (HMENU)IDL_fontName, NULL, NULL);
	PopulateFontList(hFontName, fontMap);
	y += yAdd * 2;

	// 教学文本
	hCopyright = CreateWindow("STATIC", "软件著作权由github用户yukigamau所有，服从BSD 3-Clause。",
		WS_CHILD | WS_VISIBLE, x, y, width * 26, sizeY, hwnd, NULL, NULL, NULL);
	y += sizeY;
	hAttention = CreateWindow("STATIC", "如果看不懂，就直接创建名单。",
		WS_CHILD | WS_VISIBLE, x, y, width * 13, sizeY, hwnd, NULL, NULL, NULL);	// 将被标红
	y += sizeY;
	CreateWindow("STATIC", "设置将以…的样子显示在抽取区上面那个标题栏上，用户使用时可看到。",
		WS_CHILD | WS_VISIBLE, x, y, width * 26, sizeY, hwnd, NULL, NULL, NULL);
	y += sizeY;
	CreateWindow("STATIC", "用户请不要私自修改本产品的任何文件于不会更新软件时。",
		WS_CHILD | WS_VISIBLE, x, y, width * 28, sizeY, hwnd, NULL, NULL, NULL);
	y += sizeY;
	CreateWindow("STATIC", "软件8秒时间不用会变圆。",
		WS_CHILD | WS_VISIBLE, x, y, width * 23, sizeY, hwnd, NULL, NULL, NULL);
	y += sizeY;
	CreateWindow("STATIC", "更新网站1：www.github.com/yukigamau/RandomChoser",
		WS_CHILD | WS_VISIBLE, x, y, width * 30, sizeY, hwnd, NULL, NULL, NULL);
	y += sizeY;
	CreateWindow("STATIC", "更新网站2：randomChoser.netlify.app【没有www】",
		WS_CHILD | WS_VISIBLE, x, y, width * 23, sizeY, hwnd, NULL, NULL, NULL);
	y += sizeY;
	std::string nowVersion = "当前产品版本：" + versionText;
	CreateWindow("STATIC", nowVersion.c_str(),
		WS_CHILD | WS_VISIBLE, x, y, width * 23, sizeY, hwnd, NULL, NULL, NULL);
	y += yAdd;

	hConfirmBtn = CreateWindow("BUTTON", "确定好～\\(≧▽≦)/～啦啦啦（包括右边的名单）",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		x, y, width * 22, sizeY + 10, hwnd, (HMENU)IDB_confirm, NULL, NULL);

	// 右边的名单区域
	short span = 650 * dpiScale;	// 到右边的跨度
	x += span;
	y = 10 * dpiScale;
	xCommand += span;

	AddStatic(hwnd, "选择默认名单", x, y, sizeY);
	hDefaultList = CreateWindow("COMBOBOX", "",
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
		xCommand, y, 200 * dpiScale, sizeY * 10, hwnd, (HMENU)IDL_defaultList, NULL, NULL);
	SendMessage(hDefaultList, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));	// 取消选中
	if (store.defaultList == "")
		EnableWindow(hDefaultList, FALSE);	// 禁用控件
	else
	{
		for (short i = 0; i < store.all.size(); i++)
		{
			std::string addChoice = removeFormat(store.all[i][0]);
			SendMessage(hDefaultList, CB_ADDSTRING, 0, (LPARAM)addChoice.c_str());
			if(addChoice==store.defaultList)
				SendMessage(hDefaultList, CB_SETCURSEL, i, 0);	// 设置默认项
			store.currentShowList = store.defaultList;
		}
	}
	y += yAdd;

	AddStatic(hwnd, "选择显示名单", x, y, sizeY);
	hShowList = CreateWindow("COMBOBOX", "",
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
		xCommand, y, 200 * dpiScale, sizeY * 10, hwnd, (HMENU)IDL_showList, NULL, NULL);
	SendMessage(hShowList, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));	// 取消选中
	if (!store.ifRight || ifTypeName)
	{
		EnableWindow(hShowList, FALSE);	// 禁用控件
		showName = false;
	}
	else
	{
		for (short i = 0; i < store.all.size(); i++)
		{
			// 取消all的格式
			std::string addChoice = removeFormat(store.all[i][0]);
			SendMessage(hShowList, CB_ADDSTRING, 0, (LPARAM)addChoice.c_str());
			if (addChoice == store.currentShowList)
				SendMessage(hShowList, CB_SETCURSEL, i, 0);	// 设置默认项
		}
		showName = true;
	}
	y += yAdd;

	AddControl(hwnd, hNewListBtn, "新建名单", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		x, y, width * 5, sizeY + 10 * dpiScale, (HMENU)IDB_newList);
	AddControl(hwnd, hModifyBtn, "修改名单", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		x + width * 5 + 24 * dpiScale, y, width * 5, sizeY + 10 * dpiScale, (HMENU)IDB_modify);
	if (!store.ifRight)
		EnableWindow(hModifyBtn, FALSE);	// 由于没有正确的名单，所以禁用
	AddControl(hwnd, hDeleteBtn, "删除名单", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		x + width * 10 + 48 * dpiScale, y, width * 5, sizeY + 10 * dpiScale, (HMENU)IDB_delete);
	if (!store.ifRight)
		EnableWindow(hDeleteBtn, FALSE);	// 由于没有正确的名单，所以禁用
	y += yAdd;
	
	RECT rect;
	GetClientRect(hwnd, &rect);
	short nWidth = rect.right - x;
	short nHeight = rect.bottom - y;
	// 处理显示文本
	std::string nameStr;
	if (store.ifRight && store.defaultList != "" && createSetting)
		readName = store.defaultNames;
	for (std::string str : readName)
		nameStr += str + "\r\n";
	createSetting = false;
	// 创建窗口
	hNameEdit = CreateWindow("EDIT", nameStr.c_str(),
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL |
		ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE,
		x, y, nWidth, nHeight, hwnd, (HMENU)IDE_names, nullptr, nullptr);

	// 设置子控件字体
	SetFontForAllControls(hwnd, hFSetting);
}

export void drawFontList(LPARAM lParam)
{
	LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
	if (lpdis->CtlID == IDL_fontName)
	{	// 字体列表框 ID
		char fontName[LF_FACESIZE];
		SendMessage(lpdis->hwndItem, CB_GETLBTEXT, lpdis->itemID, (LPARAM)fontName);

		// 获取字体
		HFONT hFont = fontMap[fontName];
		HDC hdc = lpdis->hDC;
		SelectObject(hdc, hFont);

		// 设置背景和前景颜色
		SetBkColor(hdc, GetSysColor(lpdis->itemState & ODS_SELECTED ? COLOR_HIGHLIGHT : COLOR_WINDOW));
		SetTextColor(hdc, GetSysColor(lpdis->itemState & ODS_SELECTED ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));

		// 填充背景
		ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &lpdis->rcItem, NULL, 0, NULL);

		// 绘制字体名
		DrawText(hdc, fontName, -1, &lpdis->rcItem, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
	}

}

// 抽取页面
HWND hChoose;	// 方便频繁转换
export void choosePage(_In_ HINSTANCE hInstance)
{
	if(hChoose==NULL)// 创建窗口
	{
		// 注册窗口类
		const char* className = "抽取页面";
		WNDCLASS wc = {};
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpfnWndProc = WPchoose;	// 设置窗口过程函数
		wc.hInstance = GetModuleHandle(nullptr);	// 获取实例句柄
		wc.hIcon = LoadIconA(hInstance, (char*)IDI_ICON1);
		wc.lpszClassName = className;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClass(&wc);

		LPCSTR lpWindowName = "点名器";
		hChoose = CreateWindowEx(
			WS_EX_LAYERED | WS_EX_TOPMOST,
			className, lpWindowName,
			WS_POPUP,
			(GetSystemMetrics(SM_CXSCREEN) - chooseWidth) / 2,
			(GetSystemMetrics(SM_CYSCREEN) - chooseHeight) / 2,   // 窗口位置
			chooseWidth, chooseHeight,                // 窗口大小
			nullptr,                        // 父窗口句柄
			nullptr,                        // 菜单句柄
			wc.hInstance,                   // 实例句柄
			nullptr                         // 附加数据
		);
		SetLayeredWindowAttributes(hChoose, 0, 255, LWA_ALPHA); // 设置窗口为支持透明

		if (!data.ifTop)	// 置顶设置处理
			SetWindowPos(hChoose, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	ShowWindow(hChoose, SW_SHOW); // 显示窗口
	UpdateWindow(hChoose);
}

export void chooseDraw(HWND& hWnd, HDC& hdc)	// mode表示是否处于滚动状态
{
	// 获取窗口的矩形坐标
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	// 创建画笔和画刷
	HBRUSH hBrushBk = CreateSolidBrush(data.clientBC);   // 背景画刷
	SelectObject(hdc, hBrushBk);
	// 绘制底色矩形
	Rectangle(hdc, 0, captionHeight - 1, clientRect.right, clientRect.bottom);
	// 删除 GDI 对象
	DeleteObject(hBrushBk);
	
	// 创建画笔和画刷
	HPEN hPen = CreatePen(PS_SOLID, 2 * dpiScale, data.captionBC); // 边框画笔
	HBRUSH hBrush = CreateSolidBrush(data.captionBC);   // 背景画刷
	SelectObject(hdc, hPen);
	SelectObject(hdc, hBrush);
	// 绘制标题栏矩形
	Rectangle(hdc, 0, 0, clientRect.right, captionHeight);
	// 删除 GDI 对象
	DeleteObject(hBrush);
	DeleteObject(hPen);

	// 设置文本颜色和背景模式
	SetTextColor(hdc, data.captionFC);	// 也用于下面的按钮
	SetBkMode(hdc, TRANSPARENT);	// 背景透明
	// 计算标题位置
	RECT textRect = { 0,0,clientRect.right,captionHeight };
	textRect.left += 10 * dpiScale;	// 给文字留点空间
	textRect.right -= 50 * dpiScale;	// 避开关闭按钮
	// 字体
	if (!hFCaption)
	{
		hFCaption = CreateFont(
			(textRect.bottom - textRect.top) * 0.8,	// 字体高度
			0, 0, 0,
			FW_NORMAL,	// 字体粗细
			FALSE,          // 是否斜体
			FALSE,          // 是否下划线
			FALSE,          // 是否删除线
			DEFAULT_CHARSET,// 字符集
			OUT_DEFAULT_PRECIS,   // 输出精度
			CLIP_DEFAULT_PRECIS,  // 裁剪精度
			CLEARTYPE_QUALITY,      // 输出质量
			DEFAULT_PITCH | FF_SWISS,	// 字体间距和家族
			data.fontName.c_str()
		);
	}
	SelectObject(hdc, hFCaption);
	// 绘制文字
	DrawText(hdc, chooseTitle.c_str(), -1, &textRect, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

	// 关闭按钮
	RECT buttonRect = { clientRect.right - buttonSize - 10 * dpiScale, 2 * dpiScale,
		clientRect.right - 10 * dpiScale, buttonSize + 2 * dpiScale };
	// 绘制关闭按钮上的“×”，与标题使用同一字体
	DrawText(hdc, "×", -1, &buttonRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	// 设置按钮
	buttonRect.right -= buttonSize;
	buttonRect.left -= buttonSize;
	DrawText(hdc, "…", -1, &buttonRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	// 客户区
	// 字体
	SetTextColor(hdc, data.clientFC);
	SetBkMode(hdc, TRANSPARENT);
	if (!hFText)
	{
		hFText = CreateFont(
			(clientRect.bottom - captionHeight) * 0.7,	// 字体高度
			0,              // 字体宽度（为 0 表示根据高度自动计算）
			0,              // 字体倾斜角度（单位 0.1 度）
			0,              // 字体基线方向角度
			FW_NORMAL,		// 字体粗细
			FALSE,          // 是否斜体
			FALSE,          // 是否下划线
			FALSE,          // 是否删除线
			DEFAULT_CHARSET,// 字符集
			OUT_DEFAULT_PRECIS,   // 输出精度
			CLIP_DEFAULT_PRECIS,  // 裁剪精度
			CLEARTYPE_QUALITY,		// 输出质量
			DEFAULT_PITCH | FF_SWISS,	// 字体间距和家族
			data.fontName.c_str()
		);
	}
	// 打印文本
	hChooseText = CreateWindow("STATIC", chooseText.c_str(), WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
		0, captionHeight, clientRect.right, clientRect.bottom - captionHeight,
		hWnd, (HMENU)IDS_chooseText, NULL, NULL);
	// 设置字体
	SendMessage(hChooseText, WM_SETFONT, (WPARAM)hFText, TRUE);
}

export bool captionMessage(HWND& hWnd, LPARAM& lParam)
{
	POINT ptMouse;
	ptMouse.y = HIWORD(lParam);
	ScreenToClient(hWnd, &ptMouse);
	if (ptMouse.y <= captionHeight)
		return true;  // 返回 HTCAPTION，表示点击在标题栏，可以拖动窗口
	else
		return false;
}

export short captionButton(HWND& hWnd, LPARAM& lParam)
{
	POINT ptMouse;
	ptMouse.x = LOWORD(lParam);
	ScreenToClient(hWnd, &ptMouse);
	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);
	if (ptMouse.x >= windowRect.right - windowRect.left - buttonSize - 10 * dpiScale)
		return 1;
	else if (ptMouse.x >= windowRect.right - windowRect.left - 2 * buttonSize - 10 * dpiScale)
		return 2;
	else
		return 0;
}

// 图标页面

// 用于窗口位置调节
void offset(HWND& hwnd, RECT& newPosition, short width, short height)
{
	// 获取原始窗口的大小和位置
	RECT originalRect;
	GetWindowRect(hwnd, &originalRect);

	// 检查窗口的位置偏好
	bool right = false;
	bool down = true;

	// 修改位置参数
	// 水平
	if (GetSystemMetrics(SM_CXSCREEN) / 2 >
		(originalRect.right + originalRect.left) / 2)
	{
		right = false;	// 偏左
		if (originalRect.left < 0)
			newPosition.left = 0;
		else
			newPosition.left = originalRect.left;
	}
	else
	{
		right = true;	// 偏右或居中
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);  // 屏幕宽度
		if (originalRect.right > screenWidth)
			newPosition.left = screenWidth - width;
		else
			newPosition.left = originalRect.right - width;
	}
	// 垂直
	if (GetSystemMetrics(SM_CYSCREEN) / 2 >
		(originalRect.bottom + originalRect.top) / 2)
	{
		down = false;	// 偏上
		if (originalRect.top < 0)
			newPosition.top = 0;
		else
			newPosition.top = originalRect.top;
	}
	else
	{
		down = true;	// 偏下或居中
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);  // 屏幕高度
		if (originalRect.bottom > screenHeight)
			newPosition.top = screenHeight - height;
		else
			newPosition.top = originalRect.bottom - height;
	}
}

HWND hIcon;	// 方便频繁转换
export void iconPage()
{
	RECT newPosition;
	offset(hChoose, newPosition, g_sizeWnd.cx, g_sizeWnd.cy);

	const char* iconWndName = "icon";
	// 创建窗口
	// 注册新窗口类型
	WNDCLASS iconWc = {};
	iconWc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	iconWc.hInstance = GetModuleHandle(nullptr); // 获取实例句柄
	iconWc.lpfnWndProc = WPicon;
	iconWc.lpszClassName = iconWndName;
	iconWc.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&iconWc);

	// 设置新窗口
	hIcon = CreateWindowEx(
		WS_EX_LAYERED,
		iconWndName,	// 窗口类名
		"点名器图标模式",		// 窗口标题
		WS_POPUP,	// 窗口风格
		newPosition.left, newPosition.top,	// 窗口位置
		g_sizeWnd.cx, g_sizeWnd.cy,			// 窗口大小
		nullptr,			// 父窗口句柄
		nullptr,			// 菜单句柄
		iconWc.hInstance,	// 实例句柄
		nullptr				// 附加数据
	);

	// 显示窗口
	ShowWindow(hChoose, SW_HIDE);
	ShowWindow(hIcon, SW_SHOWNOACTIVATE);
	UpdateWindow(hIcon);

	// 关闭抽取窗口计时
	KillTimer(hChoose, IDT_wait);

	// 调整位置
	HWND set = data.ifTop ? HWND_TOPMOST : HWND_NOTOPMOST;
	SetWindowPos(hIcon, set, newPosition.left, newPosition.top, 0, 0, SWP_NOSIZE);
	iconPng(hIcon);
}

export void ExitIconMode()
{
	ShowWindow(hIcon, SW_HIDE);
	ShowWindow(hChoose, SW_SHOW);

	SetTimer(hChoose, IDT_wait, 1000, 0);

	RECT newPosition;
	offset(hIcon, newPosition, chooseWidth, chooseHeight);
	SetWindowPos(hChoose, nullptr, newPosition.left, newPosition.top, 0, 0, SWP_NOSIZE);
}

// 其它
export void deleteAllCommand(HWND& hWnd)
{
	// 删除所有控件
	HWND child;
	child = GetWindow(hWnd, GW_CHILD); // 获取第一个子窗口
	while (child)
	{
		DestroyWindow(child); // 销毁子窗口
		child = GetWindow(hWnd, GW_CHILD); // 获取下一个子窗口
	}
}

export std::vector<std::string> readEdit(HWND& hwnd, short ID)
{
	std::vector<std::string> lines;

	// 获取 EditBox 句柄
	HWND hEdit = GetDlgItem(hwnd, ID);
	if (!hEdit) return lines;

	// 获取文本长度
	int len = GetWindowTextLengthA(hEdit);
	if (!len) return lines;  // 如果没有内容，直接返回

	// 读取文本
	std::vector<char> buffer(len + 1);
	GetWindowTextA(hEdit, buffer.data(), len + 1);

	// 按行分割字符串
	std::stringstream ss(buffer.data());
	std::string line;
	while (std::getline(ss, line))
		lines.push_back(line);

	return lines;
}

export bool deleteVoid(std::vector<std::string>& vec)
{
	int size = vec.size();

	for (int i = 0; i < size; i++)
	{
		if (!vec[i].size())
		{
			vec.erase(vec.begin() + i);	// 去除空的
			i--;	// 防止遗漏
			size--;	// 防止溢出
		}
	}

	if (!vec.size())	// 没有元素
		return false;
	else
		return true;
}

short change = 15;	// 透明度变化
export void transparency(HWND& hwnd, short& mode)
{
	if (changeAlpha)	// 增加透明度
	{
		currentAlpha += change;
		if (currentAlpha >= 255)
		{
			KillTimer(hwnd, IDT_transparency);
			changeAlpha = false;
			transparencyTimerActive = false;
		}
	}
	else // 减少透明度
	{
		currentAlpha -= change;  // 每次减少透明度
		if (currentAlpha <= change)
		{
			KillTimer(hwnd, IDT_transparency);
			changeAlpha = true;
			if (mode == normal)
			{
				mode = icon;	// 切换模式标记为图标
				iconPage();
				SetTimer(hIcon, IDT_transparency, transparencyT, 0);
			}
			else if (mode == icon)
			{
				mode = normal;	// 切换模式标记为抽取
				ExitIconMode();
				SetTimer(hChoose, IDT_transparency, transparencyT, 0);
				SetTimer(hChoose, IDT_scroll, scrollT, 0);	// 同时开始滚动以优化使用
			}
		}
	}

	// 更新透明度
	if (mode == normal)
		SetLayeredWindowAttributes(hwnd, 0, currentAlpha, LWA_ALPHA);
	else if (mode == icon)
	{
		BLENDFUNCTION blend = { 0 };
		blend.BlendOp = AC_SRC_OVER;
		blend.SourceConstantAlpha = (BYTE)currentAlpha;
		blend.AlphaFormat = AC_SRC_ALPHA;
		UpdateLayeredWindow(
			hwnd,           // 窗口句柄
			NULL,           // 目标 HDC（设为 NULL）
			NULL,           // pptDst = NULL（位置不变）
			NULL,           // psize = NULL（尺寸不变）
			g_hdcMem,         // 源 HDC（含带透明度的图像）
			NULL,           // pptSrc = (0,0)
			RGB(0, 0, 0),     // 颜色键（未使用）
			&blend,         // 混合函数（启用 Alpha）
			ULW_ALPHA       // 使用 Alpha 混合
		);
	}
}

export void selectChange(HWND& hwnd, std::string& str)
{
	int index = SendMessage(hwnd, CB_GETCURSEL, 0, 0);	// 获取当前选中索引
	int length = SendMessage(hwnd, CB_GETLBTEXTLEN, index, 0);
	str.resize(length);
	SendMessage(hwnd, CB_GETLBTEXT, index, (LPARAM)str.data());	// 获取文本
}

export void selfRestart(HWND& hwnd)
{
	// 获取当前程序的路径
	wchar_t szPath[MAX_PATH];
	GetModuleFileNameW(NULL, szPath, MAX_PATH);

	// 创建进程启动信息
	STARTUPINFOW si = { sizeof(STARTUPINFOW) };
	PROCESS_INFORMATION pi;

	// 启动新的进程
	CreateProcessW(
		szPath,   // 当前程序路径
		NULL,      // 命令行参数
		NULL,      // 进程安全属性
		NULL,      // 线程安全属性
		FALSE,     // 不继承句柄
		0,         // 创建标志
		NULL,      // 环境变量
		NULL,      // 当前目录
		&si,       // 启动信息
		&pi);    // 进程信息

	// 退出当前进程
	ExitProcess(0);
}