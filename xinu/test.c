#include <xinu.h>
#include <stdio.h>
#include <test/test.h>

void run_test(test_t * test, runner_state_t * state) {
  char msgbuf[128];

  // TODO - set timeout and handle
  if (test->before)
    test->before();
  test->fn(&test->result, msgbuf);
  if (test->after)
    test->after();

  int time = 0; // magically calculate elapsed time
  test->time_ms = time;

  state->handler(test, state, msgbuf);
}

void print_result(test_t * test, runner_state_t * state,
    char * msg)
{
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

void local_runner(test_t ** tests, result_handler_f * handler) {
  if (handler == NULL)
    handler = &print_result;

  int i = 0;
  runner_state_t state = { 0, 0, handler };
  for (test_t * it = tests[i]; it != NULL; it = tests[++i]) {
    run_test(it, &state);
  }

  fprintf(stderr, "\n%d tests run, %d failed.\n", state.n_successes +
      state.n_failures, state.n_failures);
}

process local_test_runner() {
  local_runner(all_tests, NULL);
  return OK;
}
