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

	state->handler(test, state, msgbuf);
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

void local_runner(test_t ** tests, result_handler_f * handler) {
	if (handler == NULL)
		handler = &print_result;

	int i = 0;
	runner_state_t state = { 0, 0, handler };
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
			handler(it, &state, "timeout after 5000ms");
		}

		reset_fakes();
	}

	kprintf("\n%d tests run, %d failed.\n", state.n_successes +
			state.n_failures, state.n_failures);
}

process local_test_runner() {
	local_runner(all_tests, NULL);
	return OK;
}
