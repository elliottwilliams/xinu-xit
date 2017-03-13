#pragma once

#include <test/fff.h>

#include <test/fakes.def>

// Reset fakes between tests. Called by `local_runner` after each test.
void reset_fakes();

#define XIT_RESET(NAME, ...) \
  RESET_FAKE(NAME)

#define XIT_DECL_VALUE_FAKE(NAME, PROXY, TYPE, ...) \
  TYPE PROXY(__VA_ARGS__); \
  DECLARE_FAKE_VALUE_FUNC(TYPE, NAME, __VA_ARGS__)
#define XIT_DECL_VOID_FAKE(NAME, PROXY, ...) \
  void PROXY(__VA_ARGS__); \
  DECLARE_FAKE_VOID_FUNC(NAME, __VA_ARGS__)

#define XIT_DEFN_VALUE_FAKE(NAME, PROXY, TYPE, ...) \
  DEFINE_FAKE_VALUE_FUNC(TYPE, NAME, __VA_ARGS__); \
  NAME##_Fake NAME##_fake = { .custom_fake = PROXY };
#define XIT_DEFN_VOID_FAKE(NAME, PROXY, ...) \
  DEFINE_FAKE_VOID_FUNC(NAME, __VA_ARGS__); \
  NAME##_Fake NAME##_fake = { .custom_fake = PROXY };

#define XIT_REPROXY(NAME, PROXY, ...) \
  NAME##_fake.custom_fake = PROXY;

XIT_VALUE_FAKES(XIT_DECL_VALUE_FAKE);
XIT_VOID_FAKES(XIT_DECL_VOID_FAKE);
