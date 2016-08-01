CFLAGS= -I. -I$(ORACLE_HOME)/precomp/public -I$(ORACLE_HOME)/rdbms/public -I../gyf/cutil/include

CC=gcc -m64 -g

#LFLAGS=-L$(ORACLE_HOME)/lib -lclntsh -lm  -lpthread -lsocket -lnsl
LFLAGS=-L$(ORACLE_HOME)/lib -lclntsh -lm  -lpthread
PROCPLSFLAGS=include=../gyf/cutil/include include=$(ORACLE_HOME)/rdbms/public include=./ sys_include=\(/usr/include,/usr/lib/gcc/x86_64-linux-gnu/4.8/include,/usr/include/x86_64-linux-gnu\)

#userid=crm/crm@xe
#PRECOMP= $(ORACLE_HOME)/bin/proc DYNAMIC=ANSI USERID=$(userid) sqlcheck=SEMANTICS LINE=TRUE define=_PROC_
PRECOMP= $(ORACLE_HOME)/bin/proc DYNAMIC=ANSI LINE=TRUE define=_PROC_

CPROPS=-L../gyf/cutil/lib -lcprops64 -lgyftool64
GYFLIB=-L../gyf/cutil/lib -lgyftool64

.SUFFIXES: .pc .c .o

#all: offon_manager offon_entry q_check fetch_cmd_one offon_hlr
all: offon_manager offon_entry offon_fordb

offon_manager: offon_manager.o testdir.o db_func.o daemon.o util.o file_func.o util_str.o util_tcp.o
	$(CC) -o $@ $^ $(LFLAGS)

OBJ11=offon_entry.o testdir.o db_func.o daemon.o sigtool.o util.o util_tcp.o util_str.o file_func.o
offon_entry:$(OBJ11)
	$(CC) -o $@ $^ $(LFLAGS)

q_check: q_check.o db_func.o util.o
	$(CC) -o $@ $^ $(LFLAGS) $(CFLAGS)
	cp $@ bin/

offon_fordb=offon_fordb.o testdir.o db_func.o util.o file_func.o daemon.o
offon_fordb:$(offon_fordb)
	$(CC) -o $@ $(offon_fordb) $(LFLAGS)

dul_syn_recv=dul_syn_recv.o db_func.o daemon.o util.o dul_util.o
dul_syn_recv:$(dul_syn_recv)
	$(CC) -o $@ $(dul_syn_recv) $(LFLAGS) $(CFLAGS) $(CPROPS) -lrt
	cp $@ bin/

dul_syn_recv.c:dul_syn_recv.pc
	$(PRECOMP) THREADS=YES $? $(PROCPLSFLAGS) 

offon_rongzai:$(OBJ12)
	$(CC) -o $@ $(OBJ12) $(LFLAGS) $(CFLAGS)
	mv offon_rongzai ../bin

offon_query:$(OBJ21)
	$(CC) -o $@ $(OBJ21) $(LFLAGS) $(CFLAGS)
	mv offon_query ../bin

offon_alcatel:$(OBJ31)
	$(CC) -o $@ $(OBJ31) $(LFLAGS) $(CFLAGS)
	mv offon_alcatel ../bin

offon_nokia:$(OBJ41)
	$(CC) -o $@ $(OBJ41) $(LFLAGS) $(CFLAGS)
	mv $@ ../bin

offon_hlr: offon_hlr.o util.o db_func.o daemon.o testdir.o util_tcp.o
	$(CC) -o $@ $^ $(LFLAGS)
	cp $@ bin/

offon_nokia_mml:offon_nokia_mml.o testdir.o db_func.o daemon.o sigtool.o util.o query_parse.o util_tcp.o des.o
	$(CC) -o $@ $^ $(LFLAGS) $(CFLAGS)
	ftp_put.sh 10.105.2.16 offon sco505sco505 /offon/bin $@

offon_ericsson:$(OBJ51)
	$(CC) -o $@ $(OBJ51) $(LFLAGS) $(CFLAGS)
	mv offon_ericsson ../bin

offon_siemens_cu:$(OBJ61)
	$(CC) -o $@ $? $(LFLAGS) $(CFLAGS)
#	mv $@ ../bin

offon_siemens_Q3:$(OBJ62)
	$(CC) -o $@ $? $(LFLAGS) $(CFLAGS)
	mv $@ ../bin

offon_siemens_Q3_SR11:$(OBJ63)
	$(CC) -o $@ $? $(LFLAGS) $(CFLAGS)
	mv $@ ../bin

offon_huawei:$(OBJ71)
	$(CC) -o $@ $? $(LFLAGS) $(CFLAGS)
	mv $@ ../bin

offon_huawei_3G:$(OBJ72)
	$(CC) -o $@ $(OBJ72) $(LFLAGS) $(CFLAGS)
	cp $@ ../../bin

offon_zte:$(OBJ81)
	$(CC) -o $@ $? $(LFLAGS) $(CFLAGS)
	mv $@ ../bin

fetch_cmd:$(OBJ91)
	$(CC) -o $@ $(OBJ91) $(LFLAGS) $(CFLAGS)
	mv fetch_cmd ../bin

