#pragma once

#include "asio.hpp"

#include <istream>
#include <memory>
#include <string_view>
#include <utility>

namespace ctl::net
{
	using asio::ip::tcp;

	/**
	 * @brief Controls and manages a tcp session
	 */
	class TCP_Session
	{
	public:
		static constexpr std::string_view VERSION = "1.0";

		/**
		 * @brief Create a unconnected session
		 * @param io asio io_context
		 */
		explicit TCP_Session(asio::io_context &io)
			: m_socket(io)
		{
		}

		/**
		 * @brief Create a connected session
		 *
		 * @param io asio io_context
		 * @param endpoints asio resolver endpoints
		 */
		explicit TCP_Session(asio::io_context &io, const tcp::resolver::results_type &endpoints)
			: m_socket(io)
		{
			connect(endpoints);
		}

		~TCP_Session() { close(); }

		/**
		 * @brief Connect to a endpoint
		 * @param endpoints asio resolver endpoint
		 */
		void connect(const tcp::resolver::results_type &endpoints)
		{
			m_host = endpoints->host_name();
			asio::connect(m_socket, endpoints);
		}

		/**
		 * @brief Get the host name
		 * @return host name
		 */
		[[nodiscard]] auto host() const noexcept -> std::string_view { return m_host; }

		/**
		 * @brief Get the used socket
		 * @return Used socket
		 */
		auto socket() noexcept -> auto & { return m_socket; }

		/**
		 * @brief Close the connection
		 */
		void close()
		{
			if (m_socket.is_open())
				m_socket.close();
		}

	private:
		std::string m_host;
		tcp::socket m_socket;
	};

	/**
	 * @brief Connect to a host using the specified port
	 *
	 * @tparam Session Session Type to return
	 * @param io asio io_context
	 * @param host Host to connect to
	 * @param port Port on host to connect to
	 * @return Session object
	 */
	template<typename Session, typename... _T>
	auto connect(std::string_view host, std::string_view port, asio::io_context &io, _T &&...session_args)
	{
		tcp::resolver r(io);
		const auto	  endpoints = r.resolve(host, port);
		return std::make_unique<Session>(io, std::forward<_T>(session_args)..., endpoints);
	}

} // namespace ctl::net