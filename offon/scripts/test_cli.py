
import comm as c

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
    cfg = c.HLRCfg('hei', '1')

    # send 10 command with 10 phone_no
    ent = c.Entry(cfg)
    ph = 15700240000
    imsi = 460077296271000
    for i in range(100):
        ph += 1
        imsi += 1
        for cmd in ('019', '018', '338', '339', '001'):
            s = "0|%s|%d|%d|||"%(cmd, ph, imsi)
            ent.sendone(s)
    ent.close()
    
if __name__ == '__main__':
    batchcmd()
