module;
#include <Windows.h>
export module margin;

export namespace margin
{
	class Margin
	{
	private:
		int halfMargin{ 5 };

	public:
		Margin(double dpiScale);
		void apply(HWND h);
		int getHalfMargin();
	};
}

void margin::Margin::apply(HWND h)
{
	SendMessage(h, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN,
		MAKELPARAM(halfMargin, halfMargin));
}

margin::Margin::Margin(double dpiScale)
{
	halfMargin *= dpiScale;
}

int margin::Margin::getHalfMargin()
{
	return halfMargin;
}
