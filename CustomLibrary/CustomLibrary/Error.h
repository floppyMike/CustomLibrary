#ifndef MYERROR
#define MYERROR

#include <exception>
#include <string_view>
#include <fstream>
#include <iostream>
#include <queue>

namespace ctl
{
#ifndef NDEBUG

/**
* @summary assert the condition
* @param "left" left value of condition
* @param "operator" operator of condition
* @param "right" right value of condition
* @param "note" note that will be executed and printed
* @remarks "left" and "right" will get executed once
*/
#define ASSERT_N(left, operator, right, note) \
{ \
	const auto res1 = (left); \
	const auto res2 = (right); \
	if(!(res1 operator res2)) \
	{ \
		std::cerr << "ASSERT FAILED: " \
			<< #left << ' ' << #operator << ' ' << #right \
			<< " @ " << __FILE__ << ':' << __func__ << " (" << __LINE__ << "). " \
			<< #left << " = " << res1 << "; " << #right << " = " << res2 \
			<< "\nERR_NOTE: " << (note) << '\n'; std::abort(); \
	} \
}

/**
* @summary assert the condition
* @param "left" left value of condition
* @param "operator" operator of condition
* @param "right" right value of condition
* @remarks "left" and "right" will get executed once
*/
#define ASSERT(left, operator, right) \
{ \
	const auto res1 = (left); \
	const auto res2 = (right); \
	if(!(res1 operator res2)) \
	{ \
		std::cerr << "ASSERT FAILED: " \
			<< #left << ' ' << #operator << ' ' << #right \
			<< " @ " << __FILE__ << ':' << __func__ << " (" << __LINE__ << "). " \
			<< #left << " = " << res1 << "; " << #right << " = " << res2 << '\n'; std::abort(); \
	} \
}

	constexpr bool LOG = true;

#else

/**
* @summary assert the condition
*
* @param "left" left value of condition
* @param "operator" operator of condition
* @param "right" right value of condition
* @param "note" note that will be executed and printed
*
* @remarks "left" and "right" will get executed once
* @remarks "note" won't be executed
*
*/
#define ASSERT_N(left, operator, right, note) (left); (right);

/**
* @summary assert the condition
*
* @param "left" left value of condition
* @param "operator" operator of condition
* @param "right" right value of condition
*
* @remarks "left" and "right" will get executed once
*
*/
#define ASSERT(left, operator, right) (left); (right);

	constexpr bool LOG = false;

#endif // NDEBUG

	class Log : public std::exception
	{
	public:
		enum class Sev { NOTE, WARNING, ERR };

		/**
		* @summary construct Log object
		* @param "msg" message to store
		*/
		Log(const std::string& msg)
			: m_msg(msg.c_str())
		{
		}
		Log(const char* msg)
			: m_msg(msg)
		{
		}

		/**
		* @summary converts log into text
		* @param "msg" message of log
		* @returns text string
		*/
		static void streamLog(std::ostream&, const std::string_view&, const Sev&);

		/**
		* @summary stores a log
		* @param "msg" message to log
		* @param "sev" severity of message
		* @remarks 
		*/
		static void log(const std::string_view&, const Sev&) noexcept;

		/**
		* @summary stores and writes a log
		* @param "msg" message to log
		* @param "sev" severity of message
		*/
		static void logWrite(const std::string_view&, const Sev&) noexcept;

		/**
		* @summary forces log to be written
		* @param "msg" message to log
		* @param "sev" severity of message
		*/
		static void forceLogWrite(const std::string_view&, const Sev&) noexcept;

		/**
		* @summary flushes buffer to file
		* @param "path" path to file output
		*/
		static void flush(const char*) noexcept;

		/**
		* @summary overrides "what" virtual
		* @returns c-string of message
		*/
		const char* what() const noexcept override { return m_msg.data(); }

	private:
		std::string_view m_msg;
		static std::deque<std::pair<std::string_view, Sev>> m_buf;
	};

	/**
	* @summary get message from errno code
	* @param "code" errno code
	* @returns c-string of 256 bytes
	*/
	const char* errnoMessage(const int&);
}

#endif // !MYERROR