

#reference of comm.h

import socket
import time
import struct
import sys

#define		CONN_KEEP	00000010
#define		GENERAL_ACK		00000000
#define		SRV_LOGIN		00000001

#define		SRVCODE0001		10000001  entry data
SRVCODE0001 = 10000001
SRV_LOGIN = 00000001
GETORDER  =		10000002

class HLRCfg:
    def __init__(self, hlrcode, port):
        self.hlrcode = hlrcode
        self.port = port
        self.addr = None
        self.username = None
        self.password = None
        self.term_type = None
        self.term_id = ''
        self.__getcfg()

    def __getcfg(self):
        hlrcode = self.hlrcode
        port = self.port
        import cx_Oracle
        co = cx_Oracle.connect("offon/ll@xe")
        cur = co.cursor()
        cur.execute("select ip_addr, comm_port from cmaincfginfo where hlr_code=:v1",
                     (hlrcode,))
        r = cur.fetchone()
        if r != None:
            self.addr = (r[0].strip(), r[1])
        if self.port != '' and self.port != None:
            print 'got cmainhlrcfg'
            cur.execute("""select user_name, user_pswd, term_type, term_id
        from cloginuser
        where hlr_code=:v1 and hlr_port=:v2""",
                        (hlrcode, port) )
            r = cur.fetchone()
            if r != None:
                print 'got cloginuser'
                self.username = r[0].strip()
                self.password = r[1].strip()
                self.term_type = r[2].strip()
                self.term_id = r[3].strip()
        cur.close()
        co.close()
        

def write_pkt(sock, msgtype, msgdata):
    head_len = 32
    msglen = len(msgdata) + head_len
    header = "`CD'%04d%08d%08d%s%s%s%-5s" % (
        msglen,
        msgtype,
        1, # transid
        '3', # 1-TX_BEG, 2-TX_CON, 3-TX_END
        '1', # 1-req 2-ack
        '2', # 1-encrypt 2-decrypt
        '00000', # reserve
        )
    sock.sendall(header)
    sock.sendall(msgdata)

def read_pkt(sock):
    head_len = 32
    header = sock.recv(head_len)
    if len(header) != head_len:
        return None
    msglen = int(header[4:8])
    msgtype = int(header[8:16])
    msglen -= head_len
    data = sock.recv(msglen)
    return msgtype, data

# send offon_entry data, from  cmdid|cmdcode|phone|imsi|ssinfo1|ssinfo2|ssinfo3
# 0|551|15700240244|460077296271684|||
class Entry:
    def __init__(self, cfg):
        self.hlrcode = cfg.hlrcode
        self.addr = cfg.addr
        self.sock = socket.create_connection(cfg.addr)
        self.id_seed = int(time.time())
        print "connected to", cfg.addr

    def sendone(self, s):
        flds = s.split('|')
        if len(flds) != 7:
            print 'fields count error'
            return False
        data = []   # struct op_data_req
        cmdid, cmdcode = flds[0], flds[1]
        if cmdid == '0': #
            self.id_seed += 1
            cmdid = str(self.id_seed)
        data.append(cmdid.ljust(16, '\0')) #command_id 16
        data.append(flds[2].ljust(16, '\0')) #phone_no 16
        data.append(flds[3].ljust(21, '\0')) #imsi_no 21
        data.append('0000'.ljust(6, '\0'))   #op_code 6
        data.append(cmdcode[:2].ljust(6, '\0')) #command_code 6
        data.append(cmdcode[2]) # cmd_status 1
        data.append('1') #phone_priority 1
        s1, s2 = flds[4], flds[5]
        if s1 == '': s1 = '0'
        if s2 == '': s2 = '0'
        data.append(s1.ljust(16, '\0')) #ss_info1 16
        data.append(s2.ljust(21, '\0')) #ss_info2 21
        data.append(flds[6].ljust(121, '\0')) #ss_info3 121
        #print '--', len("".join(data))
        data.append(time.strftime("%Y%m%d%H%M%S").ljust(16,'\0')) #req_time 16
        data.append('xxxxxx\0\0')
        fdata = "".join(data)
        #print len(fdata), repr(fdata)
        #print '----', repr(fdata[225:])
        try:
            write_pkt(self.sock, SRVCODE0001, fdata)
        except Exception,ex:
            print ex
            return False
        try:
            msgtype, data = read_pkt(self.sock)
            if len(data) >= 4 and data[:4] == '0000':
                return True
        except Exception,ex:
            print ex
        return False

    def close(self):
        self.sock.close()

