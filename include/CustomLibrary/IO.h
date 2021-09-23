#pragma once

#include <cstdio>

namespace ctl
{
	// -----------------------------------------------------------------------------
	// Formatters
	// -----------------------------------------------------------------------------

	template<typename T>
	T format(const T &arg)
	{
		return arg;
	}

	// -----------------------------------------------------------------------------
	// Print function
	// -----------------------------------------------------------------------------

	namespace detail
	{
		void check_format(const char *, ...) __attribute__((format(printf, 1, 2)));
	}

	template<typename... T>
	void print(const char *fmt, const T &...param)
	{
		std::printf(fmt, format(param)...);
	}

	inline void print(const char *str)
	{
		std::fputs(str, stdout);
	}

	// -----------------------------------------------------------------------------
	// File IO
	// -----------------------------------------------------------------------------

	struct File
	{
		std::FILE *fp = nullptr;

		File() = default;
		File(const char *name, const char *mode)
		{
			open(name, mode);
		}

		~File()
		{
			close();
		}

		void open(const char *name, const char *mode)
		{
			fp = std::fopen(name, mode);
		}

		void close()
		{
			if (fp != nullptr)
			{
				fclose(fp);
				fp = nullptr;
			}
		}

		template<typename... T>
		void print(const char *fmt, const T &...args)
		{
			std::fprintf(fp, fmt, format(args)...);
		}
	};

} // namespace ctl
