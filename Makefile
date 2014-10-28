PROJECT=vrrpd

ALL_EXE=vrrpd atropos

ATROPOS_OBJS = atropos.o

VRRPD_OBJS = vrrpd.o libnetlink.o ipaddr.o

MAIN_OPT=
PROF_OPT=
DBG_OPT=-g
#MACHINEOPT=-DMY_BIG_ENDIAN -DSUNOS
#MACHINEOPT=-DMY_LITTLE_ENDIAN
#INCLUDEOPT=
#INCLUDEOPT= -I./libpcap
#LINKLIB = -lpcap
COMMON_CFLAGS= $(MAIN_OPT) $(INCLUDEOPT) $(PROF_OPT) $(DBG_OPT) $(MACHINEOPT)

# set to compile with GCC
CC=gcc
CFLAGS= $(COMMON_CFLAGS) -Wall 

all: $(ALL_EXE)

vrrpd : $(VRRPD_OBJS)
	$(CC) $(PROF_OPT) -o $@ $^ -lrt $(LINKLIB)

atropos:$(ATROPOS_OBJS)
	$(CC) $(PROF_OPT) -o $@ -lrt $^

vrrpd.o: vrrpd.h 
libnetlink.o: libnetlink.h

dist:
	(make clean; rm -fr .protect/; cd ..; tar cvzf vrrpd-lastest.tgz vrrpd atropos)

##############################################
#  beyond this points only mangement target. #
##############################################
clean :
	@echo Begin to clean
	$(RM) *.[ao] *~ core
	$(RM) $(ALL_EXE) 
	@echo Clean is completed

mrproper : clean
	@echo Begin to mrproper
	$(RM) -fr .depend .protect/ $(ALL_EXE)
	@echo Mrproper is completed

strip :
	@echo Begin to strip $(ALL_EXE)
	$(STRIP) $(ALL_EXE)
	@echo $(ALL_EXE) have been striped

backup: mrproper cleanlog
	@echo Backup to backup
	(cd .. && tar cf - $(PROJECT) | gzip -9 >$(PROJECT).tgz)
	sync
	@echo Backup is completed

# only GCC. 
#depend dep:
#	@echo Start to build depedances in .depend
#	for i in *.c; do $(CPP) -M $(CFLAGS) $$i; done > .tmpdepend
#	mv -f .tmpdepend .depend
#	@echo Dependances completed

# only gmake and good make
#ifeq (.depend,$(wildcard .depend))
#include .depend
#endif


