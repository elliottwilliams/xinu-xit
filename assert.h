#ifndef ASSERT_H
#define ASSERT_H

#define FAILURE_MESSAGE_PRE "in %s:%d: "
#define LPAD "    "

#define assert_eq(lhs, rhs) do {                                         \
  if ((lhs) != (rhs)) {                                                  \
    _test_res->status = FAILURE;                                         \
    sprintf(_msgbuf,                                                     \
        LPAD FAILURE_MESSAGE_PRE "assert_eq(" #lhs ", " #rhs ")\n\n"     \
        LPAD "Expected: true\n"                                          \
        LPAD "     Got: false\n",                                        \
        __FILE__, __LINE__);                                             \
    return;                                                              \
  }                                                                      \
} while (0);

#define assert_eq_fmt(lhs, rhs, fmt) do {                                \
  if ((lhs) != (rhs)) {                                                  \
    _test_res->status = FAILURE;                                         \
    sprintf(_msgbuf,                                                     \
        LPAD FAILURE_MESSAGE_PRE "assert_str_eq(" #lhs ", " #rhs ")\n\n" \
        LPAD "Expected: " fmt "\n"                                       \
        LPAD "     Got: " fmt "\n",                                      \
        __FILE__, __LINE__, (rhs), (lhs));                               \
    return;                                                              \
  }                                                                      \
} while (0);

#define assert_str_eq(lhs, rhs) do {                                     \
  if (strcmp((lhs), (rhs)) != 0) {                                       \
    _test_res->status = FAILURE;                                         \
    sprintf(_msgbuf,                                                     \
        LPAD FAILURE_MESSAGE_PRE "assert_str_eq(" #lhs ", " #rhs ")\n\n" \
        LPAD "Expected: \"%s\"\n"                                        \
        LPAD "     Got: \"%s\"\n",                                       \
        __FILE__, __LINE__, (rhs), (lhs));                               \
    return;                                                              \
  }                                                                      \
} while (0);


#endif
