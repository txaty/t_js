#pragma once
#include <stdint.h>
#include <stdio.h>

#define JS_VERSION "2.0.8"

struct js;                // JS engine (opaque)
typedef uint64_t jsval_t; // JS value placeholder

struct js *js_create(void *buf, size_t len);              // Create JS instance
const char *js_str(struct js *, jsval_t val);             // Stringify JS value
jsval_t js_eval(struct js *, const char *, size_t);       // Execute JS code
jsval_t js_glob(struct js *);                             // Return global object
jsval_t js_mkobj(struct js *);                            // Create a new object
jsval_t js_import(struct js *, uintptr_t, const char *);  // Import native func
void js_set(struct js *, jsval_t, const char *, jsval_t); // Set obj attribute`
int js_usage(struct js *);                                // Return mem usage
