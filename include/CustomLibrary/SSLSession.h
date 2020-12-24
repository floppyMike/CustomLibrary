#pragma once

#include "asio.hpp"
#include "asio/ssl.hpp"

#include "Traits.h"

namespace ctl::net
{
	using namespace asio;
	using ip::tcp;

	class SSL_TCP_Session
	{
	public:
		static constexpr std::string_view VERSION = "1.1";

		/**
		 * @brief Create a unconnected session
		 * @param io asio io_context
		 * @param ssl_io asio ssl context
		 */
		explicit SSL_TCP_Session(asio::io_context &io, ssl::context &ssl_io)
			: m_socket(io, ssl_io)
		{
		}

		/**
		 * @brief Create a connected session
		 *
		 * @param io asio io_context
		 * @param ssl_io asio ssl context
		 * @param endpoints asio resolver endpoints
		 */
		explicit SSL_TCP_Session(asio::io_context &io, ssl::context &ssl_io,
								 const tcp::resolver::results_type &endpoints)
			: m_socket(io, ssl_io)
		{
			connect(endpoints);
		}

		~SSL_TCP_Session() { close(); }

		/**
		 * @brief Connect to a endpoint and perform the ssl handshake
		 * @param endpoints asio resolver endpoint
		 */
		void connect(const tcp::resolver::results_type &endpoints)
		{
			m_host = endpoints->host_name();
			asio::connect(m_socket.lowest_layer(), endpoints);

			m_socket.set_verify_mode(ssl::verify_peer);
			m_socket.set_verify_callback(ssl::host_name_verification(m_host));
			m_socket.handshake(ssl::stream_base::client);
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
			if (m_socket.lowest_layer().is_open())
				m_socket.shutdown();
		}

	private:
		std::string				 m_host;
		ssl::stream<tcp::socket> m_socket;
	};

} // namespace ctl::net