#pragma once


#ifdef JS_CFG_DEBUG

#define JS_DUMP js_dump
#define JS_DEBUGF printf
void js_dump(struct js *js);

#else

#define JS_DUMP(...)   /**/
#define JS_DEBUGF(...) /**/

#endif
