module;

#include <Windows.h>
#include "ctime"

export module data;
import cryptograph;
import std;

// ��ȡʱ������
export class Data
{
public:
	bool ifTop = true;
	// COLORREFҪ������
	COLORREF clientBC = 0xffffff;
	COLORREF clientFC = RGB(0, 123, 187);
	COLORREF captionBC = RGB(102, 204, 255);
	COLORREF captionFC = 0xffffff;
	std::string fontName = "����";
	std::string defaultList;	// ���������	# ����	��Щ����
	std::vector<std::string> defaultNames;
	std::vector<std::string> leftNames;
};
export Data data;

std::string binaryFile = "data.bin";	// �����ļ�
export std::random_device rd;	// Ӳ�����������
export std::mt19937 engine(rd());	// ���������

// ��ʼ��
export bool initializeData()
{
	std::vector<std::string> splitLines;
	if (!DecryptData(splitLines))
		return false;

	// �����ܺ�����ݱ���
	std::istringstream(splitLines[0]) >> std::boolalpha >> data.ifTop;
	data.clientBC = std::stoi(splitLines[1], nullptr, 16);
	data.clientFC = std::stoi(splitLines[2], nullptr, 16);
	data.captionBC = std::stoi(splitLines[3], nullptr, 16);
	data.captionFC = std::stoi(splitLines[4], nullptr, 16);
	data.fontName = splitLines[5];
	data.defaultList = splitLines[6];
	for (short i = 7; i < splitLines.size(); i++)
	{
		// �����ǰ���� '#' ��ͷ
		if (splitLines[i][0] != '#')
			continue;
		// ����Ƿ�ƥ��Ĭ���б���
		if (splitLines[i] != "# ��" + data.defaultList + "��")	// ��all��Ѱ��Ҫ��ʽ��
			continue;
		// ������һ���� '#' ��ͷ���У�ȷ����Χ
		short j = i + 1;
		while (j < splitLines.size() && splitLines[j][0] != '#')
			j++;	// j�ڲ�Ϊ����'#'���ŵ��к�ʱ���1
		// �����ݿ����� data.names
		// i+1�����룬��j��begin()��������j�Ŀ���
		data.defaultNames.resize(j - i - 2);
		std::copy(splitLines.begin() + i + 2, splitLines.begin() + j, data.defaultNames.begin());

		break;	// ��������
	}

	if (data.defaultNames.size() == 0)
	{
		// û���ҵ���������
		MessageBox(nullptr, "�����ļ���ʽ����������", "X�nX", MB_ICONERROR);
		return false;
	}

	// �޸�ʣ������
	data.leftNames = data.defaultNames;
	shuffle(data.leftNames.begin(), data.leftNames.end(), engine);

	// ����޸�
	if (!(engine() % 10))
		EncryptData(splitLines);

	return true;
}

// ר������������Ϣ�洢
export class Store :public Data	// �̳�Data�Ķ���
{
public:
	bool ifRight = true;	// ��־�����Ƿ�����
	std::vector<std::vector<std::string>> all;	// [0]����������[1]������
	std::string currentShowList;	// �洢��ǰ������ʾ��������
};
export Store store;	// ��ʹ������ҳ��ʱ����

export void initializeStore()
{
	// ȷ���Ƿ���Լ�����ʼ��
	std::vector<std::string> splitLines;
	if (!DecryptData(splitLines))
	{
		store.ifRight = false;
		return;	// ά��Ĭ��̬
	}
	else
		store.ifRight = true;

	// �ٶȼ��Ĭ��������������
	store.defaultList = splitLines[6];
	// Ĭ������
	for (short i = 7; i < splitLines.size(); i++)
	{
		// �����ǰ���� '#' ��ͷ
		if (splitLines[i][0] != '#')
			continue;
		// ����Ƿ�ƥ��Ĭ���б���
		if (splitLines[i] != "# ��" + store.defaultList + "��")
			continue;
		// ������һ���� '#' ��ͷ���У�ȷ����Χ
		short j = i + 1;
		while (j < splitLines.size() && splitLines[j][0] != '#')
			j++;	// j�ڲ�Ϊ����'#'���ŵ��к�ʱ���1
		// �����ݿ���
		// i+1�����룬��j��begin()��������j�Ŀ���
		store.defaultNames.resize(j - i - 2);
		std::copy(splitLines.begin() + i + 2, splitLines.begin() + j, store.defaultNames.begin());

		break;	// ��������
	}
	if (store.defaultNames.size() == 0)
	{
		// û���ҵ���������
		store.ifRight = false;
		return;
	}

	// ����data�̳в��ֵĳ�ʼ��
	std::istringstream(splitLines[0]) >> std::boolalpha >> store.ifTop;
	store.clientBC = std::stoi(splitLines[1], nullptr, 16);
	store.clientFC = std::stoi(splitLines[2], nullptr, 16);
	store.captionBC = std::stoi(splitLines[3], nullptr, 16);
	store.captionFC = std::stoi(splitLines[4], nullptr, 16);
	store.fontName = splitLines[5];

	// ��������������
	short listNum = 0;	// ���ڼ�¼��ǰ���������
	for (short i = 7; i < splitLines.size(); i++)
	{
		// �����ǰ���� '#' ��ͷ��������������������ֹ��
		if (splitLines[i][0] != '#')
			continue;

		// ����Ƿ���������
		if (splitLines[i].size() > 2)
		{
			std::string name = splitLines[i];
			name.erase(0, 2);	// ɾ����ͷ��'#'��' '���ַ�

			// ��all����������
			store.all.push_back({});

			store.all[listNum].push_back(splitLines[i]);
			store.all[listNum].push_back(splitLines[i + 1]);	// ��������
		}

		// ������һ���� '#' ��ͷ���У�ȷ����ǰ������Χ
		short j = i + 2;	// ��������濪ʼ
		while (j < splitLines.size() && splitLines[j][0] != '#')
			j++;	// j�ڲ�Ϊ'#'���ſ�ͷ���л��1
		// �����ݿ���
		// i+1�����룬��j��begin()��������j�Ŀ���
		store.all[listNum].resize(j - i);
		std::copy(splitLines.begin() + i + 2, splitLines.begin() + j, store.all[listNum].begin() + 2);
		listNum++;	// ׼����һ������

		// ��Ϊj��ʾ����������ֹ�����кţ�����iӦת��Ϊ��
		i = j + 1;

		break;
	}
}


export void fontStore(std::string fontName)
{
	store.fontName = fontName;
}

// ����ȡ��all[][0]�ĸ�ʽ
export std::string removeFormat(std::string& str)
{
	return str.substr(4, str.size() - 6);
}