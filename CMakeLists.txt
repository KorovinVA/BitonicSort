cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

# Not working fo VS
set(CMAKE_BUILD_TYPE "Release")

if (WIN32)
    set(CMAKE_VS_PLATFORM_TOOLSET "Intel(R) oneAPI DPC++ Compiler")
    if(NOT DEFINED ENV{ONEAPI_ROOT})
        message(FATAL_ERROR "ONEAPI_ROOT is not set. Please define it and rerun cmake.")
    endif()
    set(oneApi $ENV{ONEAPI_ROOT})

    include_directories(${oneApi}/dev-utilities/latest/include)
endif()

add_compile_options(-Wno-reorder)
set(SRC src/core/main.cpp
        src/core/bitonicsort.hpp
        src/core/bitonicsortcontrol.hpp
        src/core/bitonicsortcontrol.cpp
)

project(BitonicSort CXX)
add_executable(BitonicSort ${SRC})

if (WIN32)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT BitonicSort)
endif()