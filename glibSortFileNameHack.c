#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <glib.h>

gchar *
(*real_g_utf8_collate_key_for_filename)(const gchar *str,
                 gssize       len);

gchar *
g_utf8_collate_key_for_filename(const gchar *str,
                 gssize       len)
{
  // gssize: len of str OR -1 if null-terminated
  if (len >= 0) {
      len = (gsize)len;
  } else {
      len = strlen(str);
  }

  // raw byte ordering for hashs (05e.. < 517.. < 2f5..)
  if (len == 32 || len == 64) { // md5 or sha
    return g_utf8_collate_key(str, len);
  }

  // numerical ordering (2f5.. < 05e.. < 517..)
  if (!real_g_utf8_collate_key_for_filename) {
    real_g_utf8_collate_key_for_filename = dlsym(RTLD_NEXT, "g_utf8_collate_key_for_filename");
    if (!real_g_utf8_collate_key_for_filename) {
      fprintf(stderr, "Error loading original function: %s\n", dlerror());
      return NULL;
    }
  }

  return real_g_utf8_collate_key_for_filename(str, len);
}


void _init(void)
{
// printf("Loading hack\n");
 real_g_utf8_collate_key_for_filename = dlsym(RTLD_NEXT, "g_utf8_collate_key_for_filename");
}

