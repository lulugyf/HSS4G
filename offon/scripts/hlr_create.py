import cx_Oracle


def base_data(table=None):
    tbls = '''cbossuserinfo
cqueryparse
cqueryrevise
corderresend
corderreplyparse
ccmdtoorder
cmaincfginfo
cloginuser
cphonehlrcode
shlrcodemap
sphonecomm
COPCODEPRIORITY
CUSRDATAMGR
MSISDNINHLR
'''
    co1 = cx_Oracle.connect('offon/opr_offon292@offondb')
    co2 = cx_Oracle.connect('offon/Crmpd!06@onoffdb')

    cur1 = co1.cursor()
    cur2 = co2.cursor()
    tables = tbls
    if table is not None:
        tables = table
    for tbl in tables.split():
        print '----table:', tbl
        cur2.execute('truncate table '+tbl)
        sql = 'select * from %s'%tbl
        cur1.execute(sql)
        r = cur1.fetchone()
        var = []
        for i in range(len(r)):
            var.append(':v%d'%i)
        sql = 'insert into '+tbl+' values(' + ','.join(var) + ' )'
        print sql
        cur2.execute(sql, r)
        rowcount = 0
        while 1:
            r = cur1.fetchmany(200)
            if r == None or len(r) < 1:
                break
            cur2.executemany(sql, r)
            rowcount += len(r)
        co2.commit()
        print 'rows: ', rowcount

def create_hlr_tables(hlrcode):
    #co2 = cx_Oracle.connect('offon/ll@xe')
    co2 = cx_Oracle.connect('offon/Crmpd!06@onoffdb')
    cur2 = co2.cursor()
    sqls = file('hlr_tables_list.sql').read()
    sqls = sqls.replace('${hlrstr}', hlrcode)
    for sql in sqls.split(';'):
        sql = clear_sql(sql)
        if sql == '':
            continue
        cur2.execute(sql)
    co2.close()

# copy config data from from_hlr
def copy_hlr_config(hlrcode, copyhlr):
    co2 = cx_Oracle.connect('offon/Crmpd!06@onoffdb')
    cur2 = co2.cursor()
    sqls = file('hlr_datas.sql').read()
    sqls = sqls.replace('${hlrstr}', hlrcode)
    sqls = sqls.replace('${copyhlr}', copyhlr)
    for sql in sqls.split(';'):
        sql = clear_sql(sql)
        if sql == '':
            continue
        print '---', sql
        cur2.execute(sql)
        co2.commit()
    co2.close()

def clear_sql(sql):
    arr = sql.split('\n')
    arr1 = []
    for l in arr:
        if l.strip().startswith('-- '):
            continue
        arr1.append(l)
    sql = "\n".join(arr1)
    return sql.strip()

def create_base_tables():
    # co2 = cx_Oracle.connect('offon/ll@xe')
    co2 = cx_Oracle.connect('offon/Crmpd!06@onoffdb')
    cur2 = co2.cursor()
    sqls = file('base_tables.sql').read()
    for sql in sqls.split(';'):
        sql = clear_sql(sql)
        if sql == '':
            continue
        try:
            cur2.execute(sql)
        except Exception,e:
            s = str(e)
            if s.find('name is already used by an existing object') >= 0:
                continue
            if s.find('such column list already indexed') >= 0:
                continue
            if s.find(' table can have only one primar') >= 0:
                continue
            print e
            print sql
            break
    co2.close()


if __name__ == '__main__':
    # create_base_tables()
    #base_data()
    #base_data('cHlrCfgInfo')
    #create_hlr_tables('hei')
    #create_hlr_tables('scx')
    #copy_hlr_config('scx', 'hei')
    pass

