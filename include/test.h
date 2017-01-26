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
  test_f fn;
  result_t result;
  int time_ms;
};
typedef struct test_s test_t;

// A collection of tests. Suites can have a `before` and `after` function,
// which is called before and after each test.
struct suite_s {
  char * name;
  void (* before)();
  void (* after)();
  // The list of tests _must_ be null-terminated.
  test_t * tests[];
};
typedef struct suite_s suite_t;

struct runner_s;
typedef void (result_handler_f)(suite_t * suite, test_t * test, 
    struct runner_s * state, char * msg);

// The state of the test runner, which is shared across the entire test
// infrastructure.
struct runner_s {
  int n_successes;
  int n_failures;
  result_handler_f * handler;
};
typedef struct runner_s runner_state_t;

// Macros to define a test and silently define its test_t metadata.
#define TEST_DECL(NAME) \
  void NAME##_fn (result_t * _test_res, char * _msgbuf); \
  test_t NAME 
#define TEST(NAME) \
  TEST_DECL(NAME) = { .name = #NAME, .fn = NAME##_fn }; \
  void NAME##_fn (result_t * _test_res, char * _msgbuf)

// Define a suite, passing its name, an expression from SUITE_OPTIONS, and any
// pointers to tests to include in the suite.
#define SUITE(NAME, OPTS, TESTS) \
  suite_t NAME = { .name = #NAME OPTS, .tests = TESTS };
#define SUITE_OPTIONS(...) , ## __VA_ARGS__
#define SUITE_TESTS(...) { __VA_ARGS__, NULL }

// Declare a suite that is defined elsewhere; useful for creating a list of
// suites to run.
#define EXTERN_SUITE(NAME) extern suite_t NAME;

// Functions called by the test runner.
void run_test(suite_t * suite, test_t * test, runner_state_t * state);
void run_suite(suite_t * suite, runner_state_t * state);

// The entry point for local testing. Runs a null-terminated list of suites,
// and calls `handler` as results come in. Passing a NULL `handler` uses a
// default which prints to stderr.
void local_runner(suite_t * suites[], result_handler_f handler);
result_handler_f print_result;

#endif
