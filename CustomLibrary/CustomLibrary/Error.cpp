#include "Error.h"

void ctl::Log::streamLog(std::ostream& out, const std::string_view& msg, const Sev& sev)
{
	switch (sev)
	{
	case Sev::NOTE:	   out << "NOTE: " << msg << '\n'; break;
	case Sev::ERR:	   out << "\x1B[91mERROR: " << msg << "\033[0m\n"; break;
	case Sev::WARNING: out << "\x1B[93mWARNING: " << msg << "\033[0m\n"; break;
	}
}

void ctl::Log::log(const std::string_view& msg, const Sev& sev = Sev::NOTE) noexcept
{
	m_buf.emplace_back(msg, sev);
	if (m_buf.size() > 50)
		m_buf.pop_front();
}

//ctl::Log::OutStream ctl::Log::log(const Sev& sev) noexcept
//{
//	m_buf.emplace_back("", sev);
//	if (m_buf.size() > 50)
//		m_buf.pop_front();
//
//	return OutStream(sev);
//}

void ctl::Log::logWrite(const std::string_view& msg, const Sev& sev = Sev::NOTE) noexcept
{
	log(msg, sev);
	if constexpr (LOG)
		streamLog(std::cout, msg, sev);
}

void ctl::Log::forceLogWrite(const std::string_view& msg, const Sev& sev = Sev::NOTE) noexcept
{
	log(msg, sev);
	streamLog(std::cout, msg, sev);
}

void ctl::Log::flush(const char* path) noexcept
{
	std::ofstream fileOut(path);
	if (fileOut.is_open())
		for (const auto& i : m_buf)
			streamLog(fileOut, i.first, i.second);
}

const char* ctl::errnoMessage(const int& code)
{
	static char buffer[256];
	strerror_s(buffer, code);

	return buffer;
}

std::deque<std::pair<std::string_view, ctl::Log::Sev>> ctl::Log::m_buf;