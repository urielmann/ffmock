# @brief Mock demo library

# Copyright (C) 2019 Uriel Mann (abba.mann@gmail.com)

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
project(Service_dll)
    add_library(${PROJECT_NAME} SHARED)
    target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_SOURCE_DIR}/inc
        PRIVATE ${CMAKE_SOURCE_DIR}/lib
        )
    target_sources(${PROJECT_NAME}
        PRIVATE Service.def
                ${CMAKE_SOURCE_DIR}/lib/SCM.cpp
                ${CMAKE_SOURCE_DIR}/lib/SCM.hpp
                ${CMAKE_SOURCE_DIR}/lib/Service.cpp
                ${CMAKE_SOURCE_DIR}/lib/Service.hpp
                ${CMAKE_SOURCE_DIR}/lib/Registry.cpp
                ${CMAKE_SOURCE_DIR}/lib/Registry.hpp
        )
    add_dependencies(${PROJECT_NAME} Service)
    target_link_options(${PROJECT_NAME}
        PRIVATE "/DEF:Service.def"
                "/OUT:${PROJECT_BINARY_DIR}/$(Configuration)/Service.dll"
                "/PDB:${PROJECT_BINARY_DIR}/$(Configuration)/Service.pdb"
                "/NODEFAULTLIB:LIBCMT"
                "/NODEFAULTLIB:LIBCMTD"
                "/INCREMENTAL:NO"
                "/MANIFEST:NO"
                "/MAP"
        )
    set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE vcruntimed.lib
                msvcrtd.lib
        )
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE _DEBUG
                _AMD64_
        )
