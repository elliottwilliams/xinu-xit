/**
 * Add the signatures of functions to mock here. 
 *
 * For functions that return a value, use XIT_VALUE_FAKES.
 * Semantics are:
 *    F(<wrapped function name>, <proxy function name>, <return type>,
 *      <arg1 type>, <arg2 type>, ...)
 *
 * Example:
 *
 *    #define XIT_VALUE_FAKES(F) \
 *      F(__wrap_getutime, __real_getutime, status, uint32 *);
 *
 * For void functions, use XIT_VOID_FAKES.
 * Semantics are:
 *    F(<wrapped function name>, <proxy function name>, <arg1 type>,
 *      <arg2 type>, ...)
 *
 * Example:
 *
 *    #define XIT_VOID_FAKES(F) \
 *      F(__wrap_ip6in, __real_ip6in, struct netpacket *, struct ip6packet *);
 *
 * Functions listed here get declared as wrappers in `fake.h`. They produce
 * metadata structures `<function_name>_fake`, which contain information about
 * call history, and allow for custom handling.
 *
 * See fff's documentation for details: <https://github.com/meekrosoft/fff>
 */

#define XIT_VALUE_FAKES(F) \
  F(__wrap_getutime, __real_getutime, status, uint32 *); \
  F(__wrap_freebuf,  __real_freebuf,  syscall, char *); \
  ;

#define XIT_VOID_FAKES(F) \
  F(__wrap_icmp6in,  __real_icmp6in,  struct netpacket *, struct ip6packet *, struct icmp6msg *); \
  F(__wrap_ip6_hton, __real_ip6_hton, struct ip6packet *); \
  F(__wrap_ip6_ntoh, __real_ip6_ntoh, struct ip6packet *); \
  ;
