set(ASIO_ROOT "asio/include")

set(ASIO_VERSION_ROOT "asio-${ASIO_VERSION}/include")

find_path(ASIO_INCLUDE_DIR
		NAMES
    	asio.hpp
        HINTS
        ENV ASIOSTANDALONE
		PATHS
		/usr/include
		/usr/include/${ASIO_ROOT}
		/usr/include/${ASIO_VERSION_ROOT}
		
		/usr/local/include
    	/usr/local/include/${ASIO_ROOT}
		/usr/local/include/${ASIO_VERSION_ROOT}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	ASIO
	DEFAULT_MSG
	ASIO_INCLUDE_DIR
)

mark_as_advanced(ASIO_INCLUDE_DIR)