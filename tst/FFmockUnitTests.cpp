/**
  @brief ffmock unit tests
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

#include <gtest/gtest.h>
#include <Service.hpp>
#include <Registry.hpp>
#include <psapi.h>
#include <winuser.h>
#include <thread>
#include <chrono>
#include "Mocks.hpp"

/******************************************************
 * @brief Service class unit tests
 *
 * @todo Full functional and branch coverage
 ******************************************************/
class ServiceTestSuite : public testing::Test
                       , public Service
{
protected:
};

TEST_F(ServiceTestSuite, Test_ServiceMain_Failed)
{
    wchar_t* argv[] = {L"UnitTest"};
    Mocks::FFRegisterServiceCtrlHandlerW::Guard rschGuard;
    Mocks::FFSetServiceStatus::Guard sssGuard(
        [](_In_ SERVICE_STATUS_HANDLE ServiceHandle,
           _In_ LPSERVICE_STATUS      ServiceStatus) -> BOOL
        {
            EXPECT_EQ(ServiceHandle, nullptr);
            EXPECT_TRUE(ServiceStatus->dwWin32ExitCode == ERROR_NOT_ENOUGH_MEMORY);
            EXPECT_TRUE(ServiceStatus->dwCurrentState == SERVICE_STOPPED);
            return TRUE;
        });

    Service::ServiceMain(_countof(argv), argv);

    ASSERT_FALSE(SvcStatusHandle);
}

TEST_F(ServiceTestSuite, Test_Register_Success)
{
    char cmdLine[]{"FFmockSvc"};
    Mocks::FFRegisterServiceCtrlHandlerW::Guard guard;

    Service::Register(nullptr, nullptr, cmdLine, SW_NORMAL);

    ASSERT_FALSE(SvcStatusHandle);
}

/******************************************************
 * @brief Registry class unit tests
 *
 * @todo Full functional and branch coverage
 ******************************************************/
class RegistryTestSuite : public testing::Test
                        , public Registry
{
protected:
    void SetUp(void) override
    {
        RegDeleteTreeW(HKEY_LOCAL_MACHINE, L"Software\\_DeleteMe_");
        RegDeleteKeyW(HKEY_LOCAL_MACHINE, L"Software\\_DeleteMe_");
    }

    void TearDown(void) override
    {
        SetUp();
    }
};

TEST_F(RegistryTestSuite, Test_Open_Fail)
{
    ASSERT_FALSE(Open(L"Software\\_DeleteMe_"));
    ASSERT_FALSE(Key);
}

TEST_F(RegistryTestSuite, Test_Open_Success)
{
    ASSERT_TRUE(Open(L"Software\\Microsoft"));
    ASSERT_TRUE(Key);
}

TEST_F(RegistryTestSuite, Test_Create)
{
    ASSERT_TRUE(Create(L"Software\\_DeleteMe_"));
}

/**
 * @brief Uni tests entrypoint
 *
 * @param argc - Count of command line arguments
 * @param argv - Command line arguments strings
 *
 * @return int - 0 if successful
 */
int wmain(_In_ int argc, _In_ wchar_t* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}