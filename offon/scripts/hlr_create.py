#coding=utf-8
import cx_Oracle
import sys

# 在两个库之间复制基础配置表数据
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


# 建立新的hlrcode的时候， 从一个已有的hlrcode上复制配置数据
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

# 清理sql语句， 删掉里面以 -- 开头的行
def clear_sql(sql):
    arr = sql.split('\n')
    arr1 = []
    for l in arr:
        if l.strip().startswith('-- '):
            continue
        arr1.append(l)
    sql = "\n".join(arr1)
    return sql.strip()

# 执行建表语句， 对于重复执行的报错忽略
def create_sql_exec(cur, sql):
    try:
        cur.execute(sql)
    except Exception, e:
        s = str(e)
        if s.find('name is already used by an existing object') >= 0:
            return True
        if s.find('such column list already indexed') >= 0:
            return True
        if s.find(' table can have only one primar') >= 0:
            return True
        print e
        print sql
        return False
    return True


# 创建开关机的基本配置表
def create_base_tables():
    # co2 = cx_Oracle.connect('offon/ll@xe')
    co2 = cx_Oracle.connect('offon/Crmpd!06@onoffdb')
    cur2 = co2.cursor()
    sqls = file('base_tables.sql').read()
    for sql in sqls.split(';'):
        sql = clear_sql(sql)
        if sql == '':
            continue

        if not create_sql_exec(cur2, sql):
            break
    cur2.close()
    co2.close()

# 创建hlr的指令数据分表 sndcmdday??? soprcmd??? ssubcmd??? dusrdata???
def create_hlr_tables(hlrcodes):
    #co2 = cx_Oracle.connect('offon/ll@xe')
    co2 = cx_Oracle.connect('offon/Crmpd!06@onoffdb')
    cur2 = co2.cursor()
    for hlrcode in hlrcodes.split(","):
        hlrcode = hlrcode.strip()
        sqls = file('hlr_tables_list.sql').read()
        sqls = sqls.replace('${hlrstr}', hlrcode)
        for sql in sqls.split(';'):
            sql = clear_sql(sql)
            if sql == '':
                continue
            if not create_sql_exec(cur2, sql):
                break
    cur2.close()
    co2.close()


if __name__ == '__main__':
    # create_base_tables()
    #base_data()
    #base_data('cHlrCfgInfo')
    create_hlr_tables(sys.argv[1])
    #create_hlr_tables('scx')
    #copy_hlr_config('scx', 'hei')
    pass

