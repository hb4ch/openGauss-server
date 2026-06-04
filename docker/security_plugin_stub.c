/* LibreGauss: stub for security_plugin shared library */
#include "postgres.h"
#include "fmgr.h"

PG_MODULE_MAGIC;

extern "C" void _PG_init(void);
extern "C" void _PG_fini(void);

void _PG_init(void)
{
    ereport(LOG, (errmsg("LibreGauss security_plugin stub loaded")));
}

void _PG_fini(void)
{
}
