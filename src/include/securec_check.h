/*
 * LibreGauss: Standard C99 doesn't need these safety checks.
 * All securec_check macros are no-ops.
 */
#ifndef _SECUREC_CHECK_H_
#define _SECUREC_CHECK_H_

#define freeSecurityFuncSpace_c(str1, str2)           ((void)(str1), (void)(str2))
#define securec_check_c(errno, str1, str2)            ((void)(errno), (void)(str1), (void)(str2))
#define securec_check_ss_c(errno, str1, str2)         ((void)(errno), (void)(str1), (void)(str2))
#define securec_check_intval_core(val, express)       ((void)(val), (void)(express))

#ifndef GDS_SERVER
#define securec_check(errno, str1, str2)              ((void)(errno), (void)(str1), (void)(str2))
#define securec_check_ss(errno, str1, str2)           ((void)(errno), (void)(str1), (void)(str2))
#endif

#endif
