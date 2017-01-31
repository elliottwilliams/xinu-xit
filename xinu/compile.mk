# HOW TO USE THIS FILE:
# Include it into your Xinu Makefile:
#     -include ../test/xinu/Makefile
# Then, to run tests, `make test`.

UTIL_SRC    := ../test/xinu
TEST_SRC    := ../tests
SRC_CONFIG  := ../test/config
SRC_INCLUDE := ../test/include

GEN_BASE    := tests
TEST_BIN    := $(GEN_BASE)
UTIL_BIN    := $(GEN_BASE)/utils
GEN_INCLUDE := $(GEN_BASE)/include

# Get suite names from c files in TEST_SRC. To only compile certain suites,
# modify this from the command line, e.g. `make test TESTS=test_netin`.
TESTS := $(foreach test, \
	$(shell find $(TEST_SRC) -name *.c), \
	$(test:$(TEST_SRC)/%.c=%))

# Determine the object file paths for tests and test utilities.
TEST_OBJS = $(foreach test,$(TESTS),$(test:%=tests/%.o)) 
TEST_UTIL_OBJS = $(UTIL_BIN)/test.o $(UTIL_BIN)/fake.o

# Add the test header includes to the include variable used everywhere.
INCLUDE += -I$(SRC_INCLUDE) -I$(GEN_INCLUDE)

# The main target for running tests. 
test: test_dirs $(CONFH) $(TEST_OBJS) test_xinu
	@echo Build includes suites: $(TESTS)

test_dirs:
	@mkdir -p $(TEST_BIN) $(UTIL_BIN) $(GEN_INCLUDE)/test

# Rules to compile test utils and the tests themselves.
$(TEST_BIN)/%.o: $(TEST_SRC)/%.c $(GEN_INCLUDE)/test/fakes.def
	$(CC) $(CFLAGS) -o $@ $<
$(UTIL_BIN)/%.o: $(UTIL_SRC)/%.c $(GEN_INCLUDE)/test/tests.def $(GEN_INCLUDE)/test/fakes.def
	$(CC) $(CFLAGS) -o $@ $<

# Keep track of the tests included into this build of Xinu.
$(UTIL_BIN)/testsym: $(TEST_OBJS)
	@find $(TEST_BIN) -name \*.o -exec nm {} -g \; | awk 'NF>=3 {print $$3}' \
		| sort | uniq > $(UTIL_BIN)/testsym

$(UTIL_BIN)/testutilsym: $(TEST_UTIL_OBJS)
	@find $(UTIL_BIN) -name \*.o -exec nm {} -g \; | awk 'NF>=3 {print $$3}' \
		| sort | uniq > $(UTIL_BIN)/testutilsym

# Generate command line arguments for ld that wrap functions mocked in tests
# and test utils.
$(UTIL_BIN)/wrapargs: $(GEN_INCLUDE)/test/fakes.def
	@cat $? | sed -n 's/.*__wrap_\([[:alnum:]_]\+\).*/--wrap=\1/p' > $@

# Generate a header that declares all tests in this build. It will be included
# in all objects in UTIL_BIN.
$(GEN_INCLUDE)/test/tests.def: $(UTIL_BIN)/testsym
	@echo 'struct test_s;' > $@
	@cat $(UTIL_BIN)/testsym | sed -n 's/^\(test_.*\)/extern struct test_s \1;/p' \
		>> $@
	@echo 'struct test_s * all_tests[] = {' >> $@
	@cat $(UTIL_BIN)/testsym | sed -n 's/^\(test_.*\)/\&\1, /p' >> $@
	@echo '(struct test_s *) 0};' >> $@

$(GEN_INCLUDE)/test/fakes.def: $(SRC_CONFIG)/fakes.def.h
	@cp $? $@

# Find the real names of any wrapped functions (see ld(1)) defined in a test
# object.
test_xinu: TEST_REFS = $(TEST_FNS:%=&test_%,)
test_xinu: TEST_DECLARATIONS = $(TEST_FNS:%=extern test_t test_%;)
test_xinu: CFLAGS += -DTESTS_ENABLED
test_xinu: OBJ_FILES += $(TEST_OBJS) $(TEST_UTIL_OBJS)
test_xinu: LDFLAGS += @$(UTIL_BIN)/wrapargs
test_xinu: $(UTIL_BIN)/wrapargs $(TEST_UTIL_OBJS) xinu 

clean_tests:
	@rm -rf $(GEN_BASE)/* 

.PHONY: test test_dirs test_xinu clean_tests
