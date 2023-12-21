/**
  @brief Windows Registry utility class
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

#include "Registry.hpp"
#include <iostream>

/**
 * @brief Open registry key
 *
 * @param[in] KeyPath - Key path
 *
 * @return true if successful
 */
bool Registry::Open(_In_z_ PCWSTR KeyPath)
{
    HKEY key{};
    LSTATUS status{RegOpenKeyW(HKEY_LOCAL_MACHINE, KeyPath, &key)};
    if (status)
    {
        UserErrorMessage(L"RegOpenKeyW() failure");
        return false;
    }
    Key.reset(key);
    key = nullptr;

    return true;
}

/**
 * @brief Create new registry key
 *
 * @param[in] KeyPath - Key path
 *
 * @return true if successful
 */
bool Registry::Create(_In_z_ PCWSTR KeyPath)
{
    HKEY key{};
    LSTATUS status{RegCreateKeyW(HKEY_LOCAL_MACHINE, KeyPath, &key)};
    if (status)
    {
        UserErrorMessage(L"RegCreateKeyW() failure");
        return false;
    }
    Key.reset(key);
    key = nullptr;

    return true;
}

/**
 * @brief Add a string value to a key
 *
 * @param[in] Name - Value name
 * @param[in] Value - String value
 * @param[in] Type - String type
 *
 * @return true if successful
 */
bool Registry::AddStringValue(_In_z_ PCWSTR Name, _In_z_ PCWSTR Value, _In_ DWORD Type)
{
    LSTATUS status{RegSetValueExW(Key.get(),
                                  Name,
                                  0,
                                  Type,
                                  reinterpret_cast<const BYTE*>(Value),
                                  static_cast<DWORD>(wcslen(Value) * sizeof(*Value)))};
    if (status)
    {
        UserErrorMessage(L"RegSetValueExW() failure");
        return false;
    }

    return true;
}
