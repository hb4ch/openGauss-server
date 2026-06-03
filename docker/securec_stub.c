/* LibreGauss: stub library for any direct securec function calls not caught by macros */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <wchar.h>
typedef int errno_t;
#define EOK 0

errno_t memcpy_s(void *d, size_t dm, const void *s, size_t c) { memcpy(d,s,c); return EOK; }
errno_t memmove_s(void *d, size_t dm, const void *s, size_t c) { memmove(d,s,c); return EOK; }
errno_t memset_s(void *d, size_t dm, int ch, size_t n) { memset(d,ch,n); return EOK; }
errno_t strcpy_s(char *d, size_t dm, const char *s) { strcpy(d,s); return EOK; }
errno_t strncpy_s(char *d, size_t dm, const char *s, size_t c) { strncpy(d,s,c); return EOK; }
errno_t strcat_s(char *d, size_t dm, const char *s) { strcat(d,s); return EOK; }
errno_t strncat_s(char *d, size_t dm, const char *s, size_t c) { strncat(d,s,c); return EOK; }
errno_t sprintf_s(char *d, size_t dm, const char *f, ...) { va_list a; va_start(a,f); int r=vsnprintf(d,dm,f,a); va_end(a); return r<0 ? -1 : EOK; }
errno_t vsprintf_s(char *d, size_t dm, const char *f, va_list a) { int r=vsnprintf(d,dm,f,a); return r<0 ? -1 : EOK; }
errno_t snprintf_s(char *d, size_t dm, size_t c, const char *f, ...) { va_list a; va_start(a,f); int r=vsnprintf(d,dm,f,a); va_end(a); return r<0 ? -1 : EOK; }
errno_t vsnprintf_s(char *d, size_t dm, size_t c, const char *f, va_list a) { int r=vsnprintf(d,dm,f,a); return r<0 ? -1 : EOK; }
errno_t memcpy_sOptAsm(void *d, size_t dm, const void *s, size_t c) { memcpy(d,s,c); return EOK; }
errno_t memcpy_sOptTc(void *d, size_t dm, const void *s, size_t c) { memcpy(d,s,c); return EOK; }
errno_t memset_sOptAsm(void *d, size_t dm, int ch, size_t n) { memset(d,ch,n); return EOK; }
errno_t memset_sOptTc(void *d, size_t dm, int ch, size_t n) { memset(d,ch,n); return EOK; }
errno_t snprintf_truncated_s(char *d, size_t dm, const char *f, ...) { va_list a; va_start(a,f); int r=vsnprintf(d,dm,f,a); va_end(a); return r<0 ? -1 : EOK; }
errno_t vsnprintf_truncated_s(char *d, size_t dm, const char *f, va_list a) { int r=vsnprintf(d,dm,f,a); return r<0 ? -1 : EOK; }
void strcpy_error(const char *s, char *d, size_t sz) { strncpy(d,s,sz); d[sz-1]=0; }
errno_t wmemcpy_s(wchar_t *d, size_t dm, const wchar_t *s, size_t c) { wmemcpy(d,s,c); return EOK; }
errno_t wmemmove_s(wchar_t *d, size_t dm, const wchar_t *s, size_t c) { wmemmove(d,s,c); return EOK; }
errno_t gets_s(char *d, size_t m) { return fgets(d,(int)m,stdin)?EOK:-1; }
