module;

#include <Windows.h>
#include "ctime"

export module data;
import cryptograph;
import std;

// 抽取时的数据
export class Data
{
public:
	bool ifTop = true;
	// COLORREF要返过来
	COLORREF clientBC = 0xffffff;
	COLORREF clientFC = 0x000000;
	COLORREF captionBC = 0xffcc66;
	COLORREF captionFC = 0xff00ff;
	std::string fontName = "楷体";
	std::string defaultList;	// 这个不会有	# 《》	这些符号
	std::vector<std::string> defaultNames;
	std::vector<std::string> leftNames;
};
export Data data;

std::string binaryFile = "data.bin";	// 加密文件
std::mt19937 engine(static_cast<unsigned int>(time(0)));	// 生成随机数

// 初始化
export bool initializeData()
{
	std::vector<std::string> splitLines;
	if (!DecryptData(splitLines))
		return false;

	// 将解密后的数据保存
	std::istringstream(splitLines[0]) >> std::boolalpha >> data.ifTop;
	data.clientBC = std::stoi(splitLines[1], nullptr, 16);
	data.clientFC = std::stoi(splitLines[2], nullptr, 16);
	data.captionBC = std::stoi(splitLines[3], nullptr, 16);
	data.captionFC = std::stoi(splitLines[4], nullptr, 16);
	data.fontName = splitLines[5];
	data.defaultList = splitLines[6];
	for (short i = 7; i < splitLines.size(); i++)
	{
		// 如果当前行以 '#' 开头
		if (splitLines[i][0] != '#')
			continue;
		// 检查是否匹配默认列表标记
		if (splitLines[i] != "# 《" + data.defaultList + "》")	// 在all中寻找要格式化
			continue;
		// 查找下一个以 '#' 开头的行，确定范围
		short j = i + 1;
		while (j < splitLines.size() && splitLines[j][0] != '#')
			j++;	// j在不为代表'#'符号的行号时会加1
		// 将数据拷贝到 data.names
		// i+1是密码，加j的begin()并不包括j的拷贝
		data.defaultNames.resize(j - i - 2);
		std::copy(splitLines.begin() + i + 2, splitLines.begin() + j, data.defaultNames.begin());

		break;	// 存在数据
	}

	if (data.defaultNames.size() == 0)
	{
		// 没有找到名单名字
		MessageBox(nullptr, "数据文件格式错误了诶。", "X﹏X", MB_ICONERROR);
		return false;
	}

	// 修改剩余名单
	data.leftNames = data.defaultNames;
	shuffle(data.leftNames.begin(), data.leftNames.end(), engine);

	// 随机修改
	if (!(engine() % 10))
		EncryptData(splitLines);

	return true;
}

// 专门用于设置信息存储
export class Store :public Data	// 继承Data的东西
{
public:
	bool ifRight;	// 标志数据是否正常
	std::vector<std::vector<std::string>> all;	// [0]存名单名，[1]存密码
	std::string currentShowList;	// 存储当前正在显示的名单名
public: // 初始化
	Store()
	{
		// 确认是否可以继续初始化
		std::vector<std::string> splitLines;
		if (!DecryptData(splitLines))
		{
			ifRight = false;
			return;	// 维持默认态
		}
		else
			ifRight = true;

		// 再度检查默认名单的完整性
		defaultList = splitLines[6];
		// 默认名字
		for (short i = 7; i < splitLines.size(); i++)
		{
			// 如果当前行以 '#' 开头
			if (splitLines[i][0] != '#')
				continue;
			// 检查是否匹配默认列表标记
			if (splitLines[i] != "# 《" + defaultList + "》")
				continue;
			// 查找下一个以 '#' 开头的行，确定范围
			short j = i + 1;
			while (j < splitLines.size() && splitLines[j][0] != '#')
				j++;	// j在不为代表'#'符号的行号时会加1
			// 将数据拷贝
			// i+1是密码，加j的begin()并不包括j的拷贝
			defaultNames.resize(j - i - 2);
			std::copy(splitLines.begin() + i + 2, splitLines.begin() + j, defaultNames.begin());

			break;	// 存在数据
		}
		if (defaultNames.size() == 0)
		{
			// 没有找到名单名字
			ifRight = false;
			return;
		}

		// 其它data继承部分的初始化
		std::istringstream(splitLines[0]) >> std::boolalpha >> ifTop;
		clientBC = std::stoi(splitLines[1], nullptr, 16);
		clientFC = std::stoi(splitLines[2], nullptr, 16);
		captionBC = std::stoi(splitLines[3], nullptr, 16);
		captionFC = std::stoi(splitLines[4], nullptr, 16);
		fontName = splitLines[5];

		// 所有名单与名字
		short listNum = 0;	// 用于记录当前名单的序号
		for (short i = 7; i < splitLines.size(); i++)
		{
			// 如果当前行以 '#' 开头，则是名单名或名单终止符
			if (splitLines[i][0] != '#')
				continue;

			// 检查是否是名单名
			if (splitLines[i].size() > 2)
			{
				std::string name = splitLines[i];
				name.erase(0, 2);	// 删除开头的'#'和' '两字符

				// 给all新增空名单
				all.push_back({});

				all[listNum].push_back(splitLines[i]);
				all[listNum].push_back(splitLines[i + 1]);	// 这是密码
			}

			// 查找下一个以 '#' 开头的行，确定当前名单范围
			short j = i + 2;	// 从密码后面开始
			while (j < splitLines.size() && splitLines[j][0] != '#')
				j++;	// j在不为'#'符号开头的行会加1
			// 将数据拷贝
			// i+1是密码，加j的begin()并不包括j的拷贝
			all[listNum].resize(j - i);
			std::copy(splitLines.begin() + i + 2, splitLines.begin() + j, all[listNum].begin() + 2);
			listNum++;	// 准备下一个名单

			// 因为j表示的是名单终止符的行号，所以i应转换为：
			i = j + 1;

			break;
		}
	}
};
export Store store;	// 当使用设置页面时定义

export void fontStore(std::string fontName)
{
	store.fontName = fontName;
}

// 用于取消all[][0]的格式
export std::string removeFormat(std::string& str)
{
	return str.substr(4, str.size() - 6);
}