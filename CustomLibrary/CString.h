#pragma once

/**
* <summary>A C++ abstraction of a C-string</summary>
*
* <param name='t_size'>size of </param>
*
* <exception cref="ctl::Log">description</exception>
*
* <remarks>description</remarks>
*
* <returns>description</returns>
*
*/
template<size_t t_size>
class CString
{
	
	CString()
	{}

private:
	char m_vals[t_size];
};