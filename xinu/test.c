#include <xinu.h>
#include <stdio.h>
#include <test/test.h>
#include <test/fake.h>

//#define debug(...) kprintf(__VA_ARGS__)
#define debug(...)

// Forward declaration of private stacktrace function.
extern syscall stacktrace(int pid);

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
      kprintf("\n\x1b[31m%2d) %s failed:\x1b[0m\n%s\n", ++state->n_failures,
              test->name, msg);
			break;
    case TIMEDOUT:
      kprintf("\n\x1b[33m%2d) %s timed out\x1b[0m\n", ++state->n_failures,
              test->name);
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
		debug("running %s...", it->name);
		pid32 test_pid = create(run_test, INITSTK, INITPRIO, it->name,
								2, it, &state);

		// Start the test, and wait for its completion message.
		debug("resume(test_pid)\n");
		resume(test_pid);

		// Receive messages until kill() sends us the pid that the test exited,
		// or until a timeout elapses.
		uint32 msg;
		do msg = recvtime(5000);
		while (msg != test_pid && msg != TIMEOUT);
		debug("message received = %d\n", msg);

		// Kill timed-out tests and log their failure.
		if (msg == TIMEOUT) {
		  kprintf("[timeout in %s after 5000ms]\n", it->name);
		  stacktrace(test_pid);
			kill(test_pid);
			it->result.status = TIMEDOUT;
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
    switch (test->result.status) {
    case SUCCESS:
      kprintf("\x1b[32m  - %s [%dms]", test->name, test->time_ms); // green
      break;
    case FAILURE:
      kprintf("\x1b[31m  - %s (FAILED) [%dms]", test->name, test->time_ms); // red
      break;
    case TIMEDOUT:
      kprintf("\x1b[33m  - %s (TIMEOUT)", test->name); // yellow
      break;
    }

    // Reset text color.
    kprintf("\x1b[0m\n");
  }

  kprintf("\n%d tests run, %d failed.\n", state->n_successes +
  		  state->n_failures, state->n_failures); 
}
