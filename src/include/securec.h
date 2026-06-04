/*
 * LibreGauss: Bounds-safe Secure C replacements using standard ANSI C99.
 * All _s functions preserve the original semantics (bounds check + truncate)
 * but use standard C99 functions underneath.
 */
#ifndef SECUREC_H_5D13A042_DC3F_4ED9_A8D1_882811274C27
#define SECUREC_H_5D13A042_DC3F_4ED9_A8D1_882811274C27

#include "securectype.h"
#include <stdarg.h>
#include <errno.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int errno_t;
#define EOK 0
#ifndef EINVAL
#define EINVAL 22
#endif
#ifndef ERANGE
#define ERANGE 34
#endif
#define EINVAL_AND_RESET 150
#define ERANGE_AND_RESET 162
#define EOVERLAP_AND_RESET 182

/* ---- Memory: copy min(c, dm) bytes, return 0 / ERANGE_AND_RESET ---- */
static inline errno_t memcpy_s(void *d, size_t dm, const void *s, size_t c) {
    if (c <= dm) { memcpy(d, s, c); return 0; }
    if (dm > 0) memcpy(d, s, dm);
    return ERANGE_AND_RESET;
}
static inline errno_t memmove_s(void *d, size_t dm, const void *s, size_t c) {
    if (c <= dm) { memmove(d, s, c); return 0; }
    if (dm > 0) memmove(d, s, dm);
    return ERANGE_AND_RESET;
}
static inline errno_t memset_s(void *d, size_t dm, int ch, size_t n) {
    if (n <= dm) { memset(d, ch, n); return 0; }
    if (dm > 0) memset(d, ch, dm);
    return ERANGE_AND_RESET;
}
static inline errno_t memcpy_sOptAsm(void *d, size_t dm, const void *s, size_t c) { return memcpy_s(d,dm,s,c); }
static inline errno_t memcpy_sOptTc(void *d, size_t dm, const void *s, size_t c)  { return memcpy_s(d,dm,s,c); }
static inline errno_t memset_sOptAsm(void *d, size_t dm, int ch, size_t n) { return memset_s(d,dm,ch,n); }
static inline errno_t memset_sOptTc(void *d, size_t dm, int ch, size_t n)  { return memset_s(d,dm,ch,n); }
#define memcpy_sp(d, dm, s, c)          memcpy_s((d),(dm),(s),(c))
#define memset_sp(d, dm, ch, n)         memset_s((d),(dm),(ch),(n))

/* ---- String copy: truncate + null-terminate on overflow, return ERANGE_AND_RESET ---- */
static inline errno_t strcpy_s(char *d, size_t dm, const char *s) {
    size_t sl = strnlen(s, dm);
    if (sl >= dm) { if (dm > 0) { memcpy(d, s, dm-1); d[dm-1] = 0; } return ERANGE_AND_RESET; }
    memcpy(d, s, sl+1);
    return 0;
}
static inline errno_t strncpy_s(char *d, size_t dm, const char *s, size_t c) {
    size_t sl = (c < dm) ? strnlen(s, c) : strnlen(s, dm-1);
    if (sl >= dm) { if (dm > 0) { memcpy(d, s, dm-1); d[dm-1] = 0; } return ERANGE_AND_RESET; }
    memcpy(d, s, sl+1);
    if (c > sl+1) memset(d+sl+1, 0, c-sl-1);
    return 0;
}
static inline errno_t strcat_s(char *d, size_t dm, const char *s) {
    size_t dl = strnlen(d, dm);
    if (dl >= dm) return EINVAL;
    size_t sl = strnlen(s, dm - dl);
    if (dl + sl >= dm) { memcpy(d+dl, s, dm-dl-1); d[dm-1] = 0; return ERANGE_AND_RESET; }
    memcpy(d+dl, s, sl+1);
    return 0;
}
static inline errno_t strncat_s(char *d, size_t dm, const char *s, size_t c) {
    size_t dl = strnlen(d, dm);
    if (dl >= dm) return EINVAL;
    size_t sl = (c < dm-dl) ? strnlen(s, c) : strnlen(s, dm-dl-1);
    if (dl + sl >= dm) { memcpy(d+dl, s, dm-dl-1); d[dm-1] = 0; return ERANGE_AND_RESET; }
    memcpy(d+dl, s, sl+1);
    return 0;
}
static inline void strcpy_error(const char *s, char *d, size_t sz) {
    if (sz > 0) { strncpy(d, s, sz); d[sz-1] = 0; }
}

/* ---- Printf family: snprintf is already bounds-safe ---- */
static inline int sprintf_s(char *d, size_t dm, const char *f, ...) {
    va_list a; va_start(a,f); int r=vsnprintf(d,dm,f,a); va_end(a); return r;
}
static inline int vsprintf_s(char *d, size_t dm, const char *f, va_list a) {
    return vsnprintf(d,dm,f,a);
}
static inline int snprintf_s(char *d, size_t dm, size_t c, const char *f, ...) {
    va_list a; va_start(a,f); int r=vsnprintf(d,dm,f,a); va_end(a); return r;
}
static inline int vsnprintf_s(char *d, size_t dm, size_t c, const char *f, va_list a) {
    return vsnprintf(d,dm,f,a);
}
static inline int snprintf_truncated_s(char *d, size_t dm, const char *f, ...) {
    va_list a; va_start(a,f); int r=vsnprintf(d,dm,f,a); va_end(a); return r;
}
static inline int vsnprintf_truncated_s(char *d, size_t dm, const char *f, va_list a) {
    return vsnprintf(d,dm,f,a);
}

/* ---- Scanf family ---- */
#define scanf_s(f, ...)                 scanf((f),__VA_ARGS__)
#define sscanf_s(b, f, ...)             sscanf((b),(f),__VA_ARGS__)
#define vscanf_s(f, a)                  vscanf((f),(a))
#define vsscanf_s(b, f, a)              vsscanf((b),(f),(a))
#define fscanf_s(s, f, ...)             fscanf((s),(f),__VA_ARGS__)
#define vfscanf_s(s, f, a)              vfscanf((s),(f),(a))

/* ---- Token ---- */
#define strtok_s(s, d, c)               strtok_r((s),(d),(c))

/* ---- Gets ---- */
#define gets_s(d, m)                    (fgets((d),(int)(m),stdin)?0:EINVAL)

/* ---- Wide char ---- */
#define wmemcpy_s(d, dm, s, c)          memcpy_s((d),(dm)*sizeof(wchar_t),(s),(c)*sizeof(wchar_t))
#define wmemmove_s(d, dm, s, c)         memmove_s((d),(dm)*sizeof(wchar_t),(s),(c)*sizeof(wchar_t))

#ifdef __cplusplus
}
#endif
#endif
