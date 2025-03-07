module;
#include <ctime>
#include <Windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include "resource.h"
#pragma comment(lib,"gdiplus.lib")
export module WindowProc;
import command;
import cryptograph;
import data;
import png;
import std;
import value;


short mode = normal;	// 0正常，1图标模式
short waitNum = 0;		// 待机时长
short scrollMax = 10;	// 滚动名字数量上限
short scrollNum = scrollMax;	// 设置滚动名字数量
std::string newListName;	// 用于储存新名单名字

LRESULT CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)
		CheckDlgButton(hwnd, IDC_YES, BST_CHECKED);	// 默认选择第一个单选按钮

	if (uMsg == WM_COMMAND && LOWORD(wParam) == ID_OK)
	{
		// 获取输入的名单名字
		char name[100];
		GetDlgItemText(hwnd, IDC_NAME_EDIT, name, sizeof(name));
		newListName = name;
		newListName = "# 《" + newListName + "》";	// 设置格式
		if (!newListName.size())
			goto RETURN;
		else
		{
			for (short i = 0; i < store.all.size(); i++)
			{
				if (newListName == store.all[i][0])
				{
					SetDlgItemText(hwnd, IDC_NAME_EDIT, "不可重名");
					goto RETURN;
				}
			}
		}

		std::string password;	// 用于储存新名单密码
		// 获取输入的密码
		password = readEdit(hwnd, IDC_PASSWORD_EDIT)[0];
		if (!password.size())
			goto RETURN;

		// 新增名单
		store.all.push_back({ newListName,password});
		short listNum = store.all.size() - 1;
		store.all[listNum].resize(2 + readName.size());
		std::copy(readName.begin(), readName.end(), store.all[listNum].begin() + 2);

		// 默认名单按钮
		if (IsDlgButtonChecked(hwnd, IDC_YES) == BST_CHECKED)
		{
			// 处理格式
			newListName.erase(0, 4);
			newListName.pop_back();
			newListName.pop_back();
			// 保存
			store.defaultList = newListName;
			store.defaultNames = readName;
		}
		
		// 关闭对话框
		EndDialog(hwnd, 1);
	}
	else if(uMsg == WM_COMMAND && LOWORD(wParam) == IDNO)
		EndDialog(hwnd, 0);	// 关闭对话框
RETURN:
	return DefWindowProc(hwnd, uMsg, wParam, lParam); // 默认处理
}

std::string modifyListName;	// 要修改的名单的名字
std::string inputPassword;	// 存储输入的密码
short listNum;	// 存储名单所在的位置
LRESULT CALLBACK passwordCheck(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_CONFIRM_PASSWORD:
			// 获取输入的密码
			char inputPasswordCh[100];
			GetDlgItemText(hwnd, IDC_PASSWORD_CHECK, inputPasswordCh, sizeof(inputPasswordCh));
			inputPassword = inputPasswordCh;
			for (short i = 0; i < store.all.size(); i++)
			{
				if (modifyListName != store.all[i][0])
					continue;	// 寻找名单密码

				listNum = i;

				if (inputPassword == store.all[i][1])
					EndDialog(hwnd, 0);	// 密码正确
				else
					EndDialog(hwnd, 2);	// 密码错误
			}
			break;

		case ID_CANCELMODIFY:
			EndDialog(hwnd, 1);	// 退出修改
			break;
		}
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam); // 默认处理
		break;
	}
}

