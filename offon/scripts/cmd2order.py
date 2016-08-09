#coding=utf-8

import cx_Oracle
import yaml
import sys
import os

# 从 command_code 获取指令的文本内容

def load_order_conf(hlr_code, hss_cfg):
    c = yaml.load(file(hss_cfg))
    o1 = c['%s.1'%hlr_code]
    base_path = os.path.abspath(hss_cfg)
    base_path = os.path.dirname(base_path)
    hss_orders = yaml.load(file(os.path.join(base_path, o1['order.path'] ) ))
    o2 = c['%s.8'%hlr_code]
    tas_orders = yaml.load(file(os.path.join(base_path, o2['order.path'])) )
    return hss_orders, tas_orders

def main(hss_cfg, hlr_code, cmds):
    hss_orders, tas_orders = load_order_conf(hlr_code, hss_cfg)

    co = cx_Oracle.connect("offon/opr_offon292@offondb")
    cur = co.cursor()
    for cmd in cmds.split(','):
        cur.execute("select order_code, serial from ccmdtoorder where hlr_code=:v1 and command_code=:v2 order by serial",
                    (hlr_code, cmd.ljust(4, ' ')))
        print '====== command_code', cmd
        for r in cur.fetchall():
            order_code = r[0]
            oo = 'OP%s'%order_code
            if hss_orders.has_key(oo):
                print '-- order[HSS]', order_code
                print hss_orders[oo]
            elif tas_orders.has_key(oo):
                print '-- order[TAS]', order_code
                print tas_orders[oo]
            else:
                print '-- order NOT FOUND', order_code
    cur.close()
    co.close()

if __name__ == '__main__':
    # haa: huawei
    # hbp: ericsson
    # hc6: nokia
    # python scripts/cmd2order.py conf/hss.yaml haa 339,008,013,015
    # 009,012,338,560,005,090,120,385,386,387,388,414,050,071,495,313
    # 339,008,013,015
    main(sys.argv[1], sys.argv[2], sys.argv[3])
