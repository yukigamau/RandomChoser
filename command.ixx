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

// WindowProc�ĺ�������
export LRESULT CALLBACK WPsetting(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT CALLBACK WPchoose(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT CALLBACK WPicon(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ����ҳ��
export void settingPage(_In_ HINSTANCE hInstance)
{
	if (store.ifRight)
		initializeStore();	// ���ڳ�ʼ���洢���ݣ���������������������ã���ʹ��Ĭ��ֵ
	settingInstance = hInstance;	// ���ں���������ť

	// ע�ᴰ����
	const char* className = "����ҳ��";
	WNDCLASS wc = {};
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpfnWndProc = WPsetting; // ���ô��ڹ��̺���
	wc.hInstance = GetModuleHandle(nullptr); // ��ȡʵ�����
	wc.hIcon = LoadIconA(hInstance, (char*)IDI_ICON1);
	wc.lpszClassName = className;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wc);

	int nWidth = 742 * 1.5 * dpiScale;
	int nHeight = 550 * dpiScale;
	LPCSTR lpWindowName = "����������";
	// ��������
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int x = (screenWidth - nWidth) / 2;
	int y = (screenHeight - nHeight) / 2;
	HWND hwnd = CreateWindow(
		className, lpWindowName,
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,	// ��ֹ���ڸı��С
		(GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2, (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2,
		nWidth, nHeight,
		nullptr, nullptr,
		wc.hInstance,	// ʵ�����
		nullptr			// ��������
	);

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);	// �Ѵ�����ʾ����ǰ��
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

// �ص����������ڱ��������Ӵ���
BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
	// ���� WM_SETFONT ��Ϣ��lParam=TRUE �ÿؼ������ػ�
	SendMessage(hwnd, WM_SETFONT, (WPARAM)lParam, TRUE);
	return TRUE;
}

// �޸�ָ�����ڼ����ӿؼ�������
void SetFontForAllControls(HWND hWndParent, HFONT hFont)
{
	// ö�������ӿؼ�
	EnumChildWindows(hWndParent, EnumChildProc, (LPARAM)hFont);
}

// �ص�����������ö������
int CALLBACK EnumFontFamExProc(const LOGFONT* lpelfe, const TEXTMETRIC* lpntme, DWORD FontType,
	LPARAM lParam)
{
	if (lpelfe->lfCharSet != GB2312_CHARSET)
	{
		return 1; // ���˷���������
	}

	HWND hComboBox = (HWND)lParam;
	char fontName[LF_FACESIZE];
	int count = SendMessage(hComboBox, CB_GETCOUNT, 0, 0);

	// �����ظ����
	for (int i = 0; i < count; i++)
	{
		SendMessage(hComboBox, CB_GETLBTEXT, i, (LPARAM)fontName);
		if (strcmp(fontName, lpelfe->lfFaceName) == 0)
		{
			return 1;
		}
	}

	// ��ӵ��б�
	SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)lpelfe->lfFaceName);

	return 1;
}

// ��������б���Ϊÿ�����崴�����
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

			// ���˵� @ ��ͷ������
			if (fontName[0] != '@')
			{
				// ȷ�����ظ����
				if (SendMessage(hComboBox, CB_FINDSTRINGEXACT, -1, (LPARAM)fontName.c_str()) == CB_ERR)
				{
					SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)fontName.c_str());
				}
			}
			return 1; // ����ö��
		}, (LPARAM)hComboBox, 0);
	ReleaseDC(NULL, hdc);

	int count = SendMessage(hComboBox, CB_GETCOUNT, 0, 0);
	for (int i = 0; i < count; i++)
	{
		char fontName[LF_FACESIZE];
		SendMessage(hComboBox, CB_GETLBTEXT, i, (LPARAM)fontName);

		// �����������
		HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontName);
		fontMap[fontName] = hFont;
	}

	// ѡ�������е�����
	int index = SendMessage(hComboBox, CB_FINDSTRINGEXACT, -1, (LPARAM)store.fontName.c_str());
	SendMessage(hComboBox, CB_SETCURSEL, index, 0);
}

