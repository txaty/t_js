#pragma once
#include <stdint.h>
#include <stdio.h>

#define JS_VERSION "2.0.8"

#define F_NOEXEC 1  // Parse code, but not execute
#define F_LOOP 2    // We're inside the loop
#define F_CALL 4    // We're inside a function call
#define F_BREAK 8   // Exit the loop
#define F_RETURN 16 // Return has been executed

#ifndef JS_EXPR_MAX
#define JS_EXPR_MAX 20
#endif

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
