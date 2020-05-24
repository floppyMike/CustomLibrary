#pragma once

#include "asio.hpp"

#include <iostream>
#include <memory>

#include <CustomLibrary/Error.h>

namespace ctl::net
{
	using asio::ip::tcp;

	class FClient
	{
	public:
		FClient(asio::io_context &io)
			: m_resolver(io), m_io(io) {}

		//Connect socket to host at path and bind output
		void connect(const std::string& server, const std::string& path, std::iostream& content, std::iostream& header);

		//Handle closing
		void close()
		{
			if (m_socket->is_open())
				m_socket->shutdown(asio::ip::tcp::socket::shutdown_both);
		}

		~FClient()
		{
			close();
		}

	private:
		//Resolve connection
		void _resolve_(const std::error_code& err, tcp::resolver::iterator endIter);

		//Establish a connection
		void _connect_(const std::error_code& err);

		//Request response
		void _writeRequest_(const std::error_code& err);

		//Get status
		void _readStatusLine_(const std::error_code& err);

		//Get headers
		void _readHeaders_(const std::error_code& err);

		//Get content
		void _readContent_(const std::error_code& err);

		asio::io_context &m_io;

		tcp::resolver m_resolver;
		std::unique_ptr<tcp::socket> m_socket;

		asio::streambuf m_request;
		asio::streambuf m_response;

		std::iostream *m_headers;
		std::iostream *m_contents;
	};


//----------------------------------------------
//Implementation
//----------------------------------------------

	inline void FClient::connect(const std::string& server, const std::string& path, std::iostream& content, std::iostream& header)
	{
		m_contents = &content;
		m_headers = &header;

		m_socket = std::make_unique<tcp::socket>(m_io);

		std::ostream request_s(&m_request);
		request_s << "GET " << path << " HTTP/1.0\r\n"
			<< "Host: " << server << "\r\n"
			<< "Accept: */*\r\n"
			<< "Connection: close\r\n\r\n";

		tcp::resolver::query q(server, "http");
		m_resolver.async_resolve(q,
			std::bind(&FClient::_resolve_, this, std::placeholders::_1, std::placeholders::_2));
	}

	inline void FClient::_resolve_(const std::error_code& err, tcp::resolver::iterator endIter)
	{
		if (err)
			throw err::Log(err.message());

		asio::async_connect(*m_socket, endIter,
			std::bind(&FClient::_connect_, this, std::placeholders::_1));
	}

	inline void FClient::_connect_(const std::error_code& err)
	{
		if (err)
			throw err::Log(err.message());

		asio::async_write(*m_socket, m_request,
			std::bind(&FClient::_writeRequest_, this, std::placeholders::_1));
	}

	inline void FClient::_writeRequest_(const std::error_code& err)
	{
		if (err)
			throw err::Log(err.message());

		asio::async_read_until(*m_socket, m_response, "\r\n",
			std::bind(&FClient::_readStatusLine_, this, std::placeholders::_1));
	}

	inline void FClient::_readStatusLine_(const std::error_code& err)
	{
		if (err)
			throw err::Log(err.message());

		std::istream response_s(&m_response);
		std::string httpVersion;
		unsigned int statusCode;
		response_s >> httpVersion >> statusCode;

		std::string statusMessage;
		std::getline(response_s, statusMessage);

		if (!response_s || httpVersion.substr(0, 5) != "HTTP/")
			throw err::Log("Client: Invalid response\n");
		if (statusCode != 200)
			throw err::Log("Client: Response returned with status code " + std::to_string(statusCode));

		asio::async_read_until(*m_socket, m_response, "\r\n\r\n",
			std::bind(&FClient::_readHeaders_, this, std::placeholders::_1));
	}

	inline void FClient::_readHeaders_(const std::error_code& err)
	{
		if (err)
			throw err::Log(err.message());

		std::istream response_s(&m_response);

		std::string header;
		while (std::getline(response_s, header) && header != "\r")
			*m_headers << header << '\n';

		*m_contents << &m_response;

		asio::async_read(*m_socket, m_response, asio::transfer_at_least(1),
			std::bind(&FClient::_readContent_, this, std::placeholders::_1));
	}

	inline void FClient::_readContent_(const std::error_code& err)
	{
		if (!err)
		{
			*m_contents << &m_response;

			asio::async_read(*m_socket, m_response, asio::transfer_at_least(1),
				std::bind(&FClient::_readContent_, this, std::placeholders::_1));
		}

		else if (err != asio::error::eof)
			throw err::Log(err.message());
	}


	using Client = FClient;
}