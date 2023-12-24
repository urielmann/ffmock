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

#include "Mocks.hpp"

#pragma warning(disable:4273) // inconsistent dll linkage

static HMODULE AdvAPI32{LoadLibraryW(L"advapi32.dll")};

/**
 * @brief Instances of the mock's Guard class members
 */
DEFINE_GUARD(Mocks, RegCloseKey);
DEFINE_GUARD(Mocks, RegCreateKeyW);
DEFINE_GUARD(Mocks, RegCreateKeyExW);
DEFINE_GUARD(Mocks, RegDeleteValueW);
DEFINE_GUARD(Mocks, RegOpenKeyW);
DEFINE_GUARD(Mocks, RegSetValueExW);

/**
 * @brief Instances of the mock's static members
 */
DEFINE_MOCK(RegCloseKey, LSTATUS, ERROR_INVALID_HANDLE, NO_ERROR);
DEFINE_MOCK(RegCreateKeyW, LSTATUS, ERROR_REGISTRY_CORRUPT, NO_ERROR);
DEFINE_MOCK(RegCreateKeyExW, LSTATUS, ERROR_REGISTRY_CORRUPT, NO_ERROR);
DEFINE_MOCK(RegDeleteValueW, LSTATUS, ERROR_REGISTRY_CORRUPT, NO_ERROR);
DEFINE_MOCK(RegOpenKeyW, LSTATUS, ERROR_REGISTRY_IO_FAILED, NO_ERROR);
DEFINE_MOCK(RegSetValueExW, LSTATUS, ERROR_REGISTRY_CORRUPT, NO_ERROR);


extern "C"
{

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
    static Mocks::FFRegCloseKey mock(AdvAPI32);
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
    static Mocks::FFRegCreateKeyW mock(AdvAPI32);
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
    static Mocks::FFRegCreateKeyExW mock(AdvAPI32);
    return mock(Key, SubKey, Reserved,
                Class, Options, SamDesired,
                SecurityAttributes, Result, Disposition);
}
catch(std::bad_alloc const&)
{
    return ERROR_OUTOFMEMORY;
}

FFMOCK_IMPORT
LSTATUS
APIENTRY
RegDeleteValueW(
    _In_     HKEY    Key,
    _In_opt_ LPCWSTR ValueName
    ) try
{
    static Mocks::FFRegDeleteValueW mock(AdvAPI32);
    return mock(Key, ValueName);
}
catch(std::bad_alloc const&)
{
    return ERROR_OUTOFMEMORY;
}

FFMOCK_IMPORT
LSTATUS
APIENTRY
RegOpenKeyW(
    _In_     HKEY    Key,
    _In_opt_ LPCWSTR SubKey,
    _Out_    PHKEY   Result
    ) try
{
    static Mocks::FFRegOpenKeyW mock(AdvAPI32);
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
    static Mocks::FFRegSetValueExW mock(AdvAPI32);
    return mock(Key, ValueName, Reserved, Type, Data, DataCount);
}
catch(std::bad_alloc const&)
{
    return ERROR_OUTOFMEMORY;
}

} // extern "C"
