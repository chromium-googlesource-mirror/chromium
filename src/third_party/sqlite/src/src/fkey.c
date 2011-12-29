/*
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains code used by the compiler to add foreign key
** support to compiled SQL statements.
*/
#include "sqliteInt.h"

#ifndef SQLITE_OMIT_FOREIGN_KEY
#ifndef SQLITE_OMIT_TRIGGER

/*
** Deferred and Immediate FKs
** --------------------------
**
** Foreign keys in SQLite come in two flavours: deferred and immediate.
** If an immediate foreign key constraint is violated, SQLITE_CONSTRAINT
** is returned and the current statement transaction rolled back. If a 
** deferred foreign key constraint is violated, no action is taken 
** immediately. However if the application attempts to commit the 
** transaction before fixing the constraint violation, the attempt fails.
**
** Deferred constraints are implemented using a simple counter associated
** with the database handle. The counter is set to zero each time a 
** database transaction is opened. Each time a statement is executed 
** that causes a foreign key violation, the counter is incremented. Each
** time a statement is executed that removes an existing violation from
** the database, the counter is decremented. When the transaction is
** committed, the commit fails if the current value of the counter is
** greater than zero. This scheme has two big drawbacks:
**
**   * When a commit fails due to a deferred foreign key constraint, 
**     there is no way to tell which foreign constraint is not satisfied,
**     or which row it is not satisfied for.
**
**   * If the database contains foreign key violations when the 
**     transaction is opened, this may cause the mechanism to malfunction.
**
** Despite these problems, this approach is adopted as it seems simpler
** than the alternatives.
**
** INSERT operations:
**
**   I.1) For each FK for which the table is the child table, search
**        the parent table for a match. If none is found increment the
**        constraint counter.
**
**   I.2) For each FK for which the table is the parent table, 
**        search the child table for rows that correspond to the new
**        row in the parent table. Decrement the counter for each row
**        found (as the constraint is now satisfied).
**
** DELETE operations:
**
**   D.1) For each FK for which the table is the child table, 
**        search the parent table for a row that corresponds to the 
**        deleted row in the child table. If such a row is not found, 
**        decrement the counter.
**
**   D.2) For each FK for which the table is the parent table, search 
**        the child table for rows that correspond to the deleted row 
**        in the parent table. For each found increment the counter.
**
** UPDATE operations:
**
**   An UPDATE command requires that all 4 steps above are taken, but only
**   for FK constraints for which the affected columns are actually 
**   modified (values must be compared at runtime).
**
** Note that I.1 and D.1 are very similar operations, as are I.2 and D.2.
** This simplifies the implementation a bit.
**
** For the purposes of immediate FK constraints, the OR REPLACE conflict
** resolution is considered to delete rows before the new row is inserted.
** If a delete caused by OR REPLACE violates an FK constraint, an exception
** is thrown, even if the FK constraint would be satisfied after the new 
** row is inserted.
**
** Immediate constraints are usually handled similarly. The only difference 
** is that the counter used is stored as part of each individual statement
** object (struct Vdbe). If, after the statement has run, its immediate
** constraint counter is greater than zero, it returns SQLITE_CONSTRAINT
** and the statement transaction is rolled back. An exception is an INSERT
** statement that inserts a single row only (no triggers). In this case,
** instead of using a counter, an exception is thrown immediately if the
** INSERT violates a foreign key constraint. This is necessary as such
** an INSERT does not open a statement transaction.
**
** TODO: How should dropping a table be handled? How should renaming a 
** table be handled?
**
**
** Query API Notes
** ---------------
**
** Before coding an UPDATE or DELETE row operation, the code-generator
** for those two operations needs to know whether or not the operation
** requires any FK processing and, if so, which columns of the original
** row are required by the FK processing VDBE code (i.e. if FKs were
** implemented using triggers, which of the old.* columns would be 
** accessed). No information is required by the code-generator before
** coding an INSERT operation. The functions used by the UPDATE/DELETE
** generation code to query for this information are:
**
**   sqlite3FkRequired() - Test to see if FK processing is required.
**   sqlite3FkOldmask()  - Query for the set of required old.* columns.
**
**
** Externally accessible module functions
** --------------------------------------
**
**   sqlite3FkCheck()    - Check for foreign key violations.
**   sqlite3FkActions()  - Code triggers for ON UPDATE/ON DELETE actions.
**   sqlite3FkDelete()   - Delete an FKey structure.
*/

/*
** VDBE Calling Convention
** -----------------------
**
** Example:
**
**   For the following INSERT statement:
**
**     CREATE TABLE t1(a, b INTEGER PRIMARY KEY, c);
**     INSERT INTO t1 VALUES(1, 2, 3.1);
**
**   Register (x):        2    (type integer)
**   Register (x+1):      1    (type integer)
**   Register (x+2):      NULL (type NULL)
**   Register (x+3):      3.1  (type real)
*/

