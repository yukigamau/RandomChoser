export module dataread;

import <Windows.h>;

import sqlite;
import sqlitedefault;
import std;

using sqlite::Sql;
using sqlitedefault::database_name, sqlitedefault::DB_INI;
using sqlitedefault::list_table, sqlitedefault::skin_table;
using std::deque, std::mt19937, std::random_device, std::seed_seq;
using std::uniform_int_distribution, std::vector;
using std::filesystem::exists, std::filesystem::file_size;
using std::format, std::getline, std::stoi, std::shuffle, std::time, std::to_wstring;
using std::wstring, std::wstringstream;

constexpr int maxNameLast = 6;

export namespace dataread
{
	class Steady
	{
	public:
		Steady(int len);
		~Steady();

	private:
		deque<wstring> data;

	public:
		// 添加新的数据，并顶替最旧的数据
		void push(const wstring &val);

	public:
		auto begin()noexcept { return data.begin(); }
		auto begin()const noexcept { return data.cbegin(); }
		auto end()noexcept { return data.end(); }
		auto end()const noexcept { return data.cend(); }
		auto cbegin()const noexcept { return data.cbegin(); }
		auto cend()const noexcept { return data.cend(); }
	};

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
		wstring fontName = L"Simsun";
	public:
		bool ifTop = true;
	public:
		vector<wstring> lists;
		wstring defaultList;
		vector<wstring> defaultNames;
		vector<wstring> leftNames;
		Steady lastNames{ maxNameLast };
	private:
		// 用于得到随机数种子，在构造函数中初始化
		mt19937 rng;

	public:
		Data();
		// 在关闭前要保存数据
		~Data();

	public:
		void ini(bool ifChoose = true);
	private:
		// 初始化leftNames需要用特别的方法
		// 当前的抽取防止抽取不当用的是上次的最后几个下次也在名单的最后面。
		void iniLeftNames();

	public:
		void getLists();
	private:
		void getSkin();

	public:
		void changeDefaultList(const wstring &title);
		wstring nameOut();
		wstring nameRandom();
		void saveLists(const wstring &title, bool ifDefault);
		void saveListText(const wstring &title, const wstring &text,
			const wstring &password = sqlitedefault::not_use_password);
} data;

	// 拼接字符串
	wstring cat(const vector<wstring>& v);
	bool ifDataExists();
	bool ifFontExists(const wstring& font);
}

dataread::Steady::Steady(int len)
{
	data.resize(len);
}

dataread::Steady::~Steady()
{
}

void dataread::Steady::push(const wstring &val)
{
	data.push_back(val);
	data.pop_front();
}

dataread::Data::Data()
{
	// 初始化随机种子
	random_device rd;
	seed_seq ss{ rd(), (unsigned int)time(nullptr), 0xdeadbeef }; // 混合多个源
	rng = mt19937{ ss };
}

// 在关闭前要保存数据
dataread::Data::~Data()
{
	if (ifOk)
	{
		wstring wsText;
		for (auto &ws : lastNames)
			wsText += ws + L"\n";

		Sql sql(database_name, DB_INI);
		vector<wstring> columns{ L"name",L"data" };
		vector<wstring> values{ defaultList + L"last", wsText };
		sql.insert_replace(list_table, columns, values);
	}
}

vector<wstring> splitWString(const wstring &text)
{
	wstringstream wss(text);
	wstring line;
	vector<wstring> lines;
	while (wss >> line)
		if (line.size())
			lines.push_back(line);
	return lines;
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
	defaultNames = splitWString(defaultListWS);

	where = format(L"name = '{}last'", defaultList);
	sql.select(list_table, list, where);
	wstring defaultListLastWS;
	sql.column(defaultListLastWS, 0);

	wstringstream wss(defaultListLastWS);
	wstring listLine;
	while (wss >> listLine)
		lastNames.push(listLine);

	iniLeftNames();

	if (ifChoose)
	{
		ifOk = true;
		return;
	}
}

void copy(const dataread::Steady &s, vector<wstring> *vws)
{
	for (auto ws : s)
		vws->push_back(ws);
}

void dataread::Data::iniLeftNames()
{
	leftNames = defaultNames;

	std::unordered_set<wstring> deq_lookup(lastNames.begin(), lastNames.end());
	std::erase_if(leftNames, [&](wstring x) { return deq_lookup.contains(x); });

	copy(lastNames, &leftNames);
}

void dataread::Data::getLists()
{
	Sql sql(database_name);
	const vector<wstring> list{ L"data" };
	wstring where = L"name = 'lists'";
	sql.select(list_table, list, where);

	wstring listsLine;
	sql.column(listsLine, 0);
	lists = splitWString(listsLine);
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

void dataread::Data::changeDefaultList(const wstring &title)
{
	Sql sql(database_name, DB_INI);

	const vector<wstring> columns{ L"name",L"data" };
	const vector<wstring> values{ L"defaultList",title };
	sql.insert_replace(list_table, columns, values);
}

wstring dataread::Data::nameOut()
{
	if (leftNames.empty())
	{
		leftNames = defaultNames;
		shuffle(leftNames.begin(), leftNames.end(), rng);
		return L"抽完一轮";
	}

	wstring r = leftNames.back();
	leftNames.pop_back();
	lastNames.push(r);	// 登记最后一个名字
	return r;
}

wstring dataread::Data::nameRandom()
{
	// uniform_int_distribution是闭区间
	uniform_int_distribution<int> dist(0, defaultNames.size() - 1);
	int r = dist(rng);
	return defaultNames[r];
}

wstring dataread::cat(const vector<wstring>& v)
{
	wstring r;
	for (wstring w : v)
		r += w + L"\n";
	return r;
}

// 利用标准库的函数进行文件存在性的判断
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

/*
* 函数ifFontExists
*	用于确定是否有字体在电脑上
*/
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

/*
* 函数：		Data::saveLists
* 作用：		保存名单名
* @title：		新名单名称
* @ifDefault：	是否是默认名单
*/
void dataread::Data::saveLists(const wstring &title, bool ifDefault)
{
	Sql sql(database_name, DB_INI);
	wstring strLists;

	wstring where = L"name = 'lists'";
	const vector<wstring> list{ L"data" };
	sql.select(list_table, list, where);
	strLists = sql.column<wstring>(0);
	strLists += L"\n";	// 注意如果是用命令行去看，会换行，导致像是新的数据
	strLists += title;

	const vector<wstring> columns{ L"name",L"data" };
	const vector<wstring> values
	{
		L"lists",
		strLists
	};
	sql.insert_replace(list_table, columns, values);

	if (ifDefault)
		changeDefaultList(title);
}

/*
* 函数：		Data::saveListText
* 作用：		保存名单内容
* @title：		名单名称
* @text：		名单内容
* @password：	密码
* @ifDefault：	是否是默认名单
*/
void dataread::Data::saveListText(const wstring &title, const wstring &text, const wstring &password)
{
	Sql sql(database_name, DB_INI);
	const vector<wstring> columns{ L"name",L"data",L"password" };
	vector<wstring> values{ title,text,password };
	sql.insert_replace(list_table, columns, values);

	values[0] += L"left";

	vector<wstring> names = splitWString(text);
	shuffle(values.begin(), values.end(), rng);
	values[1] = L"";
	for (wstring ws : names)
		values[1] += ws + L"\n";

	values[2] = sqlitedefault::not_use_password;
	sql.insert_replace(list_table, columns, values);
}