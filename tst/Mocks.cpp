/**
  @brief Win32 API mocks
  @author Uriel Mann
  @copyright 2023 Uriel Mann (abba.mann@gmail.com)

:: Permission is hereby granted, free of charge, to any person obtaining a copy
:: of this software and associated documentation files (the "Software"), to deal
:: in the Software without restriction, including without limitation the rights
:: to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
:: copies of the Software, and to permit persons to whom the Software is
:: furnished to do so, subject to the following conditions:

:: The above copyright notice and this permission notice shall be included in all
:: copies or substantial portions of the Software.

:: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
:: IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
:: FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
:: AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
:: LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
:: OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
:: SOFTWARE.
*/

#include "Mocks.hpp"

#pragma warning(disable:4273) // inconsistent dll linkage

static HMODULE AdvAPI32{LoadLibraryW(L"advapi32.dll")};

DEFINE_GUARD(RegisterServiceCtrlHandlerW);
DEFINE_GUARD(SetServiceStatus);
DEFINE_GUARD(ControlService);
DEFINE_GUARD(CreateServiceW);
DEFINE_GUARD(OpenSCManagerW);
DEFINE_GUARD(OpenServiceW);
DEFINE_GUARD(QueryServiceStatus);
DEFINE_GUARD(StartServiceW);
DEFINE_GUARD(DeleteService);
DEFINE_GUARD(RegCloseKey);
DEFINE_GUARD(RegCreateKeyW);
DEFINE_GUARD(RegOpenKeyW);
DEFINE_GUARD(RegSetValueExW);

/**
 * @brief Instances of the mock's static members
 */
DEFINE_MOCK(RegisterServiceCtrlHandlerW, SERVICE_STATUS_HANDLE, nullptr, ERROR_NOT_ENOUGH_MEMORY);
DEFINE_MOCK(SetServiceStatus, BOOL, FALSE, ERROR_INVALID_HANDLE);
DEFINE_MOCK(ControlService, BOOL, FALSE, ERROR_INVALID_HANDLE);
DEFINE_MOCK(CreateServiceW, SC_HANDLE, nullptr, ERROR_INVALID_PARAMETER);
DEFINE_MOCK(OpenSCManagerW, SC_HANDLE, nullptr, ERROR_INVALID_PARAMETER);
DEFINE_MOCK(OpenServiceW, SC_HANDLE, nullptr, ERROR_INVALID_PARAMETER);
DEFINE_MOCK(QueryServiceStatus, BOOL, FALSE, ERROR_INVALID_HANDLE);
DEFINE_MOCK(StartServiceW, BOOL, FALSE, ERROR_INVALID_HANDLE);
DEFINE_MOCK(DeleteService, BOOL, FALSE, ERROR_INVALID_HANDLE);
DEFINE_MOCK(RegCloseKey, LSTATUS, ERROR_INVALID_HANDLE, NO_ERROR);
DEFINE_MOCK(RegCreateKeyW, LSTATUS, ERROR_REGISTRY_CORRUPT, NO_ERROR);
DEFINE_MOCK(RegOpenKeyW, LSTATUS, ERROR_REGISTRY_IO_FAILED, NO_ERROR);
DEFINE_MOCK(RegCreateKeyExW, LSTATUS, ERROR_REGISTRY_CORRUPT, NO_ERROR);
DEFINE_MOCK(RegSetValueExW, LSTATUS, ERROR_REGISTRY_CORRUPT, NO_ERROR);


