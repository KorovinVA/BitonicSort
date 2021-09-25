cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Not working fo VS!
set(CMAKE_BUILD_TYPE "Release")

if (WIN32)
    set(CMAKE_VS_PLATFORM_TOOLSET "Intel(R) oneAPI DPC++ Compiler")
    if(NOT DEFINED ENV{ONEAPI_ROOT})
        message(FATAL_ERROR "ONEAPI_ROOT is not set. Please define it and rerun cmake.")
    endif()
    set(oneApi $ENV{ONEAPI_ROOT})
endif()

include_directories(${oneApi}/dev-utilities/latest/include)
set(SRC src/core/main.cpp)

project(BitonicSort CXX)
add_executable(BitonicSort ${SRC})

if (WIN32)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT BitonicSort)
endif()