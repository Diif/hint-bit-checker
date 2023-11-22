#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "postmaster/bgworker.h"
#include "nodes/pg_list.h"
#include "string.h"
#include <storage/ipc.h>
#include <storage/shmem.h>
#include <storage/lwlock.h>


#include "catalog/namespace.h"
#include "catalog/pg_authid.h"
#include <miscadmin.h>
#include "access/xact.h"
#include "utils/snapmgr.h"
#include "access/tableam.h"
#include "access/relation.h"
#include "access/tupdesc.h"

PG_MODULE_MAGIC;

PGDLLEXPORT void bgw_log(Datum main_arg);

extern void _PG_init(void);

void 
bgw_log(Datum main_arg)
{
    BackgroundWorkerUnblockSignals();
    // Oid save_userid;
    // int save_sec_context;
    // GetUserIdAndSecContext(&save_userid, &save_sec_context);
	// SetUserIdAndSecContext(BOOTSTRAP_SUPERUSERID,
	// 						   save_sec_context | SECURITY_LOCAL_USERID_CHANGE);
    // BaseInit();

	InitPostgres("postgres", InvalidOid, NULL, InvalidOid, false, false, NULL);

// we don't take care of memory free
    while(true){ 
        // pg_usleep(15000000L); //WaitLatch()
        pg_usleep(10000000L); //WaitLatch()
        

        // elog(LOG, "##############");
        // lock by LockRelationOid with AccessShareLock
        // open with RelationIdGetRelation

        // must be in transaction before getting reloid
        StartTransactionCommand();
        Oid relId = RelnameGetRelid("stuff");

        if (relId == InvalidOid)
        {
            elog(LOG, "Haven't got stuff oid:%d\n", relId);
            CommitTransactionCommand();
            continue;
        }

        
        Snapshot snap = GetTransactionSnapshot();
        PushActiveSnapshot(snap);
        snap = GetActiveSnapshot();
        // copy maybe
        Relation rel = relation_open(relId, AccessShareLock);
        TupleTableSlot* slot = table_slot_create(rel, NULL);
        TableScanDesc scan = table_beginscan(rel,snap,0,NULL);
        elog(LOG, "Stuff oid:%d\n", relId);
        int c = 0;
        while (table_scan_getnextslot(scan, ForwardScanDirection, slot))
		{
			// slot_getallattrs(slot);
            if (slot->tts_tid.ip_blkid.bi_hi == 33000)
            {
                elog(LOG, "TID: %d\n" , slot->tts_tid);
            }
            
        }
        elog(LOG, "End iteration\n");
        if (ActiveSnapshotSet())
	    {
		    PopActiveSnapshot();
	    }
        // TODO: TupleDesc reference leak: TupleDesc 0x7f384ba53718 (16387,-1) still referenced
        table_endscan(scan);
        relation_close(rel, AccessShareLock);
        ExecDropSingleTupleTableSlot(slot);
        CommitTransactionCommand();
    }
}


void
_PG_init(void)
{
    BackgroundWorker worker;

    memset(&worker, 0, sizeof(worker));
    worker.bgw_flags = BGWORKER_SHMEM_ACCESS;
    worker.bgw_start_time = BgWorkerStart_RecoveryFinished;
    worker.bgw_restart_time = BGW_NEVER_RESTART;
    sprintf(worker.bgw_library_name, "hint_bits_checker");
    sprintf(worker.bgw_function_name, "bgw_log");
    sprintf(worker.bgw_name, "bit checker");
    sprintf(worker.bgw_type, "bit checker");
    elog(LOG, "register bgw");
    RegisterBackgroundWorker(&worker);

} 