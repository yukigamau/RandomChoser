module;

#include <Windows.h>

export module glob;

import std;

using std::wstring;

export namespace glob
{
	const wstring VERSION{ L"2.0.0" };

	// 横向
	constexpr double btnOuterSizeScaleH = 1.6;
	// 垂直
	constexpr double btnOuterSizeScaleV = 1.8;

	// 新创建的名单是否是默认名单
	bool ifNewListDefault = false;

	class Font
	{
	private:
		HFONT hFont = nullptr;

	public:
		Font() = default;
		Font(int height, wstring fontName);
		Font(Font &) = delete;	// 还没有写这个的打算
		~Font();

		void ini(int height, wstring fontName);
		HFONT get();
		WPARAM send();
	};

	Font title;
	Font chooseBtn;

	// 创建窗口用
	void createSettingPage();

	// 主要用于抽取窗口与图标窗口的处理
	enum class Mode
	{
		choose,
		icon
	};

	/* 用于给抽取名字时滚动 */
	int scrollNumMax = 10;	// 滚动数字上限
	int scrollNum = scrollNumMax;	// 当前滚动名字剩余数
	constexpr int scrollInterval = 50;	// 滚动间隔时间
	constexpr int TRANSPARENCY_INTERVAL = 5;
	bool transparencyTimerActive = false;	// 透明度计时器是否处于活动状态，如果是是，需要在销毁窗口时杀掉
}

glob::Font::Font(int height, wstring fontName)
{
	hFont = CreateFont(
		height,	// 字体高度
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
		fontName.c_str()
	);
}

glob::Font::~Font()
{
	DeleteObject(hFont);
	hFont = nullptr;
}

void glob::Font::ini(int height, wstring fontName)
{
	hFont = CreateFont(
		height,	// 字体高度
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
		fontName.c_str()
	);
}

HFONT glob::Font::get()
{
	return hFont;
}

WPARAM glob::Font::send()
{
	return (WPARAM)hFont;
}