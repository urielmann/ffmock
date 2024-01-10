#
# @brief CMake configuration for various unit tests
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

#
# @brief DLL hosted mocks for use by FFmockUnitTests unit tests
#
project(Mocks_dll)
    add_library(${PROJECT_NAME} SHARED)
    target_sources(${PROJECT_NAME} PRIVATE Mocks.cpp Mocks.hpp)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE _AMD64_
                "FFMOCK_IMPORT=__declspec(dllexport)"
        )

set(MANGLED_MOCKS_DEFINITIONS
    "RegCloseKey=__mock_RegCloseKey"
    "RegCreateKeyW=__mock_RegCreateKeyW"
    "RegCreateKeyExW=__mock_RegCreateKeyExW"
    "RegDeleteValueW=__mock_RegDeleteValueW"
    "RegOpenKeyW=__mock_RegOpenKeyW"
    "RegSetValueExW=__mock_RegSetValueExW"
)
include_directories(${CMAKE_SOURCE_DIR}/demo/lib $ENV{ffmock.gtest.inc.dir})

#
# @brief DLL hosted mocks unit tests
#
project(FFmockUnitTests_dll)
    add_executable(${PROJECT_NAME})
    target_link_libraries(${PROJECT_NAME}
        PRIVATE Demo_lib
                Mocks_dll
                ${GTEST_BOTH_LIBRARIES}
                vcruntimed.lib
                msvcrtd.lib
        )
    target_sources(${PROJECT_NAME}
        PRIVATE FFmockUnitTests.cpp
                FFmockUnitTests.manifest
                Mocks.hpp
                ${CMAKE_SOURCE_DIR}/demo/lib/Registry.hpp
        )
    target_sources(${PROJECT_NAME}
        PRIVATE FFmockUnitTests.rc
        )
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE _AMD64_
                "FFMOCK_IMPORT=__declspec(dllimport)"
        )
        add_dependencies(${PROJECT_NAME} Mocks_dll Demo_lib)

#
# @brief Unit tests with included sources
#
project(FFmockUnitTests_src)
    add_executable(${PROJECT_NAME})
    target_link_options(${PROJECT_NAME}
        PRIVATE "/IGNORE:4217"
        )
    target_link_libraries(${PROJECT_NAME}
        PRIVATE ${GTEST_BOTH_LIBRARIES}
                vcruntimed.lib
                msvcrtd.lib
        )
    target_sources(${PROJECT_NAME}
        PRIVATE FFmockUnitTests.cpp
                FFmockUnitTests.manifest
                Mocks.cpp
                Mocks.hpp
                ${CMAKE_SOURCE_DIR}/demo/lib/Registry.cpp
                ${CMAKE_SOURCE_DIR}/demo/lib/Registry.hpp
        )
    target_sources(${PROJECT_NAME}
        PRIVATE FFmockUnitTests.rc
        )
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE _AMD64_
                "FFMOCK_IMPORT="
                ${MANGLED_MOCKS_DEFINITIONS}
        )

#
# @brief Service static library for unit tests
#
project(Demo_tst)
    add_library(${PROJECT_NAME} STATIC)
    target_sources(${PROJECT_NAME}
        PRIVATE ${CMAKE_SOURCE_DIR}/demo/lib/Registry.cpp
                ${CMAKE_SOURCE_DIR}/demo/lib/Registry.hpp
        )
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE _AMD64_
                "FFMOCK_IMPORT="
                ${MANGLED_MOCKS_DEFINITIONS}
        )

#
# @brief Static lib hosted mocks for use by FFmockUnitTests_lib unit tests
#
project(Mocks_lib)
    add_library(${PROJECT_NAME} SHARED)
    target_sources(${PROJECT_NAME} PRIVATE Mocks.cpp Mocks.hpp)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE _AMD64_
                "FFMOCK_IMPORT="
        )

#
# @brief Unit tests with sources included from static library
#
project(FFmockUnitTests_lib)
    add_executable(${PROJECT_NAME})
    target_link_options(${PROJECT_NAME}
        PRIVATE "/IGNORE:4217"
        )
    target_link_libraries(${PROJECT_NAME}
        PRIVATE Mocks_lib
                Demo_tst
                ${GTEST_BOTH_LIBRARIES}
                vcruntimed.lib
                msvcrtd.lib
        )
    target_sources(${PROJECT_NAME}
        PRIVATE FFmockUnitTests.cpp
                FFmockUnitTests.manifest
                Mocks.cpp
                Mocks.hpp
        )
    target_sources(${PROJECT_NAME}
        PRIVATE FFmockUnitTests.rc
        )
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE _AMD64_
                "FFMOCK_IMPORT="
                ${MANGLED_MOCKS_DEFINITIONS}
        )
        add_dependencies(${PROJECT_NAME} Mocks_lib Demo_tst)
