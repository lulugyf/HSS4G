/*********************************************************
 *
 *  FileName:
 *      db_func.h
 *
 *  Version:
 *      1.00 2004.11.09
 *
 *  tabstop = 4
 *
 ********************************************************/

#define	SQLCODE			sqlca.sqlcode
#define SQLERRMSG       sqlca.sqlerrm.sqlerrmc

#define	SQL_OK			0
#define	OK			SQL_OK
#define	NO_MORE_ROWS	1403
#define	SQLROWS			sqlca.sqlerrd[2]