/*
** A foreign key constraint requires that the key columns in the parent
** table are collectively subject to a UNIQUE or PRIMARY KEY constraint.
** Given that pParent is the parent table for foreign key constraint pFKey, 
** search the schema a unique index on the parent key columns. 
**
** If successful, zero is returned. If the parent key is an INTEGER PRIMARY 
** KEY column, then output variable *ppIdx is set to NULL. Otherwise, *ppIdx 
** is set to point to the unique index. 
** 
** If the parent key consists of a single column (the foreign key constraint
** is not a composite foreign key), output variable *paiCol is set to NULL.
** Otherwise, it is set to point to an allocated array of size N, where
** N is the number of columns in the parent key. The first element of the
** array is the index of the child table column that is mapped by the FK
** constraint to the parent table column stored in the left-most column
** of index *ppIdx. The second element of the array is the index of the
** child table column that corresponds to the second left-most column of
** *ppIdx, and so on.
**
** If the required index cannot be found, either because:
**
**   1) The named parent key columns do not exist, or
**
**   2) The named parent key columns do exist, but are not subject to a
**      UNIQUE or PRIMARY KEY constraint, or
**
**   3) No parent key columns were provided explicitly as part of the
**      foreign key definition, and the parent table does not have a
**      PRIMARY KEY, or
**
**   4) No parent key columns were provided explicitly as part of the
**      foreign key definition, and the PRIMARY KEY of the parent table 
**      consists of a a different number of columns to the child key in 
**      the child table.
**
** then non-zero is returned, and a "foreign key mismatch" error loaded
** into pParse. If an OOM error occurs, non-zero is returned and the
** pParse->db->mallocFailed flag is set.
*/
static int locateFkeyIndex(
  Parse *pParse,                  /* Parse context to store any error in */
  Table *pParent,                 /* Parent table of FK constraint pFKey */
  FKey *pFKey,                    /* Foreign key to find index for */
  Index **ppIdx,                  /* OUT: Unique index on parent table */
  int **paiCol                    /* OUT: Map of index columns in pFKey */
){
  Index *pIdx = 0;                    /* Value to return via *ppIdx */
  int *aiCol = 0;                     /* Value to return via *paiCol */
  int nCol = pFKey->nCol;             /* Number of columns in parent key */
  char *zKey = pFKey->aCol[0].zCol;   /* Name of left-most parent key column */

  /* The caller is responsible for zeroing output parameters. */
  assert( ppIdx && *ppIdx==0 );
  assert( !paiCol || *paiCol==0 );
  assert( pParse );

  /* If this is a non-composite (single column) foreign key, check if it 
  ** maps to the INTEGER PRIMARY KEY of table pParent. If so, leave *ppIdx 
  ** and *paiCol set to zero and return early. 
  **
  ** Otherwise, for a composite foreign key (more than one column), allocate
  ** space for the aiCol array (returned via output parameter *paiCol).
  ** Non-composite foreign keys do not require the aiCol array.
  */
  if( nCol==1 ){
    /* The FK maps to the IPK if any of the following are true:
    **
    **   1) There is an INTEGER PRIMARY KEY column and the FK is implicitly 
    **      mapped to the primary key of table pParent, or
    **   2) The FK is explicitly mapped to a column declared as INTEGER
    **      PRIMARY KEY.
    */
    if( pParent->iPKey>=0 ){
      if( !zKey ) return 0;
      if( !sqlite3StrICmp(pParent->aCol[pParent->iPKey].zName, zKey) ) return 0;
    }
  }else if( paiCol ){
    assert( nCol>1 );
    aiCol = (int *)sqlite3DbMallocRaw(pParse->db, nCol*sizeof(int));
    if( !aiCol ) return 1;
    *paiCol = aiCol;
  }

  for(pIdx=pParent->pIndex; pIdx; pIdx=pIdx->pNext){
    if( pIdx->nColumn==nCol && pIdx->onError!=OE_None ){ 
      /* pIdx is a UNIQUE index (or a PRIMARY KEY) and has the right number
      ** of columns. If each indexed column corresponds to a foreign key
      ** column of pFKey, then this index is a winner.  */

      if( zKey==0 ){
        /* If zKey is NULL, then this foreign key is implicitly mapped to 
        ** the PRIMARY KEY of table pParent. The PRIMARY KEY index may be 
        ** identified by the test (Index.autoIndex==2).  */
        if( pIdx->autoIndex==2 ){
          if( aiCol ){
            int i;
            for(i=0; i<nCol; i++) aiCol[i] = pFKey->aCol[i].iFrom;
          }
          break;
        }
      }else{
        /* If zKey is non-NULL, then this foreign key was declared to
        ** map to an explicit list of columns in table pParent. Check if this
        ** index matches those columns. Also, check that the index uses
        ** the default collation sequences for each column. */
        int i, j;
        for(i=0; i<nCol; i++){
          int iCol = pIdx->aiColumn[i];     /* Index of column in parent tbl */
          char *zDfltColl;                  /* Def. collation for column */
          char *zIdxCol;                    /* Name of indexed column */

          /* If the index uses a collation sequence that is different from
          ** the default collation sequence for the column, this index is
          ** unusable. Bail out early in this case.  */
          zDfltColl = pParent->aCol[iCol].zColl;
          if( !zDfltColl ){
            zDfltColl = "BINARY";
          }
          if( sqlite3StrICmp(pIdx->azColl[i], zDfltColl) ) break;

          zIdxCol = pParent->aCol[iCol].zName;
          for(j=0; j<nCol; j++){
            if( sqlite3StrICmp(pFKey->aCol[j].zCol, zIdxCol)==0 ){
              if( aiCol ) aiCol[i] = pFKey->aCol[j].iFrom;
              break;
            }
          }
          if( j==nCol ) break;
        }
        if( i==nCol ) break;      /* pIdx is usable */
      }
    }
  }

  if( !pIdx ){
    if( !pParse->disableTriggers ){
      sqlite3ErrorMsg(pParse, "foreign key mismatch");
    }
    sqlite3DbFree(pParse->db, aiCol);
    return 1;
  }

  *ppIdx = pIdx;
  return 0;
}

