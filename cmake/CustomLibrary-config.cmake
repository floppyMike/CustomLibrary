foreach(component ${CustomLibrary_FIND_COMPONENTS})
    include(${CMAKE_CURRENT_LIST_DIR}/${component}-config.cmake)
endforeach()
