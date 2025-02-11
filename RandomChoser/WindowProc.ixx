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


short mode = normal;	// 0������1ͼ��ģʽ
short waitNum = 0;		// ����ʱ��
short scrollMax = 20;	// ����������������
short scrollNum = scrollMax;	// ���ù�����������
std::string newListName;	// ���ڴ�������������

LRESULT CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)
		CheckDlgButton(hwnd, IDC_YES, BST_CHECKED);	// Ĭ��ѡ���һ����ѡ��ť

	if (uMsg == WM_COMMAND && LOWORD(wParam) == ID_OK)
	{
		// ��ȡ�������������
		char name[100];
		GetDlgItemText(hwnd, IDC_NAME_EDIT, name, sizeof(name));
		newListName = name;
		newListName = "# ��" + newListName + "��";	// ���ø�ʽ
		if (!newListName.size())
			goto RETURN;
		else
		{
			for (short i = 0; i < store.all.size(); i++)
			{
				if (newListName == store.all[i][0])
				{
					SetDlgItemText(hwnd, IDC_NAME_EDIT, "��������");
					goto RETURN;
				}
			}
		}

		std::string password;	// ���ڴ�������������
		// ��ȡ���������
		password = readEdit(hwnd, IDC_PASSWORD_EDIT)[0];
		if (!password.size())
			goto RETURN;

		// ��������
		store.all.push_back({ newListName,password});
		short listNum = store.all.size() - 1;
		store.all[listNum].resize(2 + readName.size());
		std::copy(readName.begin(), readName.end(), store.all[listNum].begin() + 2);

		// Ĭ��������ť
		if (IsDlgButtonChecked(hwnd, IDC_YES) == BST_CHECKED)
		{
			// �����ʽ
			newListName.erase(0, 4);
			newListName.pop_back();
			newListName.pop_back();
			// ����
			store.defaultList = newListName;
			store.defaultNames = readName;
		}
		
		// �رնԻ���
		EndDialog(hwnd, 1);
	}
	else if(uMsg == WM_COMMAND && LOWORD(wParam) == IDNO)
		EndDialog(hwnd, 0);	// �رնԻ���
RETURN:
	return DefWindowProc(hwnd, uMsg, wParam, lParam); // Ĭ�ϴ���
}

std::string modifyListName;	// Ҫ�޸ĵ�����������
std::string inputPassword;	// �洢���������
short listNum;	// �洢�������ڵ�λ��
LRESULT CALLBACK passwordCheck(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_CONFIRM_PASSWORD:
			// ��ȡ���������
			char inputPasswordCh[100];
			GetDlgItemText(hwnd, IDC_PASSWORD_CHECK, inputPasswordCh, sizeof(inputPasswordCh));
			inputPassword = inputPasswordCh;
			for (short i = 0; i < store.all.size(); i++)
			{
				if (modifyListName != store.all[i][0])
					continue;	// Ѱ����������

				listNum = i;

				if (inputPassword == store.all[i][1])
					EndDialog(hwnd, 0);	// ������ȷ
				else
					EndDialog(hwnd, 2);	// �������
			}
			break;

		case ID_CANCELMODIFY:
			EndDialog(hwnd, 1);	// �˳��޸�
			break;
		}
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam); // Ĭ�ϴ���
		break;
	}
}

