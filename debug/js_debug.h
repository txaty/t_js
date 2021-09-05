#pragma once


#define JS_DEBUG 1u

#ifdef JS_DEBUG
#define JS_DUMP js_dump
#define JS_DEBUGF printf
#else
#define JS_DUMP(...)   /**/
#define JS_DEBUGF(...) /**/
#endif

void js_dump(struct js *js);