/*
** This function is called when a row is inserted into or deleted from the 
** child table of foreign key constraint pFKey. If an SQL UPDATE is executed 
** on the child table of pFKey, this function is invoked twice for each row
** affected - once to "delete" the old row, and then again to "insert" the
** new row.
**
** Each time it is called, this function generates VDBE code to locate the
** row in the parent table that corresponds to the row being inserted into 
** or deleted from the child table. If the parent row can be found, no 
** special action is taken. Otherwise, if the parent row can *not* be
** found in the parent table:
**
**   Operation | FK type   | Action taken
**   --------------------------------------------------------------------------
**   INSERT      immediate   Increment the "immediate constraint counter".
**
**   DELETE      immediate   Decrement the "immediate constraint counter".
**
**   INSERT      deferred    Increment the "deferred constraint counter".
**
**   DELETE      deferred    Decrement the "deferred constraint counter".
**
** These operations are identified in the comment at the top of this file 
** (fkey.c) as "I.1" and "D.1".
*/
static void fkLookupParent(
  Parse *pParse,        /* Parse context */
  int iDb,              /* Index of database housing pTab */
  Table *pTab,          /* Parent table of FK pFKey */
  Index *pIdx,          /* Unique index on parent key columns in pTab */
  FKey *pFKey,          /* Foreign key constraint */
  int *aiCol,           /* Map from parent key columns to child table columns */
  int regData,          /* Address of array containing child table row */
  int nIncr,            /* Increment constraint counter by this */
  int isIgnore          /* If true, pretend pTab contains all NULL values */
){
  int i;                                    /* Iterator variable */
  Vdbe *v = sqlite3GetVdbe(pParse);         /* Vdbe to add code to */
  int iCur = pParse->nTab - 1;              /* Cursor number to use */
  int iOk = sqlite3VdbeMakeLabel(v);        /* jump here if parent key found */

  /* If nIncr is less than zero, then check at runtime if there are any
  ** outstanding constraints to resolve. If there are not, there is no need
  ** to check if deleting this row resolves any outstanding violations.
  **
  ** Check if any of the key columns in the child table row are NULL. If 
  ** any are, then the constraint is considered satisfied. No need to 
  ** search for a matching row in the parent table.  */
  if( nIncr<0 ){
    sqlite3VdbeAddOp2(v, OP_FkIfZero, pFKey->isDeferred, iOk);
  }
  for(i=0; i<pFKey->nCol; i++){
    int iReg = aiCol[i] + regData + 1;
    sqlite3VdbeAddOp2(v, OP_IsNull, iReg, iOk);
  }

  if( isIgnore==0 ){
    if( pIdx==0 ){
      /* If pIdx is NULL, then the parent key is the INTEGER PRIMARY KEY
      ** column of the parent table (table pTab).  */
      int iMustBeInt;               /* Address of MustBeInt instruction */
      int regTemp = sqlite3GetTempReg(pParse);
  
      /* Invoke MustBeInt to coerce the child key value to an integer (i.e. 
      ** apply the affinity of the parent key). If this fails, then there
      ** is no matching parent key. Before using MustBeInt, make a copy of
      ** the value. Otherwise, the value inserted into the child key column
      ** will have INTEGER affinity applied to it, which may not be correct.  */
      sqlite3VdbeAddOp2(v, OP_SCopy, aiCol[0]+1+regData, regTemp);
      iMustBeInt = sqlite3VdbeAddOp2(v, OP_MustBeInt, regTemp, 0);
  
      /* If the parent table is the same as the child table, and we are about
      ** to increment the constraint-counter (i.e. this is an INSERT operation),
      ** then check if the row being inserted matches itself. If so, do not
      ** increment the constraint-counter.  */
      if( pTab==pFKey->pFrom && nIncr==1 ){
        sqlite3VdbeAddOp3(v, OP_Eq, regData, iOk, regTemp);
      }
  
      sqlite3OpenTable(pParse, iCur, iDb, pTab, OP_OpenRead);
      sqlite3VdbeAddOp3(v, OP_NotExists, iCur, 0, regTemp);
      sqlite3VdbeAddOp2(v, OP_Goto, 0, iOk);
      sqlite3VdbeJumpHere(v, sqlite3VdbeCurrentAddr(v)-2);
      sqlite3VdbeJumpHere(v, iMustBeInt);
      sqlite3ReleaseTempReg(pParse, regTemp);
    }else{
      int nCol = pFKey->nCol;
      int regTemp = sqlite3GetTempRange(pParse, nCol);
      int regRec = sqlite3GetTempReg(pParse);
      KeyInfo *pKey = sqlite3IndexKeyinfo(pParse, pIdx);
  
      sqlite3VdbeAddOp3(v, OP_OpenRead, iCur, pIdx->tnum, iDb);
      sqlite3VdbeChangeP4(v, -1, (char*)pKey, P4_KEYINFO_HANDOFF);
      for(i=0; i<nCol; i++){
        sqlite3VdbeAddOp2(v, OP_Copy, aiCol[i]+1+regData, regTemp+i);
      }
  
      /* If the parent table is the same as the child table, and we are about
      ** to increment the constraint-counter (i.e. this is an INSERT operation),
      ** then check if the row being inserted matches itself. If so, do not
      ** increment the constraint-counter.  */
      if( pTab==pFKey->pFrom && nIncr==1 ){
        int iJump = sqlite3VdbeCurrentAddr(v) + nCol + 1;
        for(i=0; i<nCol; i++){
          int iChild = aiCol[i]+1+regData;
          int iParent = pIdx->aiColumn[i]+1+regData;
          sqlite3VdbeAddOp3(v, OP_Ne, iChild, iJump, iParent);
        }
        sqlite3VdbeAddOp2(v, OP_Goto, 0, iOk);
      }
  
      sqlite3VdbeAddOp3(v, OP_MakeRecord, regTemp, nCol, regRec);
      sqlite3VdbeChangeP4(v, -1, sqlite3IndexAffinityStr(v,pIdx), P4_TRANSIENT);
      sqlite3VdbeAddOp4Int(v, OP_Found, iCur, iOk, regRec, 0);
  
      sqlite3ReleaseTempReg(pParse, regRec);
      sqlite3ReleaseTempRange(pParse, regTemp, nCol);
    }
  }

  if( !pFKey->isDeferred && !pParse->pToplevel && !pParse->isMultiWrite ){
    /* Special case: If this is an INSERT statement that will insert exactly
    ** one row into the table, raise a constraint immediately instead of
    ** incrementing a counter. This is necessary as the VM code is being
    ** generated for will not open a statement transaction.  */
    assert( nIncr==1 );
    sqlite3HaltConstraint(
        pParse, OE_Abort, "foreign key constraint failed", P4_STATIC
    );
  }else{
    if( nIncr>0 && pFKey->isDeferred==0 ){
      sqlite3ParseToplevel(pParse)->mayAbort = 1;
    }
    sqlite3VdbeAddOp2(v, OP_FkCounter, pFKey->isDeferred, nIncr);
  }

  sqlite3VdbeResolveLabel(v, iOk);
  sqlite3VdbeAddOp1(v, OP_Close, iCur);
}