// ����ɫ���ÿؼ�����
void addColor(HWND& hWnd, short& width, std::string staticText, COLORREF& color,
	HWND child, int id, short& x, short& y, short& sizeY, short xCommand)
{	// xCommand�����ã��Է��޸ĳ�ʼֵ
	// ��ɫ���ݴ���
	int red = GetRValue(color);
	int green = GetGValue(color);
	int blue = GetBValue(color);
	// �ؼ�����
	AddStatic(hWnd, staticText.c_str(), x, y, sizeY);
	AddStatic(hWnd, "16����", xCommand, y, sizeY);
	xCommand += width * 2.5;

	// ȷ��16λֵ��ȷ
	std::string color16 = std::format("{:06x}", color);
	colorCorrect(color16);
	AddEdit(hWnd, child, color16, xCommand, y, width * 3, sizeY, (HMENU)id);
	y += yAdd;
}

// ���
HWND hTopMostYes, hTopMostNo;

export void settingDraw(HWND& hwnd, HDC& hdc, LPARAM& lParam)
{
	// ������
	short sizeY = 20 * dpiScale;
	short x = 10 * dpiScale;
	short y = 10 * dpiScale;
	short xCommandFirst = 160 * dpiScale;
	short xCommand = xCommandFirst;

	// ��������
	if (!hFSetting)
	{
		hFSetting = CreateFont(sizeY, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, store.fontName.c_str());
	}
	SelectObject(hdc, hFSetting);
	SIZE sizeOfFont{};
	GetTextExtentPoint32(hdc, "һ", 3, &sizeOfFont);
	short width = sizeOfFont.cx;

	/* �ö�ѡ�� */
	AddStatic(hwnd, "�Ƿ��ö�", x, y, sizeY);
	AddControl(hwnd, hTopMostYes, "��", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		xCommand, y, 40 * dpiScale, 25 * dpiScale, (HMENU)IDB_topYes);
	AddControl(hwnd, hTopMostNo, "��", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		xCommand + 50 * dpiScale, 10 * dpiScale, 40 * dpiScale, 25 * dpiScale, (HMENU)IDB_topNo);
	// ���ó�ʼֵ
	if (store.ifRight && !store.ifTop)
		SendMessage(hTopMostNo, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(hTopMostYes, BM_SETCHECK, BST_CHECKED, 0);
	y += yAdd;

	addColor(hwnd, width, "������������ɫ", store.captionBC, hCaptionBC16, IDE_captionBC16,
		x, y, sizeY, xCommand);

	addColor(hwnd, width, "������������ɫ", store.captionFC, hCaptionFC16, IDE_captionFC16,
		x, y, sizeY, xCommand);

	addColor(hwnd, width, "��ȡ��������ɫ", store.clientBC, hClientBC16, IDE_clientBC16,
		x, y, sizeY, xCommand);

	addColor(hwnd, width, "��ȡ��������ɫ", store.clientFC, hClientFC16, IDE_clientFC16,
		x, y, sizeY, xCommand);

	AddStatic(hwnd, "ѡ������", x, y, sizeY);
	hFontName = CreateWindow("COMBOBOX", "",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN,
		xCommand, y, 200 * dpiScale, sizeY * 10, hwnd, (HMENU)IDL_fontName, NULL, NULL);
	PopulateFontList(hFontName, fontMap);
	y += yAdd * 2;

	// ��ѧ�ı�
	hCopyright = CreateWindow("STATIC", "�������Ȩ��github�û�yukigamau���У�����BSD 3-Clause��",
		WS_CHILD | WS_VISIBLE, x, y, width * 26, sizeY, hwnd, NULL, NULL, NULL);
	y += sizeY;
	hAttention = CreateWindow("STATIC", "�������������ֱ�Ӵ���������",
		WS_CHILD | WS_VISIBLE, x, y, width * 13, sizeY, hwnd, NULL, NULL, NULL);	// �������
	y += sizeY;
	CreateWindow("STATIC", "���ý��ԡ���������ʾ�ڳ�ȡ�������Ǹ��������ϣ��û�ʹ��ʱ�ɿ�����",
		WS_CHILD | WS_VISIBLE, x, y, width * 26, sizeY, hwnd, NULL, NULL, NULL);
	y += sizeY;
	CreateWindow("STATIC", "�û��벻Ҫ˽���޸ı���Ʒ���κ��ļ��ڲ���������ʱ��",
		WS_CHILD | WS_VISIBLE, x, y, width * 28, sizeY, hwnd, NULL, NULL, NULL);
	y += sizeY;
	CreateWindow("STATIC", "���8��ʱ�䲻�û��Բ��",
		WS_CHILD | WS_VISIBLE, x, y, width * 23, sizeY, hwnd, NULL, NULL, NULL);
	y += sizeY;
	CreateWindow("STATIC", "������վ1��www.github.com/yukigamau/RandomChoser",
		WS_CHILD | WS_VISIBLE, x, y, width * 30, sizeY, hwnd, NULL, NULL, NULL);
	y += sizeY;
	CreateWindow("STATIC", "������վ2��randomChoser.netlify.app��û��www��",
		WS_CHILD | WS_VISIBLE, x, y, width * 23, sizeY, hwnd, NULL, NULL, NULL);
	y += sizeY;
	std::string nowVersion = "��ǰ��Ʒ�汾��" + versionText;
	CreateWindow("STATIC", nowVersion.c_str(),
		WS_CHILD | WS_VISIBLE, x, y, width * 23, sizeY, hwnd, NULL, NULL, NULL);
	y += yAdd;

	hConfirmBtn = CreateWindow("BUTTON", "ȷ���á�\\(�R���Q)/���������������ұߵ�������",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		x, y, width * 22, sizeY + 10, hwnd, (HMENU)IDB_confirm, NULL, NULL);

	// �ұߵ���������
	short span = 650 * dpiScale;	// ���ұߵĿ��
	x += span;
	y = 10 * dpiScale;
	xCommand += span;

	AddStatic(hwnd, "ѡ��Ĭ������", x, y, sizeY);
	hDefaultList = CreateWindow("COMBOBOX", "",
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
		xCommand, y, 200 * dpiScale, sizeY * 10, hwnd, (HMENU)IDL_defaultList, NULL, NULL);
	SendMessage(hDefaultList, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));	// ȡ��ѡ��
	if (store.defaultList == "")
		EnableWindow(hDefaultList, FALSE);	// ���ÿؼ�
	else
	{
		for (short i = 0; i < store.all.size(); i++)
		{
			std::string addChoice = removeFormat(store.all[i][0]);
			SendMessage(hDefaultList, CB_ADDSTRING, 0, (LPARAM)addChoice.c_str());
			if(addChoice==store.defaultList)
				SendMessage(hDefaultList, CB_SETCURSEL, i, 0);	// ����Ĭ����
			store.currentShowList = store.defaultList;
		}
	}
	y += yAdd;

	AddStatic(hwnd, "ѡ����ʾ����", x, y, sizeY);
	hShowList = CreateWindow("COMBOBOX", "",
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
		xCommand, y, 200 * dpiScale, sizeY * 10, hwnd, (HMENU)IDL_showList, NULL, NULL);
	SendMessage(hShowList, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));	// ȡ��ѡ��
	if (!store.ifRight || ifTypeName)
	{
		EnableWindow(hShowList, FALSE);	// ���ÿؼ�
		showName = false;
	}
	else
	{
		for (short i = 0; i < store.all.size(); i++)
		{
			// ȡ��all�ĸ�ʽ
			std::string addChoice = removeFormat(store.all[i][0]);
			SendMessage(hShowList, CB_ADDSTRING, 0, (LPARAM)addChoice.c_str());
			if (addChoice == store.currentShowList)
				SendMessage(hShowList, CB_SETCURSEL, i, 0);	// ����Ĭ����
		}
		showName = true;
	}
	y += yAdd;

	AddControl(hwnd, hNewListBtn, "�½�����", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		x, y, width * 5, sizeY + 10 * dpiScale, (HMENU)IDB_newList);
	AddControl(hwnd, hModifyBtn, "�޸�����", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		x + width * 5 + 24 * dpiScale, y, width * 5, sizeY + 10 * dpiScale, (HMENU)IDB_modify);
	if (!store.ifRight)
		EnableWindow(hModifyBtn, FALSE);	// ����û����ȷ�����������Խ���
	AddControl(hwnd, hDeleteBtn, "ɾ������", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		x + width * 10 + 48 * dpiScale, y, width * 5, sizeY + 10 * dpiScale, (HMENU)IDB_delete);
	if (!store.ifRight)
		EnableWindow(hDeleteBtn, FALSE);	// ����û����ȷ�����������Խ���
	y += yAdd;
	
	RECT rect;
	GetClientRect(hwnd, &rect);
	short nWidth = rect.right - x;
	short nHeight = rect.bottom - y;
	// ������ʾ�ı�
	std::string nameStr;
	if (store.ifRight && store.defaultList != "" && createSetting)
		readName = store.defaultNames;
	for (std::string str : readName)
		nameStr += str + "\r\n";
	createSetting = false;
	// ��������
	hNameEdit = CreateWindow("EDIT", nameStr.c_str(),
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL |
		ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE,
		x, y, nWidth, nHeight, hwnd, (HMENU)IDE_names, nullptr, nullptr);

	// �����ӿؼ�����
	SetFontForAllControls(hwnd, hFSetting);
}

