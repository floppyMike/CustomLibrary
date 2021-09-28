#ifndef _CTL_ERROR_
#define _CTL_ERROR_

#include <chrono>
#include <string>
#include <ctime>
#include <tuple>

#include "IO.h"

#ifdef __linux__

#include <execinfo.h>
#include <unistd.h>

/**
 * @brief Dump programm call stack to stdout
 */
namespace ctl::err
{
	inline void dump_trace()
	{
		constexpr auto MAX_SIZE = 1024;

		void	 *array[MAX_SIZE];
		size_t size = backtrace(array, MAX_SIZE);

		backtrace_symbols_fd(array, size, STDOUT_FILENO);
	}
} // namespace ctl::err

#elif _WIN32
namespace ctl::err
{
	inline void dump_trace()
	{
	}
} // namespace ctl::err
#endif // __linux__

namespace ctl::err
{

	/**
	 * @brief Catagory used by the logger for assigning logging severities
	 */
	enum Catagory
	{
		INFO,
		SUCC,
		WARN,
		ERR,
		FAT,
		ALL,
	};

	static constexpr std::string_view COLOR_TABLE[Catagory::ALL] = {
		"", "\x1B[92m", "\x1B[93m", "\x1B[95m", "\x1B[91m",
	};

	static constexpr std::string_view CAT_STRING[Catagory::ALL] = {
		"[I] ", "[S] ", "[W] ", "[E] ", "[F] ",
	};

	template<typename T>
	concept Policy = requires(T t)
	{
		t.open();
		t.close();
		t.write(Catagory(), (const char *)nullptr);
	};

	template<Policy... Policies>
	class Logger
	{
	public:
		Logger() = default;

		/**
		 * @brief Initialize the logger including it's policies.
		 * @param pols Policy construction
		 */
		explicit Logger(Policies &&...pols)
			: m_p(std::forward<Policies>(pols)...)
		{
		}

		Logger(const Logger &) = delete;
		Logger(Logger &&)	   = delete;

		/**
		 * @brief Write a formatted (or not) log
		 * @param c Catagory to use
		 * @param fmt format to use or string to print
		 * @param arg Arguments for formatted print
		 */
		template<typename... Args>
		void write(Catagory c, const char *fmt, const Args &...arg)
		{
			_open_buffer_();

			_write_time_();
			_write_catagory_(c);

			_write_buffer_(c, fmt, arg...);
			_write_buffer_(c, "\n");

			_close_buffer_();
		}

	private:
		std::tuple<Policies...> m_p;

		void _write_time_()
		{
			const auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			char	   buf[21];

			tm time;
#ifdef __linux__
			gmtime_r(&t, &time);
#elif _WIN32
			gmtime_s(&time, &t);
#endif // __linux__

			std::strftime(buf, sizeof buf, "%Y-%m-%d %H:%M:%S ", &time);
			_write_buffer_(Catagory::INFO, buf);
		}

		void _write_catagory_(Catagory c)
		{
			_write_buffer_(c, CAT_STRING[c].data());
		}

		template<typename... T>
		void _write_buffer_(Catagory c, const char *fmt, const T &...args)
		{
			std::apply([&args..., c, fmt](auto &&...arg) { (arg.write(c, fmt, args...), ...); }, m_p);
		}

		void _open_buffer_()
		{
			std::apply([](auto &&...arg) { (arg.open(), ...); }, m_p);
		}

		void _close_buffer_()
		{
			std::apply([](auto &&...arg) { (arg.close(), ...); }, m_p);
		}
	};

	class FileP
	{
	public:
		/**
		 * @brief Create a file policy
		 * @param name Name of the file to log to
		 */
		FileP(const char *name)
			: file_name(name)
		{
			ctl::File(name, "w");
		}

		/**
		 * @brief Open up the file
		 */
		void open()
		{
			file.open(file_name.c_str(), "a");
			std::fseek(file.fp, pos, SEEK_SET);
		}

		/**
		 * @brief Flush and close the file
		 */
		void close()
		{
			pos = std::ftell(file.fp);
			file.close();
		}

		/**
		 * @brief Write the log to the file and reiterate when necessary
		 * @param msg Log to write
		 * @param c Catagory to use (ignored)
		 */
		template<typename... T>
		void write(Catagory c, const char *fmt, const T &...args)
		{
			file.print(fmt, args...);

			if (std::ftell(file.fp) >= std::numeric_limits<unsigned int>::max())
				std::rewind(file.fp);
		}

	private:
		std::string file_name;
		ctl::File	file;
		long		pos;
	};

	class ConP
	{
	public:
		explicit ConP() = default;

		void open()
		{
		}
		void close()
		{
		}

		/**
		 * @brief Write the log into the console
		 * @param msg Log to write
		 * @param c Catagory to use
		 */
		template<typename... T>
		void write(Catagory c, const char *fmt, const T &...args)
		{
			ctl::print(COLOR_TABLE[c].data());
			ctl::print(fmt, args...);
			ctl::print("\033[0m");
		}
	};

} // namespace ctl::err

#ifndef NDEBUG
#define ASSERT(cond, msg)                                                                                       \
	{                                                                                                           \
		if (!(cond))                                                                                            \
		{                                                                                                       \
			ctl::err::dump_trace();                                                                             \
			std::fprintf(stderr, "Assertion \"%s\" failed in %s using the function %s at line %i: %s\n", #cond, \
						 __FILE__, __FUNCTION__, __LINE__, msg);                                                \
			std::terminate();                                                                                   \
		}                                                                                                       \
	}
#else
#define ASSERT(cond, msg) cond
#endif

#define CATCH_LOG(block)                        \
	try                                         \
	{                                           \
		block;                                  \
	}                                           \
	catch (const std::exception &e)             \
	{                                           \
		std::fprintf(stderr, "%s\n", e.what()); \
	}

#endif // !_CTL_ERROR_
