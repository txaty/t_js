#pragma once
#include <stdint.h>
#include <stdio.h>
#include "js_types.h"
#include "js_driver.h"

#define JS_VERSION_MAJOR 0x02
#define JS_VERSION_MINOR 0x00
#define JS_VERSION_PATCH 0x08
#define JS_VERSION_INFO "dev"
#define JS_VERSION "2.0.8"

#ifndef JS_CFG_MEM_SIZE
#define JS_CFG_MEM_SIZE 256u
#endif

JS *js_create(void *buf, size_t len);              // Create JS instance
const char *js_str(JS *, jsval_t val);             // Stringify JS value
jsval_t js_eval(JS *, const char *, size_t);       // Execute JS code
jsval_t js_glob(JS *);                             // Return global object
jsval_t js_mkobj(JS *);                            // Create a new object
jsval_t js_import(JS *, uintptr_t, const char *);  // Import native func
void js_set(JS *, jsval_t, const char *, jsval_t); // Set obj attribute
int js_usage(JS *);                                // Return mem usage
jsval_t js_execute_script(JS *js, const char *filename);  // Execute JS code from .js file
