import os


## make order files to a single yaml file
def writefile(f, fout, tag):
    fout.write("%s: |\n"%tag)
    s = file(f).read().split('\n')
    for line in s:
        if line.find('include') > 0: continue
        line = line.strip()
        if len(line) == 0:
            continue
        fout.write('  %s\n'%line)

## make order files to a single yaml file  
def order2yaml():
    fout = file('orders.yaml', 'w')
    writefile('header.txt', fout, 'header')
    writefile('footer.txt', fout, 'footer')
    for f in os.listdir('.'):
        try:
            int(f)
        except: continue
        writefile(f, fout, "OP%s"%f)   
    fout.close()
 
def hsscfg2yaml():
    fout = file('hss.yaml', 'w')
    flds = ["hlrcode", "hlrport", "manager.ip", "manager.port", 
                 "remote.url", "remote.timeout", "http.keep.test", "http.keep.cfg", "order.path",
                 "start.tag", "end.tag", "manager.user", "manager.password", "term.type",
                 "APN_TPL_ID", "IPHEX_TO_INT", "PROPERTIES"]
    for line in file('hss.cfg'):
        line = line.strip()
        if len(line) < 50 or line.startswith('#'):
            continue
        x = line.split('|')
	l = len(x)
        if l < 14:
            continue
	
        fout.write("%s.%s:\n"%(x[0], x[1]))
	for i in range(2, 17, 1):
	    if l <= i:
		continue
	    v = x[i].strip()
	    if v == '@':
		v = ""
	    fout.write("  %s: %s\n"% ( flds[i], v) )
    fout.close()

hsscfg2yaml()
#order2yaml()
