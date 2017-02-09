#ifndef ASSERT_H
#define ASSERT_H

#define _AS_FAILURE_MESSAGE_PRE "in %s:%d: "
#define _AS_LPAD "    "
#define _AS_EXPPAD "          "
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

// Store in `out` a hex dump of `size` bytes after `va`, highlighting bytes
// where `va` differs from `vb`. Begin each line with `padstr`.
void hexcmp(char * out, const char * va, const char * vb, int size, char * padstr);
int hexcmp_buflen(int size, char * padstr);

#define assert_mem_eq(expected, actual, size) do {                                                  \
  if (memcmp((expected), (actual), (size)) != 0) {                                                  \
    _AS_FAIL;                                                                                       \
    int len = hexcmp_buflen(size, _AS_LPAD _AS_EXPPAD);                                             \
    char * exp_s = getmem(len);                                                                     \
    if (exp_s == (char*) SYSERR) exp_s = "<hexcmp error>";                                          \
    char * act_s = getmem(len);                                                                     \
    if (act_s == (char*) SYSERR) act_s = "<hexcmp error>";                                          \
    char * cmp_s = getmem(len);                                                                     \
    if (cmp_s == (char*) SYSERR) cmp_s = "<hexcmp error>";                                          \
                                                                                                    \
    hexcmp(exp_s, (const char *) (expected), (const char *) (expected),                             \
           (size), _AS_LPAD _AS_EXPPAD);                                                            \
    hexcmp(act_s, (const char *) (actual), (const char *) (actual),                                 \
           (size), _AS_LPAD _AS_EXPPAD);                                                            \
    hexcmp(cmp_s, (const char *) (expected), (const char *) (actual),                               \
           (size), _AS_LPAD _AS_EXPPAD);                                                            \
    _as_bprintf(                                                                                    \
        _AS_LPAD _AS_FAILURE_MESSAGE_PRE "assert_mem_eq(" #expected ", " #actual ", " #size ")\n\n" \
        _AS_LPAD "Expected: %s\n\n"                                                                 \
        _AS_LPAD "          to equal\n\n"                                                           \
        _AS_LPAD "          %s\n\n"                                                                 \
        _AS_LPAD "     Got: %s\n",                                                                  \
        __FILE__, __LINE__, exp_s, act_s, cmp_s);                                                   \
    freemem(exp_s, len);                                                                            \
    freemem(act_s, len);                                                                            \
  }                                                                                                 \
} while (0);

#endif

