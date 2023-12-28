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

#pragma once

#include <ffmock/ffmock.h>
#include <Common.hpp>
#include <winsvc.h>
#include <winreg.h>


namespace Mocks
{

/*****************************************************************
 * @brief Mocked APIs for services
 *****************************************************************/

/**
 * @brief Mock for RegisterServiceCtrlHandlerW
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-registerservicectrlhandlerw
 */
class UMRegisterServiceCtrlHandlerW
    : public ::ffmock::Mock<SERVICE_STATUS_HANDLE, decltype(::RegisterServiceCtrlHandlerW), nullptr, ERROR_NOT_ENOUGH_MEMORY>
{
    friend SERVICE_STATUS_HANDLE
    WINAPI
    ::RegisterServiceCtrlHandlerW(
        _In_ LPCWSTR            ServiceName,
        _In_ __callback
             LPHANDLER_FUNCTION HandlerProc
        );

    UMRegisterServiceCtrlHandlerW(HMODULE Module) : Mock_t(Module, "RegisterServiceCtrlHandlerW")
    {
    }
};

/**
 * @brief Mock for SetServiceStatus
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-setservicestatus
 */
class UMSetServiceStatus
    : public ::ffmock::Mock<BOOL, decltype(::SetServiceStatus), FALSE, ERROR_INVALID_HANDLE>
{
    friend BOOL
    WINAPI
    ::SetServiceStatus(
        _In_ SERVICE_STATUS_HANDLE ServiceHandle,
        _In_ LPSERVICE_STATUS      ServiceStatus
        );

    UMSetServiceStatus(HMODULE Module) : Mock_t(Module, "SetServiceStatus")
    {
    }
};

/**
 * @brief Mock for ControlService
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-controlservice
 */
class UMControlService
    : public ::ffmock::Mock<BOOL, decltype(::ControlService), FALSE, ERROR_INVALID_HANDLE>
{
    friend BOOL
    WINAPI
    ::ControlService(
        _In_  SC_HANDLE        ServiceHandle,
        _In_  DWORD            Control,
        _Out_ LPSERVICE_STATUS ServiceStatus
        );

    UMControlService(HMODULE Module) : Mock_t(Module, "ControlService")
    {
    }
};

/**
 * @brief Mock for CreateServiceW
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-createservicew
 */
DECLARE_MOCK(CreateServiceW, SC_HANDLE, nullptr, ERROR_INVALID_PARAMETER, WINAPI,
    (
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
    ));

/**
 * @brief Mock for DeleteService
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-deleteservice
 */
DECLARE_MOCK(DeleteService, BOOL, FALSE, ERROR_INVALID_HANDLE, WINAPI,
    (
    _In_ SC_HANDLE Service
    ));

/**
 * @brief Mock for OpenSCManagerW
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-openscmanagerw
 */
DECLARE_MOCK(OpenSCManagerW, SC_HANDLE, nullptr, ERROR_INVALID_PARAMETER, WINAPI,
    (
    _In_opt_ LPCWSTR MachineName,
    _In_opt_ LPCWSTR DatabaseName,
    _In_     DWORD   DesiredAccess
    ));

/**
 * @brief Mock for OpenServiceW
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-openservicew
 */
DECLARE_MOCK(OpenServiceW, SC_HANDLE, nullptr, ERROR_INVALID_PARAMETER, WINAPI,
    (
    _In_ SC_HANDLE SCManager,
    _In_ LPCWSTR   ServiceName,
    _In_ DWORD     DesiredAccess
    ));

/**
 * @brief Mock for QueryServiceStatus
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-queryservicestatus
 */
DECLARE_MOCK(QueryServiceStatus, BOOL, FALSE, ERROR_INVALID_HANDLE, WINAPI,
    (
    _In_  SC_HANDLE        Service,
    _Out_ LPSERVICE_STATUS ServiceStatus
    ));

/**
 * @brief Mock for StartServiceW
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-startservicew
 */
DECLARE_MOCK(StartServiceW, BOOL, FALSE, ERROR_INVALID_HANDLE, WINAPI,
    (
    _In_                                SC_HANDLE Service,
    _In_                                DWORD     NumServiceArgs,
    _In_reads_opt_(dwNumServiceArgs)
                                        LPCWSTR*  ServiceArgVectors
    ));


/*****************************************************************
 * @brief Mocked APIs for registry
 *****************************************************************/

/**
 * @brief Mock for RegCloseKey
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regclosekey
 */
class UMRegCloseKey
    : public ::ffmock::Mock<LSTATUS, decltype(::RegCloseKey), ERROR_INVALID_HANDLE>
{
    friend LSTATUS
    APIENTRY
    ::RegCloseKey(
        _In_ HKEY hKey
        );

    UMRegCloseKey(HMODULE Module) : Mock_t(Module, "RegCloseKey")
    {
    }
};

/**
 * @brief Mock for RegCreateKeyW
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regcreatekeyw
 */
DECLARE_MOCK(RegCreateKeyW, LSTATUS, ERROR_REGISTRY_CORRUPT, NO_ERROR, APIENTRY,
    (
    _In_     HKEY    Key,
    _In_opt_ LPCWSTR SubKey,
    _Out_    PHKEY   Result
    ));

/**
 * @brief Mock for RegOpenKeyW
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regopenkeyw
 */
DECLARE_MOCK(RegOpenKeyW, LSTATUS, ERROR_REGISTRY_CORRUPT, NO_ERROR, APIENTRY,
    (
    _In_     HKEY    Key,
    _In_opt_ LPCWSTR SubKey,
    _Out_    PHKEY   Result
    ));

/**
 * @brief Mock for RegSetValueExW
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regsetvalueexw
 */
DECLARE_MOCK(RegSetValueExW, LSTATUS, ERROR_REGISTRY_CORRUPT, NO_ERROR, APIENTRY,
    (
    _In_       HKEY Key,
    _In_opt_   LPCWSTR ValueName,
    _Reserved_ DWORD Reserved,
    _In_       DWORD Type,
    _In_reads_bytes_opt_(DataCount) CONST BYTE* Data,
    _In_       DWORD DataCount
    ));

#if 0
/*****************************************************************
 * @brief Mocked APIs for Event Log
 *****************************************************************/

// RegisterEventSourceW
// ReportEventW
#endif

} // namespace Mocks