export LRESULT CALLBACK WPsetting(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))	// 针对消息来源进行消息处理
	{
	case NULL:	// 主窗口
		switch (uMsg)
		{
		case WM_PAINT:
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			settingDraw(hwnd, hdc, lParam);
			EndPaint(hwnd, &ps);
			break;
		}
		break;
	}

	switch (uMsg)	// 针对消息类型进行消息处理
	{
	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
			switch (LOWORD(wParam))
			{
			case IDB_confirm:
				if (!store.defaultNames.size())
					MessageBox(nullptr, "你没有新建名单诶。", "可是……", MB_ICONQUESTION);
				else
				{
					// 关闭设置
					ShowWindow(hwnd, SW_HIDE);
					HDC hdc = GetDC(hwnd);

					// 创建数据库
					std::vector<std::string> splitLines;
					splitLines.push_back(std::format("{}",store.ifTop));
					splitLines.push_back(std::format("{:06x}", store.clientBC));
					splitLines.push_back(std::format("{:06x}", store.clientFC));
					splitLines.push_back(std::format("{:06x}", store.captionBC));
					splitLines.push_back(std::format("{:06x}", store.captionFC));
					splitLines.push_back(store.fontName);
					splitLines.push_back(store.defaultList);
					for (std::vector<std::string> strVec : store.all)
					{
						splitLines.resize(splitLines.size() + strVec.size());
						std::copy(strVec.begin(), strVec.end(), splitLines.end() - strVec.size());
						splitLines.push_back("#");
					}

					// 保存在data.bin文件中
					std::ofstream fileMake("data.bin", std::ios::binary | std::ios::trunc);	// 创建文件
					fileMake.close();
					EncryptData(splitLines);

					// 重启程序
					selfRestart(hwnd);
				}
				break;

			case IDB_delete:
				if (MessageBox(nullptr,
					"请确定删除名单。\n名单删除，因名单名字已记录，后面不可以创建同名名单，除非删除data.bin。",
					"确定？", MB_OKCANCEL) == IDOK)
				{
					std::string deleteListName = readEdit(hwnd, IDL_showList)[0];

					if (deleteListName == store.defaultList)
					{
						MessageBox(nullptr, "默认名单不可删除，请创建其它名单并确立为默认后再删除此名单。"
							, "糟糕X﹏X", MB_ICONWARNING);
						break;
					}

					for (short i=0;i<store.all.size();i++)
					{
						if (store.all[i][0] == deleteListName)
						{
							store.all[i].erase(store.all[i].begin() + 1, store.all[i].end());
						}
					}
					MessageBox(nullptr, "名单已删除。", "完成", NULL);

					// 修改显示名单列表
					LRESULT index = SendMessage(hShowList, LB_FINDSTRINGEXACT, -1,
						(LPARAM)store.defaultList.c_str());
					if (index != LB_ERR)
					{
						SendMessage(hShowList, LB_SETCURSEL, index, 0);
						std::string showText;
						for (std::string str : store.defaultNames)
							showText += str + "\r\n";
						SetWindowText(hShowList, showText.c_str());
					}
				}
				break;

			case IDB_modify:
				// 获取输入的名单
				char name[100];
				GetDlgItemText(hwnd, IDL_showList, name, sizeof(name));
				modifyListName = name;
				modifyListName = "# 《" + modifyListName + "》";	// 修改格式

				// 密码输入窗口
				switch (DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ASK_PASSWORD), hwnd,
					passwordCheck))
				{
				case 0:	// 密码正确
				{
					readName.clear();
					readName = readEdit(hwnd, IDE_names);
					store.all[listNum].resize(2 + readName.size());
					std::copy(readName.begin(), readName.end(), store.all[listNum].begin() + 2);

					MessageBox(nullptr, "已记录，记得点“确定好啦”。", "好啦\\(^o^)/~", NULL);
				}
					break;

				case 1:	// 取消修改或取消输入密码
					break;

				case 2:	// 密码错误
					MessageBox(nullptr, "密码错误。", "糟糕X﹏X", NULL);
					break;
				}

				break;

			case IDB_newList:
				if (showName)	// 当前正在显示名字
				{
					readName = newNameTip;	// 重置名单显示文本
					std::string setText;
					for (std::string str : readName)
						setText += str + "\r\n";
					SetWindowText(hNameEdit,setText.c_str());	// 修改名字显示文本
					showName = false;
					ifTypeName = true;
					EnableWindow(hModifyBtn, FALSE);	// 禁用修改名单
					EnableWindow(hDeleteBtn, FALSE);	// 禁用删除名单
					EnableWindow(hShowList, FALSE);		// 禁用显示名单
					break;
				}
				else // 不是显示名字，而是编辑名字后“新建名单”
				{
					if (!deleteVoid(readName))
					{
						MessageBox(nullptr, "没有名字啊(⊙o⊙)", "奇怪", MB_ICONQUESTION);
						break;
					}

					// 打开名字、密码输入对话框
					if (DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DialogProc))
					{
						store.ifRight = true;	// 确保正常全部重绘

						MessageBox(nullptr, "已记录，记得点“确定好啦”。", "好啦\\(^o^)/~", NULL);

						// 显示当前名单
						EnableWindow(hModifyBtn, TRUE);	// 可用修改名单
						EnableWindow(hDeleteBtn, TRUE);	// 可用删除名单
						EnableWindow(hDefaultList, TRUE);	// 可用默认名单
						EnableWindow(hShowList, TRUE);	// 可用显示名单
						// 追加名单列表
						std::string str;
						str = removeFormat(store.all.back()[0]);
						SendMessage(hDefaultList, CB_ADDSTRING, 0, (LPARAM)str.c_str());
						if (store.defaultList == str)
							SendMessage(hDefaultList, CB_SETCURSEL, store.all.size() - 1, 0);	// 设置默认项
						SendMessage(hShowList, CB_ADDSTRING, 0, (LPARAM)str.c_str());
						SendMessage(hShowList, CB_SETCURSEL, store.all.size() - 1, 0);	// 设置显示项

						showName = true;	// 表示正在显示名单
						store.currentShowList = str;	// 存储显示的名单
						ifTypeName = false;	// 标志没有在输入新名单名字
					}
				}
				break;

			case IDB_topYes:
				store.ifTop = true;
				break;

			case IDB_topNo:
				store.ifTop = false;
				break;
			}
			break;

		case CBN_EDITCHANGE:
			switch (LOWORD(wParam))
			{
			case IDL_fontName:
				char inputFont[LF_FACESIZE];
				GetWindowText(hFontName, inputFont, LF_FACESIZE);

				// 检查输入的字体是否有效
				bool isValidFont = false;
				HDC hdc = GetDC(hwnd);
				LOGFONTA logFont = { 0 };
				logFont.lfCharSet = DEFAULT_CHARSET;
				strcpy_s(logFont.lfFaceName, inputFont);
				EnumFontFamiliesEx(hdc, &logFont,
					[](const LOGFONTA* lpelfe, const TEXTMETRIC*, DWORD,LPARAM lParam) -> int
					{
						*(bool*)lParam = true;
						return 0;
					},
					(LPARAM)&isValidFont, 0);
				ReleaseDC(hwnd, hdc);

				if (isValidFont)	// 字体存在
				{
					// 先删除旧字体，防止内存泄漏
					static HFONT hCurrentFont = NULL;
					if (hCurrentFont) DeleteObject(hCurrentFont);
					DeleteObject(hFSetting);

					// 创建新字体
					hFSetting = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
						OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
						inputFont);

					data.fontName = inputFont;
					// 设置字体
					SendMessage(hFontName, WM_SETFONT, (WPARAM)hFSetting, TRUE);

					fontStore(inputFont);
				}
				break;
			}
			break;

		case CBN_SELCHANGE:
			switch (LOWORD(wParam))
			{
			case IDL_defaultList:
			{
				store.ifRight = true;	// 保证全部重绘时正确
				std::string listName;
				selectChange(hDefaultList, listName);
				store.defaultList = listName;
				listName = "# 《" + listName + "》";	// 设置格式以在all中查找
				for (std::vector<std::string> strVec : store.all)
				{
					if (strVec[0] != listName)
						continue;
					store.defaultNames.resize(strVec.size() - 2);
					std::copy(strVec.begin() + 2, strVec.end(), store.defaultNames.begin());
					break;
				}
			}
				break;

			case IDL_fontName:
				int index;
				index = SendMessage(hFontName, CB_GETCURSEL, 0, 0);
				if (index != CB_ERR)
				{
					char selectedFont[LF_FACESIZE] = { 0 };
					SendMessage(hFontName, CB_GETLBTEXT, index, (LPARAM)selectedFont);
					// 修改数据
					data.fontName = selectedFont;
					// 重新设置字体
					hFSetting = CreateFont(
						20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
						DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, selectedFont
					);
					SendMessage(hFontName, WM_SETFONT, (WPARAM)hFSetting, TRUE);

					fontStore(selectedFont);
				}
				break;

			case IDL_showList:	// 修改显示的名单名字编辑框
			{
				std::string listName;
				selectChange(hShowList, listName);
				store.currentShowList = listName;	// 保存正在显示的名单
				listName = "# 《" + listName + "》";	// 在store.all里面搜索有格式要求
				for (short i = 0; i < store.all.size(); i++)
				{
					if (store.all[i][0] != listName)
						continue;
					// 修改名字显示
					std::string showStr;
					for (short j = 2; j < store.all[i].size(); j++)
						showStr += store.all[i][j] + "\r\n";
					SetWindowText(hNameEdit, showStr.c_str());
					// 保存名字编辑框的修改
					readName.resize(store.all[i].size() - 2);
					std::copy(store.all[i].begin() + 2, store.all[i].end(), readName.begin());
					break;
				}
			}
			break;

			}
			break;

		case EN_KILLFOCUS:
			switch (LOWORD(wParam))
			{
			case IDE_names:
				readName = readEdit(hwnd, IDE_names);	// 在失去焦点时记录名字编辑框里面的内容
				break;

			case IDE_captionBC16:
				data.captionBC = std::stoi(readEdit(hwnd, IDE_captionBC16)[0], nullptr, 16);
				break;
			case IDE_captionFC16:
				data.captionFC = std::stoi(readEdit(hwnd, IDE_captionFC16)[0], nullptr, 16);
				break;
			case IDE_clientBC16:
				data.clientBC = std::stoi(readEdit(hwnd, IDE_clientBC16)[0], nullptr, 16);
				break;
			case IDE_clientFC16:
				data.clientFC = std::stoi(readEdit(hwnd, IDE_clientFC16)[0], nullptr, 16);
				break;
			}
		}
		break;

	case WM_CTLCOLORSTATIC:
		// 获取设备上下文（DC）
		HDC hdcStatic;
		hdcStatic = (HDC)wParam;

		// 设置背景颜色为透明，消除不好看奇怪的底色
		SetBkMode(hdcStatic, TRANSPARENT);

		// 强调“看不懂就新建”
		if ((HWND)lParam == hAttention)
			SetTextColor(hdcStatic, RGB(255, 0, 0));  // 红色文本
		else if((HWND)lParam==hCopyright)
			SetTextColor(hdcStatic, RGB(82, 184, 235));  // 降亮度天依蓝文本
		else
			SetTextColor(hdcStatic, RGB(0, 0, 0));	// 黑色文本
		// 返回一个画刷，用来设置背景颜色
		HBRUSH hBrush;
		hBrush = CreateSolidBrush(RGB(255, 255, 255));  // 白色背景
		return (LRESULT)hBrush;  // 返回画刷
		break;

	case WM_DESTROY:
		DeleteObject(hFSetting);
		if (pBitmap)
			delete pBitmap;
		for (auto& pair : fontMap)
			DeleteObject(pair.second);
		fontMap.clear();
		PostQuitMessage(0);
		break;

	case WM_DRAWITEM:
		drawFontList(lParam);
		break;

	case WM_MEASUREITEM:
	{
		if (wParam == (WPARAM)IDL_fontName)  // 检查消息是否针对你的 ComboBox
		{
			LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT)lParam;
			lpMeasureItem->itemHeight = 30;  // 设置每个列表项的高度以加强视觉效果
		}
		return TRUE;
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam); // 默认处理
}