extern "C"
{

/*****************************************************************
 * @brief Mocked APIs for services
 *****************************************************************/

_Must_inspect_result_
FFMOCK_IMPORT
SERVICE_STATUS_HANDLE
WINAPI
RegisterServiceCtrlHandlerW(
    _In_ LPCWSTR            ServiceName,
    _In_ __callback
         LPHANDLER_FUNCTION HandlerProc
    ) try
{
    static Mocks::UMRegisterServiceCtrlHandlerW mock(AdvAPI32);
    return mock(ServiceName, HandlerProc);
}
catch(std::bad_alloc const&)
{
    return nullptr;
}

FFMOCK_IMPORT
BOOL
WINAPI
SetServiceStatus(
    _In_ SERVICE_STATUS_HANDLE ServiceHandle,
    _In_ LPSERVICE_STATUS      ServiceStatus
    ) try
{
    static Mocks::UMSetServiceStatus mock(AdvAPI32);
    return mock(ServiceHandle, ServiceStatus);
}
catch(std::bad_alloc const&)
{
    return FALSE;
}

FFMOCK_IMPORT
BOOL
WINAPI
ControlService(
    _In_        SC_HANDLE           ServiceHandle,
    _In_        DWORD               Control,
    _Out_       LPSERVICE_STATUS    ServiceStatus
    ) try
{
    static Mocks::UMControlService mock(AdvAPI32);
    return mock(ServiceHandle, Control, ServiceStatus);
}
catch(std::bad_alloc const&)
{
    return FALSE;
}

_Must_inspect_result_
FFMOCK_IMPORT
SC_HANDLE
WINAPI
CreateServiceW(
    _In_      SC_HANDLE SCManager,
    _In_      LPCWSTR   ServiceName,
    _In_opt_  LPCWSTR   DisplayName,
    _In_      DWORD     DesiredAccess,
    _In_      DWORD     ServiceType,
    _In_      DWORD     StartType,
    _In_      DWORD     ErrorControl,
    _In_opt_  LPCWSTR   BinaryPathName,
    _In_opt_  LPCWSTR   LoadOrderGroup,
    _Out_opt_ LPDWORD   TagId,
    _In_opt_  LPCWSTR   Dependencies,
    _In_opt_  LPCWSTR   ServiceStartName,
    _In_opt_  LPCWSTR   Password
    ) try
{
    static Mocks::UMCreateServiceW mock(AdvAPI32);
    return mock(SCManager, ServiceName, DisplayName,
                DesiredAccess, ServiceType, StartType,
                ErrorControl, BinaryPathName, LoadOrderGroup,
                TagId, Dependencies, ServiceStartName, Password);
}
catch(std::bad_alloc const&)
{
    return nullptr;
}

FFMOCK_IMPORT
BOOL
WINAPI
DeleteService(
    _In_ SC_HANDLE Service
    ) try
{
    static Mocks::UMDeleteService mock(AdvAPI32);
    return mock(Service);
}
catch(std::bad_alloc const&)
{
    return FALSE;
}

_Must_inspect_result_
FFMOCK_IMPORT
SC_HANDLE
WINAPI
OpenSCManagerW(
    _In_opt_ LPCWSTR MachineName,
    _In_opt_ LPCWSTR DatabaseName,
    _In_     DWORD   DesiredAccess
    ) try
{
    static Mocks::UMOpenSCManagerW mock(AdvAPI32);
    return mock(MachineName, DatabaseName,  DesiredAccess);
}
catch(std::bad_alloc const&)
{
    return nullptr;
}

_Must_inspect_result_
FFMOCK_IMPORT
SC_HANDLE
WINAPI
OpenServiceW(
    _In_ SC_HANDLE SCManager,
    _In_ LPCWSTR   ServiceName,
    _In_ DWORD     DesiredAccess
    ) try
{
    static Mocks::UMOpenServiceW mock(AdvAPI32);
    return mock(SCManager, ServiceName,  DesiredAccess);
}
catch(std::bad_alloc const&)
{
    return nullptr;
}

FFMOCK_IMPORT
BOOL
WINAPI
QueryServiceStatus(
    _In_  SC_HANDLE        Service,
    _Out_ LPSERVICE_STATUS ServiceStatus
    ) try
{
    static Mocks::UMQueryServiceStatus mock(AdvAPI32);
    return mock(Service, ServiceStatus);
}
catch(std::bad_alloc const&)
{
    return FALSE;
}

FFMOCK_IMPORT
BOOL
WINAPI
StartServiceW(
    _In_                                SC_HANDLE Service,
    _In_                                DWORD     NumServiceArgs,
    _In_reads_opt_(dwNumServiceArgs)
                                        LPCWSTR*  ServiceArgVectors
    ) try
{
    static Mocks::UMStartServiceW mock(AdvAPI32);
    return mock(Service, NumServiceArgs, ServiceArgVectors);
}
catch(std::bad_alloc const&)
{
    return FALSE;
}

BOOL
WINAPI
CloseServiceHandle(
    _In_ SC_HANDLE Service
    ) try
{
    static Mocks::UMCloseServiceHandle mock(AdvAPI32);
    return mock(Service);
}
catch(std::bad_alloc const&)
{
    return FALSE;
}


/*****************************************************************
 * @brief Mocked APIs for registry
 *****************************************************************/

FFMOCK_IMPORT
LSTATUS
APIENTRY
RegCloseKey(
    _In_ HKEY Key
    ) try
{
    static Mocks::UMRegCloseKey mock(AdvAPI32);
    return mock(Key);
}
catch(std::bad_alloc const&)
{
    return ERROR_OUTOFMEMORY;
}

FFMOCK_IMPORT
LSTATUS
APIENTRY
RegCreateKeyW(
    _In_     HKEY    Key,
    _In_opt_ LPCWSTR SubKey,
    _Out_    PHKEY   Result
    ) try
{
    static Mocks::UMRegCreateKeyW mock(AdvAPI32);
    return mock(Key, SubKey, Result);
}
catch(std::bad_alloc const&)
{
    return ERROR_OUTOFMEMORY;
}

FFMOCK_IMPORT
LSTATUS
APIENTRY
RegCreateKeyExW(
    _In_       HKEY    Key,
    _In_opt_   LPCWSTR SubKey,
    _Reserved_ DWORD Reserved,
    _In_opt_   LPWSTR Class,
    _In_       DWORD Options,
    _In_       REGSAM SamDesired,
    _In_opt_ CONST LPSECURITY_ATTRIBUTES SecurityAttributes,
    _Out_      PHKEY   Result,
    _Out_opt_  LPDWORD Disposition
    ) try
{
    static Mocks::UMRegCreateKeyExW mock(AdvAPI32);
    return mock(Key, SubKey, Reserved,
                Class, Options, SamDesired,
                SecurityAttributes, Result, Disposition);
}
catch(std::bad_alloc const&)
{
    return ERROR_OUTOFMEMORY;
}

LSTATUS
APIENTRY
RegOpenKeyW(
    _In_     HKEY    Key,
    _In_opt_ LPCWSTR SubKey,
    _Out_    PHKEY   Result
    ) try
{
    static Mocks::UMRegOpenKeyW mock(AdvAPI32);
    return mock(Key, SubKey, Result);
}
catch(std::bad_alloc const&)
{
    return ERROR_OUTOFMEMORY;
}

FFMOCK_IMPORT
LSTATUS
APIENTRY
RegSetValueExW(
    _In_       HKEY Key,
    _In_opt_   LPCWSTR ValueName,
    _Reserved_ DWORD Reserved,
    _In_       DWORD Type,
    _In_reads_bytes_opt_(DataCount) CONST BYTE* Data,
    _In_       DWORD DataCount
    ) try
{
    static Mocks::UMRegSetValueExW mock(AdvAPI32);
    return mock(Key, ValueName, Reserved, Type, Data, DataCount);
}
catch(std::bad_alloc const&)
{
    return ERROR_OUTOFMEMORY;
}

} // extern "C"