offon_ora_put:$(OBJ92)
	$(CC) -o $@ $? $(LFLAGS) $(CFLAGS)
	mv $@ ../bin

offon_oracle_put:$(OBJ93)
	$(CC) -o $@ $? $(LFLAGS) $(CFLAGS)
	mv $@ ../bin

fetch_cmd_one: fetch_cmd_one.o daemon.o util_tcp.o db_func.o util.o dul_util.o
	$(CC) -o $@ $^ $(LFLAGS) $(CFLAGS)
	cp $@ bin/

mopen_ericsson: mopen_ericsson.o db_func.o util.o
	$(CC) -o $@ $^ $(LFLAGS) $(CFLAGS) $(GYFLIB)
	cp $@ ../bin

mopen_siemensQ3: mopen_siemensQ3.o db_func.o util.o
	$(CC) -o $@ $^ $(LFLAGS) $(CFLAGS) $(GYFLIB)
	cp $@ ../bin

dul_shmtool: dul_shmtool.o db_func.o dul_util.o util.o
	$(CC) -o $@ $^ $(LFLAGS) $(CFLAGS)
	cp $@ ../bin

CRBOBJ=crb.o util_str.o util_logs.o
crb: $(CRBOBJ)
	$(CC) -o $@ $(CRBOBJ) $(LFLAGS) $(CFLAGS)

VPMNOBJ=vpmn.o util_str.o util_logs.o
vpmn: $(VPMNOBJ)
	$(CC) -o $@ $(VPMNOBJ) $(LFLAGS) $(CFLAGS)

TOHISOBJ=tohis.o util_str.o util_logs.o
tohis: $(TOHISOBJ)
	$(CC) -o $@ $(TOHISOBJ) $(LFLAGS) $(CFLAGS)

FBATCH=fetch_cmd_batch.o daemon.o util_tcp.o db_func.o util.o dul_util.o
fetch_cmd_batch: $(FBATCH)
	$(CC) -o $@ $(FBATCH) $(LFLAGS) $(CFLAGS) $(GYFLIB)
	cp $@ ../bin

RBATCH=rcv_batch.o util_tcp.o util_str.o daemon.o boss_hlr_diff.o
rcv_batch: $(RBATCH)
	$(CC) -o $@ $(RBATCH) $(LFLAGS) $(CFLAGS)
	ps -ef|grep rcv_batch|grep -v grep|grep -v make|cut -c9-15|xargs kill
	sleep 1
	./rcv_batch -Fcrmoprdb.cfg -L9888

RBOSSHLRDIFF=boss_hlr_diff.o util_tcp.o util_str.o daemon.o
boss_hlr_diff: $(RBOSSHLRDIFF)
	$(CC) -o $@ $(RBOSSHLRDIFF) $(LFLAGS) $(CFLAGS)

file_shmtool: shmtest.o file_func.o util_str.o util_tcp.o
	$(CC) -o $@ $^ -lxnet
	cp $@ ../bin

%.c: %.pc
	$(PRECOMP) $(PROCPLSFLAGS) $?

%.o: %.c
	@echo " Compiling the $<"
	$(CC)  -c  $(CFLAGS) $<

db_func.c:db_func.pc
	$(PRECOMP) db_func.pc $(PROCPLSFLAGS) 

offon_entry.c:offon_entry.pc
	$(PRECOMP) offon_entry.pc $(PROCPLSFLAGS) 

offon_rongzai.c:offon_rongzai.pc
	$(PRECOMP) offon_rongzai.pc $(PROCPLSFLAGS) 

fetch_cmd.c:fetch_cmd.pc
	$(PRECOMP) fetch_cmd.pc $(PROCPLSFLAGS) 

offon_ora_put.c:offon_ora_put.pc
	$(PRECOMP) offon_ora_put.pc $(PROCPLSFLAGS) 

offon_oracle_put.c:offon_oracle_put.pc
	$(PRECOMP) offon_oracle_put.pc $(PROCPLSFLAGS) 

fetch_cmd_one.c:fetch_cmd_one.pc
	$(PRECOMP) fetch_cmd_one.pc $(PROCPLSFLAGS) 

fetch_cmd_batch.c:fetch_cmd_batch.pc
	$(PRECOMP) fetch_cmd_batch.pc $(PROCPLSFLAGS) 

#rcv_batch.c:rcv_batch.pc
#	$(PRECOMP) rcv_batch.pc $(PROCPLSFLAGS) 

clean:
	rm -f *.o
	rm -f db_func.c offon_entry.c offon_fordb.c offon_rongzai.c offon_ora_put.c offon_oracle_put.c fetch_cmd_one.c fetch_cmd.c 
	rm -f *.lis
	rm -f offon_manager offon_entry offon_fordb

move:
	mv offon_manager ../bin
	mv offon_entry ../bin
	mv offon_rongzai ../bin
	mv offon_alcatel ../bin
	mv offon_nokia ../bin
	mv offon_ericsson ../bin
	mv offon_siemens_cu ../bin
	mv offon_query ../bin
	mv fetch_cmd ../bin
	mv fetch_cmd_one ../bin
	mv offon_ora_put ../bin
