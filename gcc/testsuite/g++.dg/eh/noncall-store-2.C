// A direct store through a reference parameter can throw a non-call
// exception.

// { dg-do compile }
// { dg-options "-O2 -fnon-call-exceptions -fdump-rtl-expand" }

void
store_ref (int &value)
{
  try
    {
      value = 1;
    }
  catch (...)
    {
      __builtin_trap ();
    }
}

// { dg-final { scan-rtl-dump-times {REG_EH_REGION \(const_int [1-9][0-9]*} 1 "expand" } }
