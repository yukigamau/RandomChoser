export module dataread;

import <Windows.h>;

import sqlite;
import sqlitedefault;
import std;

using sqlite::Sql;
using sqlitedefault::database_name, sqlitedefault::DB_INI;
using sqlitedefault::list_table, sqlitedefault::skin_table;
using std::filesystem::exists, std::filesystem::file_size, std::mt19937, std::random_device;
using std::getline, std::stoi, std::shuffle, std::to_wstring, std::uniform_int_distribution, std::vector;
using std::wstring, std::wstringstream;

export namespace dataread
{
	class Data
	{
	private:
		bool ifOk = false;	// 表示是否有数据文件
	public:
		// COLORREF使用16进制要返过来，可读性不如RGB宏
		COLORREF clientBC = RGB(255, 255, 255);
		COLORREF clientFC = RGB(0, 123, 187);
		COLORREF captionBC = RGB(102, 204, 255);
		COLORREF captionFC = RGB(255, 255, 255);
	public:
		wstring fontName = L"楷体";
	public:
		bool ifTop = true;
	public:
		vector<wstring> lists;
		wstring defaultList;
		vector<wstring> defaultNames;
		vector<wstring> leftNames;
	private:
		// 用于得到随机数种子
		mt19937 rng{ random_device{}() };

	public:
		Data(){}
		// 在关闭前要保存数据
		~Data();

	public:
		void ini(bool ifChoose = true);

	private:
		void getSkin();

	public:
		wstring nameOut();
		wstring nameRandom();
		void saveNewList(const wstring& title, const wstring& text, bool ifDefault);
} data;

	// 拼接字符串
	wstring cat(const vector<wstring>& v);
	bool ifDataExists();
	bool ifFontExists(const wstring& font);
}

// 在关闭前要保存数据
dataread::Data::~Data()
{
	// 按理来说应该是由设置时处理
	
	//// 只有在有相关数据的情况下才进行保存，确保至少有一个名单在数据库中
	//if(ifOk)
	//{
	//	// 加入DB_INI，防止出现未输入任何信息就直接退出导致程序错误
	//	Sql sql(database_name, DB_INI);
	//	wstring w = cat(leftNames);
	//	sql.insert_replace(list_table, { L"name",L"data" }, { defaultList + L"left'", w });
	//}
}

void dataread::Data::ini(bool ifChoose)
{
	Sql sql(database_name);
	const vector<wstring> list{ L"data" };
	wstring where = L"name = 'theme'";
	sql.select(skin_table, list, where);
	wstring theme = sql.column<wstring>(0);

	if (theme != L"default")
		getSkin();

	where = L"name = 'defaultList'";
	sql.select(list_table, list, where);
	sql.column(defaultList, 0);

	where = L"name = '" + defaultList + L"'";
	sql.select(list_table, list, where);
	wstring defaultListWS;
	sql.column(defaultListWS, 0);
	{
		wstringstream wss(defaultListWS);
		wstring listLine;
		while (wss >> listLine)
			defaultNames.push_back(listLine);
	}

	where = L"name = '" + defaultList + L"left'";
	sql.select(list_table, list, where);
	wstring defaultListLeftWS;
	sql.column(defaultListLeftWS, 0);
	{
		wstringstream wss(defaultListLeftWS);
		wstring listLine;
		while (wss >> listLine)
			leftNames.push_back(listLine);
	}

	if (ifChoose)
		return;

	// 以下是只有设置页面才会用到的：
	// table = L"listdb";	已经在上面完成了这个操作
	where = L"name = 'lists'";
	sql.select(list_table, list, where);
	wstring listsLine;
	sql.column(listsLine, 0);
	{
		wstringstream wss(listsLine);
		while
		while (wss >> listsLine)
			lists.push_back(listsLine);
	}
}

void dataread::Data::getSkin()
{
	Sql sql(database_name);
	const wstring table = L"skindb";
	const vector<wstring> list{L"data" };

	/* 颜色 */
	// 颜色被以特定的顺序存储在名为color的行中
	wstring where = L"name = 'color'";
	sql.select(table, list, where);
	wstring color;
	sql.column(color, 0);

	if (color.empty())
		return;

	wstringstream wss(color);
	wstring r,g,b;

	wss >> r >> g >> b;
	clientBC = RGB(stoi(r), stoi(g), stoi(b));
	wss >> r >> g >> b;
	clientFC = RGB(stoi(r), stoi(g), stoi(b));
	wss >> r >> g >> b;
	captionBC = RGB(stoi(r), stoi(g), stoi(b));
	wss >> r >> g >> b;
	captionFC = RGB(stoi(r), stoi(g), stoi(b));

	/* 字体 */
	where = L"name = 'font'";
	sql.select(table, list, where);
	wstring font;
	sql.column(font, 0);
	if (font.size() && ifFontExists(font))
		fontName = font;

	/* 是否置顶 */
	where = L"name = 'ifTop'";
	sql.select(table, list, where);
	sql.column(ifTop, 0);
}

wstring dataread::Data::nameOut()
{
	// 防空
	if (leftNames.empty())
	{
		leftNames = defaultNames;
		// 打乱，使用洗牌函数保证随机性
		shuffle(leftNames.begin(), leftNames.end(),rng);
	}

	wstring r = leftNames.back();
	leftNames.pop_back();
	return r;
}

wstring dataread::Data::nameRandom()
{
	// uniform_int_distribution是闭区间
	uniform_int_distribution<int> dist(0, defaultNames.size() - 1);
	int r = dist(rng);
	return defaultNames.at(r);
}

wstring dataread::cat(const vector<wstring>& v)
{
	wstring r;
	for (wstring w : v)
		r += w + L"\n";
	return r;
}

bool dataread::ifDataExists()
{
	if (exists(database_name) && file_size(database_name))
		return true;
	else
		return false;
}

/* 用于下面的检查字体是否存在 */
int findFont(const LOGFONT*, const TEXTMETRIC*, DWORD, LPARAM lParam)
{
	*reinterpret_cast<bool*>(lParam) = true;
	return 0; // 找到就停
}

bool dataread::ifFontExists(const wstring& font)
{
	LOGFONT lf{};
	lf.lfCharSet = DEFAULT_CHARSET;
	wcscpy_s(lf.lfFaceName, font.c_str());

	HDC hdc = GetDC(nullptr);
	bool exists = EnumFontFamiliesEx(
		hdc, &lf, findFont,
		reinterpret_cast<LPARAM>(&exists),
		0
	);
	ReleaseDC(nullptr, hdc);
	return exists;
}

void dataread::Data::saveNewList(const wstring& title, const wstring& text, bool ifDefault)
{
	Sql sql(database_name);
	wstring where = L"name = 'lists'";
	const vector<wstring> list{ L"data" };
	sql.select(list_table, list, where);
	wstring oldLists = sql.column<wstring>(0);
}