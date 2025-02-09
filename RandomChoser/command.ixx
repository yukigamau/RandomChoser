module;

#include <ctime>
#include <Windows.h>
#include "resource.h"

export module command;
import std;
import data;
import value;

// WindowProc的函数声明
export LRESULT CALLBACK WPsetting(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT CALLBACK WPchoose(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT CALLBACK WPicon(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 其它数据
int capationHeight = GetSystemMetrics(SM_CYCAPTION);
int buttonSize = 20;
std::mt19937 engine(static_cast<unsigned int>(time(0)));	// 随机数于随机滚动

// 用于设置页面绘制

// 设置页面
export void settingPage(_In_ HINSTANCE hInstance)
{
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

	int nWidth = 742 * 1.5;
	int nHeight = 550;
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

export void AddControl(HWND hwnd, HWND& control, LPCSTR text, DWORD style, int x, int y, int width,
	int height,	HMENU id)
{
	control = CreateWindow("BUTTON", text, style, x, y, width, height, hwnd, id, NULL, NULL);
}

export void AddEdit(HWND hwnd, HWND& control, std::string text,
	int x, int y, int width, int height, HMENU id)
{
	control = CreateWindow("EDIT", text.c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER,
		x, y, width, height, hwnd, id, NULL, NULL);
}

export void AddStatic(HWND& hwnd, LPCSTR text, short& x, short& y, short& sizeY)
{
	CreateWindow("STATIC", text, WS_CHILD | WS_VISIBLE, x, y, 150, sizeY, hwnd, NULL, NULL, NULL);
}

// 回调函数，用于遍历所有子窗口
export BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
	// 发送 WM_SETFONT 消息，lParam=TRUE 让控件立即重绘
	SendMessage(hwnd, WM_SETFONT, (WPARAM)lParam, TRUE);
	return TRUE;
}

// 修改指定窗口及其子控件的字体
export void SetFontForAllControls(HWND hWndParent, HFONT hFont)
{
	// 枚举所有子控件
	EnumChildWindows(hWndParent, EnumChildProc, (LPARAM)hFont);
}

// 回调函数：用于枚举字体
export int CALLBACK EnumFontFamExProc(const LOGFONT* lpelfe, const TEXTMETRIC* lpntme, DWORD FontType,
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
export void PopulateFontList(HWND hComboBox, std::map<std::string, HFONT>& fontMap)
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
	std::vector<HWND>child, std::vector<int> id, short& x, short& y, short& sizeY, short xCommand)
{	// xCommand不引用，以防修改初始值
	// 颜色数据处理
	int red = GetRValue(color);
	int green = GetGValue(color);
	int blue = GetBValue(color);
	// 控件创建
	AddStatic(hWnd, staticText.c_str(), x, y, sizeY);

	AddStatic(hWnd, "R", xCommand, y, sizeY);
	xCommand += width / 2;

	AddEdit(hWnd, child[0], std::format("{}", red), xCommand, y, width * 2, sizeY, (HMENU)id[0]);
	xCommand += width * 3;

	AddStatic(hWnd, "G", xCommand, y, sizeY);
	xCommand += width / 2;

	AddEdit(hWnd, child[1], std::format("{}", green), xCommand, y, width * 2, sizeY, (HMENU)id[1]);
	xCommand += width * 3;

	AddStatic(hWnd, "B", xCommand, y, sizeY);
	xCommand += width / 2;

	AddEdit(hWnd, child[2], std::format("{}", blue), xCommand, y, width * 2, sizeY, (HMENU)id[2]);
	xCommand += width * 3;

	AddStatic(hWnd, "16进制", xCommand, y, sizeY);
	xCommand += width * 2.5;

	// 确保16位值正确
	std::string color16 = std::format("{:06x}", color);
	colorCorrect(color16);
	AddEdit(hWnd, child[3], color16, xCommand, y, width * 3, sizeY, (HMENU)id[3]);
	y += 40;
}

// 句柄
HWND hTopMostYes, hTopMostNo;

export void settingDraw(HWND& hwnd, HDC& hdc, LPARAM& lParam)
{
	// 坐标用
	short sizeY = 20;
	short x = 10;
	short y = 10;
	short xCommandFirst = 160;
	short xCommand = xCommandFirst;
	// 设置字体
	if (!hFSetting)
	{
		hFSetting = CreateFont(sizeY, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, store.fontName.c_str());
	}
	SelectObject(hdc, hFSetting);
	SIZE sizeOfFont{};
	GetTextExtentPoint32(hdc, "一", 3, &sizeOfFont);
	short width = sizeOfFont.cx;

	AddStatic(hwnd, "是否置顶", x, y, sizeY);
	AddControl(hwnd, hTopMostYes, "是", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		xCommand, y, 40, 25, (HMENU)IDB_topYes);
	AddControl(hwnd, hTopMostNo, "否", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		xCommand + 50, 10, 40, 25, (HMENU)IDB_topNo);
	// 设置初始值
	if (store.ifRight && !store.ifTop)
		SendMessage(hTopMostNo, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(hTopMostYes, BM_SETCHECK, BST_CHECKED, 0);
	y += 40;

	addColor(hwnd, width, "标题栏背景颜色", store.captionBC,
		{ hCaptionBCR ,hCaptionBCG ,hCaptionBCB ,hCaptionBC16 },
		{ IDE_captionBCR ,IDE_captionBCG ,IDE_captionBCB ,IDE_captionBC16 },
		x, y, sizeY, xCommand);

	addColor(hwnd, width, "标题栏字体颜色", store.captionFC,
		{ hCaptionFCR ,hCaptionFCG ,hCaptionFCB ,hCaptionFC16 },
		{ IDE_captionFCR ,IDE_captionFCG ,IDE_captionFCB ,IDE_captionFC16 },
		x, y, sizeY, xCommand);

	addColor(hwnd, width, "抽取区背景颜色", store.clientBC,
		{ hClientBCR ,hClientBCG ,hClientBCB ,hClientBC16 },
		{ IDE_clientBCR ,IDE_clientBCG ,IDE_clientBCB ,IDE_clientBC16 },
		x, y, sizeY, xCommand);

	addColor(hwnd, width, "抽取区字体颜色", store.clientFC,
		{ hClientFCR ,hClientFCG ,hClientFCB ,hClientFC16 },
		{ IDE_clientFCR ,IDE_clientFCG ,IDE_clientFCB ,IDE_clientFC16 },
		x, y, sizeY, xCommand);

	AddStatic(hwnd, "选择字体", x, y, sizeY);
	hFontName = CreateWindow("COMBOBOX", "",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL |
		CBS_DROPDOWN | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS,
		xCommand, y, 200, sizeY * 10, hwnd, (HMENU)IDL_fontName, NULL, NULL);
	PopulateFontList(hFontName, fontMap);
	y += 40;

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
	CreateWindow("STATIC", "产品将在github网站上更新，可在浏览器搜索RandomChoser找到。",
		WS_CHILD | WS_VISIBLE, x, y, width * 30, sizeY, hwnd, NULL, NULL, NULL);
	y += sizeY;
	CreateWindow("STATIC", "如果用户不会用github，可在网上搜索，但这样的话不建议更新。",
		WS_CHILD | WS_VISIBLE, x, y, width * 23, sizeY, hwnd, NULL, NULL, NULL);
	y += sizeY;
	CreateWindow("STATIC", "当前产品版本：1.0",
		WS_CHILD | WS_VISIBLE, x, y, width * 23, sizeY, hwnd, NULL, NULL, NULL);
	y += 40;

	hConfirmBtn = CreateWindow("BUTTON", "确定好～\\(≧▽≦)/～啦啦啦（包括右边的名单）",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		x, y, width * 22, sizeY + 10, hwnd, (HMENU)IDB_confirm, NULL, NULL);

	// 右边的名单区域
	short span = 650;	// 到右边的跨度
	x += span;
	y = 10;
	xCommand += span;

	AddStatic(hwnd, "选择默认名单", x, y, sizeY);
	hDefaultList = CreateWindow("COMBOBOX", "",
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
		xCommand, y, 200, sizeY * 10, hwnd, (HMENU)IDL_defaultList, NULL, NULL);
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
	y += 40;

	AddStatic(hwnd, "选择显示名单", x, y, sizeY);
	hShowList = CreateWindow("COMBOBOX", "",
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
		xCommand, y, 200, sizeY * 10, hwnd, (HMENU)IDL_showList, NULL, NULL);
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
	y += 40;

	AddControl(hwnd, hNewListBtn, "新建名单", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		x, y, width * 5, sizeY + 10, (HMENU)IDB_newList);
	AddControl(hwnd, hModifyBtn, "修改名单", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		x + width * 5 + 10, y, width * 5, sizeY + 10, (HMENU)IDB_modify);
	if (!store.ifRight)
		EnableWindow(hModifyBtn, FALSE);	// 由于没有正确的名单，所以禁用
	AddControl(hwnd, hDeleteBtn, "删除名单", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		x + width * 10 + 20, y, width * 5, sizeY + 10, (HMENU)IDB_delete);
	if (!store.ifRight)
		EnableWindow(hDeleteBtn, FALSE);	// 由于没有正确的名单，所以禁用
	y += 40;
	
	RECT rect;
	GetClientRect(hwnd, &rect);
	short nWidth = rect.right - 10 - x;
	short nHeight = rect.bottom - 10 - y;
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
short nWidth = 200, nHeight = 90;	// 方便窗口位置调节
export void choosePage(_In_ HINSTANCE hInstance)
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

	LPCSTR lpWindowName = "点名器1.0";
	if(hChoose==NULL)// 创建窗口
	{
		hChoose = CreateWindowEx(
			WS_EX_LAYERED,
			className, lpWindowName,
			WS_POPUP,
			(GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2,
			(GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2,   // 窗口位置
			nWidth, nHeight,                // 窗口大小
			nullptr,                        // 父窗口句柄
			nullptr,                        // 菜单句柄
			wc.hInstance,                   // 实例句柄
			nullptr                         // 附加数据
		);
	}

	SetLayeredWindowAttributes(hChoose, 0, 255, LWA_ALPHA); // 设置窗口为支持透明

	if (data.ifTop)	// 保持置顶
	{
		SetWindowLong(hChoose, GWL_EXSTYLE, GetWindowLong(hChoose, GWL_EXSTYLE) | WS_EX_TOPMOST);
		SetWindowPos(hChoose, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	ShowWindow(hChoose, SW_SHOW); // 显示窗口
	UpdateWindow(hChoose);
}

export void chooseDraw(HWND& hWnd, HDC& hdc)	// mode表示是否处于滚动状态
{
	// 获取窗口的矩形坐标
	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);
	// 计算标题栏矩形区域
	int left = 0, top = 0;
	int right = windowRect.right - windowRect.left;	// 宽度
	int bottom = capationHeight;	// 获取推荐标题栏高度
	// 创建画笔和画刷
	HPEN hPen = CreatePen(PS_SOLID, 2, data.captionBC); // 边框画笔
	HBRUSH hBrush = CreateSolidBrush(data.captionBC);   // 背景画刷
	SelectObject(hdc, hPen);
	SelectObject(hdc, hBrush);
	// 绘制标题栏矩形
	Rectangle(hdc, left, top, right, bottom);
	// 删除 GDI 对象
	DeleteObject(hBrush);
	DeleteObject(hPen);

	// 绘制整个窗口边框
	HPEN borderPen = CreatePen(PS_SOLID, 2, data.captionBC);
	HGDIOBJ BorderPen = SelectObject(hdc, borderPen);
	MoveToEx(hdc, right, 0, NULL);
	LineTo(hdc, right, windowRect.bottom - windowRect.top);	// 右边框
	LineTo(hdc, 0, windowRect.bottom - windowRect.top);	// 下边框
	LineTo(hdc, 0, 0);	// 左边框
	DeleteObject(borderPen);

	// 设置文本颜色和背景模式
	SetTextColor(hdc, data.captionFC);	// 也用于下面的按钮
	SetBkMode(hdc, TRANSPARENT);	// 背景透明
	// 计算标题位置
	RECT textRect = { left,top,right,bottom };
	textRect.left += 10;	// 给文字留点空间
	textRect.right -= 50;	// 避开关闭按钮
	// 字体
	if (!hFCaption)
	{
		hFCaption = CreateFont(
			(textRect.bottom - textRect.top) * 0.8,	// 字体高度
			0, 0, 0,
			FW_NORMAL,		// 字体粗细（FW_NORMAL, FW_BOLD 等）
			FALSE,          // 是否斜体
			FALSE,          // 是否下划线
			FALSE,          // 是否删除线
			DEFAULT_CHARSET,// 字符集
			OUT_DEFAULT_PRECIS,   // 输出精度
			CLIP_DEFAULT_PRECIS,  // 裁剪精度
			DEFAULT_QUALITY,      // 输出质量
			DEFAULT_PITCH | FF_SWISS,	// 字体间距和家族
			data.fontName.c_str()
		);
	}
	SelectObject(hdc, hFCaption);
	// 绘制文字
	DrawText(hdc, chooseTitle.c_str(), -1, &textRect, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

	// 关闭按钮
	RECT buttonRect = { right - buttonSize - 10, top + 2, right - 10, top + buttonSize + 2 };
	// 绘制关闭按钮上的“×”，与标题使用同一字体
	DrawText(hdc, "×", -1, &buttonRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	// 设置按钮
	buttonRect.right -= buttonSize;
	buttonRect.left -= buttonSize;
	DrawText(hdc, "…", -1, &buttonRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	// 客户区文本
	// 计算自定义客户区矩形区域
	windowRect.bottom -= windowRect.top;
	windowRect.top = capationHeight;
	windowRect.right -= windowRect.left;
	windowRect.left = 0;
	// 字体
	SetTextColor(hdc, data.clientFC);
	SetBkMode(hdc, TRANSPARENT);	// 背景透明
	if (!hFText)
	{
		hFText = CreateFont(
			(windowRect.bottom - windowRect.top) * 0.7,	// 字体高度
			0,              // 字体宽度（为 0 表示根据高度自动计算）
			0,              // 字体倾斜角度（单位 0.1 度）
			0,              // 字体基线方向角度
			FW_NORMAL,		// 字体粗细（FW_NORMAL, FW_BOLD等）
			FALSE,          // 是否斜体
			FALSE,          // 是否下划线
			FALSE,          // 是否删除线
			DEFAULT_CHARSET,// 字符集
			OUT_DEFAULT_PRECIS,   // 输出精度
			CLIP_DEFAULT_PRECIS,  // 裁剪精度
			DEFAULT_QUALITY,		// 输出质量
			DEFAULT_PITCH | FF_SWISS,	// 字体间距和家族
			data.fontName.c_str()
		);
	}
	SelectObject(hdc, hFText);
	// 在客户区显示文字
	if (data.leftNames.size() == 0)
	{
		data.leftNames = data.defaultNames;
		data.leftNames.push_back("抽完一轮");
	}
	DrawText(hdc, chooseText.c_str(), -1, &windowRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

export bool captionMessage(HWND& hWnd, LPARAM& lParam)
{
	POINT ptMouse;
	ptMouse.y = HIWORD(lParam);
	ScreenToClient(hWnd, &ptMouse);
	if (ptMouse.y <= capationHeight)
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
	if (ptMouse.x >= windowRect.right - windowRect.left - buttonSize - 10)
		return 1;
	else if (ptMouse.x >= windowRect.right - windowRect.left - 2 * buttonSize - 10)
		return 2;
	else
		return 0;
}

// 图标页面

// 用于窗口位置调节
void offset(HWND& hwnd, RECT& newPosition, short& width, short& height)
{
	// 获取原始窗口的大小和位置
	RECT originalRect;
	GetWindowRect(hwnd, &originalRect);

	// 检查窗口的位置偏好
	bool right = false;
	bool down = true;

	// 修改位置参数
	if (GetSystemMetrics(SM_CXSCREEN) / 2 >
		(originalRect.right + originalRect.left) / 2)
	{
		right = false;	// 偏左
		newPosition.left = originalRect.left;
	}
	else
	{
		right = true;	// 偏右或居中
		newPosition.left = originalRect.right - width;
	}

	if (GetSystemMetrics(SM_CYSCREEN) / 2 >
		(originalRect.bottom + originalRect.top) / 2)
	{
		down = false;	// 偏上
		newPosition.top = originalRect.top;
	}
	else
	{
		down = true;	// 偏下或居中
		newPosition.top = originalRect.bottom - height;
	}
}

HWND hIcon;	// 方便频繁转换
export void iconPage()
{
	short d = 50;	// 窗口直径

	RECT newPosition;
	offset(hChoose, newPosition, d, d);

	if (!hIcon)
	{
		// 注册新窗口类型
		const char* iconWndName = "icon";

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
			WS_POPUP | WS_SYSMENU,	// 窗口风格
			newPosition.left,
			newPosition.top,	// 窗口位置
			d, d,				// 窗口大小
			nullptr,			// 父窗口句柄
			nullptr,			// 菜单句柄
			iconWc.hInstance,	// 实例句柄
			nullptr				// 附加数据
		);

		SetLayeredWindowAttributes(hChoose, 0, 0, LWA_ALPHA); // 设置窗口为支持透明

		// 创建圆形区域并设置为窗口区域
		HRGN hRegion = CreateEllipticRgn(0, 0, d, d);
		SetWindowRgn(hIcon, hRegion, TRUE);
	}

	// 显示窗口
	ShowWindow(hChoose, SW_HIDE);
	ShowWindow(hIcon, SW_SHOW);
	UpdateWindow(hIcon);

	// 关闭抽取窗口计时
	KillTimer(hChoose, IDT_wait);

	// 调整位置
	HWND set;
	if (data.ifTop)
		set = HWND_TOPMOST;
	else
		set = nullptr;
	SetWindowPos(hIcon, set, newPosition.left, newPosition.top, 0, 0, SWP_NOSIZE);
}

export void ExitIconMode()
{
	ShowWindow(hIcon, SW_HIDE);
	ShowWindow(hChoose, SW_SHOW);

	SetTimer(hChoose, IDT_wait, 1000, 0);

	RECT newPosition;
	offset(hIcon, newPosition, nWidth, nHeight);
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

export void transparency(HWND& hwnd, short& mode)
{
	if (changeAlpha)	// 增加透明度
	{
		if (currentAlpha >= 255)
		{
			changeAlpha = false;
			KillTimer(hwnd, IDT_transparency);
			transparencyTimerActive = false;
		}
		else
		{
			currentAlpha += 5;
			SetLayeredWindowAttributes(hwnd, 0, currentAlpha, LWA_ALPHA); // 更新透明度
		}
	}
	else // 减少透明度
	{
		if (currentAlpha)	// currentAlpha为0
		{
			currentAlpha -= 5;  // 每次减少5
			SetLayeredWindowAttributes(hwnd, 0, currentAlpha, LWA_ALPHA); // 更新透明度
		}
		else
		{
			changeAlpha = true;
			KillTimer(hwnd, IDT_transparency);
			if (mode == normal)
			{
				mode = icon;	// 切换模式标记为图标
				iconPage();
				SetTimer(hIcon, IDT_transparency, 10, 0);
			}
			else if (mode == icon)
			{
				mode = normal;	// 切换模式标记为抽取
				ExitIconMode();
				SetTimer(hChoose, IDT_transparency, 10, 0);
			}
		}
	}
}

// 让颜色统一修改
export void colorSynergy(HWND& hwnd, short ID, short position)	// 协同调节颜色
{
	short mode;

	std::string colorStr = readEdit(hwnd, ID)[0];
	if (!colorStr.size())
	{
		HWND hEdit = GetDlgItem(hwnd, ID);
		SetWindowText(hEdit, "0");
	}

	if (position < 4)
		mode = 10;
	else
		mode = 16;
	unsigned int value = std::stoi(colorStr, nullptr, mode);

	if (mode == 10 && (value < 0 || value>255) || mode == 16 && (value < 0x00000 || value>0xffffff))
	{
		MessageBox(nullptr,
			"你的颜色设置不正确诶。\n如果是RGB的话，要0~255。\n如果是16位，不要0x，要000000~ffffff。",
			"不好", MB_ICONERROR);
		HWND hEdit = GetDlgItem(hwnd, ID);
		SetFocus(hEdit);	// 强制回去重新填写
		return;
	}

	// 颜色正确
	short hexID = ID + (4 - position);
	// 获取句柄
	HWND hHexEdit = GetDlgItem(hwnd, hexID);	// 获取16位编辑框句柄
	HWND hREdit = GetDlgItem(hwnd, hexID - 3);
	HWND hGEdit = GetDlgItem(hwnd, hexID - 2);
	HWND hBEdit = GetDlgItem(hwnd, hexID - 1);
	switch (position)
	{
	case 1:	// 更改R值
		colorStr = readEdit(hwnd, hexID)[0];	// 获取16位字符
		colorStr.replace(0, 2, std::format("{:02x}", value));
		SetWindowText(hHexEdit, colorStr.c_str());
		break;
	case 2:	// 更改G值
		colorStr = readEdit(hwnd, hexID)[0];	// 获取16位字符
		colorStr.replace(2, 2, std::format("{:02x}",value));
		SetWindowText(hHexEdit, colorStr.c_str());
		break;
	case 3:	// 更改B值
		colorStr = readEdit(hwnd, hexID)[0];	// 获取16位字符
		colorStr.replace(4, 2, std::format("{:02x}", value));
		SetWindowText(hHexEdit, colorStr.c_str());
		break;
	case 4:	// 更改16位
		short R;	R = (value >> 16) & 0xff;
		short G;	G = (value >> 8) & 0xff;
		short B;	B = value & 0xff;
		SetWindowText(hREdit, std::format("{}", R).c_str());
		SetWindowText(hGEdit, std::format("{}", G).c_str());
		SetWindowText(hBEdit, std::format("{}", B).c_str());
		break;
	}

	// 保存修改
	colorStr = readEdit(hwnd, hexID)[0];	// 获取16位字符
	colorCorrect(colorStr);
	if (ID >= IDE_captionBCR && ID <= IDE_captionBC16)
		store.captionBC = std::stoi(colorStr, nullptr, 16);
	else if (ID >= IDE_captionFCR && ID <= IDE_captionFC16)
		store.captionFC = std::stoi(colorStr, nullptr, 16);
	else if (ID > IDE_clientBCR && ID <= IDE_clientBC16)
		store.clientBC = std::stoi(colorStr, nullptr, 16);
	else
		store.clientFC = std::stoi(colorStr, nullptr, 16);
}

export void selectChange(HWND& hwnd, std::string& str)
{
	int index = SendMessage(hwnd, CB_GETCURSEL, 0, 0);	// 获取当前选中索引
	int length = SendMessage(hwnd, CB_GETLBTEXTLEN, index, 0);
	str.resize(length);
	SendMessage(hwnd, CB_GETLBTEXT, index, (LPARAM)str.data());	// 获取文本
}