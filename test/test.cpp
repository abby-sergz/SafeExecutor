// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <SafeExecutor/SafeExecutor.hpp>
#include <gtest/gtest.h>

using std::cerr;
using std::endl;

namespace
{ // log
  void log(const std::string& value)
  {
    cerr << value;
  }

  void logln(const std::string& value)
  {
    cerr << value << endl;
  }
}

namespace
{
  class X
  {
  public:
    X()
    {
      logln("X::ctr");
    }
    ~X()
    {
      logln("X::dtr");
    }
  };
}

namespace
{
  int HWUnsafe()
  {
    ::X x;
    // Debug  : dtr is not called
    // Release: dtr is     called
    int z = 0;
    return 1/z;
  }

  TEST(HWUnsafe, SafeExecutor)
  {
    SafeExecutorNS::SafeExecutor se(HWUnsafe);
    ASSERT_FALSE(se.Do());
  }
}

namespace
{
  int SWUnsafe1()
  {
    X x;
    // Debug  : dtr is     called
    // Release: dtr is     called
    int z = 1;
    throw std::exception();
    return 1/z;
  }

  TEST(SW_std_ex, SafeExecutor)
  {
    SafeExecutorNS::SafeExecutor se(SWUnsafe1);
    ASSERT_FALSE(se.Do());
  }
}

namespace
{
  int SWUnsafe2()
  {
    X x;
    // Debug  : dtr is not called
    // Release: dtr is not called
    int z = 1;
    throw;
    return 1/z;
  }

  TEST(SW_empty_ex, SafeExecutor)
  {
    SafeExecutorNS::SafeExecutor se(SWUnsafe2);
    ASSERT_FALSE(se.Do());
  }
}

namespace
{
  int SWUnsafe3()
  {
    X x;
    // Debug  : dtr is     called
    // Release: dtr is     called
    int z = 1;
    throw 1;
    return 1/z;
  }

  TEST(SW_int_ex, SafeExecutor)
  {
    SafeExecutorNS::SafeExecutor se(SWUnsafe3);
    ASSERT_FALSE(se.Do());
  }
}

#include <windows.h>

LONG CALLBACK veh(PEXCEPTION_POINTERS p)
{
  return EXCEPTION_CONTINUE_SEARCH;
}

int _tmain(int argc, _TCHAR* argv[])
{
  PVOID h = AddVectoredExceptionHandler(1, veh);
  testing::InitGoogleTest(&argc, argv);
  int res = RUN_ALL_TESTS();
  RemoveVectoredExceptionHandler(h);
  return res;
}