/*
** This function is called to generate code executed when a row is deleted
** from the parent table of foreign key constraint pFKey and, if pFKey is 
** deferred, when a row is inserted into the same table. When generating
** code for an SQL UPDATE operation, this function may be called twice -
** once to "delete" the old row and once to "insert" the new row.
**
** The code generated by this function scans through the rows in the child
** table that correspond to the parent table row being deleted or inserted.
** For each child row found, one of the following actions is taken:
**
**   Operation | FK type   | Action taken
**   --------------------------------------------------------------------------
**   DELETE      immediate   Increment the "immediate constraint counter".
**                           Or, if the ON (UPDATE|DELETE) action is RESTRICT,
**                           throw a "foreign key constraint failed" exception.
**
**   INSERT      immediate   Decrement the "immediate constraint counter".
**
**   DELETE      deferred    Increment the "deferred constraint counter".
**                           Or, if the ON (UPDATE|DELETE) action is RESTRICT,
**                           throw a "foreign key constraint failed" exception.
**
**   INSERT      deferred    Decrement the "deferred constraint counter".
**
** These operations are identified in the comment at the top of this file 
** (fkey.c) as "I.2" and "D.2".
*/
static void fkScanChildren(
  Parse *pParse,                  /* Parse context */
  SrcList *pSrc,                  /* SrcList containing the table to scan */
  Table *pTab,
  Index *pIdx,                    /* Foreign key index */
  FKey *pFKey,                    /* Foreign key relationship */
  int *aiCol,                     /* Map from pIdx cols to child table cols */
  int regData,                    /* Referenced table data starts here */
  int nIncr                       /* Amount to increment deferred counter by */
){
  sqlite3 *db = pParse->db;       /* Database handle */
  int i;                          /* Iterator variable */
  Expr *pWhere = 0;               /* WHERE clause to scan with */
  NameContext sNameContext;       /* Context used to resolve WHERE clause */
  WhereInfo *pWInfo;              /* Context used by sqlite3WhereXXX() */
  int iFkIfZero = 0;              /* Address of OP_FkIfZero */
  Vdbe *v = sqlite3GetVdbe(pParse);

  assert( !pIdx || pIdx->pTable==pTab );

  if( nIncr<0 ){
    iFkIfZero = sqlite3VdbeAddOp2(v, OP_FkIfZero, pFKey->isDeferred, 0);
  }

  /* Create an Expr object representing an SQL expression like:
  **
  **   <parent-key1> = <child-key1> AND <parent-key2> = <child-key2> ...
  **
  ** The collation sequence used for the comparison should be that of
  ** the parent key columns. The affinity of the parent key column should
  ** be applied to each child key value before the comparison takes place.
  */
  for(i=0; i<pFKey->nCol; i++){
    Expr *pLeft;                  /* Value from parent table row */
    Expr *pRight;                 /* Column ref to child table */
    Expr *pEq;                    /* Expression (pLeft = pRight) */
    int iCol;                     /* Index of column in child table */ 
    const char *zCol;             /* Name of column in child table */

    pLeft = sqlite3Expr(db, TK_REGISTER, 0);
    if( pLeft ){
      /* Set the collation sequence and affinity of the LHS of each TK_EQ
      ** expression to the parent key column defaults.  */
      if( pIdx ){
        Column *pCol;
        iCol = pIdx->aiColumn[i];
        pCol = &pTab->aCol[iCol];
        if( pTab->iPKey==iCol ) iCol = -1;
        pLeft->iTable = regData+iCol+1;
        pLeft->affinity = pCol->affinity;
        pLeft->pColl = sqlite3LocateCollSeq(pParse, pCol->zColl);
      }else{
        pLeft->iTable = regData;
        pLeft->affinity = SQLITE_AFF_INTEGER;
      }
    }
    iCol = aiCol ? aiCol[i] : pFKey->aCol[0].iFrom;
    assert( iCol>=0 );
    zCol = pFKey->pFrom->aCol[iCol].zName;
    pRight = sqlite3Expr(db, TK_ID, zCol);
    pEq = sqlite3PExpr(pParse, TK_EQ, pLeft, pRight, 0);
    pWhere = sqlite3ExprAnd(db, pWhere, pEq);
  }

  /* If the child table is the same as the parent table, and this scan
  ** is taking place as part of a DELETE operation (operation D.2), omit the
  ** row being deleted from the scan by adding ($rowid != rowid) to the WHERE 
  ** clause, where $rowid is the rowid of the row being deleted.  */
  if( pTab==pFKey->pFrom && nIncr>0 ){
    Expr *pEq;                    /* Expression (pLeft = pRight) */
    Expr *pLeft;                  /* Value from parent table row */
    Expr *pRight;                 /* Column ref to child table */
    pLeft = sqlite3Expr(db, TK_REGISTER, 0);
    pRight = sqlite3Expr(db, TK_COLUMN, 0);
    if( pLeft && pRight ){
      pLeft->iTable = regData;
      pLeft->affinity = SQLITE_AFF_INTEGER;
      pRight->iTable = pSrc->a[0].iCursor;
      pRight->iColumn = -1;
    }
    pEq = sqlite3PExpr(pParse, TK_NE, pLeft, pRight, 0);
    pWhere = sqlite3ExprAnd(db, pWhere, pEq);
  }

  /* Resolve the references in the WHERE clause. */
  memset(&sNameContext, 0, sizeof(NameContext));
  sNameContext.pSrcList = pSrc;
  sNameContext.pParse = pParse;
  sqlite3ResolveExprNames(&sNameContext, pWhere);

  /* Create VDBE to loop through the entries in pSrc that match the WHERE
  ** clause. If the constraint is not deferred, throw an exception for
  ** each row found. Otherwise, for deferred constraints, increment the
  ** deferred constraint counter by nIncr for each row selected.  */
  pWInfo = sqlite3WhereBegin(pParse, pSrc, pWhere, 0, 0);
  if( nIncr>0 && pFKey->isDeferred==0 ){
    sqlite3ParseToplevel(pParse)->mayAbort = 1;
  }
  sqlite3VdbeAddOp2(v, OP_FkCounter, pFKey->isDeferred, nIncr);
  if( pWInfo ){
    sqlite3WhereEnd(pWInfo);
  }

  /* Clean up the WHERE clause constructed above. */
  sqlite3ExprDelete(db, pWhere);
  if( iFkIfZero ){
    sqlite3VdbeJumpHere(v, iFkIfZero);
  }
}

/*
** This function returns a pointer to the head of a linked list of FK
** constraints for which table pTab is the parent table. For example,
** given the following schema:
**
**   CREATE TABLE t1(a PRIMARY KEY);
**   CREATE TABLE t2(b REFERENCES t1(a);
**
** Calling this function with table "t1" as an argument returns a pointer
** to the FKey structure representing the foreign key constraint on table
** "t2". Calling this function with "t2" as the argument would return a
** NULL pointer (as there are no FK constraints for which t2 is the parent
** table).
*/
FKey *sqlite3FkReferences(Table *pTab){
  int nName = sqlite3Strlen30(pTab->zName);
  return (FKey *)sqlite3HashFind(&pTab->pSchema->fkeyHash, pTab->zName, nName);
}

