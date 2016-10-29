
find_package (Threads)

set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (SERVER_SOURCES
    Main.cpp
    ThreadPool.cpp
)
add_executable (thegame-server ${SERVER_SOURCES})

target_link_libraries (thegame-server ${CMAKE_THREAD_LIBS_INIT})

set_property (TARGET thegame-server PROPERTY CXX_STANDARD 14)