export LRESULT CALLBACK WPsetting(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
			switch (LOWORD(wParam))
			{
			case IDB_confirm:
				if (!store.defaultNames.size())
					MessageBox(nullptr, "��û���½���������", "���ǡ���", MB_ICONQUESTION);
				else
				{
					// �ر�����
					ShowWindow(hwnd, SW_HIDE);
					HDC hdc = GetDC(hwnd);

					// �������ݿ�
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

					// ������data.bin�ļ���
					std::ofstream fileMake("data.bin", std::ios::binary | std::ios::trunc);	// �����ļ�
					fileMake.close();
					EncryptData(splitLines);

					// ��������
					selfRestart(hwnd);
				}
				break;

			case IDB_delete:
				if (MessageBox(nullptr,
					"��ȷ��ɾ��������\n����ɾ���������������Ѽ�¼�����治���Դ���ͬ������������ɾ��data.bin��",
					"ȷ����", MB_OKCANCEL) == IDOK)
				{
					std::string deleteListName = readEdit(hwnd, IDL_showList)[0];

					if (deleteListName == store.defaultList)
					{
						MessageBox(nullptr, "Ĭ����������ɾ�����봴������������ȷ��ΪĬ�Ϻ���ɾ����������"
							, "���X�nX", MB_ICONWARNING);
						break;
					}

					for (short i=0;i<store.all.size();i++)
					{
						if (store.all[i][0] == deleteListName)
						{
							store.all[i].erase(store.all[i].begin() + 1, store.all[i].end());
						}
					}
					MessageBox(nullptr, "������ɾ����", "���", NULL);

					// �޸���ʾ�����б�
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
				// ��ȡ���������
				char name[100];
				GetDlgItemText(hwnd, IDL_showList, name, sizeof(name));
				modifyListName = name;
				modifyListName = "# ��" + modifyListName + "��";	// �޸ĸ�ʽ

				// �������봰��
				switch (DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ASK_PASSWORD), hwnd,
					passwordCheck))
				{
				case 0:	// ������ȷ
				{
					readName.clear();
					readName = readEdit(hwnd, IDE_names);
					store.all[listNum].resize(2 + readName.size());
					std::copy(readName.begin(), readName.end(), store.all[listNum].begin() + 2);

					MessageBox(nullptr, "�Ѽ�¼���ǵõ㡰ȷ����������", "����\\(^o^)/~", NULL);
				}
					break;

				case 1:	// ȡ���޸Ļ�ȡ����������
					break;

				case 2:	// �������
					MessageBox(nullptr, "�������", "���X�nX", NULL);
					break;
				}

				break;

			case IDB_newList:
				if (showName)	// ��ǰ������ʾ����
				{
					readName = newNameTip;	// ����������ʾ�ı�
					std::string setText;
					for (std::string str : readName)
						setText += str + "\r\n";
					SetWindowText(hNameEdit,setText.c_str());	// �޸�������ʾ�ı�
					showName = false;
					ifTypeName = true;
					EnableWindow(hModifyBtn, FALSE);	// �����޸�����
					EnableWindow(hDeleteBtn, FALSE);	// ����ɾ������
					EnableWindow(hShowList, FALSE);		// ������ʾ����
					break;
				}
				else // ������ʾ���֣����Ǳ༭���ֺ��½�������
				{
					if (!deleteVoid(readName))
					{
						MessageBox(nullptr, "û�����ְ�(��o��)", "���", MB_ICONQUESTION);
						break;
					}

					// �����֡���������Ի���
					if (DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DialogProc))
					{
						store.ifRight = true;	// ȷ������ȫ���ػ�

						MessageBox(nullptr, "�Ѽ�¼���ǵõ㡰ȷ����������", "����\\(^o^)/~", NULL);

						// ��ʾ��ǰ����
						EnableWindow(hModifyBtn, TRUE);	// �����޸�����
						EnableWindow(hDeleteBtn, TRUE);	// ����ɾ������
						EnableWindow(hDefaultList, TRUE);	// ����Ĭ������
						EnableWindow(hShowList, TRUE);	// ������ʾ����
						// ׷�������б�
						std::string str;
						str = removeFormat(store.all.back()[0]);
						SendMessage(hDefaultList, CB_ADDSTRING, 0, (LPARAM)str.c_str());
						if (store.defaultList == str)
							SendMessage(hDefaultList, CB_SETCURSEL, store.all.size() - 1, 0);	// ����Ĭ����
						SendMessage(hShowList, CB_ADDSTRING, 0, (LPARAM)str.c_str());
						SendMessage(hShowList, CB_SETCURSEL, store.all.size() - 1, 0);	// ������ʾ��

						showName = true;	// ��ʾ������ʾ����
						store.currentShowList = str;	// �洢��ʾ������
						ifTypeName = false;	// ��־û������������������
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

				// �������������Ƿ���Ч
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

				if (isValidFont)	// �������
				{
					// ��ɾ�������壬��ֹ�ڴ�й©
					static HFONT hCurrentFont = NULL;
					if (hCurrentFont) DeleteObject(hCurrentFont);
					DeleteObject(hFSetting);

					// ����������
					hFSetting = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
						OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
						inputFont);

					data.fontName = inputFont;
					// ��������
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
				store.ifRight = true;	// ��֤ȫ���ػ�ʱ��ȷ
				std::string listName;
				selectChange(hDefaultList, listName);
				store.defaultList = listName;
				listName = "# ��" + listName + "��";	// ���ø�ʽ����all�в���
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
					// �޸�����
					data.fontName = selectedFont;
					// ������������
					hFSetting = CreateFont(
						20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
						DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, selectedFont
					);
					SendMessage(hFontName, WM_SETFONT, (WPARAM)hFSetting, TRUE);

					fontStore(selectedFont);
				}
				break;

			case IDL_showList:	// �޸���ʾ���������ֱ༭��
			{
				std::string listName;
				selectChange(hShowList, listName);
				store.currentShowList = listName;	// ����������ʾ������
				listName = "# ��" + listName + "��";	// ��store.all���������и�ʽҪ��
				for (short i = 0; i < store.all.size(); i++)
				{
					if (store.all[i][0] != listName)
						continue;
					// �޸�������ʾ
					std::string showStr;
					for (short j = 2; j < store.all[i].size(); j++)
						showStr += store.all[i][j] + "\r\n";
					SetWindowText(hNameEdit, showStr.c_str());
					// �������ֱ༭����޸�
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
				readName = readEdit(hwnd, IDE_names);	// ��ʧȥ����ʱ��¼���ֱ༭�����������
				break;

				// ��������ɫ�޸ĵ�Эͬ�޸���Ϣ
			case IDE_captionBCR:
			case IDE_captionFCR:
			case IDE_clientBCR:
			case IDE_clientFCR:
				colorSynergy(hwnd, LOWORD(wParam), 1);
				break;

			case IDE_captionBCG:
			case IDE_captionFCG:
			case IDE_clientBCG:
			case IDE_clientFCG:
				colorSynergy(hwnd, LOWORD(wParam), 2);
				break;

			case IDE_captionBCB:
			case IDE_captionFCB:
			case IDE_clientBCB:
			case IDE_clientFCB:
				colorSynergy(hwnd, LOWORD(wParam), 3);
				break;

			case IDE_captionBC16:
			case IDE_captionFC16:
			case IDE_clientBC16:
			case IDE_clientFC16:
				colorSynergy(hwnd, LOWORD(wParam), 4);
				break;
			}
		}
		break;

	case WM_CTLCOLORSTATIC:
		// ��ȡ�豸�����ģ�DC��
		HDC hdcStatic;
		hdcStatic = (HDC)wParam;

		// ���ñ�����ɫΪ͸�����������ÿ���ֵĵ�ɫ
		SetBkMode(hdcStatic, TRANSPARENT);

		// ǿ�������������½���
		if ((HWND)lParam == hAttention)
			SetTextColor(hdcStatic, RGB(255, 0, 0));  // ��ɫ�ı�
		else if((HWND)lParam==hCopyright)
		{
			SetTextColor(hdcStatic, RGB(82, 184, 235));  // �������������ı�
		}
		else
			SetTextColor(hdcStatic, RGB(0, 0, 0));	// ��ɫ�ı�
		// ����һ����ˢ���������ñ�����ɫ
		HBRUSH hBrush;
		hBrush = CreateSolidBrush(RGB(255, 255, 255));  // ��ɫ����
		return (LRESULT)hBrush;  // ���ػ�ˢ
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
		if (wParam == (WPARAM)IDL_fontName)  // �����Ϣ�Ƿ������� ComboBox
		{
			LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT)lParam;
			lpMeasureItem->itemHeight = 30;  // ����ÿ���б���ĸ߶��Լ�ǿ�Ӿ�Ч��
		}
		return TRUE;
	}

	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hwnd, &ps);

		deleteAllCommand(hwnd);
		settingDraw(hwnd, hdc, lParam);

		EndPaint(hwnd, &ps);
		break;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam); // Ĭ�ϴ���
}

