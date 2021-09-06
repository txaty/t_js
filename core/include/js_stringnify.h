#pragma once
#include <stdio.h>
#include "js_types.h"

// Stringify string JS value
size_t strstring(JS *js, jsval_t value, char *buf, size_t len);
// Stringify JS object
size_t strobj(JS *js, jsval_t obj, char *buf, size_t len);
// Stringify numeric JS value
size_t strnum(jsval_t value, char *buf, size_t len);
// Stringify JS function
size_t strfunc(JS *js, jsval_t value, char *buf, size_t len);
// Stringify JS value into the given buffer
size_t tostr(JS *js, jsval_t value, char *buf, size_t len);