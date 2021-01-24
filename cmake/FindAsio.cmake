set(ASIO_ROOT "asio/include")

set(ASIO_VERSION_ROOT "asio-${ASIO_VERSION}/include")

find_path(ASIO_INCLUDE_DIR
		NAMES
    	asio.hpp
        HINTS
        ENV ASIOSTANDALONE
        PATH_SUFFIXES
        asio/include include
		PATHS
		/usr/include
		/usr/local/include
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	ASIO
	DEFAULT_MSG
	ASIO_INCLUDE_DIR
)

mark_as_advanced(ASIO_INCLUDE_DIR)