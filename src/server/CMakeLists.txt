find_package (Threads)

set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror -g")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Werror -03")

set (SERVER_SOURCES
    Main.cpp
    ThreadPool.cpp
)
add_executable (thegame-server ${SERVER_SOURCES})

target_link_libraries (thegame-server ${CMAKE_THREAD_LIBS_INIT})

set_property (TARGET thegame-server PROPERTY CXX_STANDARD 14)