/*
** The second argument is a Trigger structure allocated by the 
** fkActionTrigger() routine. This function deletes the Trigger structure
** and all of its sub-components.
**
** The Trigger structure or any of its sub-components may be allocated from
** the lookaside buffer belonging to database handle dbMem.
*/
static void fkTriggerDelete(sqlite3 *dbMem, Trigger *p){
  if( p ){
    TriggerStep *pStep = p->step_list;
    sqlite3ExprDelete(dbMem, pStep->pWhere);
    sqlite3ExprListDelete(dbMem, pStep->pExprList);
    sqlite3SelectDelete(dbMem, pStep->pSelect);
    sqlite3ExprDelete(dbMem, p->pWhen);
    sqlite3DbFree(dbMem, p);
  }
}

/*
** This function is called to generate code that runs when table pTab is
** being dropped from the database. The SrcList passed as the second argument
** to this function contains a single entry guaranteed to resolve to
** table pTab.
**
** Normally, no code is required. However, if either
**
**   (a) The table is the parent table of a FK constraint, or
**   (b) The table is the child table of a deferred FK constraint and it is
**       determined at runtime that there are outstanding deferred FK 
**       constraint violations in the database,
**
** then the equivalent of "DELETE FROM <tbl>" is executed before dropping
** the table from the database. Triggers are disabled while running this
** DELETE, but foreign key actions are not.
*/
void sqlite3FkDropTable(Parse *pParse, SrcList *pName, Table *pTab){
  sqlite3 *db = pParse->db;
  if( (db->flags&SQLITE_ForeignKeys) && !IsVirtual(pTab) && !pTab->pSelect ){
    int iSkip = 0;
    Vdbe *v = sqlite3GetVdbe(pParse);

    assert( v );                  /* VDBE has already been allocated */
    if( sqlite3FkReferences(pTab)==0 ){
      /* Search for a deferred foreign key constraint for which this table
      ** is the child table. If one cannot be found, return without 
      ** generating any VDBE code. If one can be found, then jump over
      ** the entire DELETE if there are no outstanding deferred constraints
      ** when this statement is run.  */
      FKey *p;
      for(p=pTab->pFKey; p; p=p->pNextFrom){
        if( p->isDeferred ) break;
      }
      if( !p ) return;
      iSkip = sqlite3VdbeMakeLabel(v);
      sqlite3VdbeAddOp2(v, OP_FkIfZero, 1, iSkip);
    }

    pParse->disableTriggers = 1;
    sqlite3DeleteFrom(pParse, sqlite3SrcListDup(db, pName, 0), 0);
    pParse->disableTriggers = 0;

    /* If the DELETE has generated immediate foreign key constraint 
    ** violations, halt the VDBE and return an error at this point, before
    ** any modifications to the schema are made. This is because statement
    ** transactions are not able to rollback schema changes.  */
    sqlite3VdbeAddOp2(v, OP_FkIfZero, 0, sqlite3VdbeCurrentAddr(v)+2);
    sqlite3HaltConstraint(
        pParse, OE_Abort, "foreign key constraint failed", P4_STATIC
    );

    if( iSkip ){
      sqlite3VdbeResolveLabel(v, iSkip);
    }
  }
}

/*
** This function is called when inserting, deleting or updating a row of
** table pTab to generate VDBE code to perform foreign key constraint 
** processing for the operation.
**
** For a DELETE operation, parameter regOld is passed the index of the
** first register in an array of (pTab->nCol+1) registers containing the
** rowid of the row being deleted, followed by each of the column values
** of the row being deleted, from left to right. Parameter regNew is passed
** zero in this case.
**
** For an INSERT operation, regOld is passed zero and regNew is passed the
** first register of an array of (pTab->nCol+1) registers containing the new
** row data.
**
** For an UPDATE operation, this function is called twice. Once before
** the original record is deleted from the table using the calling convention
** described for DELETE. Then again after the original record is deleted
** but before the new record is inserted using the INSERT convention. 
*/
void sqlite3FkCheck(
  Parse *pParse,                  /* Parse context */
  Table *pTab,                    /* Row is being deleted from this table */ 
  int regOld,                     /* Previous row data is stored here */
  int regNew                      /* New row data is stored here */
){
  sqlite3 *db = pParse->db;       /* Database handle */
  FKey *pFKey;                    /* Used to iterate through FKs */
  int iDb;                        /* Index of database containing pTab */
  const char *zDb;                /* Name of database containing pTab */
  int isIgnoreErrors = pParse->disableTriggers;

  /* Exactly one of regOld and regNew should be non-zero. */
  assert( (regOld==0)!=(regNew==0) );

  /* If foreign-keys are disabled, this function is a no-op. */
  if( (db->flags&SQLITE_ForeignKeys)==0 ) return;

  iDb = sqlite3SchemaToIndex(db, pTab->pSchema);
  zDb = db->aDb[iDb].zName;

  /* Loop through all the foreign key constraints for which pTab is the
  ** child table (the table that the foreign key definition is part of).  */
  for(pFKey=pTab->pFKey; pFKey; pFKey=pFKey->pNextFrom){
    Table *pTo;                   /* Parent table of foreign key pFKey */
    Index *pIdx = 0;              /* Index on key columns in pTo */
    int *aiFree = 0;
    int *aiCol;
    int iCol;
    int i;
    int isIgnore = 0;

    /* Find the parent table of this foreign key. Also find a unique index 
    ** on the parent key columns in the parent table. If either of these 
    ** schema items cannot be located, set an error in pParse and return 
    ** early.  */
    if( pParse->disableTriggers ){
      pTo = sqlite3FindTable(db, pFKey->zTo, zDb);
    }else{
      pTo = sqlite3LocateTable(pParse, 0, pFKey->zTo, zDb);
    }
    if( !pTo || locateFkeyIndex(pParse, pTo, pFKey, &pIdx, &aiFree) ){
      if( !isIgnoreErrors || db->mallocFailed ) return;
      continue;
    }
    assert( pFKey->nCol==1 || (aiFree && pIdx) );

    if( aiFree ){
      aiCol = aiFree;
    }else{
      iCol = pFKey->aCol[0].iFrom;
      aiCol = &iCol;
    }
    for(i=0; i<pFKey->nCol; i++){
      if( aiCol[i]==pTab->iPKey ){
        aiCol[i] = -1;
      }
#ifndef SQLITE_OMIT_AUTHORIZATION
      /* Request permission to read the parent key columns. If the 
      ** authorization callback returns SQLITE_IGNORE, behave as if any
      ** values read from the parent table are NULL. */
      if( db->xAuth ){
        int rcauth;
        char *zCol = pTo->aCol[pIdx ? pIdx->aiColumn[i] : pTo->iPKey].zName;
        rcauth = sqlite3AuthReadCol(pParse, pTo->zName, zCol, iDb);
        isIgnore = (rcauth==SQLITE_IGNORE);
      }
#endif
    }

    /* Take a shared-cache advisory read-lock on the parent table. Allocate 
    ** a cursor to use to search the unique index on the parent key columns 
    ** in the parent table.  */
    sqlite3TableLock(pParse, iDb, pTo->tnum, 0, pTo->zName);
    pParse->nTab++;

    if( regOld!=0 ){
      /* A row is being removed from the child table. Search for the parent.
      ** If the parent does not exist, removing the child row resolves an 
      ** outstanding foreign key constraint violation. */
      fkLookupParent(pParse, iDb, pTo, pIdx, pFKey, aiCol, regOld, -1,isIgnore);
    }
    if( regNew!=0 ){
      /* A row is being added to the child table. If a parent row cannot
      ** be found, adding the child row has violated the FK constraint. */ 
      fkLookupParent(pParse, iDb, pTo, pIdx, pFKey, aiCol, regNew, +1,isIgnore);
    }

    sqlite3DbFree(db, aiFree);
  }

  /* Loop through all the foreign key constraints that refer to this table */
  for(pFKey = sqlite3FkReferences(pTab); pFKey; pFKey=pFKey->pNextTo){
    Index *pIdx = 0;              /* Foreign key index for pFKey */
    SrcList *pSrc;
    int *aiCol = 0;

    if( !pFKey->isDeferred && !pParse->pToplevel && !pParse->isMultiWrite ){
      assert( regOld==0 && regNew!=0 );
      /* Inserting a single row into a parent table cannot cause an immediate
      ** foreign key violation. So do nothing in this case.  */
      continue;
    }

    if( locateFkeyIndex(pParse, pTab, pFKey, &pIdx, &aiCol) ){
      if( !isIgnoreErrors || db->mallocFailed ) return;
      continue;
    }
    assert( aiCol || pFKey->nCol==1 );

    /* Create a SrcList structure containing a single table (the table 
    ** the foreign key that refers to this table is attached to). This
    ** is required for the sqlite3WhereXXX() interface.  */
    pSrc = sqlite3SrcListAppend(db, 0, 0, 0);
    if( pSrc ){
      struct SrcList_item *pItem = pSrc->a;
      pItem->pTab = pFKey->pFrom;
      pItem->zName = pFKey->pFrom->zName;
      pItem->pTab->nRef++;
      pItem->iCursor = pParse->nTab++;
  
      if( regNew!=0 ){
        fkScanChildren(pParse, pSrc, pTab, pIdx, pFKey, aiCol, regNew, -1);
      }
      if( regOld!=0 ){
        /* If there is a RESTRICT action configured for the current operation
        ** on the parent table of this FK, then throw an exception 
        ** immediately if the FK constraint is violated, even if this is a
        ** deferred trigger. That's what RESTRICT means. To defer checking
        ** the constraint, the FK should specify NO ACTION (represented
        ** using OE_None). NO ACTION is the default.  */
        fkScanChildren(pParse, pSrc, pTab, pIdx, pFKey, aiCol, regOld, 1);
      }
      pItem->zName = 0;
      sqlite3SrcListDelete(db, pSrc);
    }
    sqlite3DbFree(db, aiCol);
  }
}

