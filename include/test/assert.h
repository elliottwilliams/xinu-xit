#ifndef ASSERT_H
#define ASSERT_H

#define _AS_FAILURE_MESSAGE_PRE "in %s:%d: "
#define _AS_LPAD "    "
#define _AS_FAIL _test_res->status = FAILURE
#define _as_bprintf(...) sprintf(_msgbuf, __VA_ARGS__)

#define assert_eq(lhs, rhs) do {                                                                           \
  if ((lhs) != (rhs)) {                                                                                    \
    _AS_FAIL;                                                                                              \
    _as_bprintf(                                                                                           \
        _AS_LPAD _AS_FAILURE_MESSAGE_PRE "assert_eq(" #lhs ", " #rhs ")\n\n"                               \
        _AS_LPAD "Expected: true\n"                                                                        \
        _AS_LPAD "     Got: false\n",                                                                      \
        __FILE__, __LINE__);                                                                               \
    return;                                                                                                \
  }                                                                                                        \
} while (0);

#define assert_eq_fmt(lhs, rhs, fmt) do {                                                                  \
  if ((lhs) != (rhs)) {                                                                                    \
    _AS_FAIL;                                                                                              \
    _as_bprintf(                                                                                           \
        _AS_LPAD _AS_FAILURE_MESSAGE_PRE "assert_eq_fmt(" #lhs ", " #rhs ", " #fmt ")\n\n"                 \
        _AS_LPAD "Expected: " fmt "\n"                                                                     \
        _AS_LPAD "     Got: " fmt "\n",                                                                    \
        __FILE__, __LINE__, (rhs), (lhs));                                                                 \
    return;                                                                                                \
  }                                                                                                        \
} while (0);

#define assert_str_eq(lhs, rhs) do {                                                                       \
  if (strcmp((lhs), (rhs)) != 0) {                                                                         \
    _AS_FAIL;                                                                                              \
    _as_bprintf(                                                                                           \
        _AS_LPAD _AS_FAILURE_MESSAGE_PRE "assert_str_eq(" #lhs ", " #rhs ")\n\n"                           \
        _AS_LPAD "Expected: \"%s\"\n"                                                                      \
        _AS_LPAD "     Got: \"%s\"\n",                                                                     \
        __FILE__, __LINE__, (rhs), (lhs));                                                                 \
    return;                                                                                                \
  }                                                                                                        \
} while (0);

#define assert(cond) do {                                                                                  \
  if (!(cond)) {                                                                                           \
    _AS_FAIL;                                                                                              \
    _as_bprintf(                                                                                           \
        _AS_LPAD _AS_FAILURE_MESSAGE_PRE "assert(" #cond ")\n\n"                                           \
        _AS_LPAD "Expected: true\n"                                                                        \
        _AS_LPAD "     Got: false\n",                                                                      \
        __FILE__, __LINE__);                                                                               \
  }                                                                                                        \
} while (0);

#define assert_false(cond) do {                                                                            \
  if (cond) {                                                                                              \
    _AS_FAIL;                                                                                              \
    _as_bprintf(                                                                                           \
        _AS_LPAD _AS_FAILURE_MESSAGE_PRE "assert_false(" #cond ")\n\n"                                     \
        _AS_LPAD "Expected: false\n"                                                                       \
        _AS_LPAD "     Got: true\n",                                                                       \
        __FILE__, __LINE__);                                                                               \
  }                                                                                                        \
} while (0);

#define assert_in_range(expected, actual, tolerance) do {                                                  \
  if (abs((expected) - (actual)) > (tolerance)) {                                                          \
    _AS_FAIL;                                                                                              \
    _as_bprintf(                                                                                           \
        _AS_LPAD _AS_FAILURE_MESSAGE_PRE "assert_in_range(" #expected ", " #actual ", " #tolerance ")\n\n" \
        _AS_LPAD "Expected: %d = %d +/- %d\n"                                                              \
        _AS_LPAD "     Got: %d = %d %+d\n",                                                                \
        __FILE__, __LINE__, #expected, (actual), (tolerance), #expected,                                   \
        (actual), ((expected) - (actual)));                                                                \
  }                                                                                                        \
} while (0);

char * hexdump(void * v, int pad);
char * hexdump_cmp(void * va, void * vb, int pad);
#define assert_mem_eq(expected, actual, size) do {                                                         \
  if (memcmp((expected), (actual), (size)) != 0) {                                                         \
    _AS_FAIL;                                                                                              \
    char * exp_s = hexdump((actual), 14);                                                                  \
    char * dif_s = hexdump((expected), (actual), 14);                                                      \
    _as_bprintf(                                                                                           \
        _AS_LPAD _AS_FAILURE_MESSAGE_PRE "assert_mem_eq(" #expected ", " #actual ", " #size ")\n\n"        \
        _AS_LPAD "Expected: %s\n"                                                                          \
        _AS_LPAD "     Got: %s\n",                                                                         \
        __FILE__, __LINE__, exp_s, dif_s);                                                                 \
  }                                                                                                        \
} while (0);

#endif
