#pragma once

#include <test/fff.h>

#include <test/fakes.def>

// Reset fakes between tests. Called by `local_runner` after each test.
void reset_fakes();

/**
  Declare fakes here by adding a declaration macro, and adding them to the
  APPLY_FAKES_LIST macro. Then, define them with DEFINE_FAKE_* macros in fake.c.

  Semantics: FAKE_VOID_FUNC(name, arguments...)
             FAKE_VALUE_FUNC(return_type, name, arguments...)
*/


#define APPLY_FAKES_LIST(FN) \
  FAKE_VALUE_FUNCTION_LIST(FN) \
  FAKE_VOID_FUNCTION_LIST(FN)

#define RESET_VALUE_FAKE(TYPE, NAME, ...) \
  RESET_FAKE(NAME)
#define RESET_VOID_FAKE(NAME, ...) \
  RESET_FAKE(NAME)

FAKE_VALUE_FUNCTION_LIST(DECLARE_FAKE_VALUE_FUNC);
FAKE_VOID_FUNCTION_LIST(DECLARE_FAKE_VOID_FUNC);