class CmdReq:
    def __init__(self):
        self.type = '1' # '1'-ONLY_GET  '2'-REPLY_GET
        self.ack = None
        
    def set_data(self, ack):
        self.type = '2'
        self.ack = ack

    def to_pkt(self):
        if self.type == '1':
            return '00001'.ljust(242, '\0')
        elif self.type == '2':
            ack = self.ack
            data = []  # struct cmd_data_req
            data.append(ack.retn[:4].rjust(4, '0') )
            data.append(self.type[0])
            data.append(ack.phone_no.ljust(16, '\0'))
            data.append(ack.stream_id.ljust(16, '\0'))
            data.append(ack.order_code.ljust(4, '\0'))
            data.append(ack.info.ljust(201, '\0') )
            return "".join(data)

class CmdAck:
    def __init__(self, fdata):
        # struct cmd_data_ack
        if len(fdata) < 219:
            # general ack
            self.retn = fdata[:4]
            self.info = fdata[4:].strip('\0')
            return
        self.retn, stream_id, self.order_code, phone_no, imsi_no, s1, s2, s3 = struct.unpack(
            '4s16s4s16s21s16s21s121s', fdata)
        self.stream_id = stream_id.strip('\0')
        self.phone_no = phone_no.strip('\0')
        self.imsi_no = imsi_no.strip('\0')
        self.s1 = s1.strip('\0')
        self.s2 = s2.strip('\0')
        self.s3 = s3.strip('\0')

        self.info = ''

    def set_reply(self, retn, info):
        self.retn = retn
        self.info = info

    def __str__(self):
        return "retn: %s stream_id: %s ordercode: %s phone_no: %s imsi_no: %s"%(
            self.retn, self.stream_id, self.order_code, self.phone_no, self.imsi_no)
                        
class Client:
    def __init__(self, cfg):
        self.hlrcode = cfg.hlrcode
        self.hlrport = cfg.port
        self.cfg = cfg
        self.sock = None

    def login(self):
        cfg = self.cfg
        sock = socket.create_connection(cfg.addr)
        self.sock = sock

        # reference of struct login_req
        data = []
        data.append(cfg.username.ljust(8, '\0')) # username 8
        data.append(cfg.password.ljust(8, '\0')) # userpswd 8
        data.append(self.hlrcode.ljust(4, '\0')) # hlrcode  4
        data.append(self.hlrport[0])             # hlrport  1
        data.append(cfg.term_type[0])            # term_type 1
        data.append('\0\0')                      # reserve  2

        fdata = "".join(data)
        try:
            write_pkt(sock, SRV_LOGIN, fdata)
        except Exception,ex:
            print ex
            return False
        try:
            msgtype, data = read_pkt(sock)
            if len(data) >= 4 and data[:4] == '0000':
                return True
        except Exception,ex:
            print ex
        return False

    def getorder(self, ack):
        # reference of struct cmd_data_req
        c = CmdReq()
        if ack != None:
            c.set_data(ack)
        try:
            pkt = c.to_pkt()
            #print '--req len:', len(pkt)
            write_pkt(self.sock, GETORDER, pkt)
        except Exception,ex:
            print ex
            return None
        try:
            msgtype, data = read_pkt(self.sock)
            ack = CmdAck(data)
            if ack.retn == '3001': # NO DATAS
                return None
            return ack
        except Exception,ex:
            print ex
        return None
        
    
    def close(self):
        self.sock.close()
    
        

def entry():
    cfg = HLRCfg('hei', None)
    ent = Entry(cfg)
    print "eg.", "0|521|15700240244|460077296271684|||"
    while 1:
        s = raw_input('-->')
        s = s.strip()
        if s == '':
            s = "0|521|15700240244|460077296271684|||"
        if s == 'q' or s == 'Q':
            break
        ret = ent.sendone(s)
        print 'return', ret
    ent.close()

def client():
    cfg = HLRCfg('hei', '1')
    cli = Client(cfg)
    ret = cli.login()
    print 'login return', ret
    if not ret:
        cli.close()
        return
    ack_r = None
    while 1:
        print 'begin...'
        ack = cli.getorder(ack_r)
        if ack == None:
            ack_r = None
            print 'no data, sleep 5s'
            time.sleep(5.0)
            continue
        print 'getorder return', str(ack)
        ack_r = ack
        ack_r.set_reply('0000', 'success')
        
if __name__ == '__main__':
    if len(sys.argv) > 1 and sys.argv[1] == 'entry':
        entry()
    else:
        client()
