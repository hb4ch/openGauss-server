#ifndef ERRMSG_H
#define ERRMSG_H
#include <stdio.h>
#include <stddef.h>
#define STRING_MAX_LEN 1024
#define ERROR_LOCATION_NUM 5
typedef struct { char msg[STRING_MAX_LEN]; char cause[STRING_MAX_LEN]; char action[STRING_MAX_LEN]; } mppdb_detail_errmsg_t;
typedef struct { char szFileName[256]; unsigned int ulLineno; } mppdb_err_msg_location_t;
typedef struct { int ulSqlErrcode; char cSqlState[5]; int mppdb_err_msg_locnum; mppdb_err_msg_location_t *astErrLocate[ERROR_LOCATION_NUM]; mppdb_detail_errmsg_t stErrmsg; char ucOpFlag; } gsqlerr_err_msg_t;
static gsqlerr_err_msg_t g_gsqlerr_errors[] = {{0, "", 0, {0}}};
static gsqlerr_err_msg_t g_mppdb_errors[] = {{0, "", 0, {0}}};
#endif
