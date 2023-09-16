cmake_minimum_required(VERSION 3.0)

project(cringe)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if (MSVC)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /O3")
else()
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -Wall -Wpedantic")
endif()

add_subdirectory(cringelib)

add_subdirectory(cringetool)

# add_subdirectory(cringelibxx)
