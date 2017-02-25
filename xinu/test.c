#include <xinu.h>
#include <stdio.h>
#include <test/test.h>
#include <test/fake.h>

#include <test/tests.def> 
extern test_t * all_tests[];

process run_test(test_t * test, runner_state_t * state) {
	char * msgbuf = getmem(8196);
	if (msgbuf == (char *) SYSERR) {
		return SYSERR;
	}

	uint64 start_time = gettime_monotonic_ms();

	if (test->before)
		test->before();
	test->fn(&test->result, msgbuf);
	if (test->after)
		test->after();

	uint64 elapsed = gettime_monotonic_ms() - start_time;
	test->time_ms = elapsed;

	state->result_handler(test, state, msgbuf);
	freemem(msgbuf, 8196);
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

void local_runner(test_t ** tests, result_handler_f * result, stats_handler_f stats) {
	if (result == NULL)
		result = &print_result;
	if (stats == NULL)
	  stats = &print_statistics;

	int i = 0;
	runner_state_t state = { tests, 0, 0, result, stats };
	for (test_t * it = tests[i]; it != NULL; it = tests[++i]) {
		pid32 test_pid = create(run_test, INITSTK, INITPRIO, (char *) it->name,
								2, it, &state);

		// Start the test, and wait for its completion message.
		resume(test_pid);

		// Receive messages until kill() sends us the pid that the test exited,
		// or until a timeout elapses.
		uint32 msg;
		do msg = recvtime(5000);
		while (msg != test_pid && msg != TIMEOUT);

		// Kill timed-out tests and log their failure.
		if (msg == TIMEOUT) {
			kill(test_pid);
			it->result.status = FAILURE;
			result(it, &state, "timeout after 5000ms");
		}

		reset_fakes();
	}

  stats(&state);
}

process local_test_runner() {
	local_runner(all_tests, NULL, NULL);
	return OK;
}

// Takes a string like `../tests/net/icmp6.c`, and copies `net/icmp6` to `out`.
// Returns `out`.
char * suite_format(const char * name, char * out) {
  // Advance past the TEST_SRC directory and the following slash.
  const char * n = strstr(name, TEST_SRC);
  if (!n)
    return (char *) name;
  n += strlen(TEST_SRC) + 1;

  // Find the trailing .c in `name`.
  const char * ne = strstr(n, ".c");
  if (!ne)
    return (char *) name;

  // Copy between `n` and `ne`.
  memcpy(out, n, ne-n);
  out[ne-n] = '\0';
  return out;
}

void print_statistics(runner_state_t * state) {
  const char * last_suite = "";

  for (int i = 0; state->tests[i] != 0; i++) {
    test_t * test = state->tests[i];

    // Print suite name upon change.
    if (strcmp(last_suite, test->suite) != 0) {
      last_suite = test->suite;
      char name[strlen((char *) test->suite)];
      kprintf("\n%s:\n", suite_format(test->suite, name));
    }

    // Print test information based on status.
    if (test->result.status == SUCCESS) {
      kprintf("\x1b[32m  - %s [%dms]", test->name, test->time_ms); // green
    } else {
      kprintf("\x1b[31m  - %s (FAILED) [%dms]", test->name, test->time_ms); // red
    }

    // Reset text color.
    kprintf("\x1b[0m\n");
  }

  kprintf("\n%d tests run, %d failed.\n", state->n_successes +
  		  state->n_failures, state->n_failures); 
}
