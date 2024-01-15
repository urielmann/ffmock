#
# @brief CMake configuration
#
# @copyright (C) 2023-2024 Uriel Mann (abba.mann@gmail.com)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

option(BUILD_GMOCK "Build gmock" OFF)

cmake_minimum_required(VERSION 3.11)

project(ffmock
    VERSION 0.90
    DESCRIPTION "Free Functions Mocking Library")

    # Set a default build type for single-configuration
    # CMake generators if no build type is set.
    if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
    endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

    # Select flags.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /EHsc /GR- /Oi /wd4163 /Zc:externC-")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MT")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /WX
                                                          /MAP
                                                          /NODEFAULTLIB:ADVAPI32.lib
                                                          /NODEFAULTLIB:LIBCMT
                                                          /NODEFAULTLIB:LIBCMTD
                                                          /MANIFEST:NO
                                                          /INCREMENTAL:NO
                                                          /IGNORE:4099
                                                          /DEBUG:FULL
                                                          /LIBPATH:${CMAKE_BINARY_DIR}/lib/$(Configuration)")
    include_directories(${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/demo/inc)

    # Set compiler defines based on architecture
    string(TOLOWER "${CMAKE_GENERATOR_PLATFORM}" ARCHITECTURE)
    if(CMAKE_GENERATOR STREQUAL "Ninja")
        message(STATUS "CMAKE_GENERATOR = ${CMAKE_GENERATOR}")
        add_compile_definitions(_AMD64_ WIN64)
        string(REPLACE "/W3 " "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REPLACE "/GR " "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REPLACE "/MD " "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
        string(REPLACE "/MD " "" CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")
        string(REPLACE "/MD " "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
        string(REPLACE "/MDd " "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
        message(STATUS "CMAKE_CXX_FLAGS = '${CMAKE_CXX_FLAGS}'")
    elseif(ARCHITECTURE STREQUAL "win32")
        add_compile_definitions(_X86_ WIN32)
    elseif(ARCHITECTURE STREQUAL "x64")
        add_compile_definitions(_AMD64_ WIN64)
    else(ARCHITECTURE STREQUAL "win32")
        message(FATAL_ERROR "No architecture was specified!")
    endif(CMAKE_GENERATOR STREQUAL "Ninja")

    include(CTest)

    set(GOOGLETEST_VERSION 1.14.0)
    add_subdirectory(googletest/googletest)
    add_subdirectory(demo/lib)
    add_subdirectory(demo/tst)
