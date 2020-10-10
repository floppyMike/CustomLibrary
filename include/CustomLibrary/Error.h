#ifndef MYERROR
#define MYERROR

#include <chrono>
#include <fstream>
#include <sstream>
#include <string_view>
#include <concepts>
#include <tuple>
#include <iostream>

#ifndef NDEBUG
#	define ASSERT(cond, msg)                                                                               \
		{                                                                                                   \
			if (!(cond))                                                                                    \
			{                                                                                               \
				std::cerr << "Assertion \"" << #cond << " failed in " << __FILE__ << " using the function " \
						  << __FUNCTION__ << " at line " << __LINE__ << ": " << msg << std::endl;           \
				std::terminate();                                                                           \
			}                                                                                               \
		}
#else
#	define ASSERT(cond, msg)
#endif

namespace ctl::err
{
	/**
	 * @brief Catagory used by the logger for assigning logging severities
	 */
	enum Catagory
	{
		INFO,
		SUCCESS,
		WARN,
		ERR,
		FATAL
	};

	template<typename... Policies>
	class Logger
	{
	public:
		class _Stream_
		{
		public:
			explicit _Stream_(Logger *log)
				: m_log(log)
			{
			}

			~_Stream_()
			{
				m_log->_write_buffer_(m_s.str(), Catagory::INFO);
				m_log->_write_buffer_("\n", Catagory::INFO);

				m_log->_close_buffer_();
			}

			template<typename T>
			auto operator<<(const T &v) -> auto &
			{
				m_s << v;
				return *this;
			}

		private:
			std::stringstream m_s;
			Logger *		  m_log;
		};

		Logger() = default;

		/**
		 * @brief Initialize the logger including it's policies.
		 * @param pols Policy construction
		 */
		explicit Logger(Policies &&... pols)
			: m_p(std::forward<Policies>(pols)...)
		{
		}

		Logger(const Logger &) = delete;
		Logger(Logger &&)	   = delete;

		/**
		 * @brief Create a write instance
		 * @param c Catagory to use
		 * @return ostream
		 */
		auto write(Catagory c)
		{
			_open_buffer_();

			_write_time_();
			_write_catagory_(c);

			return _Stream_(this);
		}

		/**
		 * @brief Write a seperation line
		 */
		void seperate()
		{
			_open_buffer_();
			_write_buffer_("\n----------------------------------------\n\n", Catagory::INFO);
			_close_buffer_();
		}

		/**
		 * @brief Write a string to the log
		 * @param c Catagory to use
		 * @param val String to write
		 */
		void write(Catagory c, std::string_view val)
		{
			_open_buffer_();

			_write_time_();
			_write_catagory_(c);

			_write_buffer_(val, Catagory::INFO);
			_write_buffer_("\n", Catagory::INFO);

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

			_write_buffer_(std::string_view(buf, sizeof buf), Catagory::INFO);
		}

		void _write_catagory_(Catagory c)
		{
			switch (c)
			{
			case Catagory::INFO: _write_buffer_("[INFO] ", c); break;
			case Catagory::WARN: _write_buffer_("[WARN] ", c); break;
			case Catagory::ERR: _write_buffer_("[ERROR] ", c); break;
			case Catagory::FATAL: _write_buffer_("[FATAL] ", c); break;
			case Catagory::SUCCESS: _write_buffer_("[SUCCESS] ", c); break;
			default: break;
			}
		}

		void _write_buffer_(std::string_view msg, Catagory c)
		{
			std::apply([msg, c](auto &&... arg) { (arg.write(msg, c), ...); }, m_p);
		}

		void _open_buffer_()
		{
			std::apply([](auto &&... arg) { (arg.open_ostream(), ...); }, m_p);
		}

		void _close_buffer_()
		{
			std::apply([](auto &&... arg) { (arg.close_ostream(), ...); }, m_p);
		}
	};

	class FilePolicy
	{
	public:
		/**
		 * @brief Create a file policy
		 * @param name Name of the file to log to
		 */
		explicit FilePolicy(std::string_view name)
			: m_file_name(name.data())
		{
			std::ofstream(name.data());
		}

		/**
		 * @brief Open up the file
		 */
		void open_ostream()
		{
			m_out_file.open(m_file_name.data(), std::ios::in | std::ios::out);
			m_out_file.seekp(m_true_pos);
		}
		/**
		 * @brief Flush and close the file
		 */
		void close_ostream()
		{
			m_out_file.flush();
			m_true_pos = m_out_file.tellp();
			m_out_file.close();
		}

		/**
		 * @brief Write the log to the file and reiterate when necessary
		 * @param msg Log to write
		 * @param c Catagory to use (ignored)
		 */
		void write(std::string_view msg, Catagory c)
		{
			m_out_file << msg;
			if (m_out_file.tellp() >= std::numeric_limits<unsigned int>::max())
				m_out_file.seekp(0, std::ios::beg);
		}

	private:
		std::string				m_file_name;
		std::ofstream			m_out_file;
		std::ofstream::pos_type m_true_pos = 0;
	};

	class ConsolePolicy
	{
	public:
		explicit ConsolePolicy() = default;

		/**
		 * @brief Ignored
		 */
		void open_ostream() noexcept {}
		/**
		 * @brief Flush the log to the console
		 */
		void close_ostream() noexcept { std::clog.flush(); }

		/**
		 * @brief Write the log into the console
		 * @param msg Log to write
		 * @param c Catagory to use
		 */
		void write(std::string_view msg, Catagory c)
		{
			switch (c)
			{
			case Catagory::INFO: std::clog << msg; break;
			case Catagory::SUCCESS: std::clog << "\x1B[92m" << msg << "\033[m"; break;
			case Catagory::WARN: std::clog << "\x1B[93m" << msg << "\033[m"; break;
			case Catagory::ERR: std::clog << "\x1B[95m" << msg << "\033[m"; break;
			case Catagory::FATAL: std::clog << "\x1B[91m" << msg << "\033[m"; break;
			default: break;
			}
		}
	};

} // namespace ctl::err

#endif // !MYERROR