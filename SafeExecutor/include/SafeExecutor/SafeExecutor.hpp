#pragma once

#include <boost/function.hpp>

namespace SafeExecutorNS
{
  struct SafeExecutor
  {
    typedef boost::function<void()> TDoDelegate;
    SafeExecutor(TDoDelegate doDelegate);
    // true - the everything is successful
    // false - othervise
    bool Do();
  private:
    bool DoCPlusPlusExceptionWrapper();
    bool DoWorkWrapper();
  private:
    TDoDelegate m_DoDelegate;
  };
}
