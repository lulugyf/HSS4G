#encoding=utf-8
import mgrcomm as c

def main():
    cfg = c.HLRCfg('hei', '1')

    # send 10 command with 10 phone_no
    ent = c.Entry(cfg)
    ph = 15700240000
    imsi = 460077296271000
    for i in range(100):
        ph += 1
        imsi += 1
        cmd = "0|521|%d|%d|||"%(ph, imsi)
        ent.sendone(cmd)
    ent.close()

    #consume with 1 client and 10 time
    cli = c.Client(cfg)
    if cli.login() != True:
        print 'login failed'
        return
    cmds = []
    for i in range(100):
        ack = cli.getorder(None)
        if ack == None:
            print 'can not get order', i
            break
        print 'getorder stream_id', ack.stream_id, ack.phone_no
        cmds.append(ack)
        
    # batch reply to manager
    for ack in cmds:
        ack.set_reply('0000', 'success')
        ack2 = cli.getorder(ack)
        if ack2 != None:
            print 'more orders found', ack2.stream_id, ack2.phone_no
        else:
            print 'ack stream_id', ack.stream_id

    cli.close()

'''
100 phone_no
    * (2 hss cmd + 2 tas cmd + 1 hss cmd)
019 1
018 1
338 3
339 3
000 1'''


def batchcmd():
    cfg = c.HLRCfg('scx', '1')

    # send 10 command with 10 phone_no
    ent = c.Entry(cfg)
    ph = 15700240000
    imsi = 460077296271000
    for i in range(100):
        ph += 1
        imsi += 1
        for cmd in ('019', '018', '338', '339', '001'):
            s = "0|%s|%d|%d|||" % (cmd, ph, imsi)
            ent.sendone(s)
    ent.close()


'''
556  开通4g

VOLTE 升级：
===========
009 	IMS开户
338 	tas开户
560 	卸载签约信息
005 	VOLTE TCSI锚定
012 	EPS APN绑定
002 	ENUM域名添加(7701)  002

VOLTE 降级：
===========
010 	ENUM域名删除(7702)  010
013 	删除IMS APN
015 	删除VOLTE TCSI
339 	tas销户
008 	IMS销户
'''
def volte_test():
    cfg = c.HLRCfg('scx', '1')
    ph = '15700201401'
    imsi = '460028816705929'
    cmds = '009 338 560 005 012 002'
    #cmds = '010 013 015 339 008'
    ent = c.Entry(cfg)
    for cmd in cmds.split():
        s = "0|%s|%s|%s|||" % (cmd, ph, imsi)
        ent.sendone(s)
    ent.close()
    print 'sent', cmds, 'for', ph, imsi
    print 'done'

import cx_Oracle
import time
def batch_cmd():
    sql = '''insert into sndcmddayscx (COMMAND_ID, HLR_CODE, COMMAND_CODE, BUSINESS_STATUS, COMMAND_ORDER, ID_NO, BELONG_CODE, SM_CODE, PHONE_NO, NEW_PHONE, IMSI_NO, NEW_IMSI, OTHER_CHAR, OP_CODE, TOTAL_DATE, OP_TIME, LOGIN_NO, ORG_CODE, LOGIN_ACCEPT, REQUEST_TIME, SEND_STATUS, SEND_TIME, GROUP_ID, ORG_ID)
select :cmdid, 'xx', :code01, :code2, 1, 1, '3', 'sm', :phone_no, '0', :imsi_no, '0', 'null', '1104', 1, sysdate, 'xxxxxx', 'org', 1, sysdate, '0', sysdate, 'group', 'org' from dual'''
    co = cx_Oracle.connect('offon/Crmpd!06@onoffdb')
    cur = co.cursor()
    cmdid = int(time.time()) * 1000
    cmds = '009 338 560 005 012 002 010 013 015 339 008'
    cur.execute("select phone_head from cphonehlrcode where hlr_code='scx'")
    phead = (int(r[0].strip()) for r in cur.fetchmany(20))
    for p in phead:
        ph = p * 10000
        imsi = 460028816705929
        for i in range(10000):
            p1 = ph + i
            i1 = imsi + i
            rows = []
            for cmd in cmds.split():
                cmdid += 1
                rows.append([cmdid, cmd[0:2], cmd[2], p1, i1])
            cur.executemany(sql, rows)
            co.commit()
            print p1
    cur.close()
    co.close()

if __name__ == '__main__':
    batch_cmd()
