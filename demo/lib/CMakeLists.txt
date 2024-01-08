# @brief Mock demo library

# Copyright (C) 2023-2024 Uriel Mann (abba.mann@gmail.com)

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

#
# @brief Service static library
#
project(Service_lib)
    add_library(${PROJECT_NAME} STATIC)
    target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_SOURCE_DIR}/inc
        PRIVATE ${CMAKE_SOURCE_DIR}/demo/inc
        )
    target_sources(${PROJECT_NAME}
        PRIVATE Registry.cpp
                Registry.hpp
        )
        target_compile_options(${PROJECT_NAME} PRIVATE "/MT" "/W4" "/WX")
        target_compile_definitions(${PROJECT_NAME}
                PRIVATE _DEBUG
                        _AMD64_
                        "FFMOCK_IMPORT="
        )
