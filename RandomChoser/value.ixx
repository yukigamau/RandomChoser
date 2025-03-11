module;

#include <Windows.h>

export module value;
import std;

// 按钮
export constexpr short
IDB_topYes	= 1,
IDB_topNo	= 2,
IDB_newList = 3,
IDB_confirm = 4,
IDB_modify	= 5,
IDB_delete	= 6
;
// 编辑框
export constexpr short
IDE_captionBC16 = 101,
IDE_captionFC16 = 102,
IDE_clientBC16	= 103,
IDE_clientFC16	= 104,
IDE_password	= 105,
IDE_names		= 106	// 设置页面的名字编辑框
;
// 列表框
export constexpr short
IDL_fontName	= 201,
IDL_defaultList = 202,
IDL_showList	= 203
;
// 计时器
export constexpr short
IDT_scroll			= 301,
IDT_wait			= 302,
IDT_transparency	= 303
;
// 文本框
export constexpr short
IDS_chooseText = 401
;
// 句柄
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
// mode的量
export constexpr short
normal	= 1,
icon	= 2
;
// captionButton的返回值
export const short
other	= 0,
close	= 1,
setting	= 2
;

export short
transparencyT = 5,	// 透明度修改定时器
scrollT = 50
;

export bool
changeAlpha = false,	// 标志更改透明度是增加还是减少，true为增加，command要用
createSetting = true,	// 标志是否正在创建设置页面，防止用户输入的名字丢失
ifTypeName = false,	// 标志是否正在输入新名单的名字
showName = false,	// 标志是否正在显示名字
transparencyTimerActive = false	// command要用
;

export BYTE currentAlpha = 255; // 当前透明度，初始为完全不透明，command要用
export std::map<std::string, HFONT> fontMap; // 存储字体对象
export HINSTANCE settingInstance;	// 存储设置页面使用的实例

export std::string
chooseTitle,	// 抽取界面标题栏标题
chooseText = "点击抽取";	// 抽取名字

export std::vector<std::string>
newNameTip=	// 默认显示文本
{
	"请在此框输入新建名单内的名字，",
	"一行一个哦。",
	"然后点击新建名单，",
	"就可以创建新名单啦！",
	"别忘了把这五行删掉哦！"
},
readName = newNameTip	// 用于读取名单编辑框文本
;

// 交换16进制颜色文本的前后各两位
export void colorCorrect(std::string& color16)
{
	color16[0] ^= color16[4];
	color16[4] ^= color16[0];
	color16[0] ^= color16[4];
	color16[1] ^= color16[5];
	color16[5] ^= color16[1];
	color16[1] ^= color16[5];
}

// 交换16进制数的前后各两位
export void swapHexParts(COLORREF& original)
{
	// 提取前两位（高8位）和后两位（低8位）
	unsigned int front = (original >> 16) & 0xFF;  // 前两位
	unsigned int end = original & 0xFF;            // 后两位
	unsigned int middle = original & 0x00FF00;     // 中间两位

	// 交换前后两位，合并中间部分
	original = (end << 16) | middle | front;
}
