#ifndef MYERROR
#define MYERROR

#include <chrono>
#include <ctime>
#include <exception>
#include <fstream>
#include <sstream>
#include <string_view>

#ifndef NDEBUG
#	include <iostream>
#	define ASSERT(cond, msg)                                                                              \
		{                                                                                                  \
			if (!(cond))                                                                                   \
			{                                                                                              \
				std::cerr << "Assertion \"" << #cond << " failed in " << __FILE__ << " using the function " \
						  << __FUNCTION__ << " at line " << __LINE__ << ": " << msg << std::endl;          \
				std::terminate();                                                                          \
			}                                                                                              \
		}
#else
#	define ASSERT(cond, msg)
#endif

namespace ctl::err
{
	class Logger
	{
		enum class _Color_
		{
			WHITE,
			GREEN,
			YELLOW,
			ORANGE,
			RED
		};

	public:
		class _Stream_
		{
		public:
			_Stream_(Logger &log)
				: m_log(&log)
			{
			}

			~_Stream_()
			{
				m_log->_write_buffer_(m_s.str());
				m_log->_write_buffer_("\n");
			}

			template<typename T>
			auto &operator<<(const T &v)
			{
				m_s << v;
				return *this;
			}

		private:
			std::stringstream m_s;
			Logger *		  m_log;
		};

		enum class Catagory
		{
			INFO,
			WARN,
			ERR,
			FATAL
		};

		static constexpr std::string_view LOG_FILE = "log.txt";

		Logger()
			: m_out_file(LOG_FILE.data())
		{
		}

		Logger(const Logger &) = delete;
		Logger(Logger &&)	   = delete;

		auto write(Catagory c)
		{
			_write_time_();
			_write_catagory_(c);

			return _Stream_(*this);
		}

		void seperate() { write(Catagory::INFO, "\n\n----------------------------------------\n"); }

		Logger &write(Catagory c, std::string_view val) { return write(c, val.data(), val.size()); }

		Logger &write(Catagory c, const char *str, size_t amount)
		{
			_write_time_();
			_write_catagory_(c);

			_write_buffer_(str, amount);
			_write_buffer_("\n");

#ifdef _IOSTREAM_
			std::clog.flush();
#endif // _IOSTREAM_

			return *this;
		}

	private:
		std::ofstream m_out_file;

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

			_write_buffer_(buf, sizeof buf);
		}

		void _write_catagory_(Catagory c)
		{
			switch (c)
			{
			case Catagory::INFO: _write_buffer_("[INFO] ", _Color_::GREEN); break;
			case Catagory::WARN: _write_buffer_("[WARN] ", _Color_::YELLOW); break;
			case Catagory::ERR: _write_buffer_("[ERROR] ", _Color_::ORANGE); break;
			case Catagory::FATAL: _write_buffer_("[FATAL] ", _Color_::RED); break;
			default: break;
			}
		}

		void _write_buffer_(std::string_view s, _Color_ col = _Color_::WHITE)
		{
			_write_buffer_(s.data(), s.size(), col);
		}

		void _write_buffer_(const char *str, size_t amount, _Color_ col = _Color_::WHITE)
		{
			m_out_file.write(str, amount);

#ifdef _IOSTREAM_
			switch (col)
			{
			case Logger::_Color_::WHITE: std::clog.write(str, amount); break;
			case Logger::_Color_::GREEN: std::clog << "\x1B[92m" + std::string(str, amount) + "\033[m"; break;
			case Logger::_Color_::YELLOW: std::clog << "\x1B[93m" + std::string(str, amount) + "\033[m"; break;
			case Logger::_Color_::ORANGE: std::clog << "\x1B[95m" + std::string(str, amount) + "\033[m"; break;
			case Logger::_Color_::RED: std::clog << "\x1B[91m" + std::string(str, amount) + "\033[m"; break;
			default: break;
			}
#endif // _IOSTREAM_

			if (m_out_file.tellp() >= std::numeric_limits<unsigned short>::max()) m_out_file.seekp(0);
		}
	};

	namespace
	{
		Logger g_log;
	}
} // namespace ctl::err

#endif // !MYERROR