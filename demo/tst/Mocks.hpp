/**
  @brief Win32 API mocks
  @author Uriel Mann
  @copyright 2023-2024 Uriel Mann (abba.mann@gmail.com)

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
 * @brief Mocked APIs for registry
 *****************************************************************/

/**
 * @brief Mock for RegCloseKey
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regclosekey
 */
class FFRegCloseKey
    : public ::ffmock::Mock<LSTATUS, decltype(::RegCloseKey), ERROR_INVALID_HANDLE>
{
    friend
    FFMOCK_IMPORT
    LSTATUS
    APIENTRY
    ::RegCloseKey(
        _In_ HKEY hKey
        );

    FFRegCloseKey(HMODULE Module) : Mock_t(Module, "RegCloseKey")
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
 * @brief Mock for RegCreateKeyExW
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regcreatekeyexw
 */
DECLARE_MOCK(RegCreateKeyExW, LSTATUS, ERROR_REGISTRY_CORRUPT, NO_ERROR, APIENTRY,
    (
    _In_       HKEY    Key,
    _In_opt_   LPCWSTR SubKey,
    _Reserved_ DWORD Reserved,
    _In_opt_   LPWSTR Class,
    _In_       DWORD Options,
    _In_       REGSAM SamDesired,
    _In_opt_ CONST LPSECURITY_ATTRIBUTES SecurityAttributes,
    _Out_      PHKEY   Result,
    _Out_opt_  LPDWORD Disposition
    ));

/**
 * @brief Mock for RegDeleteValueW
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regdeletevaluew
 */
DECLARE_MOCK(RegDeleteValueW, LSTATUS, ERROR_REGISTRY_CORRUPT, NO_ERROR, APIENTRY,
    (
    _In_     HKEY    Key,
    _In_opt_ LPCWSTR ValueName
    ));

/**
 * @brief Mock for RegOpenKeyW
 * @see https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regopenkeyw
 */
DECLARE_MOCK(RegOpenKeyW, LSTATUS, ERROR_REGISTRY_IO_FAILED, NO_ERROR, APIENTRY,
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

} // namespace Mocks
