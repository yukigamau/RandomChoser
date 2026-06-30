export module sqlitedefault;

import sqlite;
import std;

using std::string, std::wstring, std::wstring_view;

/*
* 命名空间名：	sqlitedefault
* 作用：		包含了本解决方案中的默认数据库设定
* 修改时间：	20251227
* 修改内容：	创建
*/
export namespace sqlitedefault
{
	constexpr wstring_view database_name = L"database.db";
	/*
	* theme参数说明：	default跟随系统
	*					diy自定义
	*/
	constexpr wstring_view DB_INI =
		L"CREATE TABLE IF NOT EXISTS skin_table("
		L"name TEXT PRIMARY KEY, "
		L"data TEXT); "
		L"INSERT OR IGNORE INTO skin_table (name, data) "
		L"VALUES ('theme', 'default');"
		L"CREATE TABLE IF NOT EXISTS list_table("
		L"name TEXT PRIMARY KEY, "
		L"data TEXT, "
		L"password TEXT); "
		L"INSERT OR IGNORE INTO list_table (name, data) "
		L"VALUES('defaultList', NULL);"
		L"INSERT OR IGNORE INTO list_table (name, data) "
		L"VALUES('lists', NULL);";

	constexpr wstring_view list_table = L"list_table";
	constexpr wstring_view skin_table = L"skin_table";

	// 用于不可修改的名单
	constexpr wstring_view not_use_password = L"\u0001";
}