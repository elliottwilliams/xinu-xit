# HOW TO USE THIS FILE:
# Include it into your Xinu Makefile:
#     -include ../test/xinu/Makefile
# Then, to run tests, run `make test TESTS=[test_name] `.

TESTS= # Define tests here or on the command line to run.

TEST_UTILS_SRC=../test/xinu
TEST_UTILS_BUILD=tests/utils
TEST_SRC=../tests
TEST_BUILD=tests

# Determine the object file paths for tests in $(TESTS).
TEST_OBJS=$(foreach test,$(TESTS),$(test:%=tests/%.o)) 
TEST_UTIL_OBJS = $(TEST_UTILS_BUILD)/test.o 

# Add the test header includes to the include variable used everywhere.
INCLUDE += -I$(TOPDIR)/test/include

# Rules to compile test utils and the tests themselves.
$(TEST_BUILD)/%.o: $(TEST_SRC)/%.c
	$(CC) $(CFLAGS) -o $(TEST_BUILD)/$*.o $(TEST_SRC)/$*.c
$(TEST_UTILS_BUILD)/%.o: $(TEST_UTILS_SRC)/%.c $(TEST_UTILS_BUILD)/tests.def
	$(CC) $(CFLAGS) -include $(TEST_UTILS_BUILD)/tests.def -o $(TEST_UTILS_BUILD)/$*.o $(TEST_UTILS_SRC)/$*.c

# Keep track of the tests included into this build of Xinu.
$(TEST_UTILS_BUILD)/testsym: $(TEST_OBJS)
	@echo 'storing test symbols...'
	@find $(TEST_BUILD) -name \*.o -exec nm {} -g \; | awk 'NF>=3 {print $$3}' | sort | uniq > $(TEST_UTILS_BUILD)/testsym

# Generate command line arguments for ld that wrap functions mocked in tests.
$(TEST_UTILS_BUILD)/wrapargs: $(TEST_UTILS_BUILD)/testsym
	@cat $(TEST_UTILS_BUILD)/testsym | sed -n 's/^__wrap_\(.*\)/--wrap=\1/p' > $(TEST_UTILS_BUILD)/wrapargs

# Generate a header that declares all tests in this build. It will be included
# in all objects in TEST_UTILS_BUILD.
$(TEST_UTILS_BUILD)/tests.def: $(TEST_UTILS_BUILD)/testsym
	@echo 'struct test_s;' > $(TEST_UTILS_BUILD)/tests.def
	@cat $(TEST_UTILS_BUILD)/testsym | sed -n 's/^\(test_.*\)/extern struct test_s \1;/p' >> $(TEST_UTILS_BUILD)/tests.def
	@echo 'struct test_s * all_tests[] = {' >> $(TEST_UTILS_BUILD)/tests.def
	@cat $(TEST_UTILS_BUILD)/testsym | sed -n 's/^\(test_.*\)/\&\1, /p' >> $(TEST_UTILS_BUILD)/tests.def
	@echo '(struct test_s *) 0};' >> $(TEST_UTILS_BUILD)/tests.def

# The main target for running tests. 
test: $(CONFH) $(TEST_OBJS) test_xinu

# Find the real names of any wrapped functions (see ld(1)) defined in a test
# object.
test_xinu: TEST_REFS = $(TEST_FNS:%=&test_%,)
test_xinu: TEST_DECLARATIONS = $(TEST_FNS:%=extern test_t test_%;)
test_xinu: CFLAGS += -DTESTS_ENABLED
test_xinu: OBJ_FILES += $(TEST_OBJS) $(TEST_UTIL_OBJS)
test_xinu: LDFLAGS += @$(TEST_UTILS_BUILD)/wrapargs
test_xinu: $(TEST_UTILS_BUILD)/wrapargs $(TEST_UTIL_OBJS) xinu 

clean_tests:
	@rm -f $(TEST_BUILD)/*.o $(TEST_UTILS_BUILD)/{testsym,wrapargs,tests.def} $(TEST_UTILS_BUILD)/*.o

.PHONY: test test_xinu clean_tests
