#pragma once

#include "asio.hpp"

#include <istream>
#include <memory>
#include <utility>

namespace ctl::net
{
	using asio::ip::tcp;

	class TCP_Session
	{
	public:
		TCP_Session() = delete;

		explicit TCP_Session(asio::io_context &io)
			: m_socket(io)
		{
		}

		explicit TCP_Session(asio::io_context &io, const tcp::resolver::results_type &endpoints)
			: m_socket(io)
		{
			connect(endpoints);
		}

		~TCP_Session() { close(); }

		void connect(const tcp::resolver::results_type &endpoints)
		{
			m_host = endpoints->host_name();
			asio::connect(m_socket, endpoints);
		}

		[[nodiscard]] auto host() const noexcept -> std::string_view { return m_host; }
		auto			   socket() noexcept -> auto & { return m_socket; }

		void close()
		{
			if (m_socket.is_open())
				m_socket.close();
		}

	private:
		std::string m_host;
		tcp::socket m_socket;
	};

	template<typename Session>
	auto connect(asio::io_context &io, std::string_view host, std::string_view port)
	{
		tcp::resolver r(io);
		const auto	  endpoints = r.resolve(host, port);
		return std::make_unique<Session>(io, endpoints);
	}

} // namespace ctl::net