void chooseModeDestroy()
{
	if (pBitmap)
		delete pBitmap;

	if(transparencyTimerActive)
		KillTimer(nullptr, IDT_transparency);  // 完全透明后停止定时器

	// 删除字体对象，释放资源
	DeleteObject(hFCaption);
	DeleteObject(hFText);

	PostQuitMessage(0);
}

std::string versionText = "点名器1.1";
std::mt19937 engine(static_cast<unsigned int>(time(0)));	// 随机数于随机滚动
export LRESULT CALLBACK WPchoose(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		SetTimer(hwnd, IDT_wait, 1000, nullptr);	// 等待5秒切换至图标模式用
		chooseTitle = versionText;
		break;

	case WM_DESTROY:
		KillTimer(hwnd, IDT_wait);
		chooseModeDestroy();
		break;
		
	case WM_DRAWITEM:
		LPDRAWITEMSTRUCT pDrawItem;
		pDrawItem = (LPDRAWITEMSTRUCT)lParam;
		if (pDrawItem->CtlID == IDS_chooseText)
		{
			HDC hdc = pDrawItem->hDC;
			RECT rc = pDrawItem->rcItem;

			// 1. 绘制背景
			FillRect(hdc, &rc, CreateSolidBrush(data.clientBC));

			// 2. 绘制边框
			FrameRect(hdc, &rc, CreateSolidBrush(data.captionBC));

			// 3. 设置文本颜色和透明背景
			SetTextColor(hdc, data.clientFC);
			SetBkMode(hdc, TRANSPARENT);

			// 4. 绘制文本
			DrawText(hdc, chooseText.c_str(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			return TRUE;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_SPACE:
		case VK_RETURN:
			goto CHOOSE;
			break;
		}
		break;

	case WM_LBUTTONUP:
	CHOOSE:	// 抽取操作
		if (chooseTitle == versionText)	// 修改标题
		{
			chooseTitle = std::format("{}", data.defaultNames.size()) + "《" + data.defaultList + "》";
		}

		// 清除可能的透明度
		if (currentAlpha != 255 && changeAlpha == false)
		{
			currentAlpha = 255;
			changeAlpha = false;
			KillTimer(hwnd, IDT_transparency);
			transparencyTimerActive = false;
			SetLayeredWindowAttributes(hwnd, 0, currentAlpha, LWA_ALPHA); // 更新透明度
		}

		// 清空waitNum并预留时间给滚动效果
		waitNum = -1;
		// 抽取名字操作
		SetTimer(hwnd, IDT_scroll, scrollT, nullptr);	// 滚动效果速度设置
		break;

	case WM_NCHITTEST:
		if (captionMessage(hwnd, lParam))
		{
			if (captionButton(hwnd, lParam))
				return HTCLOSE;	// 可能是关闭按钮，也可能是设置按钮
			else
				return HTCAPTION;
		}
		break;

	case WM_NCLBUTTONDOWN:	// 使用UP会因哪怕极小的拖动而不触发
		switch (wParam)
		{
		case HTCLOSE:
			short button = captionButton(hwnd, lParam);
			switch (button)
			{
			case close:
				PostMessage(hwnd, WM_CLOSE, 0, 0);	// 手动发送 WM_CLOSE 消息
				break;

			case setting:
				ShowWindow(hwnd, SW_HIDE);	// 关闭窗口
				KillTimer(hwnd, IDT_wait);	// 关闭计时
				waitNum = 0;
				settingPage(GetModuleHandle(nullptr));
				break;
			}
			break;
		}
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hwnd, &ps);

		chooseDraw(hwnd, hdc);

		EndPaint(hwnd, &ps);
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case IDT_scroll:
			scrollNum--;
			RECT invalida;
			GetClientRect(hwnd, &invalida); // 获取客户区尺寸
			if (!scrollNum)	// 滚动次数用尽
			{
				chooseText = data.leftNames.back();	// 最终名字
				data.leftNames.pop_back();	// 删除名字
				scrollNum = scrollMax;
				KillTimer(hwnd, IDT_scroll);	// 关闭计时
			}
			else
				chooseText = data.defaultNames[engine() % data.defaultNames.size()];	// 随机名字

			SetWindowText(hChooseText, chooseText.c_str());	// 设置显示文本
			break;

		case IDT_transparency:
			transparency(hwnd, mode);
			break;

		case IDT_wait:
			waitNum++;
			if (waitNum == 8)
			{
				waitNum = 0;	// 清零waitNum
				SetTimer(hwnd, IDT_transparency, transparencyT, nullptr);// 用于设置透明度修改时间
				transparencyTimerActive = true;
			}
			break;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam); // 默认处理
}

