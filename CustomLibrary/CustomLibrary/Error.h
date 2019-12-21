#ifndef MYERROR
#define MYERROR

#include <exception>
#include <string_view>
#include <fstream>
#include <iostream>


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



namespace ctl::err
{
	class Log : public std::exception
	{
	public:
		static constexpr std::string_view DEFAULT_FILE = "log.txt";
		enum class Sev { NOTE, WARNING, ERR };

		/**
		* @summary construct Log object
		* @param "msg" message to store
		*/
		Log(std::string_view);
		Log();

		~Log();

		void initiate(std::string_view what)
		{
			_write_noflush_("Initiating " + std::string(what) + "...");
		}

		void complete()
		{
			write("\x1B[91mDone\033[0m\n");
		}
		
		void seperate()
		{
			write("\x1B[93m\n----------------------------------------\033[0m\n\n");
		}

		Log& write(std::string_view val)
		{
			return write(val.data(), val.size());
		}

		Log& write(const char* str, size_t amount)
		{
			_write_noflush_(str, amount);
			if (m_buffer.size() >= 1024)
				_flush_();

			return *this;
		}

		/**
		* @summary overrides "what" virtual
		* @returns c-string of message
		*/
		const char* what() const noexcept override;

	private:
		std::string m_buffer;


		void _write_noflush_(const char* str, size_t amount)
		{
			std::clog.write(str, amount);
			m_buffer.append(str, amount);
		}
		inline void _write_noflush_(std::string_view str)
		{
			_write_noflush_(str.data(), str.size());
		}

		void _flush_()
		{
			std::ofstream file(DEFAULT_FILE.data(), std::ios::out | std::ios::app);
			file.write(m_buffer.data(), m_buffer.size());
			m_buffer.clear();
		}
	};


	std::string errno_message(int code)
	{
		char buffer[128];
		strerror_s(buffer, code);

		return std::string(buffer);
	}

	//----------------------------------------------
	//Implementation
	//----------------------------------------------

	inline Log::Log(std::string_view msg)
		: m_buffer(msg)
	{
	}
	inline Log::Log()
	{
		std::ofstream(DEFAULT_FILE.data());
	}

	inline Log::~Log()
	{
		std::ofstream file(DEFAULT_FILE.data(), std::ios::out | std::ios::binary | std::ios::app);
		file.write(m_buffer.data(), m_buffer.size());
	}

	inline const char* Log::what() const noexcept 
	{ 
		return m_buffer.c_str();
	}


	static Log g_log;
}

#endif // !MYERROR