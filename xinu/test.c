#include <xinu.h>
#include <stdio.h>
#include <test/test.h>
#include <test/fake.h>

#include <test/tests.def> 
extern test_t * all_tests[];

process run_test(test_t * test, runner_state_t * state, pid32 parent) {
  char * msgbuf = getmem(8196);
  if (msgbuf == (char *) SYSERR) {
    return SYSERR;
  }

  // TODO - set timeout and handle
  if (test->before)
    test->before();
  test->fn(&test->result, msgbuf);
  if (test->after)
    test->after();

  int time = 0; // magically calculate elapsed time
  test->time_ms = time;

  state->handler(test, state, msgbuf);
  freemem(msgbuf, 8196);
  send(parent, 0);
  return OK;
}

void print_result(test_t * test, runner_state_t * state, char * msg) {
  switch (test->result.status) {
    case SUCCESS:
      kprintf(".");
      state->n_successes++;
      break;
    case FAILURE:
      kprintf("\n%2d) %s failed:\n%s\n", ++state->n_failures,
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
    resume(create(run_test, INITSTK, INITPRIO, (char *) it->name, 3, it,
                  &state, currpid));
    // TODO - use recvtime to handle tests that hang
    receive();
    reset_fakes();
  }

  kprintf("\n%d tests run, %d failed.\n", state.n_successes +
      state.n_failures, state.n_failures);
}

process local_test_runner() {
  local_runner(all_tests, NULL);
  return OK;
}