POINT ptOld;	// 记录鼠标点击时的位置以判断是否是点击
bool isDragging;	// 标志是否是移动
export LRESULT CALLBACK WPicon(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		break;

	case WM_DESTROY:
		chooseModeDestroy();
		break;

	case WM_LBUTTONDOWN:
		ptOld.x = GET_X_LPARAM(lParam);
		ptOld.y = GET_Y_LPARAM(lParam);
		// 转换为屏幕坐标（因为窗口可能被移动，局部坐标不足以处理）
		ClientToScreen(hwnd, &ptOld);
		isDragging = false;	// 重置isDragging
		// 捕获鼠标事件，确保拖动不会因鼠标移出窗口而中断
		SetCapture(hwnd);
		break;

	case WM_LBUTTONUP:
		if (!isDragging)
		{
			SetTimer(hwnd, IDT_transparency, transparencyT, nullptr);	// 动态回复抽取窗口
			transparencyTimerActive = true;
		}
		ReleaseCapture();
		break;

	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON)
		{
			// 如果鼠标左键按下并且发生了移动，则开始拖动
			POINT ptNew;
			ptNew.x = GET_X_LPARAM(lParam);
			ptNew.y = GET_Y_LPARAM(lParam);
			ClientToScreen(hwnd, &ptNew);
			// 判断鼠标是否移动了，如果移动了，则认为是拖动
			int dx = ptNew.x - ptOld.x;
			int dy = ptNew.y - ptOld.y;

			if (abs(dx) > 5 || abs(dy) > 5)
			{  // 如果移动的距离大于阈值，认为是拖动
				isDragging = true;
			}

			if (isDragging)
			{
				// 移动窗口
				RECT rect;
				GetWindowRect(hwnd, &rect);
				SetWindowPos(hwnd, NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				ptOld = ptNew;  // 更新鼠标位置
			}
		}
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hwnd, &ps);

		pngDraw(hdc, hwnd);

		EndPaint(hwnd, &ps);
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case IDT_transparency:
			transparency(hwnd, mode);
			break;
		}
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam); // 默认处理
}
