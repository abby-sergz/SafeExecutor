#include <SafeExecutor/SafeExecutor.hpp>
#include <sstream>
#include <iomanip>
#include <windows.h>

namespace
{
  const char* GetExceptionDescription(DWORD code)
  {
    const char* ret = "Unknown description";
    switch (code)
    {
    case EXCEPTION_ACCESS_VIOLATION         : ret = "EXCEPTION_ACCESS_VIOLATION"         ;break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED    : ret = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED"    ;break;
    case EXCEPTION_BREAKPOINT               : ret = "EXCEPTION_BREAKPOINT"               ;break;
    case EXCEPTION_DATATYPE_MISALIGNMENT    : ret = "EXCEPTION_DATATYPE_MISALIGNMENT"    ;break;
    case EXCEPTION_FLT_DENORMAL_OPERAND     : ret = "EXCEPTION_FLT_DENORMAL_OPERAND"     ;break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO       : ret = "EXCEPTION_FLT_DIVIDE_BY_ZERO"       ;break;
    case EXCEPTION_FLT_INEXACT_RESULT       : ret = "EXCEPTION_FLT_INEXACT_RESULT"       ;break;
    case EXCEPTION_FLT_INVALID_OPERATION    : ret = "EXCEPTION_FLT_INVALID_OPERATION"    ;break;
    case EXCEPTION_FLT_OVERFLOW             : ret = "EXCEPTION_FLT_OVERFLOW"             ;break;
    case EXCEPTION_FLT_STACK_CHECK          : ret = "EXCEPTION_FLT_STACK_CHECK"          ;break;
    case EXCEPTION_FLT_UNDERFLOW            : ret = "EXCEPTION_FLT_UNDERFLOW"            ;break;
    case EXCEPTION_ILLEGAL_INSTRUCTION      : ret = "EXCEPTION_ILLEGAL_INSTRUCTION"      ;break;
    case EXCEPTION_IN_PAGE_ERROR            : ret = "EXCEPTION_IN_PAGE_ERROR"            ;break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO       : ret = "EXCEPTION_INT_DIVIDE_BY_ZERO"       ;break;
    case EXCEPTION_INT_OVERFLOW             : ret = "EXCEPTION_INT_OVERFLOW"             ;break;
    case EXCEPTION_INVALID_DISPOSITION      : ret = "EXCEPTION_INVALID_DISPOSITION"      ;break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION : ret = "EXCEPTION_NONCONTINUABLE_EXCEPTION" ;break;
    case EXCEPTION_PRIV_INSTRUCTION         : ret = "EXCEPTION_PRIV_INSTRUCTION"         ;break;
    case EXCEPTION_SINGLE_STEP              : ret = "EXCEPTION_SINGLE_STEP"              ;break;
    case EXCEPTION_STACK_OVERFLOW           : ret = "EXCEPTION_STACK_OVERFLOW"           ;break;
    default: ;
    }
    return ret;
  }

  void dump(std::ostream& os, const PEXCEPTION_POINTERS pep)
  {
    os << "ExceptionCode = " << std::setbase(16) << std::setiosflags(std::ios::uppercase) << std::setfill('0') << pep->ExceptionRecord->ExceptionCode << " ";
    os << "(" << GetExceptionDescription(pep->ExceptionRecord->ExceptionCode) << ")";
    os << "\n";
    os << "ExceptionAddress = " << pep->ExceptionRecord->ExceptionAddress << " ";
  }
}

namespace SafeExecutorNS
{
  LONG Filter( PEXCEPTION_POINTERS pep )
  {
    std::stringstream ss;
    dump(ss, pep);
    std::string str = ss.str();

    // GetModules();
    // GetCallStack();

    return EXCEPTION_CONTINUE_SEARCH;
  }
}


using namespace SafeExecutorNS;


SafeExecutor::SafeExecutor(TDoDelegate doDelegate)
: m_DoDelegate(doDelegate)
{
}

bool
SafeExecutor::Do()
{
  bool abnornal_termination = false;
  bool IsExecSuccessful = true;
  {
    __try
    {
      IsExecSuccessful = DoCPlusPlusExceptionWrapper();
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
      abnornal_termination = true;
    }
  }
  return !abnornal_termination && IsExecSuccessful;
}

bool
SafeExecutor::DoCPlusPlusExceptionWrapper()
{
  bool res = true;
  try
  {
    res = DoWorkWrapper();
  }
  catch(std::exception& /*ex*/)
  {
    // smth like log(ex.what());
    //assert(false);
    res = false;
  }
  catch(...)
  {
    // smth like log("unknown sw-exception);
    //assert(false);
    res = false;
  }
  return res;
}

bool
SafeExecutor::DoWorkWrapper()
{
  bool res = false;
  if (!m_DoDelegate.empty())
  {
    __try
    {
      m_DoDelegate();
      res = true;
    } __except(SafeExecutorNS::Filter(GetExceptionInformation()))  // we must dump stack inside this Filter
    {
      // never be executed because SafeExecutorNS::Filter always returns `CONTINUE_SEARCH`
    }
  }
  return res;
}

