#include <Windows.h>
import command;
import std;
using command::Button;
using std::wstring;

command::Button::Button(HWND hParent, HINSTANCE hInstance, wstring text)
	:Command(hParent, hInstance), text{ text }
{ }

void command::Button::enable(bool able)
{
	auto hSelf = GetDlgItem(hParent, id);

	EnableWindow(hSelf, able);
}