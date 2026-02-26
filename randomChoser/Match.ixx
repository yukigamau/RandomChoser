export module match;

export namespace match
{
	enum class MatchDegree : char
	{
		same,
		diff,
		none	// 没有输入确认密码时用
	};
	class Match
	{
	private:
		MatchDegree md = MatchDegree::none;

	public:
		bool change(MatchDegree md);
		auto degree();
	};
}

bool match::Match::change(MatchDegree md)
{
	if (this->md == md)
		return false;	// 表示没有改动

	this->md = md;
	return true;	// 表示有改动
}

auto match::Match::degree()
{
	return md;
}