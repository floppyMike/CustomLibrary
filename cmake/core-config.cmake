find_package(SDL2)
if (NOT SDL2_FOUND)
	set(SDL2_INCLUDE_DIR "")
endif()