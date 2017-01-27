# HOW TO USE THIS FILE:
# Include it into your Xinu Makefile:
#     -include ../test/xinu/Makefile
# Then, to run tests, run `make test TESTS=[test_name] `.

TESTS= # Define tests here or on the command line to run.

# Determine the object file paths for tests in $(TESTS).
TEST_OBJS=$(foreach test,$(TESTS),$(test:%=tests/%.o)) 
# Find the real names of any wrapped functions (see ld(1)) defined in a test
# object.
WRAPPED_FNS ::= $(foreach o, $(TEST_OBJS), $(shell nm $(o) | sed -n 's/.*__wrap_//p'))

TEST_UTILS_SRC=../test/xinu
TEST_UTILS_SRC_BUILD=tests/utils
TEST_SRC=../tests
TEST_BUILD=tests

TEST_UTILS = $(TEST_UTILS_SRC_BUILD)/test.o

# Rules to compile test utils and the tests themselves.
$(TEST_BUILD)/%.o: $(TEST_SRC)/%.c
	$(CC) $(CFLAGS) -o $(TEST_BUILD)/$*.o $(TEST_SRC)/$*.c
$(TEST_UTILS_SRC_BUILD)/%.o: $(TEST_UTILS_SRC)/%.c
	$(CC) $(CFLAGS) -o $(TEST_UTILS_SRC_BUILD)/$*.o $(TEST_UTILS_SRC)/$*.c

# The main target for running tests. 
test: INCLUDE += -I$(TOPDIR)/test/include
test: CFLAGS += -DTESTS_ENABLED
test: OBJ_FILES += $(TEST_OBJS) $(TEST_UTILS)
test: LDFLAGS += $(foreach fn, $(WRAPPED_FNS), --wrap=$(fn))
test: $(CONFH) $(TEST_OBJS) $(TEST_UTILS) xinu 

clean_tests:
	@rm -f $(TEST_BUILD)/*.o $(TEST_UTILS_SRC_BUILD)/*.o

.PHONY: test 
