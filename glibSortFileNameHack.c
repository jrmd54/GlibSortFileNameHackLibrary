#define _GNU_SOURCE
#include <dlfcn.h>
#include <glib.h>
#include <stdio.h>

const gchar *hash_chars = "0123456789abcdef";

gchar *(*real_g_utf8_collate_key_for_filename)(const gchar *str,
                                               gssize len);

gchar *
g_utf8_collate_key_for_filename(const gchar *str,
                                gssize len) {
    // gssize: len of str OR -1 if null-terminated
    if (len >= 0) {
        len = (gsize)len;
    } else {
        len = strlen(str);
    }

    // raw byte ordering for hashs (05e.. < 517.. < 2f5..)
    int is_hash = 1;
    if (len == 32 || len == 64) { // md5 or sha
        for (int i=0; i < len; i++) {
            if (strchr(hash_chars, *(str + i)) == NULL) {
                is_hash = 0;
                break;
            }
        }
        if (is_hash) {
            return g_utf8_collate_key(str, len);
        }
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

void _init(void) {
    // printf("Loading hack\n");
    real_g_utf8_collate_key_for_filename = dlsym(RTLD_NEXT, "g_utf8_collate_key_for_filename");
}
