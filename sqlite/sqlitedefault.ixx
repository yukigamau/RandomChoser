export module sqlitedefault;

import sqlite;
import std;

using std::string, std::wstring;

/*
* 命名空间名：	sqlitedefault
* 作用：		包含了本解决方案中的默认数据库设定
* 修改时间：	20251227
* 修改内容：	创建
*/
export namespace sqlitedefault
{
	const wstring database_name = L"database.db";
	/*
	* theme参数说明：	default跟随系统
	*					diy自定义
	*/
	const wstring DB_INI =
		L"CREATE TABLE IF NOT EXISTS skin_table("
		L"name TEXT PRIMARY KEY, "
		L"data TEXT);"
		L"INSERT OR IGNORE INTO skin_table (name, data) "
		L"VALUES ('theme', 'default');"
		L"CREATE TABLE IF NOT EXISTS list_table("
		L"name TEXT PRIMARY KEY, "
		L"data TEXT);"
		L"INSERT OR IGNORE INTO list_table (name, data) "
		L"VALUES('defaultList', NULL);"
		L"INSERT OR IGNORE INTO list_table (name, data) "
		L"VALUES('lists', NULL);";

	const wstring list_table = L"list_table";
	const wstring skin_table = L"skin_table";

	// 用于在必要时充当数据里面的分割符，常规的空格、换行不好用时使用
	// 这个字在一般的输入法里面是打不出来的
	//const wstring SPLIT = L"棫";
}