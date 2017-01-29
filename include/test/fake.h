#pragma once

#include <test/fff.h>

#define APPLY_FAKES_LIST(FN) \
  FN(__wrap_getutime)

void reset_fakes();

// Declare fakes here. Then, define them in fake.c
// Semantics: FAKE_VOID_FUNC(name, arguments...)
//            FAKE_VALUE_FUNC(return_type, name, arguments...)
DECLARE_FAKE_VALUE_FUNC(status, __wrap_getutime, uint32 *); 
