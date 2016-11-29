find_package (Threads)

set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror -g")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Werror -03")

set (COMMON_SOURCES
    EventDispatcher.cpp
    Socket.cpp
)

add_library (common ${COMMON_SOURCES})

set_property (TARGET common PROPERTY CXX_STANDARD 14)
