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

#include <functional>
#include <minwindef.h>
#include <winerror.h>

namespace umock
{

/**
 * @brief Primary template
 */
template<typename T>
struct function_traits;

/**
 * @brief Template specialization for APIs
 */
template<typename RetType_t, typename... Args_t>
struct function_traits<RetType_t(Args_t...)>
{
    using Ret_t = RetType_t;
    using Sig_t = Ret_t(Args_t...);
    using Ptr_t = Ret_t(*)(Args_t...);
    using Api_t = std::function<Sig_t>;

#pragma warning(push)
#pragma warning(disable:4127) // conditional expression is constant
    /**
     * @brief Method which always fail returning hard coded return value
     *
     * @details Optionally, this method can also set the last error for the OS
     *
     * @tparam Error_k - The value to return indicating an error
     * @tparam Error2Set_k - Set this value as the last error
     *
     * @return Ret_t - Always Error_k
     */
    template<Ret_t Error_k, DWORD Error2Set_k>
    __declspec(noinline)
    static
    Ret_t AlwaysError(Args_t...)
    {
        if (Error2Set_k)
        {
            SetLastError(Error2Set_k);
        }
        return Error_k;
    }
#pragma warning(pop)
};

/**
 * @brief Template implementing the basic mocking functionality for Win32 APIs
 *
 * @tparam RetType - API return type
 * @tparam API - API signature type
 * @tparam Error - Error value to return as generic failure
 * @tparam Error2Set - Value to set as last error (optional)
 */
#pragma warning(disable:4251)
template<typename RetType, typename API, RetType Error, DWORD Error2Set = NO_ERROR>
class
UMOCK_IMPORT
Mock
{
protected:

    using Traits_t = function_traits<API>;
    using Ret_t = typename Traits_t::Ret_t;
    using Ptr_t = typename Traits_t::Ptr_t;
    using Api_t = typename Traits_t::Api_t;
    using Mock_t = Mock;

    static Api_t RealAPI;
    static Api_t MockAPI;
    static constexpr Ret_t Error_k = Error;

    /**
     * @brief Construct a new Mock object capturing the pointer to the real API call
     *
     * @param Module - Module handle of the DLL exporting the Win32 API
     * @param ApiName - Name of the exported API
     */
    Mock(HMODULE Module, const char* ApiName)
    {
        RealAPI = Ptr_t(GetProcAddress(Module, ApiName));
        if (!MockAPI)
        {
            MockAPI = RealAPI;
        }
    }

    /**
     * @brief Operator to make the mock callable
     *
     * @tparam Args_t - Arguments pack of the API
     *
     * @param Args - API arguments
     * @return Ret_t - Return value of the mock
     */
    template<typename... Args_t>
    Ret_t operator()(Args_t&... Args)
    {
        return MockAPI(Args...);
    }

public:

    /**
     * @brief Scoped guard object for setting and clearing the mock
     */
    class Guard
    {
    public:
        /**
         * @brief Construct the Guard object using custom mock
         *
         * @param[in] MockImpl - Lambda to implement the mocking action (default to
         *                       failing the API).
         *
         * @details Allow any custom action by passing a lambda. The lambda signature
         *          must match the API signature.
         * @example
         * @code {.cpp}
         * // Validate parameters passed into the API
         * umock::UMSetServiceStatus::Guard guard(
         *      [](_In_ SERVICE_STATUS_HANDLE ServiceHandle,
         *         _In_ LPSERVICE_STATUS      ServiceStatus) -> BOOL
         *      {
         *          EXPECT_TRUE(ServiceHandle == NULL);
         *          EXPECT_TRUE(ServiceStatus->dwWin32ExitCode == ERROR_NOT_ENOUGH_MEMORY);
         *          EXPECT_TRUE(ServiceStatus->dwCurrentState == SERVICE_STOPPED);
         *          return TRUE;
         *      });
         * @endcode
         *
         */
        Guard(Api_t&& MockImpl = &Traits_t::AlwaysError<Error_k, Error2Set>)
        {
            Mock_t::MockAPI = MockImpl;
        }

        /**
         * @brief Destroy the Guard object and restore the real API
         */
        ~Guard(void)
        {
            Mock_t::MockAPI = Mock_t::RealAPI;
        }
    };
};

} // namespace umock


/**
 * @brief Declaration of mocked Win32 API
 */
#define DECLARE_MOCK(API_NAME, RET_TYPE, RET_ERROR, LAST_ERROR, CALL_TYPE, CALL_ARGS)   \
class UM##API_NAME                                                                      \
    : public ::umock::Mock<RET_TYPE, decltype(::API_NAME), RET_ERROR, LAST_ERROR>       \
{                                                                                       \
    friend RET_TYPE                                                                     \
    CALL_TYPE                                                                           \
    ::API_NAME CALL_ARGS;                                                               \
    UM##API_NAME(HMODULE Module) : Mock_t(Module, #API_NAME) {}                         \
}

/**
 * @brief Instances of the mock's static members
 */
#define DEFINE_MOCK(API_NAME, RET_TYPE, RET_ERROR, LAST_ERROR)                          \
template<>                                                                              \
typename ::umock::Mock<RET_TYPE, decltype(::API_NAME), RET_ERROR, LAST_ERROR>::Api_t    \
    umock::Mock<RET_TYPE, decltype(::API_NAME), RET_ERROR, LAST_ERROR>::RealAPI;        \
template<>                                                                              \
typename ::umock::Mock<RET_TYPE, decltype(::API_NAME), RET_ERROR, LAST_ERROR>::Api_t    \
    umock::Mock<RET_TYPE, decltype(::API_NAME), RET_ERROR, LAST_ERROR>::MockAPI;
