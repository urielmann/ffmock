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

cmake_minimum_required(VERSION 3.11)
project(ffmock
    VERSION 0.90
    DESCRIPTION "Free Functions Mocking Library")

    # Set default GTest location
    if(NOT GTEST_ROOT)
        set(GTEST_ROOT "C:/Program Files (x86)/googletest-distribution")
    endif(NOT GTEST_ROOT)
    find_package(GTest REQUIRED)

    # Default architecture
    if(NOT FFMOCK_ARCH)
        set(FFMOCK_ARCH _AMD64_)
    endif(NOT FFMOCK_ARCH)

    # Set a default build type for single-configuration
    # CMake generators if no build type is set.
    if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
    endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

    # Select flags.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /EHsc /GR-")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
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
                                                          /LIBPATH:$ENV{ffmock.gtest.lib.dir}/$(Configuration)")
    include_directories(${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/demo/inc)
    
    include(CTest)

    add_subdirectory(demo/lib)
    add_subdirectory(demo/tst)
