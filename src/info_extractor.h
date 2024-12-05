#include "postgres.h"

#include <math.h>
#include "postgres.h"

#include <float.h>

#include "access/amapi.h"
#include "access/reloptions.h"
#include "commands/progress.h"
#include "commands/vacuum.h"
#include "utils/guc.h"
#include "utils/selfuncs.h"
#include "utils/spccache.h"
#include "access/relscan.h" // For IndexScanDesc

#include "catalog/pg_type.h"
#include "common/shortest_dec.h"
#include "fmgr.h"
#include "executor/spi.h"
#include "lib/stringinfo.h"
#include "libpq/pqformat.h"
#include "port.h"				/* for strtof() */
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/float.h"
#include "utils/lsyscache.h"
#include "utils/numeric.h"
#include "vector.h"
#include "commands/defrem.h"
#include "access/htup_details.h"
#include "utils/elog.h"
#include "funcapi.h"


#include "postgres.h"
#include "nodes/plannodes.h"
#include "nodes/pg_list.h"
#include "optimizer/planner.h"
#include "optimizer/cost.h"


static char *extract_table_name(FunctionScan *funcScan) {
    ListCell *lc;

    foreach (lc, funcScan->functions) {
        RangeTblFunction *rtfunc = (RangeTblFunction *) lfirst(lc);

        if (rtfunc->funcexpr && IsA(rtfunc->funcexpr, FuncExpr)) {
            FuncExpr *funcExpr = (FuncExpr *) rtfunc->funcexpr;

            /* Check if the function has at least two arguments */
            if (list_length(funcExpr->args) >= 2) {
                /* Extract the second argument (table_name_text) */
                Node *tableNode = (Node *) list_nth(funcExpr->args, 1);
                if (IsA(tableNode, Const)) {
                    Const *tableConst = (Const *) tableNode;
                    if (!tableConst->constisnull) {
                        /* Convert the text Datum to a C string */
                        text *tableText = DatumGetTextPP(tableConst->constvalue);
                        char *tableName = text_to_cstring(tableText);
                        elog(LOG, "Extracted table_name_text: %s", tableName);
                        return tableName;
                    }
                }
            }
        }
    }

    elog(LOG, "Could not extract table_name_text, returning NULL");
    return NULL;
}


static int extract_limit(FunctionScan *funcScan) {
    ListCell *lc;

    foreach (lc, funcScan->functions) {
        RangeTblFunction *rtfunc = (RangeTblFunction *) lfirst(lc);

        if (rtfunc->funcexpr && IsA(rtfunc->funcexpr, FuncExpr)) {
            FuncExpr *funcExpr = (FuncExpr *) rtfunc->funcexpr;

            /* Check if the function has at least four arguments */
            if (list_length(funcExpr->args) >= 4) {
                /* Extract the fourth argument (limit) */
                Node *limitNode = (Node *) list_nth(funcExpr->args, 3);
                if (IsA(limitNode, Const)) {
                    Const *limitConst = (Const *) limitNode;
                    if (!limitConst->constisnull) {
                        /* Convert the integer Datum to a C integer */
                        int limit = DatumGetInt32(limitConst->constvalue);
                        elog(LOG, "Extracted limit: %d", limit);
                        return limit;
                    }
                }
            }
        }
    }

    elog(LOG, "Could not extract limit, defaulting to 0");
    return 0;  // Default value
}