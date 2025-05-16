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
	COLORREF clientFC = RGB(0, 123, 187);
	COLORREF captionBC = RGB(102, 204, 255);
	COLORREF captionFC = 0xffffff;
	std::string fontName = "楷体";
	std::string defaultList;	// 这个不会有	# 《》	这些符号
	std::vector<std::string> defaultNames;
	std::vector<std::string> leftNames;
};
export Data data;

export std::string binaryFile = "data.bin";	// 加密文件
export std::random_device rd;	// 硬件生成随机数
export std::mt19937 engine(rd());	// 用于随机数

// 用于寻找名单名字
template <typename Container>
bool searchData(std::vector<std::string>& scope, Container& save, short skip)
{
	for (short i = 0; i < scope.size(); i++)
	{
		// 如果当前行以 '#' 开头
		if (scope[i][0] != '#')
			continue;
		// 检查是否匹配默认列表标记
		if (scope[i] != "# 《" + data.defaultList + "》")
			continue;
		// 查找下一个以 '#' 开头的行，确定范围
		short j = i + 1;
		while (j < scope.size() && scope[j][0] != '#')
			j++;	// j在不为代表'#'符号的行号时会加1

		// 将数据拷贝
		// 对于Data和Store的赋值，i+1是密码，加j的begin()并不包括j的拷贝
		save.resize(j - i - 1 - skip);	// 单纯的j-i会包括[j]，所以要-1
		// 注意[i]是名单名字，要+1
		std::copy(scope.begin() + i + 1 + skip, scope.begin() + j, save.begin());

		return true;	// 存在数据

	}
	return false;
}

// 初始化
export bool initializeData()
{
	std::vector<std::string> splitLines;
	if (!DecryptData(splitLines, binaryFile))
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
		//// 如果当前行以 '#' 开头
		//if (splitLines[i][0] != '#')
		//	continue;
		//// 检查是否匹配默认列表标记
		//if (splitLines[i] != "# 《" + data.defaultList + "》")	// 在all中寻找要格式化
		//	continue;
		//// 查找下一个以 '#' 开头的行，确定范围
		//short j = i + 1;
		//while (j < splitLines.size() && splitLines[j][0] != '#')
		//	j++;	// j在不为代表'#'符号的行号时会加1
		//// 将数据拷贝到 data.names
		//// i+1是密码，加j的begin()并不包括j的拷贝
		//data.defaultNames.resize(j - i - 2);
		//std::copy(splitLines.begin() + i + 2, splitLines.begin() + j, data.defaultNames.begin());

		searchData(splitLines, data.defaultNames, 1);	// 读取剩余名单

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
		EncryptData(splitLines, "data.bin");

	return true;
}

// 专门用于设置信息存储
export class Store :public Data	// 继承Data的东西
{
public:
	bool ifRight = true;	// 标志数据是否正常
	std::vector<std::vector<std::string>> all;	// [0]存名单名，[1]存密码
	std::string currentShowList;	// 存储当前正在显示的名单名
	std::vector<std::string> changedNames;	// 用于存储修改后的名单名
};
export Store store;	// 当使用设置页面时定义

export void initializeStore()
{
	// 确认是否可以继续初始化
	std::vector<std::string> splitLines;
	if (!DecryptData(splitLines, "data.bin"))
	{
		store.ifRight = false;
		return;	// 维持默认态
	}
	else
		store.ifRight = true;

	// 再度检查默认名单的完整性
	store.defaultList = splitLines[6];
	// 默认名字
	if(!searchData(splitLines, store.defaultNames, 1))
	{
		// 没有找到名单名字
		store.ifRight = false;
		return;
	}

	// 其它data继承部分的初始化
	std::istringstream(splitLines[0]) >> std::boolalpha >> store.ifTop;
	store.clientBC = std::stoi(splitLines[1], nullptr, 16);
	store.clientFC = std::stoi(splitLines[2], nullptr, 16);
	store.captionBC = std::stoi(splitLines[3], nullptr, 16);
	store.captionFC = std::stoi(splitLines[4], nullptr, 16);
	store.fontName = splitLines[5];

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
			store.all.push_back({});

			store.all[listNum].push_back(splitLines[i]);
			store.all[listNum].push_back(splitLines[i + 1]);	// 这是密码
		}

		// 查找下一个以 '#' 开头的行，确定当前名单范围
		short j = i + 2;	// 从密码后面开始
		while (j < splitLines.size() && splitLines[j][0] != '#')
			j++;	// j在不为'#'符号开头的行会加1
		// 将数据拷贝
		// i+1是密码，加j的begin()并不包括j的拷贝
		store.all[listNum].resize(j - i);
		std::copy(splitLines.begin() + i + 2, splitLines.begin() + j, store.all[listNum].begin() + 2);
		listNum++;	// 准备下一个名单

		// 因为j表示的是名单终止符的行号，所以i应转换为：
		i = j + 1;

		break;
	}
}


