#pragma once

#include "Traits.h"
#include "utility.h"
#include <asio.hpp>
#include <algorithm>

namespace ctl::net
{
	// -----------------------------------------------------------------------------
	// Predefinitions
	// -----------------------------------------------------------------------------

	class TCP_Session;

	// -----------------------------------------------------------------------------
	// HTTP Methods
	// -----------------------------------------------------------------------------

	namespace detail
	{
		using asio::ip::tcp;

		template<typename, typename, typename>
		class _HTTP_Method_;

		/**
		 * @brief Handles a TCP session HTTP commands
		 */
		template<typename Full, typename Impl>
		class _HTTP_Method_<TCP_Session, Full, Impl> : public Impl
		{
		public:
			using Impl::Impl;

			/**
			 * @brief Add a request header with name and value
			 * 
			 * @param name Header name
			 * @param val Header value
			 * @return this reference
			 */
			auto add_header(std::string_view name, std::string_view val) -> auto &
			{
				assert(std::find_if(m_headers.begin(), m_headers.end(),
									[name](const std::pair<std::string, std::string> &e) { return e.first == name; })
						   == m_headers.end()
					   && "Header name already exists.");

				m_headers.emplace_back(name, val);
				return *this;
			}

			/**
			 * @brief Sends a HTTP GET request with the headers
			 * 
			 * @param path GET request path
			 * @return pair with { response header , response content }
			 */
			auto GET(std::string_view path) -> std::pair<std::string, std::string>
			{
				auto &			p = *this->obj();
				asio::streambuf buf;

				// Get response
				_construct_headers_(path, buf);
				asio::write(p.socket(), buf);

				// Read and validate response headers
				asio::read_until(p.socket(), buf, "\r\n");
				_validate_response_(&buf);

				// Read response headers and response
				return { _read_headers_(buf), _read_content_(buf) };
			}

		private:
			std::vector<std::pair<std::string, std::string>> m_headers;

			auto _read_headers_(asio::streambuf &buf) -> std::string
			{
				const auto	n = asio::read_until(this->obj()->socket(), buf, "\r\n\r\n");
				std::string data =
					std::string(asio::buffers_begin(buf.data()), asio::buffers_begin(buf.data()) + n - 4);
				buf.consume(n);

				return data;
			}

			auto _read_content_(asio::streambuf &buf) -> std::string
			{
				std::string data;

				for (std::error_code err; err != asio::error::eof;)
				{
					const auto size = buf.size() + asio::read(this->obj()->socket(), buf, err);

					data.append(std::string(asio::buffers_begin(buf.data()), asio::buffers_begin(buf.data()) + size));
					buf.consume(size);
				}

				return data;
			}

			void _construct_headers_(std::string_view path, asio::streambuf &buf) const
			{
				std::ostream r(&buf);
				r << "GET " << path
				  << " HTTP/1.0\r\n"
					 "Host: "
				  << this->obj()->host()
				  << "\r\n"
					 "Accept: */*\r\n"
					 "Connection: close\r\n";

				for (const auto &entry : m_headers) r << entry.first << ": " << entry.second << "\r\n";
				r << "\r\n";
			}

			void _validate_response_(asio::streambuf *buf) const
			{
				// Get http details
				std::istream s(buf);
				std::string	 version, status;
				s >> version >> status;

				// Get status message
				std::string stat_msg;
				std::getline(s, stat_msg);

				// Validate headers
				if (!s || version.substr(0, 5) != "HTTP/")
					throw std::runtime_error("Invalid response. Msg: " + stat_msg);
				if (status != "200")
					throw std::runtime_error("Response with status code " + status + ". Msg: " + stat_msg);
			}
		};
	} // namespace detail

	// -----------------------------------------------------------------------------
	// HTTP Extension
	// -----------------------------------------------------------------------------

	/**
	 * @brief Type for sending HTTP requests
	 * @tparam T Object to send HTTP with
	 */
	template<typename T>
	using HTTP_Method = typename Filter<detail::_HTTP_Method_, Functor<T>, T>::type;

	/**
	 * @brief Gives options for a HTTP request
	 * 
	 * @param ptr ptr to object
	 * @return options object
	 */
	template<typename T>
	auto http_method(T *const ptr)
	{
		return HTTP_Method<T>(ptr);
	}

} // namespace ctl::net