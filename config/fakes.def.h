/**
 * Add the signatures of functions to mock here. 
 *
 * For functions that return a value, use FAKE_VALUE_FUNCTIONS.
 * Semantics are:
 *    F(<return type>, <function name>, <arg1 type>, <arg2 type>, ...)
 *
 * Example:
 *
 *    #define FAKE_VALUE_FUNCTION_LIST(F) \
 *      F(status, __wrap_getutime, uint32 *);
 *
 * For void functions, use FAKE_VOID_FUNCTIONS.
 * Semantics are:
 *    F(<function name, <arg1 type>, <arg2 type>, ...)
 *
 * Example:
 *
 *    #define FAKE_VOID_FUNCTION_LIST(F) \
 *      F(__wrap_ip6in, struct netpacket *, struct ip6packet *);
 *
 * Functions listed here get declared as wrappers in `fake.h`. They produce
 * metadata structures `<function_name>_fake`, which contain information about
 * call history, and allow for custom handling.
 *
 * See fff's documentation for details: <https://github.com/meekrosoft/fff>
 */

#define FAKE_VALUE_FUNCTION_LIST(F) \
  ;

#define FAKE_VOID_FUNCTION_LIST(F) \
  ;
