#include <stdbool.h>
#include "js_value_size.h"
#include "js_include.h"

#ifdef JS_CFG_32_BIT

// Pack JS values into uin32_t, float -infinity
// 32bit "float": 1 bit sign, 8 bits exponent, 23 bits mantissa
//
//  7f80 0000 = 01111111 10000000 00000000 00000000 = infinity
//  ff80 0000 = 11111111 10000000 00000000 00000000 = −infinity
//  ffc0 0001 = x1111111 11000000 00000000 00000001 = qNaN (on x86 and ARM)
//  ff80 0001 = x1111111 10000000 00000000 00000001 = sNaN (on x86 and ARM)
//
//  seeeeeee|emmmmmmm|mmmmmmmm|mmmmmmmm
//  11111111|1ttttvvv|vvvvvvvv|vvvvvvvv
//    INF     TYPE     PAYLOAD
jsval_t tov(float d)
{
  union
  {
    float d;
    jsval_t v;
  } u = {d};
  return u.v;
}
float tod(jsval_t v)
{
  union
  {
    jsval_t v;
    float d;
  } u = {v};
  return u.d;
}
jsval_t mkval(uint8_t type, unsigned long data)
{
  return ((jsval_t)0xff800000) | ((jsval_t)(type) << 19) | data;
}

bool is_nan(jsval_t v)
{
  return (v >> 23) == 0x1ff;
}

uint8_t vtype(jsval_t v)
{
  return is_nan(v) ? (v >> 19) & 15 : JS_NUM;
}

unsigned long vdata(jsval_t v)
{
  return v & ~((jsval_t)0xfff80000);
}

jsval_t mkcoderef(jsval_t off, jsoff_t len)
{
  return mkval(JS_CODEREF, (off & 0xfff) | ((len & 127) << 12));
}

jsoff_t coderefoff(jsval_t v)
{
  return v & 0xfff;
}

jsoff_t codereflen(jsval_t v)
{
  return (v >> 12) & 127;
}

#else

// Pack JS values into uin64_t, double nan, per IEEE 754
// 64bit "double": 1 bit sign, 11 bits exponent, 52 bits mantissa
//
// seeeeeee|eeeemmmm|mmmmmmmm|mmmmmmmm|mmmmmmmm|mmmmmmmm|mmmmmmmm|mmmmmmmm
// 11111111|11110000|00000000|00000000|00000000|00000000|00000000|00000000 inf
// 11111111|11111000|00000000|00000000|00000000|00000000|00000000|00000000 qnan
//
// 11111111|1111tttt|vvvvvvvv|vvvvvvvv|vvvvvvvv|vvvvvvvv|vvvvvvvv|vvvvvvvv
//  NaN marker |type|  48-bit placeholder for values: pointers, strings
//
// On 64-bit platforms, pointers are really 48 bit only, so they can fit,
// provided they are sign extended
jsval_t tov(double d)
{
  union
  {
    double d;
    jsval_t v;
  } u = {d};
  return u.v;
}

double tod(jsval_t v)
{
  union
  {
    jsval_t v;
    double d;
  } u = {v};
  return u.d;
}
jsval_t mkval(uint8_t type, unsigned long data)
{
  return ((jsval_t)0x7ff0 << 48) | ((jsval_t)(type) << 48) | data;
}

bool is_nan(jsval_t v)
{
  return (v >> 52) == 0x7ff;
}

uint8_t vtype(jsval_t v)
{
  return is_nan(v) ? ((v >> 48) & 15) : (uint8_t)JS_NUM;
}

unsigned long vdata(jsval_t v)
{
  return (unsigned long)(v & ~((jsval_t)0x7fff << 48));
}

jsval_t mkcoderef(jsval_t off, jsoff_t len)
{
  return mkval(JS_CODEREF, (off & 0xffffff) | ((len & 0xffffff) << 24));
}

jsoff_t coderefoff(jsval_t v)
{
  return v & 0xffffff;
}

jsoff_t codereflen(jsval_t v)
{
  return (v >> 24) & 0xffffff;
}

#endif