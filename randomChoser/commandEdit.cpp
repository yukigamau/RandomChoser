#include <Windows.h>
import command;
import margin;
import std;
using command::Edit, command::WinStyle;
using margin::Margin;
using std::make_unique;

Edit::Edit(HWND hWnd, HINSTANCE hInstance, Margin* mg) :Command(hWnd, hInstance)
{
	this->margin = make_unique<Margin>(*mg);
}

void command::Edit::addWinStyle(WinStyle ws)
{
	this->ws |= (DWORD)ws;
}

void command::Edit::limit(int len)
{
	auto hEdit{ GetDlgItem(hParent,id) };
	SendMessage(hEdit, EM_SETLIMITTEXT, len, 0);
}