#define COLUMN_MASK(x) (((x)>31) ? 0xffffffff : ((u32)1<<(x)))

/*
** This function is called before generating code to update or delete a 
** row contained in table pTab.
*/
u32 sqlite3FkOldmask(
  Parse *pParse,                  /* Parse context */
  Table *pTab                     /* Table being modified */
){
  u32 mask = 0;
  if( pParse->db->flags&SQLITE_ForeignKeys ){
    FKey *p;
    int i;
    for(p=pTab->pFKey; p; p=p->pNextFrom){
      for(i=0; i<p->nCol; i++) mask |= COLUMN_MASK(p->aCol[i].iFrom);
    }
    for(p=sqlite3FkReferences(pTab); p; p=p->pNextTo){
      Index *pIdx = 0;
      locateFkeyIndex(pParse, pTab, p, &pIdx, 0);
      if( pIdx ){
        for(i=0; i<pIdx->nColumn; i++) mask |= COLUMN_MASK(pIdx->aiColumn[i]);
      }
    }
  }
  return mask;
}

/*
** This function is called before generating code to update or delete a 
** row contained in table pTab. If the operation is a DELETE, then
** parameter aChange is passed a NULL value. For an UPDATE, aChange points
** to an array of size N, where N is the number of columns in table pTab.
** If the i'th column is not modified by the UPDATE, then the corresponding 
** entry in the aChange[] array is set to -1. If the column is modified,
** the value is 0 or greater. Parameter chngRowid is set to true if the
** UPDATE statement modifies the rowid fields of the table.
**
** If any foreign key processing will be required, this function returns
** true. If there is no foreign key related processing, this function 
** returns false.
*/
int sqlite3FkRequired(
  Parse *pParse,                  /* Parse context */
  Table *pTab,                    /* Table being modified */
  int *aChange,                   /* Non-NULL for UPDATE operations */
  int chngRowid                   /* True for UPDATE that affects rowid */
){
  if( pParse->db->flags&SQLITE_ForeignKeys ){
    if( !aChange ){
      /* A DELETE operation. Foreign key processing is required if the 
      ** table in question is either the child or parent table for any 
      ** foreign key constraint.  */
      return (sqlite3FkReferences(pTab) || pTab->pFKey);
    }else{
      /* This is an UPDATE. Foreign key processing is only required if the
      ** operation modifies one or more child or parent key columns. */
      int i;
      FKey *p;

      /* Check if any child key columns are being modified. */
      for(p=pTab->pFKey; p; p=p->pNextFrom){
        for(i=0; i<p->nCol; i++){
          int iChildKey = p->aCol[i].iFrom;
          if( aChange[iChildKey]>=0 ) return 1;
          if( iChildKey==pTab->iPKey && chngRowid ) return 1;
        }
      }

      /* Check if any parent key columns are being modified. */
      for(p=sqlite3FkReferences(pTab); p; p=p->pNextTo){
        for(i=0; i<p->nCol; i++){
          char *zKey = p->aCol[i].zCol;
          int iKey;
          for(iKey=0; iKey<pTab->nCol; iKey++){
            Column *pCol = &pTab->aCol[iKey];
            if( (zKey ? !sqlite3StrICmp(pCol->zName, zKey) : pCol->isPrimKey) ){
              if( aChange[iKey]>=0 ) return 1;
              if( iKey==pTab->iPKey && chngRowid ) return 1;
            }
          }
        }
      }
    }
  }
  return 0;
}

