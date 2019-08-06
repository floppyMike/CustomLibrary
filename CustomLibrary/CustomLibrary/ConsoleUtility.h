#pragma once

#include <limits>
#include <iostream>
#include <string>
#include <string_view>

namespace ctl
{
	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	constexpr T numberInput(const std::string_view& str = "")
	{
		T input;

		while (true)
		{
			//Print and request input
			std::cout << str;
			std::cin >> input;

			//If cin failed reset and try again
			if (std::cin.fail())
			{
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cerr << "Invalid." << std::endl;
			}
			//Else return the result
			else
			{
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				return input;
			}
		}
	}

	template<typename Iter, typename Str>
	auto stringInput(const Iter &begin, const Iter &end, const Str& head)
	{
		std::string str;

		while (true)
		{
			//Print head and request string
			std::cout << head;
			std::getline(std::cin, str);

			//If it's help, print all key strings
			if (str == "help")
				for (auto b = begin; b != end; ++b)
					std::cout << "\"" << *b << "\"\n";

			//Else if find key string with request string
			else if (std::find(begin, end, str) != end)
				return str;

			//Else create error
			else
				std::cerr << "Invalid.\n";
		}

		return str;
	}
	template<typename Iter, typename Str>
	auto wstringInput(const Iter& begin, const Iter& end, const Str& head)
	{
		std::wstring str;

		while (true)
		{
			//Print head and request string
			std::wcout << head;
			std::getline(std::wcin, str);

			//If it's help, print all key strings
			if (str == L"help")
				for (auto b = begin; b != end; ++b)
					std::wcout << L"\"" << *b << L"\"\n";

			//Else if find key string with request string
			else if (std::find(begin, end, str) != end)
				return str;

			//Else create error
			else
				std::wcerr << L"Invalid.\n";
		}

		return str;
	}

	template<typename StringType, typename Str>
	auto stringInput(const std::initializer_list<StringType> &list, const Str& head)
	{ 
		return stringInput(list.begin(), list.end(), head); 
	}
	template<typename StringType, typename Str>
	auto wstringInput(const std::initializer_list<StringType>& list, const Str& head)
	{
		return wstringInput(list.begin(), list.end(), head);
	}
}