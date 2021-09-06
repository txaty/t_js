#pragma once
#include "js_types.h"


#ifdef JS_CFG_32_BIT

jsval_t tov(float d);
float tod(jsval_t v);
jsval_t mkval(uint8_t type, unsigned long data);
bool is_nan(jsval_t v);
uint8_t vtype(jsval_t v);
unsigned long vdata(jsval_t v);
jsval_t mkcoderef(jsval_t off, jsoff_t len);
jsoff_t coderefoff(jsval_t v);
jsoff_t codereflen(jsval_t v);

#else

jsval_t tov(double d);
double tod(jsval_t v);
jsval_t mkval(uint8_t type, unsigned long data);
bool is_nan(jsval_t v);
uint8_t vtype(jsval_t v);
unsigned long vdata(jsval_t v);
jsval_t mkcoderef(jsval_t off, jsoff_t len);
jsoff_t coderefoff(jsval_t v);
jsoff_t codereflen(jsval_t v);

#endif