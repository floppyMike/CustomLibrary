#ifndef MYERROR
#define MYERROR

#include <exception>
#include <string_view>
#include <fstream>
#include <queue>
#include <iostream>
#include <assert.h>
#include <mutex>

namespace ctl
{
#ifndef NDEBUG
	constexpr bool LOG = true;
#else
	constexpr bool LOG = false;
#endif // LOGGING

	class Log : public std::exception
	{
	public:
		enum class Sev { NOTE, WARNING, ERR0R };

		Log(const std::string_view&, const Sev&) noexcept;

		static void log(const std::string_view&, const Sev&) noexcept;
		static void note(const std::string_view&, const Sev&) noexcept;

		static void flushToFile(const std::string&) noexcept;

	private:
		static std::string_view _typToString_(const Sev& typ) noexcept;

		inline static std::queue<std::string_view> m_buf;
		inline static std::mutex m_mutex;
	};

	Log::Log(const std::string_view& msg, const Sev& typ = Sev::ERR0R) noexcept
		: exception(std::string(_typToString_(typ)).append(msg).c_str())
	{
		//Lock to avoid data races
		std::scoped_lock(m_mutex);

		//Emplace to queue and check if limit is reached
		m_buf.emplace(what());
		if (m_buf.size() > 50) //50 is the message limit
			m_buf.pop();
	}

	void Log::log(const std::string_view& msg, const Sev& typ = Sev::NOTE) noexcept
	{
		//Lock to avoid data races
		std::scoped_lock(m_mutex);

		//Emplace to queue, log it and check if limit is reached
		m_buf.emplace(std::string(_typToString_(typ)).append(msg));
		std::clog << _typToString_(typ) << msg << '\n';
		if (m_buf.size() > 50) //50 is the message limit
			m_buf.pop();
	}

	void Log::note(const std::string_view& msg, const Sev& typ = Sev::NOTE) noexcept
	{
		if constexpr (LOG)
			log(msg, typ);
	}

	void Log::flushToFile(const std::string& fileName) noexcept
	{
		std::ofstream errorFile("Logs-" + fileName + ".txt", std::ios::out);
		while (!m_buf.empty())
		{
			errorFile << m_buf.front() << '\n';
			m_buf.pop();
		}
	}

	std::string_view Log::_typToString_(const Sev& typ) noexcept
	{
		switch (typ)
		{
		case Log::Sev::NOTE:	 return "NOTE: ";
		case Log::Sev::ERR0R:	 return "ERROR: ";
		case Log::Sev::WARNING:  return "WARNING: ";
		default:				 return "UNKNOWN: ";
		}
	}

}

#endif // !MYERROR