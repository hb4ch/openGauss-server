/*
 * LibreGauss: Replace Huawei Secure C with standard ANSI C99.
 * All _s functions are mapped to their standard C equivalents.
 */
#ifndef SECUREC_H_5D13A042_DC3F_4ED9_A8D1_882811274C27
#define SECUREC_H_5D13A042_DC3F_4ED9_A8D1_882811274C27

#include "securectype.h"
#include <stdarg.h>
#include <errno.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

/* errno_t returns 0 on success */
typedef int errno_t;

#define EOK 0
#define ERANGE_AND_RESET 162  /* unused but kept for compat */

/* ---- Memory ---- */
#define memcpy_s(d, dm, s, c)       (memcpy((d), (s), (c)), 0)
#define memmove_s(d, dm, s, c)      (memmove((d), (s), (c)), 0)
#define memset_s(d, dm, c, n)       (memset((d), (c), (n)), 0)
#define memcpy_sOptAsm(d, dm, s, c) (memcpy((d), (s), (c)), 0)
#define memcpy_sOptTc(d, dm, s, c)  (memcpy((d), (s), (c)), 0)
#define memset_sOptAsm(d, dm, c, n) (memset((d), (c), (n)), 0)
#define memset_sOptTc(d, dm, c, n)  (memset((d), (c), (n)), 0)
#define memcpy_sp(d, dm, s, c)      (memcpy((d), (s), (c)))
#define memset_sp(d, dm, c, n)      (memset((d), (c), (n)))

/* ---- String copy ---- */
#define strcpy_s(d, dm, s)          (strcpy((d), (s)), 0)
#define strncpy_s(d, dm, s, c)      (strncpy((d), (s), (c)), 0)
#define strcat_s(d, dm, s)          (strcat((d), (s)), 0)
#define strncat_s(d, dm, s, c)      (strncat((d), (s), (c)), 0)
#define strcpy_error(s, d, sz)      (strncpy((d), (s), (sz)), (void)0)
#define wcscpy_s(d, dm, s)          (wcscpy((d), (s)), 0)
#define wcsncpy_s(d, dm, s, c)      (wcsncpy((d), (s), (c)), 0)

/* ---- Printf family ---- */
#define sprintf_s(d, dm, f, ...)    ((int)snprintf((d), (dm), (f), __VA_ARGS__))
#define vsprintf_s(d, dm, f, a)     (vsnprintf((d), (dm), (f), (a)), 0)
#define snprintf_s(d, dm, c, f, ...)((int)snprintf((d), (dm), (f), __VA_ARGS__))
#define vsnprintf_s(d, dm, c, f, a) (vsnprintf((d), (dm), (f), (a)), 0)
#define snprintf_truncated_s(d, dm, f, ...) ((int)snprintf((d), (dm), (f), __VA_ARGS__))
#define vsnprintf_truncated_s(d, dm, f, a)  (vsnprintf((d), (dm), (f), (a)), 0)

/* ---- Scanf family ---- */
#define scanf_s(f, ...)             scanf((f), __VA_ARGS__)
#define sscanf_s(b, f, ...)         sscanf((b), (f), __VA_ARGS__)
#define vscanf_s(f, a)              vscanf((f), (a))
#define vsscanf_s(b, f, a)          vsscanf((b), (f), (a))
#define fscanf_s(s, f, ...)         fscanf((s), (f), __VA_ARGS__)
#define vfscanf_s(s, f, a)          vfscanf((s), (f), (a))

/* ---- Token ---- */
#define strtok_s(s, d, c)           strtok_r((s), (d), (c))

/* ---- Gets ---- */
#define gets_s(d, m)                (fgets((d), (int)(m), stdin) != NULL ? 0 : EINVAL)

/* ---- Wide char ---- */
#define wmemcpy_s(d, dm, s, c)      (wmemcpy((d), (s), (c)), 0)
#define wmemmove_s(d, dm, s, c)     (wmemmove((d), (s), (c)), 0)

#ifdef __cplusplus
}
#endif
#endif
