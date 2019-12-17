#pragma once

#include "Globals.h"

template<template<typename> class... Ex>
class Test : public Ex<Test<Ex...>>...
{
public:
	Test()
	{
	}

	template<template<typename> class... T>
	Test(T<Test>&&... arg)
		: T<Test>(std::move(arg))...
	{
	}

	template<template<typename> class... T>
	Test(const Test<T...>& cast)
		: T<Test>(static_cast<T<Test>>(*reinterpret_cast<T<Test>*>(static_cast<T<Test<T...>>*>(&cast))))...
	{
	}

	template<template<typename> class... T>
	Test(Test<T...>&& cast)
		: T<Test>(static_cast<T<Test>&&>(*reinterpret_cast<T<Test>*>(static_cast<T<Test<T...>>*>(&cast))))...
	{
	}
};

template<typename Impl>
class ETest1
{
	Impl* pthis = static_cast<Impl*>(this);

public:
	ETest1() = default;

	ETest1(int a)
		: m_int(a)
	{
	}

	void print_int()
	{
		std::cout << "Integer is: " << m_int << " String is: " << *static_cast<Impl*>(this)->get_string() << '\n';
	}

private:
	int m_int = 0;
};

template<typename Impl>
class ETest2
{
public:
	ETest2()
		: m_string(std::make_unique<int>(90))
	{

	}
	ETest2(const ETest2&) = delete;
	ETest2(ETest2&& val)
		: m_string(std::move(val.m_string))
	{

	}
	ETest2(int a)
		: m_string(std::make_unique<int>(a))
	{
	}

	auto& get_string()
	{
		return m_string;
	}

private:
	std::unique_ptr<int> m_string;
};