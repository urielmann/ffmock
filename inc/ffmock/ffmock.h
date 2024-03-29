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

#include <functional>
#include <minwindef.h>
#include <winerror.h>
#include <libloaderapi.h>

namespace ffmock
{

/**
 * @brief Primary template
 */
template<typename T>
struct function_traits;

/**
 * @brief Template specialization for __stdcall APIs
 *
 * @tparam RetType_t - Free function return type
 * @tparam Args_t - Free function arguments pack
 */
template<typename RetType_t, typename... Args_t>
struct function_traits<RetType_t __stdcall(Args_t...)>
{
    //! @brief Return type of mocked free function
    using Ret_t = RetType_t;
    //! @brief Free function (API) signature
    using Sig_t = Ret_t __stdcall(Args_t...);
    //! @brief Free function (API) pointer
    using Ptr_t = Ret_t(__stdcall*)(Args_t...);
    //! @brief Functor declaration for the API
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

#if !defined(WIN64)
/**
 * @brief Template specialization for __cdecl APIs
 *
 * @tparam RetType_t - Free function return type
 * @tparam Args_t - Free function arguments pack
 */
template<typename RetType_t, typename... Args_t>
struct function_traits<RetType_t __cdecl(Args_t...)>
{
    //! @brief Return type of mocked free function
    using Ret_t = RetType_t;
    //! @brief Free function (API) signature
    using Sig_t = Ret_t __stdcall(Args_t...);
    //! @brief Free function (API) pointer
    using Ptr_t = Ret_t(__stdcall*)(Args_t...);
    //! @brief Functor declaration for the API
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
#endif // !defined(WIN64)

/**
 * @brief Template implementing the basic mocking functionality for Win32 APIs
 *
 * @tparam RetType_t - API return type
 * @tparam API_t - API signature type
 * @tparam RetValue - Error value to return as generic failure
 * @tparam Error2Set - Value to set as last error (optional)
 */
template<typename RetType_t, typename API_t, RetType_t RetValue, DWORD Error2Set = NO_ERROR>
class
Mock
{
protected:

    //! @brief API traits specialization
    using Traits_t = function_traits<API_t>;
    //! @brief Return type of mocked free function
    using Ret_t = typename Traits_t::Ret_t;
    //! @brief Free function (API) signature
    using Sig_t = typename Traits_t::Sig_t;
    //! @brief Free function (API) pointer
    using Ptr_t = typename Traits_t::Ptr_t;
    //! @brief Functor declaration for the API
    using Api_t = typename Traits_t::Api_t;
    //! @brief Type declaration for this template
    using Mock_t = Mock;

    FFMOCK_IMPORT
    static Api_t RealAPI;
    FFMOCK_IMPORT
    static Api_t MockAPI;
    static constexpr Ret_t Error_k = RetValue;

    /**
     * @brief Construct a new Mock object capturing the pointer to the real API call
     *
     * @param Module - Module handle of the DLL exporting the Win32 API
     * @param ApiName - Name of the exported API
     */
    Mock(HMODULE Module, const char* ApiName)
    {
        RealAPI = Ptr_t(GetProcAddress(Module, ApiName));
        _ASSERT(RealAPI);
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
         * ffmock::FFSetServiceStatus::Guard guard(
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
        FFMOCK_IMPORT                                                                                   \
        Guard(Api_t&& MockImpl = &Traits_t::AlwaysError<RetValue, Error2Set>);

        /**
         * @brief Destroy the Guard object and restore the real API
         */
        FFMOCK_IMPORT                                                                                   \
        ~Guard(void);

        /**
         * @brief Set object
         * 
         * @param MockImpl - See the constructor above for details
         */
        FFMOCK_IMPORT                                                                                   \
        void Set(const Api_t& MockImpl = &Traits_t::AlwaysError<RetValue, Error2Set>);

        /**
         * @brief Clear the Guard object and restore the real API
         */
        FFMOCK_IMPORT                                                                                   \
        void Clear(void);
    };
};

} // namespace ffmock


/**
 * @brief Declaration of mocked Win32 API
 *
 * @param API_NAME - The API being mocked
 * @param RET_TYPE - Return type of the API
 * @param RET_ERROR - Default value to return when the API fails
 * @param LAST_ERROR - Win32 API commonly set last error code to be retrieved by
 *                     GetLastError(). This is always used for functions returning
 *                     BOOL and the return value is set to FALSE.
 * @param CALL_TYPE - Function calling convention
 * @param CALL_ARGS - Parenthesize list of API arguments
 */
#define DECLARE_MOCK(API_NAME, RET_TYPE, RET_ERROR, LAST_ERROR, CALL_TYPE, CALL_ARGS)   \
class FF##API_NAME                                                                      \
    : public ::ffmock::Mock<RET_TYPE, decltype(::API_NAME), RET_ERROR, LAST_ERROR>      \
{                                                                                       \
    friend                                                                              \
    FFMOCK_IMPORT                                                                       \
    RET_TYPE                                                                            \
    CALL_TYPE                                                                           \
    ::API_NAME CALL_ARGS;                                                               \
    FF##API_NAME(HMODULE Module) : Mock_t(Module, #API_NAME) {}                         \
}

/**
 * @brief Instances of the mock's static members
 *
 * @param API_NAME - The API being mocked
 * @param RET_TYPE - Return type of the API
 * @param RET_ERROR - Default value to return when the API fails
 * @param LAST_ERROR - Win32 API commonly set last error code to be retrieved by
 *                     GetLastError(). This is always used for functions returning
 *                     BOOL and the return value is set to FALSE.
 */
#define DEFINE_MOCK(API_NAME, RET_TYPE, RET_ERROR, LAST_ERROR)                          \
template<>                                                                              \
FFMOCK_IMPORT                                                                           \
typename ::ffmock::Mock<RET_TYPE, decltype(::API_NAME), RET_ERROR, LAST_ERROR>::Api_t   \
    ffmock::Mock<RET_TYPE, decltype(::API_NAME), RET_ERROR, LAST_ERROR>::RealAPI;       \
template<>                                                                              \
FFMOCK_IMPORT                                                                           \
typename ::ffmock::Mock<RET_TYPE, decltype(::API_NAME), RET_ERROR, LAST_ERROR>::Api_t   \
    ffmock::Mock<RET_TYPE, decltype(::API_NAME), RET_ERROR, LAST_ERROR>::MockAPI

/**
 * @brief Instances of the mock's Guard members
 *
 * @param NAME_SPACE - Optional namespace of the mocked class
 * @param API_NAME - The API being mocked
 *
 * @warning The Guard class members must be linked into the same binary as the mocks.
 *          It is very important that the same source file has both the mock as well
 *          as the Guard definitions.
 * @example
 * @code {.cpp}
 * // Define Guard methods, and mock class static members
 * DEFINE_GUARD(Mocks, RegOpenKeyW);
 * DEFINE_MOCK(RegOpenKeyW, LSTATUS, ERROR_REGISTRY_IO_FAILED, NO_ERROR);
 *
 * // Mock API definition
 * FFMOCK_IMPORT
 * LSTATUS
 * APIENTRY
 * RegOpenKeyW(
 *     _In_     HKEY    Key,
 *     _In_opt_ LPCWSTR SubKey,
 *     _Out_    PHKEY   Result
 *     ) try
 * {
 *     static Mocks::FFRegOpenKeyW mock(AdvAPI32);
 *     return mock(Key, SubKey, Result);
 * }
 * catch(std::bad_alloc const&)
 * {
 *     return ERROR_OUTOFMEMORY;
 * }
 * @endcode
 */
#define DEFINE_GUARD(NAME_SPACE, API_NAME)                          \
FFMOCK_IMPORT                                                       \
NAME_SPACE::FF##API_NAME::Guard::Guard(Api_t&& MockImpl)            \
{                                                                   \
    Set(std::forward<Api_t>(MockImpl));                             \
}                                                                   \
FFMOCK_IMPORT                                                       \
NAME_SPACE::FF##API_NAME::Guard::~Guard(void)                       \
{                                                                   \
    Clear();                                                        \
}                                                                   \
template <>                                                         \
FFMOCK_IMPORT                                                       \
void NAME_SPACE::FF##API_NAME::Guard::Set(const Api_t& MockImpl)    \
{                                                                   \
    _ASSERT(MockImpl);                                              \
    MockAPI = MockImpl;                                             \
}                                                                   \
template <>                                                         \
FFMOCK_IMPORT                                                       \
void NAME_SPACE::FF##API_NAME::Guard::Clear(void)                   \
{                                                                   \
    MockAPI = RealAPI;                                              \
}
