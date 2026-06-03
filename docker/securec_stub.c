#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <errno.h>
#include <wchar.h>

typedef int errno_t;

errno_t memcpy_s(void *dest, size_t destMax, const void *src, size_t count)
{
    if (dest == NULL || src == NULL || destMax == 0) return EINVAL;
    if (count > destMax) return ERANGE;
    memcpy(dest, src, count);
    return 0;
}

errno_t memmove_s(void *dest, size_t destMax, const void *src, size_t count)
{
    if (dest == NULL || src == NULL || destMax == 0) return EINVAL;
    if (count > destMax) return ERANGE;
    memmove(dest, src, count);
    return 0;
}

errno_t memset_s(void *dest, size_t destMax, int c, size_t count)
{
    if (dest == NULL || destMax == 0) return EINVAL;
    if (count > destMax) return ERANGE;
    memset(dest, c, count);
    return 0;
}

errno_t strcpy_s(char *strDest, size_t destMax, const char *strSrc)
{
    if (strDest == NULL || strSrc == NULL || destMax == 0) return EINVAL;
    size_t slen = strlen(strSrc);
    if (slen >= destMax) return ERANGE;
    strcpy(strDest, strSrc);
    return 0;
}

errno_t strncpy_s(char *strDest, size_t destMax, const char *strSrc, size_t count)
{
    if (strDest == NULL || strSrc == NULL || destMax == 0) return EINVAL;
    size_t slen = strnlen(strSrc, count);
    if (slen >= destMax) return ERANGE;
    strncpy(strDest, strSrc, count);
    return 0;
}

errno_t strcat_s(char *strDest, size_t destMax, const char *strSrc)
{
    if (strDest == NULL || strSrc == NULL || destMax == 0) return EINVAL;
    size_t dlen = strnlen(strDest, destMax);
    if (dlen >= destMax) return ERANGE;
    size_t slen = strlen(strSrc);
    if (dlen + slen >= destMax) return ERANGE;
    strcat(strDest, strSrc);
    return 0;
}

errno_t strncat_s(char *strDest, size_t destMax, const char *strSrc, size_t count)
{
    if (strDest == NULL || strSrc == NULL || destMax == 0) return EINVAL;
    size_t dlen = strnlen(strDest, destMax);
    if (dlen >= destMax) return ERANGE;
    size_t slen = strnlen(strSrc, count);
    if (dlen + slen >= destMax) return ERANGE;
    strncat(strDest, strSrc, count);
    return 0;
}

errno_t sprintf_s(char *strDest, size_t destMax, const char *format, ...)
{
    if (strDest == NULL || destMax == 0 || format == NULL) return EINVAL;
    va_list args;
    va_start(args, format);
    int ret = vsnprintf(strDest, destMax, format, args);
    va_end(args);
    return (ret < 0) ? EINVAL : 0;
}

errno_t vsprintf_s(char *strDest, size_t destMax, const char *format, va_list arglist)
{
    if (strDest == NULL || destMax == 0 || format == NULL) return EINVAL;
    int ret = vsnprintf(strDest, destMax, format, arglist);
    return (ret < 0) ? EINVAL : 0;
}

errno_t snprintf_s(char *strDest, size_t destMax, size_t count, const char *format, ...)
{
    if (strDest == NULL || destMax == 0 || format == NULL) return EINVAL;
    size_t real_count = (count > destMax) ? destMax : count;
    va_list args;
    va_start(args, format);
    int ret = vsnprintf(strDest, real_count, format, args);
    va_end(args);
    return (ret < 0) ? EINVAL : 0;
}

errno_t vsnprintf_s(char *strDest, size_t destMax, size_t count, const char *format, va_list arglist)
{
    if (strDest == NULL || destMax == 0 || format == NULL) return EINVAL;
    size_t real_count = (count > destMax) ? destMax : count;
    int ret = vsnprintf(strDest, real_count, format, arglist);
    return (ret < 0) ? EINVAL : 0;
}

errno_t strtok_s(char *strToken, const char *strDelimit, char **context)
{
    if (strDelimit == NULL || context == NULL) return EINVAL;
    char *result = strtok_r(strToken, strDelimit, context);
    return (result == NULL && strToken != NULL) ? ERANGE : 0;
}

errno_t gets_s(char *strDest, size_t destMax)
{
    if (strDest == NULL || destMax == 0) return EINVAL;
    if (fgets(strDest, (int)destMax, stdin) == NULL) return EINVAL;
    return 0;
}

errno_t wmemcpy_s(wchar_t *dest, size_t destMax, const wchar_t *src, size_t count)
{
    if (dest == NULL || src == NULL || destMax == 0) return EINVAL;
    if (count > destMax) return ERANGE;
    wmemcpy(dest, src, count);
    return 0;
}

errno_t wmemmove_s(wchar_t *dest, size_t destMax, const wchar_t *src, size_t count)
{
    if (dest == NULL || src == NULL || destMax == 0) return EINVAL;
    if (count > destMax) return ERANGE;
    wmemmove(dest, src, count);
    return 0;
}

int vscanf_s(const char *format, va_list arglist) { return vscanf(format, arglist); }
int scanf_s(const char *format, ...) { va_list args; va_start(args, format); int ret = vscanf(format, args); va_end(args); return ret; }
int sscanf_s(const char *buffer, const char *format, ...) { va_list args; va_start(args, format); int ret = vsscanf(buffer, format, args); va_end(args); return ret; }
int vsscanf_s(const char *buffer, const char *format, va_list arglist) { return vsscanf(buffer, format, arglist); }
int fscanf_s(FILE *stream, const char *format, ...) { va_list args; va_start(args, format); int ret = vfscanf(stream, format, args); va_end(args); return ret; }
int vfscanf_s(FILE *stream, const char *format, va_list arglist) { return vfscanf(stream, format, arglist); }

/* Additional securec functions needed by openGauss */
errno_t memcpy_sOptAsm(void *dest, size_t destMax, const void *src, size_t count)
{
    return memcpy_s(dest, destMax, src, count);
}
errno_t snprintf_truncated_s(char *strDest, size_t destMax, const char *format, ...)
{
    va_list args; va_start(args, format);
    int ret = vsnprintf(strDest, destMax, format, args); va_end(args);
    return (ret < 0) ? EINVAL : 0;
}
errno_t vsnprintf_truncated_s(char *strDest, size_t destMax, const char *format, va_list arglist)
{
    int ret = vsnprintf(strDest, destMax, format, arglist);
    return (ret < 0) ? EINVAL : 0;
}
void strcpy_error(const char *src, char *dst, size_t dst_size)
{
    if (dst != NULL && dst_size > 0) {
        size_t slen = src ? strnlen(src, dst_size - 1) : 0;
        memcpy(dst, src ? src : "", slen);
        dst[slen] = '\0';
    }
}
errno_t memset_sOptAsm(void *d, size_t dm, int c, size_t n) { return memset_s(d,dm,c,n); }
