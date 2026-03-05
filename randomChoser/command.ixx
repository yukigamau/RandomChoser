module;
#include <Windows.h>
#include <cassert>
#include <commctrl.h>
export module command;
import margin;
import std;
using margin::Margin;
using std::unique_ptr, std::wstring;

export namespace command
{
	enum class WinStyle :DWORD
	{
		border = WS_BORDER,
		center = ES_CENTER,
		left = ES_LEFT,
		password = ES_PASSWORD,
		notify = SS_NOTIFY
	};

	class Command
	{
	public:
		HWND hParent = nullptr;
		HINSTANCE hInstance;
		int id{ 0 };
		int x{ 0 }, y{ 0 };
		int w{ 0 }, h{ 0 };
		HFONT hFont = nullptr;

	public:
		virtual void create() = 0;

	public:
		Command(HWND hParent, HINSTANCE hInstance);

	public:
		void setSubclass(SUBCLASSPROC subProc);
	};

	class Button :public Command
	{
	public:
		wstring text;

	public:
		Button(HWND hParent, HINSTANCE hInstance, wstring text);

	public:
		void create() override
		{
			HWND hCur = CreateWindow(L"BUTTON", text.c_str(), WS_CHILD | WS_VISIBLE, x, y, w, h,
				hParent, (HMENU)id, hInstance, nullptr);
			if (hFont)
				SendMessage(hCur, WM_SETFONT, (WPARAM)hFont, TRUE);
		}
	};

	class Static :public Command
	{
	public:
		wstring text;

	public:
		// 主要是用于在toggle类的处理
		DWORD style = { WS_CHILD | WS_VISIBLE };

	public:
		Static(HWND hParent, HINSTANCE hInstance, wstring text);

	public:
		void create() override
		{
			HWND hCur = CreateWindow(L"STATIC", text.c_str(), style, x, y, w, h,
				hParent, (HMENU)id, hInstance, nullptr);
			if (hFont)
				SendMessage(hCur, WM_SETFONT, (WPARAM)hFont, TRUE);
		}

		void addWinStyle(WinStyle ws);
	};

	class Edit :public Command
	{
	public:
		unique_ptr<Margin> margin;
		DWORD ws{ 0UL };

	public:
		Edit(HWND hWnd, HINSTANCE hInstance, Margin* margin);

	public:
		void addWinStyle(WinStyle ws);

		void create() override
		{
			auto winStyle{ WS_CHILD | WS_VISIBLE | WS_BORDER };
			winStyle |= this->ws;
			HWND hCur = CreateWindow(L"EDIT", L"", winStyle,
				x, y - margin.get()->getHalfMargin(), w, h + 2 * margin.get()->getHalfMargin(),
				hParent, (HMENU)id, hInstance, nullptr);
			margin.get()->apply(hCur);
			if (hFont)
				SendMessage(hCur, WM_SETFONT, (WPARAM)hFont, TRUE);
		}

		// 限制文本长度
		void limit(int len);
	};

	class Toggle : public Command
	{
	public:
		wstring text;
		wstring first;
		wstring second;

		int interval;
		int id1;
		int id2;

	public:
		Toggle(HWND hParent, HINSTANCE hInstance, wstring text, wstring first, wstring second, int interval,
			int id1, int id2);

	public:
		void create() override
		{
			toggleCreate();
		}

	private:
		void toggleCreate();
	};
}

command::Command::Command(HWND hParent, HINSTANCE hInstance) :hParent{ hParent }, hInstance{ hInstance }
{ }

void command::Command::setSubclass(SUBCLASSPROC subProc)
{
	auto hCur{ GetDlgItem(hParent,id) };
	SetWindowSubclass(hCur, subProc, (UINT_PTR)id, 0);
}