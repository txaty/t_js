#include <assert.h>
#include "js_scope.h"
#include "js_include.h"

void mkscope(JS *js)
{
  assert((js->flags & F_NOEXEC) == 0);
  jsoff_t prev = vdata(js->scope);
  js->scope = mkobj(js, prev);
  // printf("ENTER SCOPE %u, prev %u\n", (jsoff_t) vdata(js->scope), prev);
}

void delscope(JS *js)
{
  js->scope = upper(js, js->scope);
  // printf("EXIT  SCOPE %u\n", (jsoff_t) vdata(js->scope));
}

// Lookup variable in the scope chain
jsval_t lookup(JS *js, const char *buf, size_t len)
{
  for (jsval_t scope = js->scope;;)
  {
    jsoff_t off = lkp(js, scope, buf, len);
    if (off != 0)
      return mkval(JS_PROP, off);
    if (vdata(scope) == 0)
      break;
    scope = mkval(JS_OBJ, loadoff(js, vdata(scope) + sizeof(jsoff_t)));
  }
  return js_err(js, "'%.*s' not found", (int)len, buf);
}