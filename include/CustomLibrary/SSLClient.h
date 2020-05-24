#pragma once

#include "asio.hpp"
#include "asio/ssl.hpp"

#include <iostream>
#include <memory>
#include <functional>

#include "Error.h"
#include "Traits.h"

namespace ctl::net
{
	using asio::ip::tcp;

	struct SSL {};
	struct NonSSL {};


	template<template<typename> class... Ex>
	class SSLSession : tcp::resolver, public Ex<SSLSession<Ex...>>...
	{
	public:
		using session_type = SSL;

		static constexpr std::string_view SERVICE = "https";
		static constexpr std::string_view GETTER = "HTTP/1.1";

		SSLSession(asio::io_context& io, asio::ssl::context& ctx)
			: m_io(io)
			, m_ctx(ctx)
			, tcp::resolver(io)
		{
		}

		void create_socket()
		{
			m_socket = std::make_unique<asio::ssl::stream<tcp::socket>>(m_io, m_ctx);
		}

		auto& basic_socket()
		{
			return m_socket->lowest_layer();
		}
		auto& socket()
		{
			return *m_socket;
		}

		using tcp::resolver::async_resolve;

	private:
		asio::io_context& m_io;
		asio::ssl::context& m_ctx;

		std::unique_ptr<asio::ssl::stream<tcp::socket>> m_socket;
	};

	template<template<typename> class... Ex>
	class Session : tcp::resolver, public Ex<Session<Ex...>>...
	{
	public:
		using session_type = NonSSL;

		static constexpr std::string_view SERVICE = "http";
		static constexpr std::string_view GETTER = "HTTP/1.0";

		Session(asio::io_context& io)
			: m_io(io)
			, tcp::resolver(io)
		{
		}

		void create_socket()
		{
			m_socket = std::make_unique<tcp::socket>(m_io);
		}

		auto& basic_socket()
		{
			return socket();
		}
		auto& socket()
		{
			return *m_socket;
		}

		using tcp::resolver::async_resolve;

	private:
		asio::io_context& m_io;
		std::unique_ptr<tcp::socket> m_socket;
	};


	template<typename Session_t>
	class E_BasicAsync : public crtp<Session_t, E_BasicAsync>
	{
		HAS_METHOD_TYPE(socket)
		HAS_METHOD_TYPE(basic_socket)

		inline constexpr void _socket_accessor_check_() const noexcept { 
			static_assert(ctl::is_detected_v<has_socket, Session_t>, "Async operations require the \"socket\" method."); 
		}
		inline constexpr void _basic_socket_accessor_check_() const noexcept { 
			static_assert(ctl::is_detected_v<has_basic_socket, Session_t>, "Async operations require the \"basic_socket\" method."); 
		}

	public:
		template<typename... T>
		auto async_connect(T&&... arg)
		{
			_socket_accessor_check_();
			_basic_socket_accessor_check_();
			return asio::async_connect(this->_().basic_socket(), std::forward<T>(arg)...);
		}
		template<typename... T>
		auto async_write(T&&... arg)
		{
			_socket_accessor_check_();
			return asio::async_write(this->_().socket(), std::forward<T>(arg)...);
		}
		template<typename... T>
		auto async_read_until(T&&... arg)
		{
			_socket_accessor_check_();
			return asio::async_read_until(this->_().socket(), std::forward<T>(arg)...);
		}
		template<typename... T>
		auto async_read(T&&... arg)
		{
			_socket_accessor_check_();
			return asio::async_read(this->_().socket(), std::forward<T>(arg)...);
		}
	};

	template<typename T>
	class E_SSLAsync : public crtp<T, E_SSLAsync>
	{
	public:
		FORWARDING_MEMBER_FUNCTIONS_C(asio::ssl::stream<tcp::socket>, this->_().socket(), async_handshake, &)
	};


	template<typename Session_t>
	class Client
	{
	public:
		template<typename... T>
		Client(asio::io_context& io, T&&... arg)
			: m_session(io, std::forward<T>(arg)...)
		{
		}

