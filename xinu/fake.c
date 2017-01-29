#include <xinu.h>
#include <test/fake.h>

DEFINE_FFF_GLOBALS;

void reset_fakes() {
  APPLY_FAKES_LIST(RESET_FAKE);
  FFF_RESET_HISTORY();
}

// Define fakes here.
DEFINE_FAKE_VALUE_FUNC(status, __wrap_getutime, uint32 *);
