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

#include "info_extractor.h"
#include "hashtable_store.h"


static planner_hook_type prev_planner_hook = NULL;

Hashtable *tableSize_mapper;
void initialize_mapper() {
    tableSize_mapper = create_hashtable();
    table_insert(tableSize_mapper, "vectors", 18);
    table_insert(tableSize_mapper, "aka_title", 361472);
    table_insert(tableSize_mapper, "char_name", 3140339);
    table_insert(tableSize_mapper, "company_name", 234997);
    table_insert(tableSize_mapper, "keyword", 134170);
    table_insert(tableSize_mapper, "title", 2528312);
    table_insert(tableSize_mapper, "person_info", 2963664);
}


double vector_knn_cost_estimation(FunctionScan *funcScan){

    initialize_mapper();

    int topK, tableSize;
    double plannedRows;
    char * table_name;

    topK = extract_limit(funcScan);
    table_name = extract_table_name(funcScan);
    tableSize = get_value(tableSize_mapper, table_name);
    
    if (tableSize == -1){
        tableSize = 10000;
        elog(ERROR, "Table size not found in the mapper");}

    double c1 = 0.006; // Cost per distance computation
    double c2 = 0.001; // Cost per sorting operation
    
    double distance_cost = c1 * tableSize; // Cost of computing distances
    double sort_cost = c2 * tableSize * log(tableSize); // Cost of sorting
    double total_cost = distance_cost + sort_cost;

    return total_cost;
    // elog(LOG, "---- We are in new cost estimation function ----");
    // elog(LOG, "---- topK: %d ----", topK);
    // elog(LOG, "---- table_name: %s ----", table_name);
}



static bool is_knn_function(RangeTblEntry *rte) {
    /* Check if the RangeTblEntry represents a function RTE */
    if (rte->rtekind == RTE_FUNCTION) {
        ListCell *lc;

        /* Traverse the list of functions in the RTE */
        foreach (lc, rte->functions) {
            RangeTblFunction *rtfunc = (RangeTblFunction *) lfirst(lc);

            /* Check if the function name matches "vector_k_nearest_neighbor" */
            if (rtfunc->funcexpr && IsA(rtfunc->funcexpr, FuncExpr)) {
                FuncExpr *funcExpr = (FuncExpr *) rtfunc->funcexpr;

                /* Get the function's OID */
                Oid funcOid = funcExpr->funcid;

                /* Retrieve the function name */
                char *funcName = get_func_name(funcOid);

                if (funcName && strcmp(funcName, "vector_k_nearest_neighbor") == 0) {
                    return true; /* Found a match */
                }
            }
        }
    }
    return false; /* Not a match */
}


static bool is_knn_function_funcscan(FunctionScan *funcScan) {
    ListCell *lc;

    /* Iterate over the functions in the FunctionScan node */
    foreach (lc, funcScan->functions) {
        RangeTblFunction *rtfunc = (RangeTblFunction *) lfirst(lc);

        /* Check if the function expression is valid */
        if (rtfunc->funcexpr && IsA(rtfunc->funcexpr, FuncExpr)) {
            FuncExpr *funcExpr = (FuncExpr *) rtfunc->funcexpr;

            /* Get the function OID */
            Oid funcOid = funcExpr->funcid;

            /* Retrieve the function name */
            char *funcName = get_func_name(funcOid);

            /* Log the function name for debugging */
            elog(LOG, "Checking FunctionScan function: %s", funcName ? funcName : "NULL");

            /* Compare the function name to "vector_k_nearest_neighbor" */
            if (funcName && strcmp(funcName, "vector_k_nearest_neighbor") == 0) {
                return true; /* Found a match */
            }
        }
    }

    return false; /* No match found */
}


static void adjust_plan_cost(Plan *plan) {
    elog(LOG, "We are in the adjust plan cost function");
    if (plan == NULL) {
        return;
    }

    /* Example of applying a fixed cost for specific nodes */
    if (IsA(plan, FunctionScan)) {

        FunctionScan *funcScan = (FunctionScan *) plan;

        /* Check if the function being scanned is our KNN function */
        if (is_knn_function_funcscan(funcScan)) {
            double currCost;
            currCost = vector_knn_cost_estimation(plan);
            plan->startup_cost = 0.01;
            plan->total_cost = currCost;
            plan->plan_rows = extract_limit(funcScan);
        }
    }

    if (plan->initPlan != NULL) {
        elog(LOG, "Skipping initPlan processing");
    }

    /* Recursively adjust costs for child nodes */
    if (plan->lefttree) {
        adjust_plan_cost(plan->lefttree);
    }
    if (plan->righttree) {
        adjust_plan_cost(plan->righttree);
    }

    /* Adjust costs for any init plans (stored as SubPlan nodes) */
    ListCell *lc;
    foreach (lc, plan->initPlan) {
        SubPlan *subPlan = (SubPlan *) lfirst(lc);
        elog(LOG, "Encountered SubPlan, skipping cost adjustment");}
}


static bool query_contains_knn_function(Query *parse) {
    ListCell *lc;

    /* Traverse the range table entries */
    foreach (lc, parse->rtable) {
        RangeTblEntry *rte = (RangeTblEntry *) lfirst(lc);

        /* Check if the RangeTblEntry represents a function and matches our KNN function */
        if (is_knn_function(rte)) {
            elog(LOG, "Query contains KNN function: %s", rte->eref->aliasname);
            return true;
        }
    }
    return false; /* KNN function not found */
}


static PlannedStmt *custom_planner(Query *parse, int cursorOptions, ParamListInfo boundParams) {
    PlannedStmt *result;

    /* Call the previous planner hook if it exists */
    if (prev_planner_hook) {
        result = prev_planner_hook(parse, cursorOptions, boundParams);
    } else {
        result = standard_planner(parse, cursorOptions, boundParams);
    }

    /* Check if the query contains our custom function */
    if (query_contains_knn_function(parse)) {
        ListCell *lc;
        foreach (lc, parse->rtable) {
            RangeTblEntry *rte = (RangeTblEntry *) lfirst(lc);
            if (is_knn_function(rte)) {
                adjust_plan_cost(result->planTree);
                elog(LOG, "Custom KNN cost logic applied for table: %s", rte->eref->aliasname);
            }
        }
    }
    return result;
}
