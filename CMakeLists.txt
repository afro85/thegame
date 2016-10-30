cmake_minimum_required (VERSION 3.1)
project (TheGame)

# Check whether the CMAKEU_BUILD_TYPE is provided
IF (NOT CMAKE_BUILD_TYPE)
    message (FATAL_ERROR "Choose the type of build (CMAKE_BUILD_TYPE): Debug or Release")
endif()
message (STATUS "Build type: " ${CMAKE_BUILD_TYPE} )

include_directories (include)

add_subdirectory (src)
