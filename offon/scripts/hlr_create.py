import cx_Oracle


def base_data(hlrcode):
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
sphonecomm'''
    co1 = cx_Oracle.connect('offon/opr_offon292@offondb')
    co2 = cx_Oracle.connect('offon/ll@xe')

    cur1 = co1.cursor()
    cur2 = co2.cursor()
    for tbl in tbls.split():
        print '----table:', tbl
        cur2.execute('truncate table '+tbl)
        sql = 'select * from %s where hlr_code=:hlr'%tbl
        cur1.execute(sql, (hlrcode,))
        r = cur1.fetchone()
        var = []
        for i in range(len(r)):
            var.append(':v%d'%i)
        sql = 'insert into '+tbl+' values(' + ','.join(var) + ' )'
        print sql
        cur2.execute(sql, r)
        while 1:
            r = cur1.fetchone()
            if r == None:
                break
            cur2.execute(sql, r)
        co2.commit()

def create_hlr_tables(hlrcode):
    co2 = cx_Oracle.connect('offon/ll@xe')
    cur2 = co2.cursor()
    sqls = file('hlr_tables_list.sql').read()
    sqls = sqls.replace('${hlrstr}', hlrcode)
    for sql in sqls.split(';'):
        cur2.execute(sql)
    co2.close()
    

if __name__ == '__main__':
    #base_data('hei')
    create_hlr_tables('hei')