/*
** This function is called when an UPDATE or DELETE operation is being 
** compiled on table pTab, which is the parent table of foreign-key pFKey.
** If the current operation is an UPDATE, then the pChanges parameter is
** passed a pointer to the list of columns being modified. If it is a
** DELETE, pChanges is passed a NULL pointer.
**
** It returns a pointer to a Trigger structure containing a trigger
** equivalent to the ON UPDATE or ON DELETE action specified by pFKey.
** If the action is "NO ACTION" or "RESTRICT", then a NULL pointer is
** returned (these actions require no special handling by the triggers
** sub-system, code for them is created by fkScanChildren()).
**
** For example, if pFKey is the foreign key and pTab is table "p" in 
** the following schema:
**
**   CREATE TABLE p(pk PRIMARY KEY);
**   CREATE TABLE c(ck REFERENCES p ON DELETE CASCADE);
**
** then the returned trigger structure is equivalent to:
**
**   CREATE TRIGGER ... DELETE ON p BEGIN
**     DELETE FROM c WHERE ck = old.pk;
**   END;
**
** The returned pointer is cached as part of the foreign key object. It
** is eventually freed along with the rest of the foreign key object by 
** sqlite3FkDelete().
*/
static Trigger *fkActionTrigger(
  Parse *pParse,                  /* Parse context */
  Table *pTab,                    /* Table being updated or deleted from */
  FKey *pFKey,                    /* Foreign key to get action for */
  ExprList *pChanges              /* Change-list for UPDATE, NULL for DELETE */
){
  sqlite3 *db = pParse->db;       /* Database handle */
  int action;                     /* One of OE_None, OE_Cascade etc. */
  Trigger *pTrigger;              /* Trigger definition to return */
  int iAction = (pChanges!=0);    /* 1 for UPDATE, 0 for DELETE */

  action = pFKey->aAction[iAction];
  pTrigger = pFKey->apTrigger[iAction];

  if( action!=OE_None && !pTrigger ){
    u8 enableLookaside;           /* Copy of db->lookaside.bEnabled */
    char const *zFrom;            /* Name of child table */
    int nFrom;                    /* Length in bytes of zFrom */
    Index *pIdx = 0;              /* Parent key index for this FK */
    int *aiCol = 0;               /* child table cols -> parent key cols */
    TriggerStep *pStep = 0;        /* First (only) step of trigger program */
    Expr *pWhere = 0;             /* WHERE clause of trigger step */
    ExprList *pList = 0;          /* Changes list if ON UPDATE CASCADE */
    Select *pSelect = 0;          /* If RESTRICT, "SELECT RAISE(...)" */
    int i;                        /* Iterator variable */
    Expr *pWhen = 0;              /* WHEN clause for the trigger */

    if( locateFkeyIndex(pParse, pTab, pFKey, &pIdx, &aiCol) ) return 0;
    assert( aiCol || pFKey->nCol==1 );

    for(i=0; i<pFKey->nCol; i++){
      Token tOld = { "old", 3 };  /* Literal "old" token */
      Token tNew = { "new", 3 };  /* Literal "new" token */
      Token tFromCol;             /* Name of column in child table */
      Token tToCol;               /* Name of column in parent table */
      int iFromCol;               /* Idx of column in child table */
      Expr *pEq;                  /* tFromCol = OLD.tToCol */

      iFromCol = aiCol ? aiCol[i] : pFKey->aCol[0].iFrom;
      assert( iFromCol>=0 );
      tToCol.z = pIdx ? pTab->aCol[pIdx->aiColumn[i]].zName : "oid";
      tFromCol.z = pFKey->pFrom->aCol[iFromCol].zName;

      tToCol.n = sqlite3Strlen30(tToCol.z);
      tFromCol.n = sqlite3Strlen30(tFromCol.z);

      /* Create the expression "OLD.zToCol = zFromCol". It is important
      ** that the "OLD.zToCol" term is on the LHS of the = operator, so
      ** that the affinity and collation sequence associated with the
      ** parent table are used for the comparison. */
      pEq = sqlite3PExpr(pParse, TK_EQ,
          sqlite3PExpr(pParse, TK_DOT, 
            sqlite3PExpr(pParse, TK_ID, 0, 0, &tOld),
            sqlite3PExpr(pParse, TK_ID, 0, 0, &tToCol)
          , 0),
          sqlite3PExpr(pParse, TK_ID, 0, 0, &tFromCol)
      , 0);
      pWhere = sqlite3ExprAnd(db, pWhere, pEq);

      /* For ON UPDATE, construct the next term of the WHEN clause.
      ** The final WHEN clause will be like this:
      **
      **    WHEN NOT(old.col1 IS new.col1 AND ... AND old.colN IS new.colN)
      */
      if( pChanges ){
        pEq = sqlite3PExpr(pParse, TK_IS,
            sqlite3PExpr(pParse, TK_DOT, 
              sqlite3PExpr(pParse, TK_ID, 0, 0, &tOld),
              sqlite3PExpr(pParse, TK_ID, 0, 0, &tToCol),
              0),
            sqlite3PExpr(pParse, TK_DOT, 
              sqlite3PExpr(pParse, TK_ID, 0, 0, &tNew),
              sqlite3PExpr(pParse, TK_ID, 0, 0, &tToCol),
              0),
            0);
        pWhen = sqlite3ExprAnd(db, pWhen, pEq);
      }
  
      if( action!=OE_Restrict && (action!=OE_Cascade || pChanges) ){
        Expr *pNew;
        if( action==OE_Cascade ){
          pNew = sqlite3PExpr(pParse, TK_DOT, 
            sqlite3PExpr(pParse, TK_ID, 0, 0, &tNew),
            sqlite3PExpr(pParse, TK_ID, 0, 0, &tToCol)
          , 0);
        }else if( action==OE_SetDflt ){
          Expr *pDflt = pFKey->pFrom->aCol[iFromCol].pDflt;
          if( pDflt ){
            pNew = sqlite3ExprDup(db, pDflt, 0);
          }else{
            pNew = sqlite3PExpr(pParse, TK_NULL, 0, 0, 0);
          }
        }else{
          pNew = sqlite3PExpr(pParse, TK_NULL, 0, 0, 0);
        }
        pList = sqlite3ExprListAppend(pParse, pList, pNew);
        sqlite3ExprListSetName(pParse, pList, &tFromCol, 0);
      }
    }
    sqlite3DbFree(db, aiCol);

    zFrom = pFKey->pFrom->zName;
    nFrom = sqlite3Strlen30(zFrom);

    if( action==OE_Restrict ){
      Token tFrom;
      Expr *pRaise; 

      tFrom.z = zFrom;
      tFrom.n = nFrom;
      pRaise = sqlite3Expr(db, TK_RAISE, "foreign key constraint failed");
      if( pRaise ){
        pRaise->affinity = OE_Abort;
      }
      pSelect = sqlite3SelectNew(pParse, 
          sqlite3ExprListAppend(pParse, 0, pRaise),
          sqlite3SrcListAppend(db, 0, &tFrom, 0),
          pWhere,
          0, 0, 0, 0, 0, 0
      );
      pWhere = 0;
    }

    /* Disable lookaside memory allocation */
    enableLookaside = db->lookaside.bEnabled;
    db->lookaside.bEnabled = 0;

    pTrigger = (Trigger *)sqlite3DbMallocZero(db, 
        sizeof(Trigger) +         /* struct Trigger */
        sizeof(TriggerStep) +     /* Single step in trigger program */
        nFrom + 1                 /* Space for pStep->target.z */
    );
    if( pTrigger ){
      pStep = pTrigger->step_list = (TriggerStep *)&pTrigger[1];
      pStep->target.z = (char *)&pStep[1];
      pStep->target.n = nFrom;
      memcpy((char *)pStep->target.z, zFrom, nFrom);
  
      pStep->pWhere = sqlite3ExprDup(db, pWhere, EXPRDUP_REDUCE);
      pStep->pExprList = sqlite3ExprListDup(db, pList, EXPRDUP_REDUCE);
      pStep->pSelect = sqlite3SelectDup(db, pSelect, EXPRDUP_REDUCE);
      if( pWhen ){
        pWhen = sqlite3PExpr(pParse, TK_NOT, pWhen, 0, 0);
        pTrigger->pWhen = sqlite3ExprDup(db, pWhen, EXPRDUP_REDUCE);
      }
    }

    /* Re-enable the lookaside buffer, if it was disabled earlier. */
    db->lookaside.bEnabled = enableLookaside;

    sqlite3ExprDelete(db, pWhere);
    sqlite3ExprDelete(db, pWhen);
    sqlite3ExprListDelete(db, pList);
    sqlite3SelectDelete(db, pSelect);
    if( db->mallocFailed==1 ){
      fkTriggerDelete(db, pTrigger);
      return 0;
    }

    switch( action ){
      case OE_Restrict:
        pStep->op = TK_SELECT; 
        break;
      case OE_Cascade: 
        if( !pChanges ){ 
          pStep->op = TK_DELETE; 
          break; 
        }
      default:
        pStep->op = TK_UPDATE;
    }
    pStep->pTrig = pTrigger;
    pTrigger->pSchema = pTab->pSchema;
    pTrigger->pTabSchema = pTab->pSchema;
    pFKey->apTrigger[iAction] = pTrigger;
    pTrigger->op = (pChanges ? TK_UPDATE : TK_DELETE);
  }

  return pTrigger;
}

