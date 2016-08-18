#coding=utf-8
import cx_Oracle
import sys

# 在两个库之间复制 数据
def copy_data():
    co1 = cx_Oracle.connect('offon/opr_offon292@offondb')
    co2 = cx_Oracle.connect('offon/Crmpd!06@onoffdb')

    cur1 = co1.cursor()
    cur2 = co2.cursor()

    if 1:
        #print '----table:', tbl
        tbl = 'sndcmddayhbp'
        sql = 'select * from %s where request_time>sysdate-0.1/24'%tbl
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
            rows = cur1.fetchmany(200)
            if rows == None or len(rows) < 1:
                break
            #for r in rows:
            #    r[-4] = '0' #send_status='0'
            cur2.executemany(sql, rows)
            rowcount += len(rows)
            co2.commit()
        print 'rows: ', rowcount
    cur1.close()
    cur2.close()
    co1.close()
    co2.close()



if __name__ == '__main__':
    copy_data()

