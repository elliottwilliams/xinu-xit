#include <xinu.h>
#include <stdio.h>
#include <test/test.h>

void run_test(suite_t * suite, test_t * test, runner_state_t * state) {
  char msgbuf[128];

  // TODO - set timeout and handle
  test->fn(&test->result, msgbuf);

  int time = 0; // magically calculate elapsed time
  test->time_ms = time;

  state->handler(suite, test, state, msgbuf);
}

void run_suite(suite_t * suite, runner_state_t * state) {
  int i = 0;
  for (test_t * it = suite->tests[i];
      it != NULL;
      it = suite->tests[++i])
  {
    if (suite->before) suite->before();
    run_test(suite, it, state);
    if (suite->after) suite->after();
  }
}

void print_result(suite_t * suite, test_t * test, runner_state_t * state,
    char * msg)
{
  if (test == suite->tests[0]) {
    fprintf(stderr, "[%s]\n", suite->name);
  }

  switch (test->result.status) {
    case SUCCESS:
      fprintf(stderr, ".");
      state->n_successes++;
      break;
    case FAILURE:
      fprintf(stderr, "\n%2d) %s failed:\n%s\n", ++state->n_failures,
          test->name, msg);
      break;
  }
}

void local_runner(suite_t * suites[], result_handler_f * handler) {
  if (handler == NULL)
    handler = &print_result;

  int i = 0;
  runner_state_t state = { 0, 0, handler };
  for (suite_t * it = suites[i]; it != NULL; it = suites[++i]) {
    run_suite(it, &state);
  }

  fprintf(stderr, "\n%d tests run, %d failed.\n", state.n_successes +
      state.n_failures, state.n_failures);
}
