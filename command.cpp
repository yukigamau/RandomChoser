#include <Windows.h>

module command;

import data;
import error;
import std;
import value;
using namespace std;

//————————————————————
// 用于读取EditBox中的文本内容

// 只读一行
bool readEditText(HWND& hwnd, short ID, _Out_ std::vector<char>& buffer)
{
	// 获取 EditBox 句柄
	HWND hEdit = GetDlgItem(hwnd, ID);
	if (!hEdit) return false;

	// 获取文本长度
	int len = GetWindowTextLength(hEdit);
	if (!len) return false;  // 如果没有内容，直接返回

	// 读取文本
	buffer.resize(len + 1);
	GetWindowText(hEdit, buffer.data(), len + 1);
	return true;
}

void spiltStr(_In_ std::vector<char>& buffer, _Out_ std::vector<std::string>& lines)
{
	// 按行分割字符串
	std::stringstream ss(buffer.data());
	std::string line;
	while (std::getline(ss, line))
		lines.push_back(line);
}

std::vector<std::string> readEdit(HWND& hwnd, short ID)
{
	std::vector<char> buffer;
	std::vector<std::string> lines;

	if (readEditText(hwnd, ID, buffer))
		spiltStr(buffer, lines);
	else
		lines = { " " };	// 表示什么都没有，并防止后续出错

	return lines;
}

//————————————————————
// 用于处理16进制颜色的读取和检查

bool colorCheck(std::string& color)
{
	// 判断字符数量是否正确
	if (color.size() != 6)
		return false;

	// 利用ASCII码判断字符是否为十六进制字符
	for (char ch : color)
	{
		if (ch < '0')
			return false;
		else if (ch > '9' && ch < 'A')
			return false;
		else if (ch > 'F' && ch < 'a')
			return false;
		else if (ch > 'f')
			return false;
		else
			continue;
	}

	// 全部检查正确
	return true;
}

void colorError()
{
	ShowError("颜色格式错误！");
}

void colorStore(string& color, short& which)
{
	switch (which)
	{
	case cliBC:
		store.clientBC = std::stoi(color, nullptr, 16);
		swapHexParts(store.clientBC);
		break;

	case cliFC:
		store.clientFC = std::stoi(color, nullptr, 16);
		swapHexParts(store.clientFC);
		break;
		
	case capBC:
		store.captionBC = std::stoi(color, nullptr, 16);
		swapHexParts(store.captionBC);
		break;

	case capFC:
		store.captionFC = std::stoi(color, nullptr, 16);
		swapHexParts(store.captionFC);
		break;
	}
}

// 设置原本颜色
void colorBack(HWND& hwnd, short& id, short& which)
{
	COLORREF color;

	switch (which)
	{
	case cliBC:
		color = store.clientBC;
		break;

	case cliFC:
		color = store.clientFC;
		break;

	case capBC:
		color = store.captionBC;
		break;

	case capFC:
		color = store.captionFC;
		break;
	}

	string text = std::format("{:06x}", color);
	colorCorrect(text);
	SetWindowText(GetDlgItem(hwnd, id), text.c_str());	// 设置默认颜色
}

void color(HWND& hwnd, short id, short which)
{
	string color = readEdit(hwnd, id)[0];

	if (colorCheck(color))
		colorStore(color, which);
	else
	{
		colorError();
		colorBack(hwnd, id, which);
	}
}

//————————————————————
// 用于重启
vector<wchar_t> getPath()
{
	// 获取当前程序的路径
	wchar_t szPath[MAX_PATH];
	GetModuleFileNameW(NULL, szPath, MAX_PATH);	// 必须使用宽字符集
	vector<wchar_t> path = { std::begin(szPath),std::end(szPath) };
	return path;
}

void openNewProcess()
{
	vector<wchar_t> path = getPath();

	// 创建进程启动信息
	STARTUPINFOW si = { sizeof(STARTUPINFOW) };
	PROCESS_INFORMATION pi;

	// 启动新的进程
	CreateProcessW(
		path.data(),   // 当前程序路径
		NULL,			// 命令行参数
		NULL,			// 进程安全属性
		NULL,			// 线程安全属性
		FALSE,			// 不继承句柄
		0,				// 创建标志
		NULL,			// 环境变量
		NULL,			// 当前目录
		&si,			// 启动信息
		&pi);			// 进程信息
}

void selfRestart()
{
	openNewProcess();

	// 退出当前进程
	ExitProcess(0);
}