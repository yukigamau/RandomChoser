import command;
import std;

using command::Static;
using std::wstring;

command::Static::Static(HWND hParent, HINSTANCE hInstance, wstring text)
	:Command(hParent, hInstance), text{ text }
{
}

void Static::addWinStyle(WinStyle ws)
{
	style |= (DWORD)ws;
}