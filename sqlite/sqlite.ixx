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
		// pos从0开始
		template<class T>
		void column(T& out, int pos);

	private:
		void doprepare(const wstring& s);
		void exec(const wstring& s);
		void open(const wstring& s);
		void prepare(const wstring& s);
		void step() const;

	public:
		void insert_replace(const wstring& table, const vector<wstring>& columns, const vector<wstring>& values);
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

void sqlite::Sql::doprepare(const wstring& s)
{
	int rc = sqlite3_prepare16_v2(pdb, s.c_str(), -1, &stmt.stmt, nullptr);
	if (rc != SQLITE_OK)
		throw runtime_error("sql语法错误。");
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
	wstring sql = L"INSERT OR REPLACE INTO " + table + L' ' + L'(';
	for (size_t i = 0; i < columns.size() - 1; i++)
		sql += columns[i] + L", ";
	sql += columns.back();
	sql += L") VALUES (";
	for (size_t i = 0; i < values.size() - 1; i++)
		sql += L'\''+values[i] + L"\', ";
	sql += L'\'' + values.back() + L'\'';
	sql += L");";

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
		sql += L" " + where;
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