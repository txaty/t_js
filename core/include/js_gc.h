#pragma once
#include "js_types.h"

void js_delete_marked_entities(JS *js);
void js_mark_all_entities_for_deletion(JS *js);
void js_unmark_used_entities(JS *js);
void js_gc(JS *js);