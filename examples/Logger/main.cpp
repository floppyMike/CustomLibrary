#include <ostream>
#include <vector>

// Be aware that intended overloads must be declared before the logger implementation
auto operator<<(std::ostream &o, const std::vector<int> &s) noexcept -> std::ostream &
{
	for (const auto &v : s) o << v;
	return o;
}

#include <CustomLibrary/Error.h>

using namespace ctl;

auto main() -> int
{
	err::Logger<err::ConsolePolicy> log;
	log.write(err::Catagory::FATAL) << "This is a fatal error!";
	log.write(err::Catagory::SUCCESS, "Or maybe not!");

	std::vector<int> v = { 1, 2, 3, 4, 5, 6 };
	log.write(err::Catagory::WARN) << v;

	log.seperate();

	err::Logger<err::FilePolicy, err::ConsolePolicy> log_file(err::FilePolicy("Log.txt"),
																	err::ConsolePolicy());

	log_file.write(err::Catagory::ERR) << "Written in both console and file!";
    log_file.write(err::Catagory::INFO, "And it efficiently saves on each write!");


	return 0;
}