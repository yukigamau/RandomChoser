module;

#include <Windows.h>

export module value;
import std;

// ��ť
export constexpr short
IDB_topYes	= 1,
IDB_topNo	= 2,
IDB_newList = 3,
IDB_confirm = 4,
IDB_modify	= 5,
IDB_delete	= 6
;
// �༭��
export constexpr short
IDE_captionBC16 = 101,
IDE_captionFC16 = 102,
IDE_clientBC16	= 103,
IDE_clientFC16	= 104,
IDE_password	= 105,
IDE_names		= 106	// ����ҳ������ֱ༭��
;
// �б��
export constexpr short
IDL_fontName	= 201,
IDL_defaultList = 202,
IDL_showList	= 203
;
// ��ʱ��
export constexpr short
IDT_scroll			= 301,
IDT_wait			= 302,
IDT_transparency	= 303
;
// �ı���
export constexpr short
IDS_chooseText = 401
;
// ���
export HFONT hFCaption;
export HFONT hFText;
export HFONT hFSetting;
export HWND
hAttention,		hCopyright,
hDefaultList,	hFontName,		hNameEdit,		hShowList,
hCaptionBC16,	hCaptionFC16,	hClientBC16,	hClientFC16,
hNewListBtn,	hConfirmBtn,	hModifyBtn,		hDeleteBtn,
hChooseText
;
// mode����
export constexpr short
normal	= 1,
icon	= 2
;
// captionButton�ķ���ֵ
export const short
other	= 0,
close	= 1,
setting	= 2
;

export short
transparencyT = 5,	// ͸�����޸Ķ�ʱ��
scrollT = 50
;

export bool
changeAlpha = false,	// ��־����͸���������ӻ��Ǽ��٣�trueΪ���ӣ�commandҪ��
createSetting = true,	// ��־�Ƿ����ڴ�������ҳ�棬��ֹ�û���������ֶ�ʧ
ifTypeName = false,	// ��־�Ƿ���������������������
showName = false,	// ��־�Ƿ�������ʾ����
transparencyTimerActive = false	// commandҪ��
;

export BYTE currentAlpha = 255; // ��ǰ͸���ȣ���ʼΪ��ȫ��͸����commandҪ��
export std::map<std::string, HFONT> fontMap; // �洢�������
export HINSTANCE settingInstance;	// �洢����ҳ��ʹ�õ�ʵ��

export std::string
versionText = "1.4",
chooseTitle = "������" + versionText,	// ��ȡ�������������
chooseText = "�����ȡ";	// ��ȡ����

// ����ͼ��ֲ㴰��
export SIZE	g_sizeWnd = { 50, 50 };	// ͼ�괰�ڴ�С
export HDC g_hdcMem;	// �ڴ�DC

export std::vector<std::string>
newNameTip=	// Ĭ����ʾ�ı�
{
	"���ڴ˿������½������ڵ����֣�",
	"һ��һ��Ŷ��",
	"Ȼ�����½�������",
	"�Ϳ��Դ�������������",
	"�����˰�������ɾ��Ŷ��"
},
readName = newNameTip	// ���ڶ�ȡ�����༭���ı�
;

// ����16������ɫ�ı���ǰ�����λ
export void colorCorrect(std::string& color16)
{
	color16[0] ^= color16[4];
	color16[4] ^= color16[0];
	color16[0] ^= color16[4];
	color16[1] ^= color16[5];
	color16[5] ^= color16[1];
	color16[1] ^= color16[5];
}

// ����16��������ǰ�����λ
export void swapHexParts(COLORREF& original)
{
	// ��ȡǰ��λ����8λ���ͺ���λ����8λ��
	unsigned int front = (original >> 16) & 0xFF;  // ǰ��λ
	unsigned int end = original & 0xFF;            // ����λ
	unsigned int middle = original & 0x00FF00;     // �м���λ

	// ����ǰ����λ���ϲ��м䲿��
	original = (end << 16) | middle | front;
}

// ��DPI����
export double dpiScale = 1;	// Ĭ��DPI
// ��ȡ��ǰ����ʾ���� DPI ���ű���
export UINT GetDPIScalingFactor()
{
	HDC hdc = GetDC(NULL);
	UINT dpi = GetDeviceCaps(hdc, LOGPIXELSX); // 96 DPI Ϊ 100% ����
	ReleaseDC(NULL, hdc);
	return MulDiv(dpi, 100, 96); // ���ذٷֱȣ��� 200 ��ʾ 200%��
}
// ��DPIӰ��ĳߴ�
export int buttonSize = 20;	// ��ť��С
export short chooseWidth = 200, chooseHeight = 90;	// ��ȡ���ڴ�С
export int captionHeight;	// �������߶�
export short yAdd = 30;	// ����ҳ��ؼ�������
export void sizeByDPI()
{
	dpiScale = GetDPIScalingFactor() / 96.0;

	// ͼ�괰��
	g_sizeWnd.cx *= dpiScale;
	g_sizeWnd.cy *= dpiScale;

	// ��ȡ����
	buttonSize *= dpiScale;
	chooseWidth *= dpiScale;
	chooseHeight *= dpiScale;
	captionHeight = GetSystemMetrics(SM_CYCAPTION);

	// ����ҳ��
	yAdd *= dpiScale;
}