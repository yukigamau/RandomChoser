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
IDE_captionBCR	= 101,
IDE_captionBCG	= 102,
IDE_captionBCB	= 103,
IDE_captionBC16 = 104,
IDE_captionFCR	= 105,
IDE_captionFCG	= 106,
IDE_captionFCB	= 107,
IDE_captionFC16 = 108,
IDE_clientBCR	= 109,
IDE_clientBCG	= 110,
IDE_clientBCB	= 111,
IDE_clientBC16	= 112,
IDE_clientFCR	= 113,
IDE_clientFCG	= 114,
IDE_clientFCB	= 115,
IDE_clientFC16	= 116,
IDE_password	=117,
IDE_names		= 118	// ����ҳ������ֱ༭��
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
// ���
export HFONT hFCaption;
export HFONT hFText;
export HFONT hFSetting;
export HWND
hAttention,		hCopyright,
hDefaultList,	hFontName,		hNameEdit,		hShowList,
hCaptionBCR,	hCaptionBCG,	hCaptionBCB,	hCaptionBC16,
hCaptionFCR,	hCaptionFCG,	hCaptionFCB,	hCaptionFC16,
hClientBCR,		hClientBCG,		hClientBCB,		hClientBC16,
hClientFCR,		hClientFCG,		hClientFCB,		hClientFC16,
hNewListBtn,	hConfirmBtn,	hModifyBtn,		hDeleteBtn;
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
chooseTitle,	// ��ȡ�������������
chooseText = "�����ȡ";	// ��ȡ����

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

export void colorCorrect(std::string& color16)
{
	color16[0] ^= color16[4];
	color16[4] ^= color16[0];
	color16[0] ^= color16[4];
	color16[1] ^= color16[5];
	color16[5] ^= color16[1];
	color16[1] ^= color16[5];
}