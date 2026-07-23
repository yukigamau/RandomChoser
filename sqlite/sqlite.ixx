module;

#include <sqlite3.h>;

export module sqlite;

import std;

using std::function, std::getline, std::is_same_v, std::runtime_error, std::vector;
using std::wstring, std::wstringstream;

/*
* 命名空间名：	sqlite
* 作用：		封装数据库操作 
* 修改时间：	20251227
* 修改内容：	创建
*/
export namespace sqlite
{
	class Stmt
	{
	public:
		sqlite3_stmt* stmt = nullptr;

	public:
		~Stmt();

	public:
		void finalize();
	};

	class Sql
	{
	private:
		sqlite3* pdb = nullptr;
		Stmt stmt;

	public:
		Sql(const wstring& dir);
		Sql(const wstring& dir, const wstring& ini);
		~Sql();

	public:
#pragma region 查询声明

		// pos从0开始

		// 这个没有下面的好，需要逐步丢弃
		template<class T>
		void column(T& out, int pos);

		template<class T>
		T column(int pos);

#pragma endregion


	private:
		// 预准备sql语句
		void doprepare(const wstring& s);
		void exec(const wstring& s);
		void open(const wstring& s);
		void prepare(const wstring& s);
		void step() const;

	public:
		void insert_replace(const wstring& table, const vector<wstring>& columns, const vector<wstring>& values);
		// 构建select语句
		void select(const wstring& table, const vector<wstring>& list, const wstring where = L"");
	};
}

sqlite::Stmt::~Stmt()
{
	finalize();
}

void sqlite::Stmt::finalize()
{
	if (stmt)
	{
		sqlite3_finalize(stmt);
		stmt = nullptr;
	}
}

sqlite::Sql::Sql(const wstring& dir)
{
	open(dir);
}
sqlite::Sql::Sql(const wstring& dir, const wstring& ini)
{
	open(dir);
	exec(ini);
}
sqlite::Sql::~Sql()
{
	if (pdb)
		sqlite3_close(pdb);
}

#pragma region 查询实现

template<class T>
void sqlite::Sql::column(T& out, int pos)
{
	if constexpr (is_same_v<T, bool>)
	{
		const wchar_t* p = static_cast<const wchar_t*>(sqlite3_column_text16(stmt.stmt, pos));
		if (p == L"true")
			out = true;
		else
			out = false;
	}
	else if constexpr (is_same_v<T, wstring>)
	{
		const wchar_t* p = static_cast<const wchar_t*>(sqlite3_column_text16(stmt.stmt, pos));
		out = p ? p : L"";
	}
	else
		throw runtime_error("未处理的查询类型");
}

template<class T>
T sqlite::Sql::column(int pos)
{
	if constexpr (is_same_v<T, bool>)
	{
		const wchar_t* p = static_cast<const wchar_t*>(sqlite3_column_text16(stmt.stmt, pos));
		return p == L"true";
	}
	else if constexpr (is_same_v<T, wstring>)
	{
		const wchar_t* p = static_cast<const wchar_t*>(sqlite3_column_text16(stmt.stmt, pos));
		return p ? p : L"";
	}
	else
		throw runtime_error("未处理的查询类型");
}

#pragma endregion

void sqlite::Sql::doprepare(const wstring& s)
#if _DEBUG
#else
noexcept
#endif
{
	int rc = sqlite3_prepare16_v2(pdb, s.c_str(), -1, &stmt.stmt, nullptr);
#if _DEBUG
	if (rc != SQLITE_OK)
	{
		std::string sErr = "sql错误：\n";
		sErr += sqlite3_errmsg(pdb);
		throw runtime_error(sErr);
	}
#endif
}

void sqlite::Sql::exec(const wstring& s)
{
	wstringstream wss(s);
	wstring single;
	while (getline(wss, single, L';'))
	{
		if (single.empty())
			continue;

		// 补回被分割的分号
		single += L";";

		doprepare(single);
		step();

		stmt.finalize();
	}
}

void sqlite::Sql::open(const wstring& s)
{
	int rc = sqlite3_open16(s.c_str(), &pdb);
	if (rc)
		throw runtime_error("sql语法错误。");
}

void sqlite::Sql::prepare(const wstring& s)
{
	stmt.finalize();
	doprepare(s);
}

void sqlite::Sql::insert_replace(const wstring& table, const vector<wstring>& columns, const vector<wstring>& values)
{
	// 列表名
	wstring wsColumns;
	for (const wstring &ws : columns)
		wsColumns += ws + L", ";
	// 去除最后的", "
	wsColumns.pop_back();
	wsColumns.pop_back();

	// 值内容
	wstring wsValue;
	for (const wstring &ws : values)
	{
		// 可能有多个\0结尾，需要去除
		if (ws.size() >= 2 && ws[ws.size() - 2] == L'\0')
		{
			wstring cutEnd = ws;
			cutEnd.resize(std::wcslen(cutEnd.c_str()));
			wsValue += std::format(L"'{}', ", cutEnd);
		}
		else
			wsValue += std::format(L"\'{}\', ", ws);
	}
	// 去除最后的", "
	wsValue.pop_back();
	wsValue.pop_back();

	wstring sql = std::format(L"INSERT OR REPLACE INTO {} ({}) VALUES ({});",
		table, wsColumns, wsValue);

	prepare(sql);
	step();
}

void sqlite::Sql::select(const wstring& table, const vector<wstring>& list, const wstring where)
{
	wstring sql = L"SELECT ";

	for (size_t i = 0; i < list.size() - 1; i++)
		sql += list[i] + L", ";
	sql += list.back() + L" ";

	sql += L"FROM ";
	sql += table;

	if (where.size())
	{
		sql += L" where " + where;
	}

	sql += L";";

	prepare(sql);
	step();
}

void sqlite::Sql::step() const
{
	int rc = sqlite3_step(stmt.stmt);
	if (rc != SQLITE_DONE && rc != SQLITE_ROW)
		throw runtime_error("sql语句执行失败。");
}