#coding=utf-8

import yaml
import json
import cx_Oracle
import sys
import re
import os
import copy
import time

'''
hss.yaml 文件格式检查
'''

# 1. 标签是否有重复
def ck_dulplicate(hss_file):
    h, lineno = {}, 0
    rstr = '(^h??\\.?):.*'
    for line in file(hss_file):
        lineno += 1
        m = re.search(rstr, line)
        if m is None:
            continue
        tag = m.group(1)
        if h.has_key(tag):
            print "ERROR1, TAG dulicate %s lineno: %d"%(tag, lineno)
            return False
        h[tag] = 1
    print '1 ck_dulplicate done.'
    return True

# 2 检查 yaml 格式 是否完整
def ck_yaml(hss_file, skip_hlrs):
    try:
        t1 = time.time()
        if hss_file.endswith('.yaml'):
            conf = yaml.load(file(hss_file))
        elif hss_file.endswith('.json'):
            conf = json.load(file(hss_file))
        else:
            print 'not support file format'
            return None
        t2 = time.time()
        print 'yaml.load time: %.3f'%(t2-t1)
        # had.9: had.1
        # had.9:
        #   copy_from: had.1
        #   ...
        # 上面两种配置方式的展开
        conf1 = {}
        for k, v in conf.items():
            print '--', k
            if type(v) == type('') or type(v) == type(u''):
                x = conf.get(v, None)
                if x is None:
                    print 'ERROR2, replicate config %s from %s error, %s not found'%(k, v, v)
                    return None
                v = x
            if type(v) != dict:
                print 'ERROR2: not a dict %s of %s'%(k, repr(v))
                return None
            if v.has_key('copy_from'):
                x = conf[v['copy_from']]
                x = copy.deepcopy(x)
                for y, z in v.items():
                    x[y] = z
                v = x
            conf1[k] = v
        conf = conf1

        if len(skip_hlrs) > 2:
            for k in conf.keys():
                hlr = k[0:3]
                if skip_hlrs.find(hlr) >= 0:
                    del conf[k]
        print '2 ck_yaml done.'
        return conf
    except Exception,ex:
        print 'ERROR2, yaml format error', ex
        return None

def conndb():
    s = file('%s/gdblogin.cfg'%os.environ['ETCDIR']).read()
    user,pswd,db = s.strip().split()
    co = cx_Oracle.connect('%s/%s@%s'%(user,pswd, db))
    return co

# 3 检查配置的端口与库中的 cmaincfginfo 表是否匹配
def ck_manager_port(conf):
    co = conndb()
    cur = co.cursor()
    ports = {}
    cur.execute('select hlr_code, ip_addr, comm_port from cmaincfginfo')
    for r in cur.fetchall():
        ports[r[0].strip()] = (r[1].strip(), r[2])
    cur.close()
    co.close()

    for k, v in conf.items():
        hlrcode = k[0:3]
        db_h = ports.get(hlrcode, None)
        if db_h is None:
            print 'ERROR3, port not found in database'%k
            return False
        if v['manager.ip'] != db_h[0]:
            print 'ERROR3, manager ip not match in %s, db:%s yaml: %s'%(k, db_h[0], v['manager.ip'])
            return False
        if v['manager.port'] != db_h[1]:
            print 'ERROR3, manager port not match in %s, db:%d yaml: %d'%(k, db_h[1], v['manager.port'])
            return False
    print '3 ck_manager_port done.'
    return True

# 4 检查order文件的 yaml 格式
def ck_order_file(conf):
    for k, v in conf.items():
        order_file = v['order.path']
        try:
            c = yaml.load(file(order_file))
        except Exception,ex:
            print 'ERROR4, order.path %s for %s format error: %s'%(order_file, k, ex)
            return False
    print "4 ck_order_file done."
    return True

def main():
    hss_file = sys.argv[1]
    skip_hlrs = ''
    if len(sys.argv) > 2:
        skip_hlrs = sys.argv[2]

    if hss_file.endswith('.yaml'):
        if not ck_dulplicate(hss_file):
            return
    conf = ck_yaml(hss_file, skip_hlrs)
    if conf is None: return

    if not ck_manager_port(conf): return
    if not ck_order_file(conf): return


if __name__ == '__main__':
    main()


