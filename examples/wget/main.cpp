#include <asio.hpp>

#include "CustomLibrary/Client.h"
#include "CustomLibrary/HTTP.h"

using namespace ctl;

auto main(int argc, char **argv) -> int
{
	if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0]
				  << " <host> <port> <path>\n"
					 "host: Server hostname\n"
					 "port: Server port to get download from\n"
					 "path: Path to the file of the host\n\n";

		return 1;
	}

	try
	{
		asio::io_context io;
		auto			 session = net::connect<net::TCP_Session>(io, argv[1], argv[2]);

		const auto [headers, content] = net::http_method(&*session).GET(argv[3]);
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