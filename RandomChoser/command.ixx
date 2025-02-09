module;

#include <ctime>
#include <Windows.h>
#include "resource.h"

export module command;
import std;
import data;
import value;

// WindowProc�ĺ�������
export LRESULT CALLBACK WPsetting(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT CALLBACK WPchoose(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
export LRESULT CALLBACK WPicon(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ��������
int capationHeight = GetSystemMetrics(SM_CYCAPTION);
int buttonSize = 20;
std::mt19937 engine(static_cast<unsigned int>(time(0)));	// ��������������

// ��������ҳ�����

// ����ҳ��
export void settingPage(_In_ HINSTANCE hInstance)
{
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

	int nWidth = 742 * 1.5;
	int nHeight = 550;
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

// �ص����������ڱ��������Ӵ���
export BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
	// ���� WM_SETFONT ��Ϣ��lParam=TRUE �ÿؼ������ػ�
	SendMessage(hwnd, WM_SETFONT, (WPARAM)lParam, TRUE);
	return TRUE;
}

// �޸�ָ�����ڼ����ӿؼ�������
export void SetFontForAllControls(HWND hWndParent, HFONT hFont)
{
	// ö�������ӿؼ�
	EnumChildWindows(hWndParent, EnumChildProc, (LPARAM)hFont);
}

// �ص�����������ö������
export int CALLBACK EnumFontFamExProc(const LOGFONT* lpelfe, const TEXTMETRIC* lpntme, DWORD FontType,
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
	std::vector<HWND>child, std::vector<int> id, short& x, short& y, short& sizeY, short xCommand)
{	// xCommand�����ã��Է��޸ĳ�ʼֵ
	// ��ɫ���ݴ���
	int red = GetRValue(color);
	int green = GetGValue(color);
	int blue = GetBValue(color);
	// �ؼ�����
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

	AddStatic(hWnd, "16����", xCommand, y, sizeY);
	xCommand += width * 2.5;

	// ȷ��16λֵ��ȷ
	std::string color16 = std::format("{:06x}", color);
	colorCorrect(color16);
	AddEdit(hWnd, child[3], color16, xCommand, y, width * 3, sizeY, (HMENU)id[3]);
	y += 40;
}

// ���
HWND hTopMostYes, hTopMostNo;

export void settingDraw(HWND& hwnd, HDC& hdc, LPARAM& lParam)
{
	// ������
	short sizeY = 20;
	short x = 10;
	short y = 10;
	short xCommandFirst = 160;
	short xCommand = xCommandFirst;
	// ��������
	if (!hFSetting)
	{
		hFSetting = CreateFont(sizeY, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, store.fontName.c_str());
	}
	SelectObject(hdc, hFSetting);
	SIZE sizeOfFont{};
	GetTextExtentPoint32(hdc, "һ", 3, &sizeOfFont);
	short width = sizeOfFont.cx;

	AddStatic(hwnd, "�Ƿ��ö�", x, y, sizeY);
	AddControl(hwnd, hTopMostYes, "��", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		xCommand, y, 40, 25, (HMENU)IDB_topYes);
	AddControl(hwnd, hTopMostNo, "��", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		xCommand + 50, 10, 40, 25, (HMENU)IDB_topNo);
	// ���ó�ʼֵ
	if (store.ifRight && !store.ifTop)
		SendMessage(hTopMostNo, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(hTopMostYes, BM_SETCHECK, BST_CHECKED, 0);
	y += 40;

	addColor(hwnd, width, "������������ɫ", store.captionBC,
		{ hCaptionBCR ,hCaptionBCG ,hCaptionBCB ,hCaptionBC16 },
		{ IDE_captionBCR ,IDE_captionBCG ,IDE_captionBCB ,IDE_captionBC16 },
		x, y, sizeY, xCommand);

	addColor(hwnd, width, "������������ɫ", store.captionFC,
		{ hCaptionFCR ,hCaptionFCG ,hCaptionFCB ,hCaptionFC16 },
		{ IDE_captionFCR ,IDE_captionFCG ,IDE_captionFCB ,IDE_captionFC16 },
		x, y, sizeY, xCommand);

	addColor(hwnd, width, "��ȡ��������ɫ", store.clientBC,
		{ hClientBCR ,hClientBCG ,hClientBCB ,hClientBC16 },
		{ IDE_clientBCR ,IDE_clientBCG ,IDE_clientBCB ,IDE_clientBC16 },
		x, y, sizeY, xCommand);

	addColor(hwnd, width, "��ȡ��������ɫ", store.clientFC,
		{ hClientFCR ,hClientFCG ,hClientFCB ,hClientFC16 },
		{ IDE_clientFCR ,IDE_clientFCG ,IDE_clientFCB ,IDE_clientFC16 },
		x, y, sizeY, xCommand);

	AddStatic(hwnd, "ѡ������", x, y, sizeY);
	hFontName = CreateWindow("COMBOBOX", "",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL |
		CBS_DROPDOWN | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS,
		xCommand, y, 200, sizeY * 10, hwnd, (HMENU)IDL_fontName, NULL, NULL);
	PopulateFontList(hFontName, fontMap);
	y += 40;

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
	CreateWindow("STATIC", "��Ʒ����github��վ�ϸ��£��������������RandomChoser�ҵ���",
		WS_CHILD | WS_VISIBLE, x, y, width * 30, sizeY, hwnd, NULL, NULL, NULL);
	y += sizeY;
	CreateWindow("STATIC", "����û�������github�����������������������Ļ���������¡�",
		WS_CHILD | WS_VISIBLE, x, y, width * 23, sizeY, hwnd, NULL, NULL, NULL);
	y += sizeY;
	CreateWindow("STATIC", "��ǰ��Ʒ�汾��1.0",
		WS_CHILD | WS_VISIBLE, x, y, width * 23, sizeY, hwnd, NULL, NULL, NULL);
	y += 40;

	hConfirmBtn = CreateWindow("BUTTON", "ȷ���á�\\(�R���Q)/���������������ұߵ�������",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		x, y, width * 22, sizeY + 10, hwnd, (HMENU)IDB_confirm, NULL, NULL);

	// �ұߵ���������
	short span = 650;	// ���ұߵĿ��
	x += span;
	y = 10;
	xCommand += span;

	AddStatic(hwnd, "ѡ��Ĭ������", x, y, sizeY);
	hDefaultList = CreateWindow("COMBOBOX", "",
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
		xCommand, y, 200, sizeY * 10, hwnd, (HMENU)IDL_defaultList, NULL, NULL);
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
	y += 40;

	AddStatic(hwnd, "ѡ����ʾ����", x, y, sizeY);
	hShowList = CreateWindow("COMBOBOX", "",
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
		xCommand, y, 200, sizeY * 10, hwnd, (HMENU)IDL_showList, NULL, NULL);
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
	y += 40;

	AddControl(hwnd, hNewListBtn, "�½�����", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		x, y, width * 5, sizeY + 10, (HMENU)IDB_newList);
	AddControl(hwnd, hModifyBtn, "�޸�����", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		x + width * 5 + 10, y, width * 5, sizeY + 10, (HMENU)IDB_modify);
	if (!store.ifRight)
		EnableWindow(hModifyBtn, FALSE);	// ����û����ȷ�����������Խ���
	AddControl(hwnd, hDeleteBtn, "ɾ������", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		x + width * 10 + 20, y, width * 5, sizeY + 10, (HMENU)IDB_delete);
	if (!store.ifRight)
		EnableWindow(hDeleteBtn, FALSE);	// ����û����ȷ�����������Խ���
	y += 40;
	
	RECT rect;
	GetClientRect(hwnd, &rect);
	short nWidth = rect.right - 10 - x;
	short nHeight = rect.bottom - 10 - y;
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
short nWidth = 200, nHeight = 90;	// ���㴰��λ�õ���
export void choosePage(_In_ HINSTANCE hInstance)
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

	LPCSTR lpWindowName = "������1.0";
	if(hChoose==NULL)// ��������
	{
		hChoose = CreateWindowEx(
			WS_EX_LAYERED,
			className, lpWindowName,
			WS_POPUP,
			(GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2,
			(GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2,   // ����λ��
			nWidth, nHeight,                // ���ڴ�С
			nullptr,                        // �����ھ��
			nullptr,                        // �˵����
			wc.hInstance,                   // ʵ�����
			nullptr                         // ��������
		);
	}

	SetLayeredWindowAttributes(hChoose, 0, 255, LWA_ALPHA); // ���ô���Ϊ֧��͸��

	if (data.ifTop)	// �����ö�
	{
		SetWindowLong(hChoose, GWL_EXSTYLE, GetWindowLong(hChoose, GWL_EXSTYLE) | WS_EX_TOPMOST);
		SetWindowPos(hChoose, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	ShowWindow(hChoose, SW_SHOW); // ��ʾ����
	UpdateWindow(hChoose);
}

export void chooseDraw(HWND& hWnd, HDC& hdc)	// mode��ʾ�Ƿ��ڹ���״̬
{
	// ��ȡ���ڵľ�������
	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);
	// �����������������
	int left = 0, top = 0;
	int right = windowRect.right - windowRect.left;	// ���
	int bottom = capationHeight;	// ��ȡ�Ƽ��������߶�
	// �������ʺͻ�ˢ
	HPEN hPen = CreatePen(PS_SOLID, 2, data.captionBC); // �߿򻭱�
	HBRUSH hBrush = CreateSolidBrush(data.captionBC);   // ������ˢ
	SelectObject(hdc, hPen);
	SelectObject(hdc, hBrush);
	// ���Ʊ���������
	Rectangle(hdc, left, top, right, bottom);
	// ɾ�� GDI ����
	DeleteObject(hBrush);
	DeleteObject(hPen);

	// �����������ڱ߿�
	HPEN borderPen = CreatePen(PS_SOLID, 2, data.captionBC);
	HGDIOBJ BorderPen = SelectObject(hdc, borderPen);
	MoveToEx(hdc, right, 0, NULL);
	LineTo(hdc, right, windowRect.bottom - windowRect.top);	// �ұ߿�
	LineTo(hdc, 0, windowRect.bottom - windowRect.top);	// �±߿�
	LineTo(hdc, 0, 0);	// ��߿�
	DeleteObject(borderPen);

	// �����ı���ɫ�ͱ���ģʽ
	SetTextColor(hdc, data.captionFC);	// Ҳ��������İ�ť
	SetBkMode(hdc, TRANSPARENT);	// ����͸��
	// �������λ��
	RECT textRect = { left,top,right,bottom };
	textRect.left += 10;	// ����������ռ�
	textRect.right -= 50;	// �ܿ��رհ�ť
	// ����
	if (!hFCaption)
	{
		hFCaption = CreateFont(
			(textRect.bottom - textRect.top) * 0.8,	// ����߶�
			0, 0, 0,
			FW_NORMAL,		// �����ϸ��FW_NORMAL, FW_BOLD �ȣ�
			FALSE,          // �Ƿ�б��
			FALSE,          // �Ƿ��»���
			FALSE,          // �Ƿ�ɾ����
			DEFAULT_CHARSET,// �ַ���
			OUT_DEFAULT_PRECIS,   // �������
			CLIP_DEFAULT_PRECIS,  // �ü�����
			DEFAULT_QUALITY,      // �������
			DEFAULT_PITCH | FF_SWISS,	// ������ͼ���
			data.fontName.c_str()
		);
	}
	SelectObject(hdc, hFCaption);
	// ��������
	DrawText(hdc, chooseTitle.c_str(), -1, &textRect, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

	// �رհ�ť
	RECT buttonRect = { right - buttonSize - 10, top + 2, right - 10, top + buttonSize + 2 };
	// ���ƹرհ�ť�ϵġ������������ʹ��ͬһ����
	DrawText(hdc, "��", -1, &buttonRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	// ���ð�ť
	buttonRect.right -= buttonSize;
	buttonRect.left -= buttonSize;
	DrawText(hdc, "��", -1, &buttonRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	// �ͻ����ı�
	// �����Զ���ͻ�����������
	windowRect.bottom -= windowRect.top;
	windowRect.top = capationHeight;
	windowRect.right -= windowRect.left;
	windowRect.left = 0;
	// ����
	SetTextColor(hdc, data.clientFC);
	SetBkMode(hdc, TRANSPARENT);	// ����͸��
	if (!hFText)
	{
		hFText = CreateFont(
			(windowRect.bottom - windowRect.top) * 0.7,	// ����߶�
			0,              // �����ȣ�Ϊ 0 ��ʾ���ݸ߶��Զ����㣩
			0,              // ������б�Ƕȣ���λ 0.1 �ȣ�
			0,              // ������߷���Ƕ�
			FW_NORMAL,		// �����ϸ��FW_NORMAL, FW_BOLD�ȣ�
			FALSE,          // �Ƿ�б��
			FALSE,          // �Ƿ��»���
			FALSE,          // �Ƿ�ɾ����
			DEFAULT_CHARSET,// �ַ���
			OUT_DEFAULT_PRECIS,   // �������
			CLIP_DEFAULT_PRECIS,  // �ü�����
			DEFAULT_QUALITY,		// �������
			DEFAULT_PITCH | FF_SWISS,	// ������ͼ���
			data.fontName.c_str()
		);
	}
	SelectObject(hdc, hFText);
	// �ڿͻ�����ʾ����
	if (data.leftNames.size() == 0)
	{
		data.leftNames = data.defaultNames;
		data.leftNames.push_back("����һ��");
	}
	DrawText(hdc, chooseText.c_str(), -1, &windowRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

export bool captionMessage(HWND& hWnd, LPARAM& lParam)
{
	POINT ptMouse;
	ptMouse.y = HIWORD(lParam);
	ScreenToClient(hWnd, &ptMouse);
	if (ptMouse.y <= capationHeight)
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
	if (ptMouse.x >= windowRect.right - windowRect.left - buttonSize - 10)
		return 1;
	else if (ptMouse.x >= windowRect.right - windowRect.left - 2 * buttonSize - 10)
		return 2;
	else
		return 0;
}

// ͼ��ҳ��

// ���ڴ���λ�õ���
void offset(HWND& hwnd, RECT& newPosition, short& width, short& height)
{
	// ��ȡԭʼ���ڵĴ�С��λ��
	RECT originalRect;
	GetWindowRect(hwnd, &originalRect);

	// ��鴰�ڵ�λ��ƫ��
	bool right = false;
	bool down = true;

	// �޸�λ�ò���
	if (GetSystemMetrics(SM_CXSCREEN) / 2 >
		(originalRect.right + originalRect.left) / 2)
	{
		right = false;	// ƫ��
		newPosition.left = originalRect.left;
	}
	else
	{
		right = true;	// ƫ�һ����
		newPosition.left = originalRect.right - width;
	}

	if (GetSystemMetrics(SM_CYSCREEN) / 2 >
		(originalRect.bottom + originalRect.top) / 2)
	{
		down = false;	// ƫ��
		newPosition.top = originalRect.top;
	}
	else
	{
		down = true;	// ƫ�»����
		newPosition.top = originalRect.bottom - height;
	}
}

HWND hIcon;	// ����Ƶ��ת��
export void iconPage()
{
	short d = 50;	// ����ֱ��

	RECT newPosition;
	offset(hChoose, newPosition, d, d);

	if (!hIcon)
	{
		// ע���´�������
		const char* iconWndName = "icon";

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
			WS_POPUP | WS_SYSMENU,	// ���ڷ��
			newPosition.left,
			newPosition.top,	// ����λ��
			d, d,				// ���ڴ�С
			nullptr,			// �����ھ��
			nullptr,			// �˵����
			iconWc.hInstance,	// ʵ�����
			nullptr				// ��������
		);

		SetLayeredWindowAttributes(hChoose, 0, 0, LWA_ALPHA); // ���ô���Ϊ֧��͸��

		// ����Բ����������Ϊ��������
		HRGN hRegion = CreateEllipticRgn(0, 0, d, d);
		SetWindowRgn(hIcon, hRegion, TRUE);
	}

	// ��ʾ����
	ShowWindow(hChoose, SW_HIDE);
	ShowWindow(hIcon, SW_SHOW);
	UpdateWindow(hIcon);

	// �رճ�ȡ���ڼ�ʱ
	KillTimer(hChoose, IDT_wait);

	// ����λ��
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

export void transparency(HWND& hwnd, short& mode)
{
	if (changeAlpha)	// ����͸����
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
			SetLayeredWindowAttributes(hwnd, 0, currentAlpha, LWA_ALPHA); // ����͸����
		}
	}
	else // ����͸����
	{
		if (currentAlpha)	// currentAlphaΪ0
		{
			currentAlpha -= 5;  // ÿ�μ���5
			SetLayeredWindowAttributes(hwnd, 0, currentAlpha, LWA_ALPHA); // ����͸����
		}
		else
		{
			changeAlpha = true;
			KillTimer(hwnd, IDT_transparency);
			if (mode == normal)
			{
				mode = icon;	// �л�ģʽ���Ϊͼ��
				iconPage();
				SetTimer(hIcon, IDT_transparency, 10, 0);
			}
			else if (mode == icon)
			{
				mode = normal;	// �л�ģʽ���Ϊ��ȡ
				ExitIconMode();
				SetTimer(hChoose, IDT_transparency, 10, 0);
			}
		}
	}
}

// ����ɫͳһ�޸�
export void colorSynergy(HWND& hwnd, short ID, short position)	// Эͬ������ɫ
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
			"�����ɫ���ò���ȷ����\n�����RGB�Ļ���Ҫ0~255��\n�����16λ����Ҫ0x��Ҫ000000~ffffff��",
			"����", MB_ICONERROR);
		HWND hEdit = GetDlgItem(hwnd, ID);
		SetFocus(hEdit);	// ǿ�ƻ�ȥ������д
		return;
	}

	// ��ɫ��ȷ
	short hexID = ID + (4 - position);
	// ��ȡ���
	HWND hHexEdit = GetDlgItem(hwnd, hexID);	// ��ȡ16λ�༭����
	HWND hREdit = GetDlgItem(hwnd, hexID - 3);
	HWND hGEdit = GetDlgItem(hwnd, hexID - 2);
	HWND hBEdit = GetDlgItem(hwnd, hexID - 1);
	switch (position)
	{
	case 1:	// ����Rֵ
		colorStr = readEdit(hwnd, hexID)[0];	// ��ȡ16λ�ַ�
		colorStr.replace(0, 2, std::format("{:02x}", value));
		SetWindowText(hHexEdit, colorStr.c_str());
		break;
	case 2:	// ����Gֵ
		colorStr = readEdit(hwnd, hexID)[0];	// ��ȡ16λ�ַ�
		colorStr.replace(2, 2, std::format("{:02x}",value));
		SetWindowText(hHexEdit, colorStr.c_str());
		break;
	case 3:	// ����Bֵ
		colorStr = readEdit(hwnd, hexID)[0];	// ��ȡ16λ�ַ�
		colorStr.replace(4, 2, std::format("{:02x}", value));
		SetWindowText(hHexEdit, colorStr.c_str());
		break;
	case 4:	// ����16λ
		short R;	R = (value >> 16) & 0xff;
		short G;	G = (value >> 8) & 0xff;
		short B;	B = value & 0xff;
		SetWindowText(hREdit, std::format("{}", R).c_str());
		SetWindowText(hGEdit, std::format("{}", G).c_str());
		SetWindowText(hBEdit, std::format("{}", B).c_str());
		break;
	}

	// �����޸�
	colorStr = readEdit(hwnd, hexID)[0];	// ��ȡ16λ�ַ�
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
	int index = SendMessage(hwnd, CB_GETCURSEL, 0, 0);	// ��ȡ��ǰѡ������
	int length = SendMessage(hwnd, CB_GETLBTEXTLEN, index, 0);
	str.resize(length);
	SendMessage(hwnd, CB_GETLBTEXT, index, (LPARAM)str.data());	// ��ȡ�ı�
}