export void fontStore(std::string fontName)
{
	store.fontName = fontName;
}

// 用于取消all[][0]的格式
export std::string removeFormat(std::string& str)
{
	return str.substr(4, str.size() - 6);
}

// 用于记录，加强伪随机性
export std::deque<std::string> history;
short historyNum = 0;	// 用于记录当前已历史记录的序号
short historyMax = 1;	// 最大历史记录数
// 读取历史记录
export void readHistory()
{
	historyMax = data.defaultNames.size() / 6;	// 在扫描文件前处理

	std::vector<std::string> read;
	if(!DecryptData(read, "history.bin"))
	{
		std::ofstream filemake("history.bin", std::ios::binary | std::ios::trunc);
		filemake.close();

		std::vector<std::string> saveKey;
		EncryptData(saveKey, "history.bin");

		return;
	}

	// 寻找read中保存的history
	searchData(read, history, 0);

	// 逆序遍历 history，确保后出现的名字更靠近末尾
	short i = 0;	// 已经处理的历史记录数
	for (std::string target : history)
	{
		// 在 leftNames 中查找目标
		auto it = std::find(data.leftNames.begin(), data.leftNames.end(), target);
		if (it != data.leftNames.begin())
		{
			// 将找到的元素移动到名单末尾
			std::rotate(data.leftNames.begin() + i, it, data.leftNames.end());
			i++;
		}
	}
}
// 记录历史
export void recordHistory(std::string& name)
{
	if (historyNum >= historyMax)
		return;	// 不再记录
	else
	{
		history.insert(history.begin(), name);
		historyNum++;
	}
}
// 写入历史记录
export void writeHistory()
{
	// 限制历史记录数
	if (history.size() > historyMax)
		history.resize(historyMax);

	std::vector<std::string> write;
	// 获取原始历史记录
	if (!DecryptData(write, "history.bin"))
	{
		// 不存在历史记录文件，创建一个
		std::ofstream filemake("history.bin", std::ios::binary | std::ios::trunc);
		filemake.close();
		return;
	}

	bool ok = 0;	// 记录是否存在当前名单的历史记录数据
	for (short i = 0; i < write.size(); i++)
	{
		// 如果当前行以 '#' 开头
		if (write[i][0] != '#')
			continue;
		// 检查是否匹配默认列表名字
		if (write[i] != "# 《" + data.defaultList + "》")
			continue;

		std::copy(history.begin(), history.end(), write.begin() + i + 1);	// 依赖修改名单后删除历史记录
		ok = 1;
		break;	// 存在数据
	}
	// 没有找到名单
	if (!ok)
	{
		write.push_back("# 《" + data.defaultList + "》");
		write.resize(write.size() + historyMax);
		std::copy(history.begin(), history.end(), write.end() - historyMax);
		write.push_back("#");
	}

	EncryptData(write, "history.bin");
}
// 调整历史记录
export void rewriteHistory()
{
	std::vector<std::string> write;

	// 获取原始历史记录
	if (!DecryptData(write, "history.bin"))
	{
		// 不存在历史记录文件，创建一个
		std::ofstream filemake("history.bin", std::ios::binary | std::ios::trunc);
		filemake.close();
		return;
	}

	for (short i = 0; i < store.changedNames.size(); i++)
	{
		for (short j = 0; j < write.size(); j++)
		{
			// 1.寻找名单
			// 寻找名单标记
			if (write[i][0] != '#')
				continue;
			// 确认是否是当前修改的名单
			if (write[i] != "# 《" + store.changedNames[i] + "》")
				continue;

			// 2.确定名单范围并修改名单
			for(short k=j;k<write.size();k++)
			{
				// 如果当前行以 '#' 开头
				if (write[k][0] != '#')
					continue;
				// 删除所有记录
				write.erase(write.begin() + j, write.begin() + k + 1);	// 当心第二个是不包括的
			}
		}
	}
}

// 调整用户在设置页面修改的名单中的特殊符号以防错误
export void adjustListName(std::string& name)
{
}