/*
** This function is called when deleting or updating a row to implement
** any required CASCADE, SET NULL or SET DEFAULT actions.
*/
void sqlite3FkActions(
  Parse *pParse,                  /* Parse context */
  Table *pTab,                    /* Table being updated or deleted from */
  ExprList *pChanges,             /* Change-list for UPDATE, NULL for DELETE */
  int regOld                      /* Address of array containing old row */
){
  /* If foreign-key support is enabled, iterate through all FKs that 
  ** refer to table pTab. If there is an action associated with the FK 
  ** for this operation (either update or delete), invoke the associated 
  ** trigger sub-program.  */
  if( pParse->db->flags&SQLITE_ForeignKeys ){
    FKey *pFKey;                  /* Iterator variable */
    for(pFKey = sqlite3FkReferences(pTab); pFKey; pFKey=pFKey->pNextTo){
      Trigger *pAction = fkActionTrigger(pParse, pTab, pFKey, pChanges);
      if( pAction ){
        sqlite3CodeRowTriggerDirect(pParse, pAction, pTab, regOld, OE_Abort, 0);
      }
    }
  }
}

#endif /* ifndef SQLITE_OMIT_TRIGGER */

/*
** Free all memory associated with foreign key definitions attached to
** table pTab. Remove the deleted foreign keys from the Schema.fkeyHash
** hash table.
*/
void sqlite3FkDelete(sqlite3 *db, Table *pTab){
  FKey *pFKey;                    /* Iterator variable */
  FKey *pNext;                    /* Copy of pFKey->pNextFrom */

  assert( db==0 || sqlite3SchemaMutexHeld(db, 0, pTab->pSchema) );
  for(pFKey=pTab->pFKey; pFKey; pFKey=pNext){

    /* Remove the FK from the fkeyHash hash table. */
    if( !db || db->pnBytesFreed==0 ){
      if( pFKey->pPrevTo ){
        pFKey->pPrevTo->pNextTo = pFKey->pNextTo;
      }else{
        void *p = (void *)pFKey->pNextTo;
        const char *z = (p ? pFKey->pNextTo->zTo : pFKey->zTo);
        sqlite3HashInsert(&pTab->pSchema->fkeyHash, z, sqlite3Strlen30(z), p);
      }
      if( pFKey->pNextTo ){
        pFKey->pNextTo->pPrevTo = pFKey->pPrevTo;
      }
    }

    /* EV: R-30323-21917 Each foreign key constraint in SQLite is
    ** classified as either immediate or deferred.
    */
    assert( pFKey->isDeferred==0 || pFKey->isDeferred==1 );

    /* Delete any triggers created to implement actions for this FK. */
#ifndef SQLITE_OMIT_TRIGGER
    fkTriggerDelete(db, pFKey->apTrigger[0]);
    fkTriggerDelete(db, pFKey->apTrigger[1]);
#endif

    pNext = pFKey->pNextFrom;
    sqlite3DbFree(db, pFKey);
  }
}
#endif /* ifndef SQLITE_OMIT_FOREIGN_KEY */
