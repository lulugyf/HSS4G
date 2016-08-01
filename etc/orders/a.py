import os

def writefile(f, fout, tag):
    fout.write("%s: |\n"%tag)
    s = file(f).read().split('\n')
    for line in s:
        if line.find('include') > 0: continue
        if line.find('soapenv') > 0: continue
        if line.find('PassWord') > 0: continue
        if line.find('UserName') > 0: continue
        line = line.strip()
        if len(line) == 0:
            continue
        fout.write('  %s\n'%line)
    
def main():
    fout = file('orders.yaml', 'w')
    writefile('header.txt', fout, 'header')
    writefile('footer.txt', fout, 'footer')
    for f in os.listdir('.'):
        try:
            int(f)
        except: continue
        if len(f) != 4: continue
        writefile(f, fout, "OP%s"%f)   
    fout.close()
            

main()