export void drawFontList(LPARAM lParam)
{
	LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
	if (lpdis->CtlID == IDL_fontName)
	{	// �����б�� ID
		char fontName[LF_FACESIZE];
		SendMessage(lpdis->hwndItem, CB_GETLBTEXT, lpdis->itemID, (LPARAM)fontName);

		// ��ȡ����
		HFONT hFont = fontMap[fontName];
		HDC hdc = lpdis->hDC;
		SelectObject(hdc, hFont);

		// ���ñ�����ǰ����ɫ
		SetBkColor(hdc, GetSysColor(lpdis->itemState & ODS_SELECTED ? COLOR_HIGHLIGHT : COLOR_WINDOW));
		SetTextColor(hdc, GetSysColor(lpdis->itemState & ODS_SELECTED ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));

		// ��䱳��
		ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &lpdis->rcItem, NULL, 0, NULL);

		// ����������
		DrawText(hdc, fontName, -1, &lpdis->rcItem, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
	}

}

// ��ȡҳ��
HWND hChoose;	// ����Ƶ��ת��
export void choosePage(_In_ HINSTANCE hInstance)
{
	if(hChoose==NULL)// ��������
	{
		// ע�ᴰ����
		const char* className = "��ȡҳ��";
		WNDCLASS wc = {};
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpfnWndProc = WPchoose;	// ���ô��ڹ��̺���
		wc.hInstance = GetModuleHandle(nullptr);	// ��ȡʵ�����
		wc.hIcon = LoadIconA(hInstance, (char*)IDI_ICON1);
		wc.lpszClassName = className;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClass(&wc);

		LPCSTR lpWindowName = "������";
		hChoose = CreateWindowEx(
			WS_EX_LAYERED | WS_EX_TOPMOST,
			className, lpWindowName,
			WS_POPUP,
			(GetSystemMetrics(SM_CXSCREEN) - chooseWidth) / 2,
			(GetSystemMetrics(SM_CYSCREEN) - chooseHeight) / 2,   // ����λ��
			chooseWidth, chooseHeight,                // ���ڴ�С
			nullptr,                        // �����ھ��
			nullptr,                        // �˵����
			wc.hInstance,                   // ʵ�����
			nullptr                         // ��������
		);
		SetLayeredWindowAttributes(hChoose, 0, 255, LWA_ALPHA); // ���ô���Ϊ֧��͸��

		if (!data.ifTop)	// �ö����ô���
			SetWindowPos(hChoose, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	ShowWindow(hChoose, SW_SHOW); // ��ʾ����
	UpdateWindow(hChoose);
}

export void chooseDraw(HWND& hWnd, HDC& hdc)	// mode��ʾ�Ƿ��ڹ���״̬
{
	// ��ȡ���ڵľ�������
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	// �������ʺͻ�ˢ
	HBRUSH hBrushBk = CreateSolidBrush(data.clientBC);   // ������ˢ
	SelectObject(hdc, hBrushBk);
	// ���Ƶ�ɫ����
	Rectangle(hdc, 0, captionHeight - 1, clientRect.right, clientRect.bottom);
	// ɾ�� GDI ����
	DeleteObject(hBrushBk);
	
	// �������ʺͻ�ˢ
	HPEN hPen = CreatePen(PS_SOLID, 2 * dpiScale, data.captionBC); // �߿򻭱�
	HBRUSH hBrush = CreateSolidBrush(data.captionBC);   // ������ˢ
	SelectObject(hdc, hPen);
	SelectObject(hdc, hBrush);
	// ���Ʊ���������
	Rectangle(hdc, 0, 0, clientRect.right, captionHeight);
	// ɾ�� GDI ����
	DeleteObject(hBrush);
	DeleteObject(hPen);

	// �����ı���ɫ�ͱ���ģʽ
	SetTextColor(hdc, data.captionFC);	// Ҳ��������İ�ť
	SetBkMode(hdc, TRANSPARENT);	// ����͸��
	// �������λ��
	RECT textRect = { 0,0,clientRect.right,captionHeight };
	textRect.left += 10 * dpiScale;	// ����������ռ�
	textRect.right -= 50 * dpiScale;	// �ܿ��رհ�ť
	// ����
	if (!hFCaption)
	{
		hFCaption = CreateFont(
			(textRect.bottom - textRect.top) * 0.8,	// ����߶�
			0, 0, 0,
			FW_NORMAL,	// �����ϸ
			FALSE,          // �Ƿ�б��
			FALSE,          // �Ƿ��»���
			FALSE,          // �Ƿ�ɾ����
			DEFAULT_CHARSET,// �ַ���
			OUT_DEFAULT_PRECIS,   // �������
			CLIP_DEFAULT_PRECIS,  // �ü�����
			CLEARTYPE_QUALITY,      // �������
			DEFAULT_PITCH | FF_SWISS,	// ������ͼ���
			data.fontName.c_str()
		);
	}
	SelectObject(hdc, hFCaption);
	// ��������
	DrawText(hdc, chooseTitle.c_str(), -1, &textRect, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

	// �رհ�ť
	RECT buttonRect = { clientRect.right - buttonSize - 10 * dpiScale, 2 * dpiScale,
		clientRect.right - 10 * dpiScale, buttonSize + 2 * dpiScale };
	// ���ƹرհ�ť�ϵġ������������ʹ��ͬһ����
	DrawText(hdc, "��", -1, &buttonRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	// ���ð�ť
	buttonRect.right -= buttonSize;
	buttonRect.left -= buttonSize;
	DrawText(hdc, "��", -1, &buttonRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	// �ͻ���
	// ����
	SetTextColor(hdc, data.clientFC);
	SetBkMode(hdc, TRANSPARENT);
	if (!hFText)
	{
		hFText = CreateFont(
			(clientRect.bottom - captionHeight) * 0.7,	// ����߶�
			0,              // �����ȣ�Ϊ 0 ��ʾ���ݸ߶��Զ����㣩
			0,              // ������б�Ƕȣ���λ 0.1 �ȣ�
			0,              // ������߷���Ƕ�
			FW_NORMAL,		// �����ϸ
			FALSE,          // �Ƿ�б��
			FALSE,          // �Ƿ��»���
			FALSE,          // �Ƿ�ɾ����
			DEFAULT_CHARSET,// �ַ���
			OUT_DEFAULT_PRECIS,   // �������
			CLIP_DEFAULT_PRECIS,  // �ü�����
			CLEARTYPE_QUALITY,		// �������
			DEFAULT_PITCH | FF_SWISS,	// ������ͼ���
			data.fontName.c_str()
		);
	}
	// ��ӡ�ı�
	hChooseText = CreateWindow("STATIC", chooseText.c_str(), WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
		0, captionHeight, clientRect.right, clientRect.bottom - captionHeight,
		hWnd, (HMENU)IDS_chooseText, NULL, NULL);
	// ��������
	SendMessage(hChooseText, WM_SETFONT, (WPARAM)hFText, TRUE);
}

export bool captionMessage(HWND& hWnd, LPARAM& lParam)
{
	POINT ptMouse;
	ptMouse.y = HIWORD(lParam);
	ScreenToClient(hWnd, &ptMouse);
	if (ptMouse.y <= captionHeight)
		return true;  // ���� HTCAPTION����ʾ����ڱ������������϶�����
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

// ͼ��ҳ��

// ���ڴ���λ�õ���
void offset(HWND& hwnd, RECT& newPosition, short width, short height)
{
	// ��ȡԭʼ���ڵĴ�С��λ��
	RECT originalRect;
	GetWindowRect(hwnd, &originalRect);

	// ��鴰�ڵ�λ��ƫ��
	bool right = false;
	bool down = true;

	// �޸�λ�ò���
	// ˮƽ
	if (GetSystemMetrics(SM_CXSCREEN) / 2 >
		(originalRect.right + originalRect.left) / 2)
	{
		right = false;	// ƫ��
		if (originalRect.left < 0)
			newPosition.left = 0;
		else
			newPosition.left = originalRect.left;
	}
	else
	{
		right = true;	// ƫ�һ����
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);  // ��Ļ���
		if (originalRect.right > screenWidth)
			newPosition.left = screenWidth - width;
		else
			newPosition.left = originalRect.right - width;
	}
	// ��ֱ
	if (GetSystemMetrics(SM_CYSCREEN) / 2 >
		(originalRect.bottom + originalRect.top) / 2)
	{
		down = false;	// ƫ��
		if (originalRect.top < 0)
			newPosition.top = 0;
		else
			newPosition.top = originalRect.top;
	}
	else
	{
		down = true;	// ƫ�»����
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);  // ��Ļ�߶�
		if (originalRect.bottom > screenHeight)
			newPosition.top = screenHeight - height;
		else
			newPosition.top = originalRect.bottom - height;
	}
}

HWND hIcon;	// ����Ƶ��ת��
export void iconPage()
{
	RECT newPosition;
	offset(hChoose, newPosition, g_sizeWnd.cx, g_sizeWnd.cy);

	const char* iconWndName = "icon";
	// ��������
	// ע���´�������
	WNDCLASS iconWc = {};
	iconWc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	iconWc.hInstance = GetModuleHandle(nullptr); // ��ȡʵ�����
	iconWc.lpfnWndProc = WPicon;
	iconWc.lpszClassName = iconWndName;
	iconWc.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&iconWc);

	// �����´���
	hIcon = CreateWindowEx(
		WS_EX_LAYERED,
		iconWndName,	// ��������
		"������ͼ��ģʽ",		// ���ڱ���
		WS_POPUP,	// ���ڷ��
		newPosition.left, newPosition.top,	// ����λ��
		g_sizeWnd.cx, g_sizeWnd.cy,			// ���ڴ�С
		nullptr,			// �����ھ��
		nullptr,			// �˵����
		iconWc.hInstance,	// ʵ�����
		nullptr				// ��������
	);

	// ��ʾ����
	ShowWindow(hChoose, SW_HIDE);
	ShowWindow(hIcon, SW_SHOWNOACTIVATE);
	UpdateWindow(hIcon);

	// �رճ�ȡ���ڼ�ʱ
	KillTimer(hChoose, IDT_wait);

	// ����λ��
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

// ����
export void deleteAllCommand(HWND& hWnd)
{
	// ɾ�����пؼ�
	HWND child;
	child = GetWindow(hWnd, GW_CHILD); // ��ȡ��һ���Ӵ���
	while (child)
	{
		DestroyWindow(child); // �����Ӵ���
		child = GetWindow(hWnd, GW_CHILD); // ��ȡ��һ���Ӵ���
	}
}

export std::vector<std::string> readEdit(HWND& hwnd, short ID)
{
	std::vector<std::string> lines;

	// ��ȡ EditBox ���
	HWND hEdit = GetDlgItem(hwnd, ID);
	if (!hEdit) return lines;

	// ��ȡ�ı�����
	int len = GetWindowTextLengthA(hEdit);
	if (!len) return lines;  // ���û�����ݣ�ֱ�ӷ���

	// ��ȡ�ı�
	std::vector<char> buffer(len + 1);
	GetWindowTextA(hEdit, buffer.data(), len + 1);

	// ���зָ��ַ���
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
			vec.erase(vec.begin() + i);	// ȥ���յ�
			i--;	// ��ֹ��©
			size--;	// ��ֹ���
		}
	}

	if (!vec.size())	// û��Ԫ��
		return false;
	else
		return true;
}

short change = 15;	// ͸���ȱ仯
export void transparency(HWND& hwnd, short& mode)
{
	if (changeAlpha)	// ����͸����
	{
		currentAlpha += change;
		if (currentAlpha >= 255)
		{
			KillTimer(hwnd, IDT_transparency);
			changeAlpha = false;
			transparencyTimerActive = false;
		}
	}
	else // ����͸����
	{
		currentAlpha -= change;  // ÿ�μ���͸����
		if (currentAlpha <= change)
		{
			KillTimer(hwnd, IDT_transparency);
			changeAlpha = true;
			if (mode == normal)
			{
				mode = icon;	// �л�ģʽ���Ϊͼ��
				iconPage();
				SetTimer(hIcon, IDT_transparency, transparencyT, 0);
			}
			else if (mode == icon)
			{
				mode = normal;	// �л�ģʽ���Ϊ��ȡ
				ExitIconMode();
				SetTimer(hChoose, IDT_transparency, transparencyT, 0);
				SetTimer(hChoose, IDT_scroll, scrollT, 0);	// ͬʱ��ʼ�������Ż�ʹ��
			}
		}
	}

	// ����͸����
	if (mode == normal)
		SetLayeredWindowAttributes(hwnd, 0, currentAlpha, LWA_ALPHA);
	else if (mode == icon)
	{
		BLENDFUNCTION blend = { 0 };
		blend.BlendOp = AC_SRC_OVER;
		blend.SourceConstantAlpha = (BYTE)currentAlpha;
		blend.AlphaFormat = AC_SRC_ALPHA;
		UpdateLayeredWindow(
			hwnd,           // ���ھ��
			NULL,           // Ŀ�� HDC����Ϊ NULL��
			NULL,           // pptDst = NULL��λ�ò��䣩
			NULL,           // psize = NULL���ߴ粻�䣩
			g_hdcMem,         // Դ HDC������͸���ȵ�ͼ��
			NULL,           // pptSrc = (0,0)
			RGB(0, 0, 0),     // ��ɫ����δʹ�ã�
			&blend,         // ��Ϻ��������� Alpha��
			ULW_ALPHA       // ʹ�� Alpha ���
		);
	}
}

export void selectChange(HWND& hwnd, std::string& str)
{
	int index = SendMessage(hwnd, CB_GETCURSEL, 0, 0);	// ��ȡ��ǰѡ������
	int length = SendMessage(hwnd, CB_GETLBTEXTLEN, index, 0);
	str.resize(length);
	SendMessage(hwnd, CB_GETLBTEXT, index, (LPARAM)str.data());	// ��ȡ�ı�
}

export void selfRestart(HWND& hwnd)
{
	// ��ȡ��ǰ�����·��
	wchar_t szPath[MAX_PATH];
	GetModuleFileNameW(NULL, szPath, MAX_PATH);

	// ��������������Ϣ
	STARTUPINFOW si = { sizeof(STARTUPINFOW) };
	PROCESS_INFORMATION pi;

	// �����µĽ���
	CreateProcessW(
		szPath,   // ��ǰ����·��
		NULL,      // �����в���
		NULL,      // ���̰�ȫ����
		NULL,      // �̰߳�ȫ����
		FALSE,     // ���̳о��
		0,         // ������־
		NULL,      // ��������
		NULL,      // ��ǰĿ¼
		&si,       // ������Ϣ
		&pi);    // ������Ϣ

	// �˳���ǰ����
	ExitProcess(0);
}