		void connect(const char* host, const char* path)
		{
			m_session.create_socket();

			std::ostream request_s(&m_buffer);
			request_s << "GET " << path << ' ' << Session_t::GETTER << "\r\n"
				<< "Host: " << host << "\r\n"
				<< "Accept: */*\r\n"
				<< "Connection: close\r\n\r\n";

			m_session.async_resolve(host, Session_t::SERVICE, [this](const std::error_code& err, tcp::resolver::results_type endIter) { _resolve_(err, endIter); });
		}

		template<typename Func>
		void contentHandler(Func&& f)
		{
			m_content = f;
		}

	private:
		Session_t m_session;
		asio::streambuf m_buffer;

		std::string m_resHeaders;
		std::function<void(asio::streambuf&, size_t)> m_content;


		void _throwErr_(const std::error_code& err)
		{
			if (err)
				throw err::Log(err.message());
		}

		void _resolve_(const std::error_code& err, tcp::resolver::results_type endIter);
		void _connect_(const std::error_code& err);
		void _handshake_(const std::error_code& err);
		void _write_(const std::error_code& err);
		void _readStatusLine_(const std::error_code& err);
		void _readHeaders_(const std::error_code& err, size_t bytes);
		void _readContent_(const std::error_code& err, size_t bytes);
	};


	//----------------------------------------------
	//Implementation
	//----------------------------------------------

	template<typename Session_t>
	inline void Client<Session_t>::_resolve_(const std::error_code& err, tcp::resolver::results_type endIter)
	{
		_throwErr_(err);
		m_session.async_connect(endIter, [this](const std::error_code& err, const tcp::endpoint& ep) { _connect_(err); });
	}

	template<typename Session_t>
	inline void Client<Session_t>::_connect_(const std::error_code& err)
	{
		_throwErr_(err);

		if constexpr (std::is_same_v<Session_t::session_type, SSL>)
			m_session.async_handshake(asio::ssl::stream_base::client, [this](const std::error_code& err) { _handshake_(err); });
		else
			m_session.async_write(m_buffer, [this](const std::error_code& err, size_t bytes) { _write_(err); });
	}

	template<typename Session_t>
	inline void Client<Session_t>::_handshake_(const std::error_code& err)
	{
		_throwErr_(err);
		m_session.async_write(m_buffer, [this](const std::error_code& err, size_t bytes) { _write_(err); });
	}

	template<typename Session_t>
	inline void Client<Session_t>::_write_(const std::error_code& err)
	{
		_throwErr_(err);
		m_session.async_read_until(m_buffer, "\r\n", [this](const std::error_code& err, size_t bytes) { _readStatusLine_(err); });
	}

	template<typename Session_t>
	inline void Client<Session_t>::_readStatusLine_(const std::error_code& err)
	{
		_throwErr_(err);

		std::istream response_s(&m_buffer);
		std::string httpVersion;
		unsigned int statusCode;
		response_s >> httpVersion >> statusCode;

		std::string statusMessage;
		std::getline(response_s, statusMessage);

		if (!response_s || httpVersion.substr(0, 5) != "HTTP/")
			throw err::Log("Invalid response\n");
		if (statusCode != 200)
			throw err::Log("Response returned with status code " + std::to_string(statusCode));

		m_session.async_read_until(m_buffer, "\r\n\r\n", [this](const std::error_code& err, size_t bytes) { _readHeaders_(err, bytes); });
	}

	template<typename Session_t>
	inline void Client<Session_t>::_readHeaders_(const std::error_code& err, size_t bytes)
	{
		_throwErr_(err);

		std::istream response_s(&m_buffer);
		for (std::string header; std::getline(response_s, header) && header != "\r";)
		{
			header.push_back(' ');
			m_resHeaders += header;
		}
		m_content(m_buffer, bytes);

		m_session.async_read(m_buffer, asio::transfer_at_least(1), [this](const std::error_code& err, size_t bytes) { _readContent_(err, bytes); });
	}

	template<typename Session_t>
	inline void Client<Session_t>::_readContent_(const std::error_code& err, size_t bytes)
	{
		if (!err)
		{
			m_content(m_buffer, bytes);
			m_session.async_read(m_buffer, asio::transfer_at_least(1), [this](const std::error_code& err, size_t bytes) { _readContent_(err, bytes); });
		}

		else if (err != asio::error::eof)
			throw err::Log(err.message());
	}
}