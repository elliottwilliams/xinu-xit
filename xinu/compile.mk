# HOW TO USE THIS FILE:
# Include it into your Xinu Makefile:
#     -include ../test/xinu/Makefile
# Then, to run tests, `make test`.

UTIL_SRC    := ../test/xinu
TEST_SRC    := ../tests
SRC_CONFIG  := ../tests/config
SRC_INCLUDE := ../test/include

GEN_BASE    := tests
TEST_BIN    := $(GEN_BASE)
UTIL_BIN    := $(GEN_BASE)/utils
GEN_INCLUDE := $(GEN_BASE)/include

# Include all tests in TEST_SRC matching one of the given patterns. To only
# compile certain tests, modify this from the command line, e.g. `make test
# TESTS=unit/net`
TESTS := '*'

# Determine the object file paths for tests and test utilities. Words in TESTS
# are used as path selectors for `find`. Tests in TEST_SRC are matched using a
# selector, and those paths are transformed to the object file path in
# GEN_BASE.
TEST_OBJS = $(patsubst $(TEST_SRC)/%.c, $(GEN_BASE)/%.o, \
						$(shell find $(TEST_SRC) -false \
						$(TESTS:%=-o -path '$(TEST_SRC)/%*.c' )))
TEST_UTIL_OBJS = $(UTIL_BIN)/test.o $(UTIL_BIN)/fake.o $(UTIL_BIN)/hexcmp.o

# Add the test header includes to the include variable used everywhere.
INCLUDE += -I$(SRC_INCLUDE) -I$(GEN_INCLUDE)

# The main target for running tests. 
test: clean_tests test_dirs $(CONFH) $(TEST_OBJS) test_xinu
	@echo 'Includes all tests matching selector(s): $(TESTS)'
	@echo -e '\t $(TEST_OBJS:$(GEN_BASE)/%.o=%\n\t)'


test_dirs:
	@mkdir -p $(TEST_BIN) $(UTIL_BIN) $(GEN_INCLUDE)/test $(TEST_SRC) $(SRC_CONFIG)

# Rules to compile test utils and the tests themselves.
$(TEST_BIN)/%.o: $(TEST_SRC)/%.c $(GEN_INCLUDE)/test/fakes.def
	@mkdir -p "$(TEST_BIN)/$(shell dirname $<)"
	$(CC) $(CFLAGS) -o $@ $<
$(UTIL_BIN)/%.o: $(UTIL_SRC)/%.c $(GEN_INCLUDE)/test/tests.def $(GEN_INCLUDE)/test/fakes.def
	$(CC) $(CFLAGS) -o $@ $<

# Keep track of the tests included into this build of Xinu.
$(UTIL_BIN)/testsym: $(TEST_OBJS)
	@find $(TEST_BIN) -name \*.o -exec nm {} -g \; | awk 'NF>=3 {print $$3}' \
		> $(UTIL_BIN)/testsym

# Generate command line arguments for ld that wrap functions mocked in tests
# and test utils. Clean the build, since the OS needs to be relinked to use
# wraps.
$(UTIL_BIN)/wrapargs: $(UTIL_BIN)/fake.o
	@nm $< -g | sed -n 's/.*__wrap_\([[:alnum:]_]\+\).*/--wrap=\1/p' > $@

# Generate a header that declares all tests in this build. It will be included
# in all objects in UTIL_BIN.
$(GEN_INCLUDE)/test/tests.def: $(UTIL_BIN)/testsym
	@echo 'struct test_s;' > $@
	@cat $(UTIL_BIN)/testsym | sed -n 's/^\(test_.*\)/extern struct test_s \1;/p' \
		>> $@
	@echo 'struct test_s * all_tests[] = {' >> $@
	@cat $(UTIL_BIN)/testsym | sed -n 's/^\(test_.*\)/\&\1, /p' >> $@
	@echo '(struct test_s *) 0};' >> $@

# Copy the fakes config to the include path (dropping the .h to imply this
# file's dynamic nature. Delete xinu object files to re-link the source with
# the new fakes.
$(GEN_INCLUDE)/test/fakes.def: $(SRC_CONFIG)/fakes.def.h
	@rm -f $(XINU) $(XINU_BIN) $(XINU_XBIN)
	@cp $? $@

$(SRC_CONFIG)/fakes.def.h: 
	@cp $(UTIL_SRC)/fakes.def.example.h $@

# Find the real names of any wrapped functions (see ld(1)) defined in a test
# object.
test_xinu: TEST_REFS = $(TEST_FNS:%=&test_%,)
test_xinu: TEST_DECLARATIONS = $(TEST_FNS:%=extern test_t test_%;)
test_xinu: CFLAGS += -DTESTS_ENABLED -DTEST_SRC='"$(TEST_SRC)"'
test_xinu: OBJ_FILES += $(TEST_OBJS) $(TEST_UTIL_OBJS)
test_xinu: LDFLAGS += @$(UTIL_BIN)/wrapargs
test_xinu: $(UTIL_BIN)/wrapargs $(TEST_UTIL_OBJS) test_touch_main xinu 

test_touch_main:
	@touch ../system/main.c

clean_tests:
	@rm -rf $(GEN_BASE)/* 


.PHONY: test test_dirs test_xinu clean_tests pre_clean test_touch_main 