void chooseModeDestroy()
{
	if (pBitmap)
		delete pBitmap;

	if(transparencyTimerActive)
		KillTimer(nullptr, IDT_transparency);  // ��ȫ͸����ֹͣ��ʱ��

	// ɾ����������ͷ���Դ
	DeleteObject(hFCaption);
	DeleteObject(hFText);

	PostQuitMessage(0);
}

std::string versionText = "������1.0";
std::mt19937 engine(static_cast<unsigned int>(time(0)));	// ��������������
export LRESULT CALLBACK WPchoose(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		SetTimer(hwnd, IDT_wait, 1000, nullptr);	// �ȴ�5���л���ͼ��ģʽ��
		chooseTitle = versionText;
		break;

	case WM_DESTROY:
		KillTimer(hwnd, IDT_wait);
		chooseModeDestroy();
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
	CHOOSE:	// ��ȡ����
		if (chooseTitle == versionText)	// �޸ı���
		{
			chooseTitle = std::format("{}", data.defaultNames.size()) + "��" + data.defaultList + "��";
		}

		// ������ܵ�͸����
		if (currentAlpha != 255)
		{
			currentAlpha = 255;
			changeAlpha = false;
			KillTimer(hwnd, IDT_transparency);
			transparencyTimerActive = false;
			SetLayeredWindowAttributes(hwnd, 0, currentAlpha, LWA_ALPHA); // ����͸����
		}

		// ���waitNum��Ԥ��ʱ�������Ч��
		waitNum = -1;
		// ��ȡ���ֲ���
		SetTimer(hwnd, IDT_scroll, 50, nullptr);	// ����Ч���ٶ�����
		break;

	case WM_NCHITTEST:
		if (captionMessage(hwnd, lParam))
		{
			if (captionButton(hwnd, lParam))
				return HTCLOSE;	// �����ǹرհ�ť��Ҳ���������ð�ť
			else
				return HTCAPTION;
		}
		break;

	case WM_NCLBUTTONDOWN:	// ʹ��UP�������¼�С���϶���������
		switch (wParam)
		{
		case HTCLOSE:
			short button = captionButton(hwnd, lParam);
			switch (button)
			{
			case close:
				PostMessage(hwnd, WM_CLOSE, 0, 0);	// �ֶ����� WM_CLOSE ��Ϣ
				break;

			case setting:
				ShowWindow(hwnd, SW_HIDE);	// �رմ���
				KillTimer(hwnd, IDT_wait);	// �رռ�ʱ
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
			GetClientRect(hwnd, &invalida); // ��ȡ�ͻ����ߴ�
			if (!scrollNum)	// ���������þ�
			{
				chooseText = data.leftNames.back();	// ��������
				data.leftNames.pop_back();	// ɾ������
				scrollNum = scrollMax;
				KillTimer(hwnd, IDT_scroll);	// �رռ�ʱ
			}
			else
				chooseText = data.defaultNames[engine() % data.defaultNames.size()];	// �������

			InvalidateRect(hwnd, &invalida, true);
			break;

		case IDT_transparency:
			transparency(hwnd, mode);
			break;

		case IDT_wait:
			waitNum++;
			if (waitNum == 8)
			{
				waitNum = 0;	// ����waitNum
				SetTimer(hwnd, IDT_transparency, transparencyT, nullptr);// ��������͸�����޸�ʱ��
				transparencyTimerActive = true;
			}
			break;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam); // Ĭ�ϴ���
}

POINT ptOld;	// ��¼�����ʱ��λ�����ж��Ƿ��ǵ��
bool isDragging;	// ��־�Ƿ����ƶ�
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
		// ת��Ϊ��Ļ���꣨��Ϊ���ڿ��ܱ��ƶ����ֲ����겻���Դ���
		ClientToScreen(hwnd, &ptOld);
		isDragging = false;	// ����isDragging
		// ��������¼���ȷ���϶�����������Ƴ����ڶ��ж�
		SetCapture(hwnd);
		break;

	case WM_LBUTTONUP:
		if (!isDragging)
		{
			SetTimer(hwnd, IDT_transparency, transparencyT, nullptr);	// ��̬�ظ���ȡ����
			transparencyTimerActive = true;
		}
		ReleaseCapture();
		break;

	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON)
		{
			// ������������²��ҷ������ƶ�����ʼ�϶�
			POINT ptNew;
			ptNew.x = GET_X_LPARAM(lParam);
			ptNew.y = GET_Y_LPARAM(lParam);
			ClientToScreen(hwnd, &ptNew);
			// �ж�����Ƿ��ƶ��ˣ�����ƶ��ˣ�����Ϊ���϶�
			int dx = ptNew.x - ptOld.x;
			int dy = ptNew.y - ptOld.y;

			if (abs(dx) > 5 || abs(dy) > 5)
			{  // ����ƶ��ľ��������ֵ����Ϊ���϶�
				isDragging = true;
			}

			if (isDragging)
			{
				// �ƶ�����
				RECT rect;
				GetWindowRect(hwnd, &rect);
				SetWindowPos(hwnd, NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				ptOld = ptNew;  // �������λ��
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
	return DefWindowProc(hwnd, uMsg, wParam, lParam); // Ĭ�ϴ���
}