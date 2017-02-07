#include <xinu.h>
#include <test/test.h>
#include <test/fake.h>
#include <test/assert.h>

/**
  This suite check the behavior of the `gettime` function, which should perform
  timezone offsetting when a connection to a NTP server is available.
  */

uint32 demo_time = 1485460361; // Thu, 26 Jan 2017 19:52:41 GMT

// Two different mocks to use on `getutime`, which is called by `gettime`.
static status getutime_ok_fake(uint32 * timvar) {
  // Return the demo_time defined above.
  *timvar = demo_time;
  return OK;
}
static status getutime_er_fake(uint32 * _) {
  // Return an error (indicating unavailable NTP connection).
  return SYSERR;
}

TEST(gettime_converts_utc_to_local) {
  // Given a working time server
  __wrap_getutime_fake.custom_fake = getutime_ok_fake;

  // When gettime is called with a result pointer
  uint32 time;
  status retval = gettime(&time);

  // It should call getutime and offset the result to EST
  assert_eq(__wrap_getutime_fake.call_count, 1);
  assert_eq(time, demo_time - (ZONE_EST * SECPERHR));
  assert_eq(retval, OK);
}

TEST(gettime_forwards_syserr) {
  // Given a disconnected time server
  __wrap_getutime_fake.custom_fake = getutime_er_fake;

  // When gettime is called with a result pointer
  uint32 time = 0;
  status retval = gettime(&time);

  // It should return SYSERR and not modify `time`
  assert_eq(retval, SYSERR);
  assert_eq(time, 0);
}
