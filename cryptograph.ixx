// cryptoAPI加密
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
	oss << msg << " (错误代码: " << errorCode << ")";

	std::string result = oss.str();
	MessageBox(NULL, result.c_str(), "错误", MB_OK | MB_ICONERROR);
}

// 加密函数：将 vector<string> 序列化后加密，并保存到文件中
export template <typename Container>
bool EncryptData(Container& vec, std::string file)
{
	// 1. 序列化 vector<string>
	// 格式： [DWORD: 字符串数量] { [DWORD: 字符串长度][字符串内容] }...
	std::vector<BYTE> plainData;
	DWORD numStrings = (DWORD)vec.size();
	plainData.insert(plainData.end(), (BYTE*)&numStrings, (BYTE*)&numStrings + sizeof(numStrings));
	for (size_t i = 0; i < vec.size(); i++)
	{
		DWORD strLen = (DWORD)vec[i].size();
		plainData.insert(plainData.end(), (BYTE*)&strLen, (BYTE*)&strLen + sizeof(strLen));
		// 写入字符串内容（不包括结束符）
		plainData.insert(plainData.end(), vec[i].begin(), vec[i].end());
	}

	// 2. 获取加密上下文
	HCRYPTPROV hProv = 0;
	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
	{
		ShowError("CryptAcquireContext 失败");
		return false;
	}

	// 3. 生成一个对称会话密钥（每次随机），这里选用 RC4 算法，设置 CRYPT_EXPORTABLE 以便后续导出
	// 由于只是防止学生修改名单，安全要求低。使用RC4加快速度。
	HCRYPTKEY hKey = 0;
	if (!CryptGenKey(hProv, CALG_RC4, CRYPT_EXPORTABLE, &hKey))
	{
		ShowError("CryptGenKey 失败");
		CryptReleaseContext(hProv, 0);
		return false;
	}

	// 4. 导出密钥到一个 blob 中，便于存入文件中
	DWORD dwKeyBlobLen = 0;
	if (!CryptExportKey(hKey, 0, PLAINTEXTKEYBLOB, 0, NULL, &dwKeyBlobLen))
	{
		ShowError("CryptExportKey (获取大小) 失败");
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	std::vector<BYTE> keyBlob(dwKeyBlobLen);
	if (!CryptExportKey(hKey, 0, PLAINTEXTKEYBLOB, 0, keyBlob.data(), &dwKeyBlobLen))
	{
		ShowError("CryptExportKey 失败");
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}

	// 5. 对 plainData 进行加密
	// RC4 为流密码，加密后数据大小与原始数据一致
	DWORD dwDataLen = (DWORD)plainData.size();
	DWORD dwBufLen = dwDataLen;  // 对于 RC4，无需额外缓冲区
	std::vector<BYTE> cipherData(dwBufLen);
	memcpy(cipherData.data(), plainData.data(), dwDataLen);
	if (!CryptEncrypt(hKey, 0, TRUE, 0, cipherData.data(), &dwDataLen, dwBufLen))
	{
		ShowError("CryptEncrypt 失败");
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}

	// 6. 打开文件（写模式），写入：
	// [DWORD: key blob 长度] + [key blob] + [DWORD: 加密数据长度] + [加密数据]
	HANDLE hFile = CreateFile(file.c_str(),
		GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		ShowError("文件创建失败");
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	DWORD dwWritten = 0;
	if (!WriteFile(hFile, &dwKeyBlobLen, sizeof(dwKeyBlobLen), &dwWritten, NULL) || dwWritten != sizeof(dwKeyBlobLen))
	{
		ShowError("写入密钥 blob 长度失败");
		CloseHandle(hFile);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	if (!WriteFile(hFile, keyBlob.data(), dwKeyBlobLen, &dwWritten, NULL) || dwWritten != dwKeyBlobLen)
	{
		ShowError("写入密钥 blob 失败");
		CloseHandle(hFile);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	if (!WriteFile(hFile, &dwDataLen, sizeof(dwDataLen), &dwWritten, NULL) || dwWritten != sizeof(dwDataLen))
	{
		ShowError("写入加密数据长度失败");
		CloseHandle(hFile);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	if (!WriteFile(hFile, cipherData.data(), dwDataLen, &dwWritten, NULL) || dwWritten != dwDataLen)
	{
		ShowError("写入加密数据失败");
		CloseHandle(hFile);
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}
	CloseHandle(hFile);

	// 释放资源
	CryptDestroyKey(hKey);
	CryptReleaseContext(hProv, 0);
	return true;
}

// 解密函数：从 文件中读取密钥 blob 与加密数据，解密后还原为 vector<string>
export template <typename Container>
bool DecryptData(Container& vec, std::string file)
{
	// 1. 打开文件（读模式）
	HANDLE hFile = CreateFile(file.c_str(),
		GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		ShowError("打开文件失败");
		return false;
	}
	DWORD dwRead = 0;
	DWORD dwKeyBlobLen = 0;
	if (!ReadFile(hFile, &dwKeyBlobLen, sizeof(dwKeyBlobLen), &dwRead, NULL) || dwRead != sizeof(dwKeyBlobLen))
	{
		ShowError("读取密钥 blob 长度失败");
		CloseHandle(hFile);
		return false;
	}
	std::vector<BYTE> keyBlob(dwKeyBlobLen);
	if (!ReadFile(hFile, keyBlob.data(), dwKeyBlobLen, &dwRead, NULL) || dwRead != dwKeyBlobLen)
	{
		ShowError("读取密钥 blob 失败");
		CloseHandle(hFile);
		return false;
	}
	DWORD dwDataLen = 0;
	if (!ReadFile(hFile, &dwDataLen, sizeof(dwDataLen), &dwRead, NULL) || dwRead != sizeof(dwDataLen))
	{
		ShowError("读取加密数据长度失败");
		CloseHandle(hFile);
		return false;
	}
	std::vector<BYTE> cipherData(dwDataLen);
	if (!ReadFile(hFile, cipherData.data(), dwDataLen, &dwRead, NULL) || dwRead != dwDataLen)
	{
		ShowError("读取加密数据失败");
		CloseHandle(hFile);
		return false;
	}
	CloseHandle(hFile);

	// 2. 获取加密上下文
	HCRYPTPROV hProv = 0;
	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
	{
		ShowError("CryptAcquireContext失败");
		return false;
	}

	// 3. 利用导出的 key blob 导入会话密钥
	HCRYPTKEY hKey = 0;
	if (!CryptImportKey(hProv, keyBlob.data(), dwKeyBlobLen, 0, 0, &hKey))
	{
		ShowError("CryptImportKey 失败");
		CryptReleaseContext(hProv, 0);
		return false;
	}

	// 4. 解密数据（解密是在原缓冲区中进行）
	if (!CryptDecrypt(hKey, 0, TRUE, 0, cipherData.data(), &dwDataLen))
	{
		ShowError("CryptDecrypt 失败");
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return false;
	}

	// 5. 反序列化出 vector<string>
	// 数据格式： [DWORD: 字符串数量] { [DWORD: 字符串长度][字符串内容] }...
	if (dwDataLen < sizeof(DWORD))
	{
		ShowError("解密后的数据太短");
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
			ShowError("解密数据格式错误（读取字符串长度）");
			CryptDestroyKey(hKey);
			CryptReleaseContext(hProv, 0);
			return false;
		}
		DWORD strLen = *(DWORD*)pData;
		pData += sizeof(DWORD);
		remaining -= sizeof(DWORD);
		if (remaining < strLen)
		{
			ShowError("解密数据格式错误（读取字符串内容）");
			CryptDestroyKey(hKey);
			CryptReleaseContext(hProv, 0);
			return false;
		}
		std::string s((char*)pData, strLen);
		vec.push_back(s);
		pData += strLen;
		remaining -= strLen;
	}

	// 释放资源
	CryptDestroyKey(hKey);
	CryptReleaseContext(hProv, 0);
	return true;
}