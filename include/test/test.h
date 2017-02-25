#ifndef TEST_H
#define TEST_H

// Represents the result of a test.
struct result_s {
  // TODO - add SKIP, TIMEOUT
  enum { SUCCESS, FAILURE } status;
};
typedef struct result_s result_t;

// A test procedure as declared by TEST and TEST_DECL.
typedef void (* test_f)(result_t *, char * msgbuf);

// Metadata for a test. Groups of `test_t` make up suites.
struct test_s {
  const char * name;
  const char * suite;
  void (* before)();
  void (* after)();
  test_f fn;
  result_t result;
  int time_ms;
};
typedef struct test_s test_t;

struct runner_s;
typedef void (result_handler_f)(test_t * test, struct runner_s * state, char * msg);
typedef void (stats_handler_f)(struct runner_s * state);

// The state of the test runner, which is shared across the entire test
// infrastructure.
struct runner_s {
  test_t ** tests;
  int n_successes;
  int n_failures;
  result_handler_f * result_handler;
  stats_handler_f * stats_handler;
};
typedef struct runner_s runner_state_t;

// Names for test_t structs and test functions.
#define TEST_T(NAME) test_##NAME
#define TEST_F(NAME) testfn_##NAME

// Declare a test and its test function.
#define TEST_DECL(NAME) \
  void TEST_F(NAME) (result_t * _test_res, char * _msgbuf); \
  test_t TEST_T(NAME) 

// Declare and define a test function, optionally specifying before and after
// functions.
#define TEST(NAME, ...) \
  TEST_DECL(NAME) = { .name = #NAME, .suite = __FILE__, .fn = TEST_F(NAME), \
    __VA_ARGS__ }; \
  void TEST_F(NAME) (result_t * _test_res, char * _msgbuf)

// Functions called by the test runner.
process run_test(test_t * test, runner_state_t * state);

// The entry point for local testing. Runs a null-terminated list of tests,
// and calls `result` as results come in. Once testing completes, calls
// `stats`. Passing a NULL for either handler function uses a default
// implementation.
void local_runner(test_t ** tests, result_handler_f result, stats_handler_f stats);
result_handler_f print_result;
stats_handler_f print_statistics;

// A xinu process that runs all tests declared by compile-time defines.
process local_test_runner();

#endif
