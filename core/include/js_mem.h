#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "js_types.h"

#define MARK ~(((jsoff_t)~0) >> 1) // Entity deletion marker

void saveoff(JS *js, jsoff_t off, jsoff_t val);
void saveval(JS *js, jsoff_t off, jsval_t val);
jsval_t loadval(JS *js, jsoff_t off);
jsoff_t loadoff(JS *js, jsoff_t off);
jsoff_t js_alloc(JS *js, size_t size);
bool is_mem_entity(uint8_t t);
void js_fixup_offsets(JS *js, jsoff_t start, jsoff_t size);
uint8_t getri(uint32_t mask, uint8_t ri);