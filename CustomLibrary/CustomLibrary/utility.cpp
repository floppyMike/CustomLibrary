#include "utility.h"

std::string ctl::toHexadecimal(int val)
{
	static constexpr std::string& (*hex)(int&, std::string&) =
		[](int& val, std::string& str) constexpr->std::string &
	{
		if (val == 0)
			return str;

		const char& rem = val % 16;
		val /= 16;

		hex(val, str);

		if (rem > 9)
			str.push_back(rem - 10 + 'a');
		else
			str.push_back(rem + '0');

		return str;
	};

	if (val == 0)
		return std::string(1, '0');

	std::string buf;
	return hex(val, buf);
}
