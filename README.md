# xit, a unit testing framework for Xinu

xit is a C-based unit testing library for [Xinu][xinu], an
embedded operating system developed at Purdue University. xit provides the
following functionality:

- [x] Organization of tests into suites and test cases.
- [x] Assertions for different data types, with useful, customizable, error
	reporting.
- [x] System function mocking through the [fff][fff] library.
- [x] Cleanup and teardown functions per-test.
- [x] Automatic discovery, compilation, and declaration of tests.
- [x] Integration into the Xinu compilation infrastructure. (`make test` is all
	you need!)

Features slated for development:

- [ ] Network-based distributed testing. Using infrastructure of the [xinu
	lab][lab], xit will be able to spin up multiple Xinu backends and run a
	coordinated sequence of tests across them.  This should be useful for
	integration testing, where behavior of the entire networking stack is
	observed.
- [ ] Time measurement. xit will keep track of how long tests run, and kill
	tests that take too long.

xit is pronounced "exit" and is a portmanteau of "xinu" and "test".

[xinu]: http://www.xinu.cs.purdue.edu
[lab]: http://www.xinu.cs.purdue.edu/#lab


## Examples

See [`examples/`](examples) for an example suite that tests
the behavior of the `gettime` system function.


## Installation

1. Clone this repository into your Xinu source tree. (I use a directory
	 `test/`, but anywhere is sufficient.) Use a submodule to keep track of your
	 xit version within your Xinu repo:

	```sh
	git submodule add https://github.com/elliottwilliams/cs636_test.git test
	```

2. Include xit's makefile extension into Xinu's `Makefile`. Add this to the
	 bottom of `compile/Makefile` (substitute the directory xit is cloned
	 into):

	```make
	-include ../test/xinu/compile.mk
	```

3. Edit your `main.c` code to start a test runner on startup, like this:

	```c
	#include <xinu.h>

	// Add these lines near the top of the file:
	#ifdef TESTS_ENABLED
	#include <test/test.h>
	#endif

	// ...
	process main(void)
	{
		// Add these lines to main():
	#ifdef TESTS_ENABLED
		resume(create(local_test_runner, INITSTK, INITPRIO, "local_test_runner",
		0));
		return OK;
	#endif

		// ...
	}
	```

4. Create a `tests/` directory in the root of your source tree, and write tests
	 in it. See the section on writing tests below. 


# Running tests

1. Compile Xinu with tests enabled by running `make test` from the `compile/`
	 directory. This will discover test functions in `tests/`, compile them,
	 and link to the rest of the xinu codebase. Mock functions declared as
	 [wraps][wrap] will be discovered automatically by the linker.

2. To run only a subset of your tests, pass a `TESTS` variable containing
	 space-separated suite names to `make`. Suite names correspond to the names
	 of test files without their extension. For example, `tests/test_netin.c` has a
	 suite name of `test_netin`, and could be selected by running:

	 ```sh
	 make test TESTS="test_netin"
	 ```

The resulting `xinu.xbin` image can be used to boot up a Xinu backend as
usual, and will run all tests specified.

[wrap]: #mock-functions


# Writing tests

Test suites are C source files that import the following headers:

- `#include <test/test.h>` for test macros and data structures
- `#include <test/assert.h>` for the assertion library
- `#include <test/fake.h>` for access to mockable [fake functions][fff]


### Basic tests

Each suite should contain one or more test cases, declared with the `TEST`
macro. For example:

```c
TEST(sky_is_blue) {
  // Set up any values to test
  char * color = getskycolor();

  // Perform assertions
  assert_str_eq(color, "blue");
}
```

Inside the expansion of the `TEST` macro, a `test_t` structure is defined
corresponding to the test case's name, and the test case's function signature
is inserted.

The `assert_*` macros check their arguments, and return a failure result to the
test runner if the assertion failed. By default, failures are printed out to
the xinu console. A list of [all available assertions][assertions] is provided
below.

[assertions]: #assertions

### Before and after functions

Variants of the `TEST` macro accept functions that are run before or after a
test case. This allows for setup/teardown functions to be shared across
multiple cases:

- `TEST_BEFORE(test_name, before_fn) { /* ... */ }`
- `TEST_AFTER(test_name, after_fn) { /* ... */ }`
- `TEST_BEFORE_AFTER(test_name, before_fn, after_fn) { /* ... */ }`


### Assertions

The `test/assert.h` header defines the following assertion macros:

- **_`assert(cond)`_**   
  Passes if `cond != 0`.
- **_`assert_eq(lhs, rhs)`_**  
  Passes if `lhs == rhs`.
- **_`assert_eq_fmt(lhs, rhs, fmt)`_**  
	Passes if `lhs == rhs`. Uses format string `fmt` to generate a failure
  message.
- **_`assert_str_eq(lhs, rhs)`_**  
  Passes if `lhs` and `rhs` are determined equivalent by `strcmp`.
- **_`assert_in_range(expected, actual, tolerance)`_**  
  Passes if `expected` = `actual` ± `tolerance`.
- **_`assert_mem_eq(expected, actual, size)`_**  
  Passes if the first `size` bytes of `expected` and `actual` are equivalent. 
    - The failure message for this assertion depends on hex dump functions I've
    	not implemented yet.


### Mock functions

xit allows functions called within the OS to be stubbed and monitored at
runtime. To see these "fake functions" in action, look at
`examples/test_gettime.c`, and see their definitions in `include/test/fake.h`
and `xinu/fake.c`.

#### Background

xit uses [fff][fff], a C microframework for creating "fake functions", which
return stubbed values and invoke handlers set at runtime, and keep track of
calls made. It combines this library with ld wraps: linker options which
rewrite all references for a symbol *fn* to *__wrap_fn*. This allows function
calls within the Xinu source code to be rerouted at link time.

xit's approach is to create "fakes" of `__wrap_` functions using fff. Its
makefile discovers all `__wrap_` symbols declared in tests, and automatically
passes the proper wrap option to ld. Then, calls in Xinu to a function
`function_name` get linked to the wrap function `__wrap_function_name`, which
in turn stores its metadata in an [fff struct][fake],
`__wrap_function_name_fake`.

One caveat to this approach is that (since wrapping a function involves the
linker) a function can only be faked once in the entire codebase. Because of
this, fakes are centrally declared in `config/fakes.def.h`. The following
sections describe how to add and use fakes.

[fff]: https://github.com/meekrosoft/fff
[fake]: https://github.com/meekrosoft/fff#hello-fake-world


#### Faking a system function

Add the function and its signature to `config/fakes.def.h`. See the
documentation and example in that file. The format for both VALUE and VOID
functions follows [fff's syntax][fff_cheat].

[use]: #using-a-faked-function
[fff_cheat]: https://github.com/meekrosoft/fff#cheat-sheet


#### Using a faked function

1. Ensure your test suite includes `<test/fake.h>`.

2. In a test case, call a function which calls the function being faked. The
   fake function will be used automatically.

3. Inspect the call history of the fake function by looking at its `_fake`
   struct. For example, the if `__wrap_ip6in` is faked, it's struct is
   `__wrap_ip6in_fake`. See [fff's documentation][fff] for details.
