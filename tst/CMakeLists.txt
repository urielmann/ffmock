#
# @brief Unit tests using mocks in different ways
#
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
# @brief DLL hosted mocks for use by FFmockUnitTtsts_dll unit tests
#
project(Mocks_dll)
    add_library(${PROJECT_NAME} SHARED)
    target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_SOURCE_DIR}/inc
        )
    target_sources(${PROJECT_NAME} PRIVATE Mocks.cpp Mocks.hpp)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _DEBUG
        _AMD64_
        "FFMOCK_IMPORT=__declspec(dllexport)"
        )

#
# @brief DLL hosted mocks unit tests
#
project(FFmockUnitTtsts_dll)
    add_executable(${PROJECT_NAME})
    target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_SOURCE_DIR}/inc
        PRIVATE ${CMAKE_SOURCE_DIR}/lib
                "$ENV{ffmock.gtest.inc.dir}"
        )
    target_compile_options(${PROJECT_NAME} PRIVATE "/MT")
    target_link_directories(${PROJECT_NAME} PRIVATE "$ENV{ffmock.gtest.lib.dir}")
    target_link_options(${PROJECT_NAME}
        PRIVATE "/NODEFAULTLIB:ADVAPI32.lib"
                "/NODEFAULTLIB:LIBCMT"
                "/NODEFAULTLIB:LIBCMTD"
                "/INCREMENTAL:NO"
                "/MANIFEST:NO"
                "/MAP"
        )
    target_link_libraries(${PROJECT_NAME}
        PRIVATE ffmock
                Mocks_dll
                gtest.lib
                vcruntimed.lib
                msvcrtd.lib
        )
    target_sources(${PROJECT_NAME}
        PRIVATE FFmockUnitTests.cpp
                FFmockUnitTests.manifest
                Mocks.hpp
                ${CMAKE_SOURCE_DIR}/lib/SCM.hpp
                ${CMAKE_SOURCE_DIR}/lib/Service.hpp
                ${CMAKE_SOURCE_DIR}/lib/Registry.hpp
        )
    target_sources(${PROJECT_NAME}
        PRIVATE FFmockUnitTests.rc
        )
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE _DEBUG
                _AMD64_
                "FFMOCK_IMPORT=__declspec(dllimport)"
        )
    add_dependencies(${PROJECT_NAME} Mocks_dll ffmock)

#
# @brief Unit tests with included sources
#
project(FFmockUnitTtsts_src)
    add_executable(${PROJECT_NAME})
    target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_SOURCE_DIR}/inc
        PRIVATE ${CMAKE_SOURCE_DIR}/lib
                "$ENV{ffmock.gtest.inc.dir}"
        )
    target_compile_options(${PROJECT_NAME} PRIVATE "/MT")
    target_link_directories(${PROJECT_NAME} PRIVATE "$ENV{ffmock.gtest.lib.dir}")
    target_link_options(${PROJECT_NAME}
        PRIVATE "/NODEFAULTLIB:ADVAPI32.lib"
                "/NODEFAULTLIB:LIBCMT"
                "/NODEFAULTLIB:LIBCMTD"
                "/INCREMENTAL:NO"
                "/MANIFEST:NO"
                "/IGNORE:4217"
                "/MAP"
        )
    target_link_libraries(${PROJECT_NAME}
        PRIVATE gtest.lib
                vcruntimed.lib
                msvcrtd.lib
        )
    target_sources(${PROJECT_NAME}
        PRIVATE FFmockUnitTests.cpp
                FFmockUnitTests.manifest
                Mocks.cpp
                Mocks.hpp
                ${CMAKE_SOURCE_DIR}/lib/SCM.cpp
                ${CMAKE_SOURCE_DIR}/lib/SCM.hpp
                ${CMAKE_SOURCE_DIR}/lib/Service.cpp
                ${CMAKE_SOURCE_DIR}/lib/Service.hpp
                ${CMAKE_SOURCE_DIR}/lib/Registry.cpp
                ${CMAKE_SOURCE_DIR}/lib/Registry.hpp
        )
    target_sources(${PROJECT_NAME}
        PRIVATE FFmockUnitTests.rc
        )
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE _DEBUG
                _AMD64_
                "FFMOCK_IMPORT="
                "ControlService=__mock_ControlService"
                "RegisterServiceCtrlHandlerW=__mock_RegisterServiceCtrlHandlerW"
                "SetServiceStatus=__mock_SetServiceStatus"
                "CreateServiceW=__mock_CreateServiceW"
                "DeleteService=__mock_DeleteService"
                "OpenSCManagerW=__mock_OpenSCManagerW"
                "OpenServiceW=__mock_OpenServiceW"
                "QueryServiceStatus=__mock_QueryServiceStatus"
                "StartServiceW=__mock_StartServiceW"
                "CloseServiceHandle=__mock_CloseServiceHandle"
                "RegCloseKey=__mock_RegCloseKey"
                "RegCreateKeyW=__mock_RegCreateKeyW"
                "RegCreateKeyExW=__mock_RegCreateKeyExW"
                "RegOpenKeyW=__mock_RegOpenKeyW"
                "RegSetValueExW=__mock_RegSetValueExW"
        )
