module;
#include <windows.h>
export module rgb;

typedef unsigned char UC;
typedef unsigned long UL;

export namespace rgb
{
	enum class Color :DWORD
	{
		grape = RGB(159, 0, 82),	// 葡萄
		jade = RGB(21, 174, 103)	// 翡翠绿
	};

	class Rgb
	{
	private:
		UC r, g, b;

	public:
		Rgb() = default;
		Rgb(UC r, UC g, UC b);
		Rgb(Color c);

	public:
		void set(UC r, UC g, UC b);

	public:
		auto cur() const;
		auto rev() const;
	};
}

rgb::Rgb::Rgb(UC r, UC g, UC b) :r{ r }, g{ g }, b{ b }
{ }

rgb::Rgb::Rgb(Color c)
{
	r = (UL)c & 0x0000FFUL;
	auto _g = (UL)c & 0x00FF00UL;
	_g >>= 8;
	g = _g;
	auto _b = (UL)c & 0xFF0000UL;
	_b >>= 16;
	b = _b;
}

void rgb::Rgb::set(UC r, UC g, UC b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}

auto rgb::Rgb::cur() const
{
	return RGB(r, g, b);
}

auto rgb::Rgb::rev() const
{
	return RGB(255 - r, 255 - g, 255 - b);
}