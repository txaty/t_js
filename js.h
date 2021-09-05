#pragma once
#include <stdint.h>
#include <stdio.h>

#define F_NOEXEC 1  // Parse code, but not execute
#define F_LOOP 2    // We're inside the loop
#define F_CALL 4    // We're inside a function call
#define F_BREAK 8   // Exit the loop
#define F_RETURN 16 // Return has been executed

#ifndef JS_EXPR_MAX
#define JS_EXPR_MAX 20
#endif

// struct js;
// JS engine (opaque)
struct js
{
    const char *code; // Currently parsed code snippet
    char errmsg[36];  // Error message placeholder
    uint8_t tok;      // Last parsed token value
    uint8_t flags;    // Execution flags, see F_* enum below
    uint16_t lev;     // Recursion level
    jsoff_t clen;     // Code snippet length
    jsoff_t pos;      // Current parsing position
    jsoff_t toff;     // Offset of the last parsed token
    jsoff_t tlen;     // Length of the last parsed token
    jsval_t tval;     // Holds last parsed numeric or string literal value
    jsval_t scope;    // Current scope
    uint8_t *mem;     // Available JS memory
    jsoff_t size;     // Memory size
    jsoff_t brk;      // Current mem usage boundary
    jsoff_t ncbs;     // Number of FFI-ed C "userdata" callback pointers
};
typedef uint64_t jsval_t; // JS value placeholder
typedef uint32_t jsoff_t;

struct js *js_create(void *buf, size_t len);              // Create JS instance
const char *js_str(struct js *, jsval_t val);             // Stringify JS value
jsval_t js_eval(struct js *, const char *, size_t);       // Execute JS code
jsval_t js_glob(struct js *);                             // Return global object
jsval_t js_mkobj(struct js *);                            // Create a new object
jsval_t js_import(struct js *, uintptr_t, const char *);  // Import native func
void js_set(struct js *, jsval_t, const char *, jsval_t); // Set obj attribute
int js_usage(struct js *);                                // Return mem usage


#define JS_VERSION_MAJOR 0x02
#define JS_VERSION_MINOR 0x00
#define JS_VERSION_PATCH 0x08
#define JS_VERSION_INFO "dev"
#define JS_VERSION "2.0.8"