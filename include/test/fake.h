#pragma once

#include <test/fff.h>

// Reset fakes between tests. Called by `local_runner` after each test.
void reset_fakes();


/**
  Declare fakes here by adding a declaration macro, and adding them to the
  APPLY_FAKES_LIST macro. Then, define them with DEFINE_FAKE_* macros in fake.c.

  Semantics: FAKE_VOID_FUNC(name, arguments...)
             FAKE_VALUE_FUNC(return_type, name, arguments...)
*/

#define APPLY_FAKES_LIST(FN) \
  FN(__wrap_getutime)

DECLARE_FAKE_VALUE_FUNC(status, __wrap_getutime, uint32 *); 
