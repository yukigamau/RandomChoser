// cryptoAPI����
module;

#include <windows.h>
#include <wincrypt.h>
#pragma comment(lib, "advapi32.lib")

export module cryptograph;
import std;

void ShowError(const char* msg)
{
	DWORD errorCode = GetLastError();

	std::ostringstream oss;
	oss << msg << " (�������: " << errorCode << ")";

	std::string result = oss.str();
	MessageBox(NULL, result.c_str(), "����", MB_OK | MB_ICONERROR);
}

// ���ܺ������� vector<string> ���л�����ܣ������浽�ļ���
export template <typename Container>
bool EncryptData(Container& vec, std::string file)
{
	// 1. ���л� vector<string>
	// ��ʽ�� [DWORD: �ַ�������] { [DWORD: �ַ�������][�ַ�������] }...
	std::vector<BYTE> plainData;
	DWORD numStrings = (DWORD)vec.size();
	plainData.insert(plainData.end(), (BYTE*)&numStrings, (BYTE*)&numStrings + sizeof(numStrings));
	for (size_t i = 0; i < vec.size(); i++)
	{
		DWORD strLen = (DWORD)vec[i].size();
		plainData.insert(plainData.end(), (BYTE*)&strLen, (BYTE*)&strLen + sizeof(strLen));
		// д���ַ������ݣ���������������
		plainData.insert(plainData.end(), vec[i].begin(), vec[i].end());
	}

	// 2. ��ȡ����������
	HCRYPTPROV hProv = 0;
	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
	{
		ShowError("CryptAcquireContext ʧ��");
		return false;
	}

	// 3. ����һ���ԳƻỰ��Կ��ÿ�������������ѡ�� RC4 �㷨������ CRYPT_EXPORTABLE �Ա��������
	// ����ֻ�Ƿ�ֹѧ���޸���������ȫҪ��͡�ʹ��RC4�ӿ��ٶȡ�
	HCRYPTKEY hKey = 0;
	if (!CryptGenKey(hProv, CALG_RC4, CRYPT_EXPORTABLE, &hKey))
	{
		ShowError("CryptGenKey ʧ��");
		CryptReleaseContext(hProv, 0);
		return false;
	}

	// 4. ������Կ��һ�� blob �У����ڴ����ļ���
	DWORD dwKeyBlobLen = 0;
	if (!CryptExportKey(hKey, 0, PLAINTEXTKEYBLOB, 0, NULL, &dwKeyBlobLen))
	{
		ShowError("CryptExportKey (��ȡ��С) ʧ��");
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	std::vector<BYTE> keyBlob(dwKeyBlobLen);
	if (!CryptExportKey(hKey, 0, PLAINTEXTKEYBLOB, 0, keyBlob.data(), &dwKeyBlobLen))
	{
		ShowError("CryptExportKey ʧ��");
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}

	// 5. �� plainData ���м���
	// RC4 Ϊ�����룬���ܺ����ݴ�С��ԭʼ����һ��
	DWORD dwDataLen = (DWORD)plainData.size();
	DWORD dwBufLen = dwDataLen;  // ���� RC4��������⻺����
	std::vector<BYTE> cipherData(dwBufLen);
	memcpy(cipherData.data(), plainData.data(), dwDataLen);
	if (!CryptEncrypt(hKey, 0, TRUE, 0, cipherData.data(), &dwDataLen, dwBufLen))
	{
		ShowError("CryptEncrypt ʧ��");
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}

	// 6. ���ļ���дģʽ����д�룺
	// [DWORD: key blob ����] + [key blob] + [DWORD: �������ݳ���] + [��������]
	HANDLE hFile = CreateFile(file.c_str(),
		GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		ShowError("�ļ�����ʧ��");
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	DWORD dwWritten = 0;
	if (!WriteFile(hFile, &dwKeyBlobLen, sizeof(dwKeyBlobLen), &dwWritten, NULL) || dwWritten != sizeof(dwKeyBlobLen))
	{
		ShowError("д����Կ blob ����ʧ��");
		CloseHandle(hFile);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	if (!WriteFile(hFile, keyBlob.data(), dwKeyBlobLen, &dwWritten, NULL) || dwWritten != dwKeyBlobLen)
	{
		ShowError("д����Կ blob ʧ��");
		CloseHandle(hFile);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	if (!WriteFile(hFile, &dwDataLen, sizeof(dwDataLen), &dwWritten, NULL) || dwWritten != sizeof(dwDataLen))
	{
		ShowError("д��������ݳ���ʧ��");
		CloseHandle(hFile);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	if (!WriteFile(hFile, cipherData.data(), dwDataLen, &dwWritten, NULL) || dwWritten != dwDataLen)
	{
		ShowError("д���������ʧ��");
		CloseHandle(hFile);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	CloseHandle(hFile);

	// �ͷ���Դ
	CryptDestroyKey(hKey);
	CryptReleaseContext(hProv, 0);
	return true;
}

// ���ܺ������� �ļ��ж�ȡ��Կ blob ��������ݣ����ܺ�ԭΪ vector<string>
export template <typename Container>
bool DecryptData(Container& vec, std::string file)
{
	// 1. ���ļ�����ģʽ��
	HANDLE hFile = CreateFile(file.c_str(),
		GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		ShowError("���ļ�ʧ��");
		return false;
	}
	DWORD dwRead = 0;
	DWORD dwKeyBlobLen = 0;
	if (!ReadFile(hFile, &dwKeyBlobLen, sizeof(dwKeyBlobLen), &dwRead, NULL) || dwRead != sizeof(dwKeyBlobLen))
	{
		ShowError("��ȡ��Կ blob ����ʧ��");
		CloseHandle(hFile);
		return false;
	}
	std::vector<BYTE> keyBlob(dwKeyBlobLen);
	if (!ReadFile(hFile, keyBlob.data(), dwKeyBlobLen, &dwRead, NULL) || dwRead != dwKeyBlobLen)
	{
		ShowError("��ȡ��Կ blob ʧ��");
		CloseHandle(hFile);
		return false;
	}
	DWORD dwDataLen = 0;
	if (!ReadFile(hFile, &dwDataLen, sizeof(dwDataLen), &dwRead, NULL) || dwRead != sizeof(dwDataLen))
	{
		ShowError("��ȡ�������ݳ���ʧ��");
		CloseHandle(hFile);
		return false;
	}
	std::vector<BYTE> cipherData(dwDataLen);
	if (!ReadFile(hFile, cipherData.data(), dwDataLen, &dwRead, NULL) || dwRead != dwDataLen)
	{
		ShowError("��ȡ��������ʧ��");
		CloseHandle(hFile);
		return false;
	}
	CloseHandle(hFile);

	// 2. ��ȡ����������
	HCRYPTPROV hProv = 0;
	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
	{
		ShowError("CryptAcquireContextʧ��");
		return false;
	}

	// 3. ���õ����� key blob ����Ự��Կ
	HCRYPTKEY hKey = 0;
	if (!CryptImportKey(hProv, keyBlob.data(), dwKeyBlobLen, 0, 0, &hKey))
	{
		ShowError("CryptImportKey ʧ��");
		CryptReleaseContext(hProv, 0);
		return false;
	}

	// 4. �������ݣ���������ԭ�������н��У�
	if (!CryptDecrypt(hKey, 0, TRUE, 0, cipherData.data(), &dwDataLen))
	{
		ShowError("CryptDecrypt ʧ��");
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}

	// 5. �����л��� vector<string>
	// ���ݸ�ʽ�� [DWORD: �ַ�������] { [DWORD: �ַ�������][�ַ�������] }...
	if (dwDataLen < sizeof(DWORD))
	{
		ShowError("���ܺ������̫��");
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	BYTE* pData = cipherData.data();
	DWORD numStrings = *(DWORD*)pData;
	pData += sizeof(DWORD);
	DWORD remaining = dwDataLen - sizeof(DWORD);
	vec.clear();
	for (DWORD i = 0; i < numStrings; i++)
	{
		if (remaining < sizeof(DWORD))
		{
			ShowError("�������ݸ�ʽ���󣨶�ȡ�ַ������ȣ�");
			CryptDestroyKey(hKey);
			CryptReleaseContext(hProv, 0);
			return false;
		}
		DWORD strLen = *(DWORD*)pData;
		pData += sizeof(DWORD);
		remaining -= sizeof(DWORD);
		if (remaining < strLen)
		{
			ShowError("�������ݸ�ʽ���󣨶�ȡ�ַ������ݣ�");
			CryptDestroyKey(hKey);
			CryptReleaseContext(hProv, 0);
			return false;
		}
		std::string s((char*)pData, strLen);
		vec.push_back(s);
		pData += strLen;
		remaining -= strLen;
	}

	// �ͷ���Դ
	CryptDestroyKey(hKey);
	CryptReleaseContext(hProv, 0);
	return true;
}