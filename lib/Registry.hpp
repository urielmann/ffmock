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

#pragma once

#include <Common.hpp>
#include <winreg.h>

/**
 * @brief Class implementing Windows registry functionality
 */
class Registry
{
    using HANDLE_t = std::unique_ptr<std::remove_pointer<HKEY>::type, decltype(&RegCloseKey)>;

    /**
     * @brief Allow access to privets by the unit tests
     */
protected:

    HANDLE_t Key{nullptr, &RegCloseKey};

public:
    Registry(void) = default;
    ~Registry(void) = default;

    Registry(Registry const&) = delete;
    Registry(Registry&&) = delete;
    Registry& operator=(Registry const&) = delete;

    bool Open(_In_z_ PCWSTR KeyPath);
    bool Create(_In_z_ PCWSTR KeyPath);
    bool AddStringValue(_In_z_ PCWSTR Name, _In_z_ PCWSTR Value, _In_ DWORD Type);
};
