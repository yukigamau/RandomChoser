#include <Windows.h>
import command;
import std;
import window;

using command::Toggle;
using std::wstring;
using std::tie;
using window::WindowAdjuster;

Toggle::Toggle(HWND hParent, HINSTANCE hInstance, wstring text, wstring first, wstring second, int interval,
	int id1, int id2)
	:Command(hParent, hInstance), text{ text }, first{ first }, second{ second }, interval{ interval },
	id1{ id1 }, id2{ id2 }
{ }

void command::Toggle::toggleCreate()
{
	POINT begin{ x,y };
	WindowAdjuster wa(hParent, hFont, interval, begin);

	Static stc(hParent, hInstance, text);
	auto [width, height] = wa.getCtlSize(text);
	stc.x = wa.x;
	stc.y = wa.y;
	stc.w = width;
	stc.h = height;
	stc.hFont = hFont;
	stc.create();

	wa.ctlBeside(width);

	wstring first{ L"กั " + this->first };
	tie(width, height) = wa.getCtlSize(first);
	Static stcFirst(hParent, hInstance, first);
	stcFirst.x = wa.x;
	stcFirst.y = wa.y;
	stcFirst.w = width;
	stcFirst.h = height;
	stcFirst.id = id1;
	stcFirst.hFont = hFont;
	stcFirst.addWinStyle(WinStyle::notify);
	stcFirst.create();

	wa.ctlBeside(width);

	wstring second{ L"ก๐ " + this->second };
	tie(width, height) = wa.getCtlSize(second);
	Static stcSecond(hParent, hInstance, second);
	stcSecond.x = wa.x;
	stcSecond.y = wa.y;
	stcSecond.w = width;
	stcSecond.h = height;
	stcSecond.id = id2;
	stcSecond.hFont = hFont;
	stcSecond.addWinStyle(WinStyle::notify);
	stcSecond.create();
}
