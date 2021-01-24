#include <asio.hpp>
#include <iostream>

#include "CustomLibrary/Session.h"
#include "CustomLibrary/SSLSession.h"
#include "CustomLibrary/HTTP.h"

using namespace ctl;

auto main(int argc, char **argv) -> int
{
	if (argc < 4)
	{
		std::cerr << "Usage: " << argv[0]
				  << " <host> <port> <path> [flags]\n"
					 "Not a serious example. Can only use HTTP with TCP.\n"
					 "host: Server hostname\n"
					 "port: Server port to get download from\n"
					 "path: Path to the file of the host\n"
					 "--ssl: Use SSL to connect\n\n";

		return 1;
	}

	try
	{
		asio::io_context io;

		std::string headers, content;

		if (argc == 4)
		{
			auto session			   = net::connect<net::TCP_Session>(argv[1], argv[2], io);
			std::tie(headers, content) = net::http_method(&*session).GET(argv[3]);
		}
		else
		{
			asio::ssl::context ctx(asio::ssl::context::sslv23);
			ctx.set_default_verify_paths();

			auto session = net::connect<net::SSL_TCP_Session>(argv[1], argv[2], io, ctx);

			std::tie(headers, content) = net::http_method(&*session).GET(argv[3]);
		}

		std::clog << headers << "\n\n";
		std::cout << content;
		std::clog << std::endl;
	}
	catch (const asio::system_error &err)
	{
		std::cerr << "Error Code: " << err.code() << " Msg: " << err.what() << '\n';
		return 1;
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << '\n';
		return 1;
	}

	return 0;
}