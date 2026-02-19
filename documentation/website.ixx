module;
#include <cassert>
#include <Windows.h>
#include <shellapi.h>
export module website;
import std;
using std::wstring;

export namespace website
{
	void openWebsite(const wstring& web)
	{
		auto rs = ShellExecute(nullptr, L"open", web.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
		if ((INT_PTR)rs <= 32)
		{
			auto errText = L"´ò¿ªÍøÒ³" + web + L"ERROR";
			MessageBox(nullptr, errText.c_str(), L"ERROR", MB_ICONERROR);
		}
	}
}