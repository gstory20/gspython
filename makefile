#
#  Makefile
#
#  Glenn Story -- 3/10/93
#
#  This is my general unix makefile
#

INCLS = gs.h gslib.h

BIN = $(HOME)/bin

#  
#  Platform-specific variables
#

CC ?= gcc
CXX ?= c++
CFLAGS ?= -fPIC
CXXFLAGS ?= 
LD ?= ld
LDFLAGS ?= 
OBJDIR ?= x86_obj
BINDIR ?= x86_bin
PYTHON_INC ?= -I/usr/include/python3.9 -I/usr/include/python2.7 -I/usr/include/python2.6 -I/usr/include/python2.5 -I/usr/include/python2.4 -I.

#
#  Ubuntu sets an obsolescent cc command for CC
#  We don't want that
#

ifeq ($(CC),cc)
	CC = gcc
endif

#
#  end of platform-specific variables
#

X11 = /usr/X11R6
XINCLUDES = -I. -I$(X11)/include
XLIBS =	 -L$(X11)/lib
LTERMCAP=

C_SOURCES = \
difftime.c testsum.c testaccum.c title.c watchdog.c crash.c timecalc.c \
whatis.c wwibm.c wwtdm.c wwmac1.c wwmac2.c wwunix.c strupr.c getfileh.c \
getfilel.c stripnl.c getword.c trim.c getachar.c lscrn.c hscrn.c ch.c \
fileck.c getachar.c box.c lscrntst.c rpad.c hscrntst.c gettext.c merge.c \
cal2.c cal2txt.c weekday.c rpt.c rpttxt.o datetime.c getone.c share.c \
amount.c n2e.c qacomp.c csv2.cpp namesimp.cpp bindiff.c binfind.c msgtest.c \
cdate.c newpage.c setsec.c jdate.c ztrim.c helpsub.c jday.c patcmp.c bhi.c \
alarm.c ctime.c dirpath.c lists.c names.c namestxt.c projects.c datecmp.c \
tick2.c tick2txt.c dbstruct.c dbase3.c dial.c update.c screen.c bike.c \
proj2.c cdatetst.c holiday.c pcount.c ccount.c stpblk.c codecnt.c myhex.c \
ex3.cpp pause.c screensize.c kaboom.c extname.c isnumeric.c saferun.c \
x10.cpp dailyck.c cal2imp.c ctimetst.c alarm2.c xcheck.c catmail.c crop.c \
stringc.cpp bconv.c macbin.c uufix.c crdbase.c	pgcnt.c l24.c cvtdate.c \
mysystem.c getname.c split.c rejoin.c newspick.c reset_modem.c countdown.c \
iozone.c whereis.c lsit.c cls.c fixdb.c rand.c listtxt.c listutl.c build.c \
uucombine.c inirdr.c tclck.c trimcmd.c sd.c aa.c xmenu.c xshowfonts.c \
phoneck.c getfile.c args.c bldtele.c phone.c callcnt.c mailsort.c sendelm.c \
getini.c mailedit.c stopwatch.c savea.c checka.c ring.c filer.c wotimer.c \
pattxt.c regexp.c dbexport.c dbimport.c except.cpp xbase.cpp xbtsttxt.cpp \
cmsgtxt.cpp wwtxt.c pbase.c ts.c proj2txt.c liststxt.c msgtxt.c tele2eudora.c \
trunc.c strlen.c truncline.c fprint.c printer.c histogram.c do.c cmds.c \
debuglog.c filertxt.c clearcal.c threads.c tester.c pbaseutl.c runmenu.c \
sysdpy.c pmem.c nutriwatch.c pickone.c xb.c pickdb.c sdb.c printwrap.c \
cpuclock.c showfloat.c isvm.c dump_termiios.c code_analyzer.c disassembe.c \
vdso_test.c mlm.c exec_test.c get_parent.c

.SUFFIXES : .o .c .cpp

$(OBJDIR)/%.o :: %.c
	$(CC) $(CFLAGS) -c -g -o $(OBJDIR)/$*.o $<

$(OBJDIR)/%.o : %.cpp 
	$(CXX) $(CXXFLAGS) -c -g -o $(OBJDIR)/$*.o $<

$(BIN)/%:: $(BINDIR)/%
	install.sh $(BINDIR)/$* $(BIN)

%.tst:: $(BINDIR)/%
	./testone.sh $(BINDIR)/$*

ACTIVE_EXE = $(BINDIR)/difftime $(BINDIR)/title $(BINDIR)/pbase \
$(BINDIR)/whatis $(BINDIR)/testsum $(BINDIR)/trim $(BINDIR)/testaccum \
$(BINDIR)/names $(BINDIR)/getfile $(BINDIR)/isnumeric $(BINDIR)/saya \
$(BINDIR)/binfind $(BINDIR)/truncline $(BINDIR)/fixdb $(BINDIR)/pgcnt \
$(BINDIR)/rand $(BINDIR)/watchdog $(BINDIR)/proj2 $(BINDIR)/tester \
$(BINDIR)/calendar $(BINDIR)/cpuclock $(BINDIR)/showfloat \
$(BINDIR)/myhex $(BINDIR)/bconv $(BINDIR)/merge $(BINDIR)/timecalc \
$(BINDIR)/crash $(BINDIR)/qacomp $(BINDIR)/calendar $(BINDIR)/lists \
$(BINDIR)/tickler2 $(BINDIR)/extname $(BINDIR)/saferun $(BINDIR)/dump_termios \
$(BINDIR)/merge3 $(BINDIR)/merge3b $(BINDIR)/convert $(BINDIR)/screen \
$(BINDIR)/sysdpy $(BINDIR)/mergecal $(BINDIR)/countdown $(BINDIR)/nutriwatch.exe \
$(BINDIR)/whereis $(BINDIR)/cls $(BINDIR)/runmenu $(BINDIR)/mergedb \
$(BINDIR)/pickone $(BINDIR)/cvttime $(BINDIR)/fprint $(BINDIR)/xb \
$(BINDIR)/sd.exe $(BINDIR)/ring $(BINDIR)/bindiff \
$(BINDIR)/fmeo.exe $(BINDIR)/cvtdate $(BINDIR)/ccount $(BINDIR)/pcount \
$(BINDIR)/nmerge $(BINDIR)/dbstruct $(BINDIR)/pmem $(BINDIR)/checkdb \
$(BINDIR)/stopwatch $(BINDIR)/printwrap $(BINDIR)/dscan \
$(BINDIR)/_pydbase.so $(BINDIR)/pydbase.py $(BINDIR)/__pyscrn.so \
$(BINDIR)/_cpu_info_report.so $(BINDIR)/cpu_info_report.py \
$(BINDIR)/_patcmp.so $(BINDIR)/patcmp.py $(BINDIR)/exec_test \
$(BINDIR)/__remote_pause.so $(BINDIR)/_remote_pause.py \
$(BINDIR)/_pyscrn.py $(BINDIR)/__cdate.so $(BINDIR)/_cdate.py \
$(BINDIR)/__cvttime.so $(BINDIR)/_cvttime.py $(BINDIR)/xcheck \
$(BINDIR)/_pylist.py $(BINDIR)/__pylist.so $(BINDIR)/args $(BINDIR)/wordsize \
$(BINDIR)/__getfile.so $(BINDIR)/_getfile.py $(BINDIR)/isvm \
$(BINDIR)/disassemble $(BINDIR)/vdso_test $(BINDIR)/mlm.exe


RLSE_EXE =  \
$(BIN)/_pydbase.so $(BIN)/_pyscrn.py $(BIN)/__pyscrn.so $(BIN)/__cdate.so \
$(BIN)/__remote_pause.so $(BIN)/_remote_pause.py \
$(BIN)/_cpu_info_report.so $(BIN)/cpu_info_report.py \
$(BIN)/_patcmp.so $(BIN)/patcmp.py $(BIN)/dump_termios \
$(BIN)/_cdate.py $(BIN)/_cvttime.py $(BIN)/__cvttime.so \
$(BIN)/_pylist.py $(BIN)/__pylist.so $(BIN)/args $(BIN)/cpuclock \
$(BIN)/_getfile.py $(BIN)/__getfile.so $(BIN)/wordsize $(BIN)/xcheck \
$(BIN)/testsum $(BIN)/pbase $(BIN)/whatis  $(BIN)/trim \
$(BIN)/binfind $(BIN)/truncline $(BIN)/testaccum $(BIN)/fixdb \
$(BIN)/difftime $(BIN)/title $(BIN)/myhex $(BIN)/bconv $(BIN)/merge \
$(BIN)/merge3 $(BIN)/pgcnt $(BIN)/rand $(BIN)/timecalc $(BIN)/runmenu \
$(BIN)/whereis $(BIN)/cls $(BIN)/proj2 $(BIN)/qacomp $(BIN)/fmeo.exe \
$(BIN)/sd.exe $(BIN)/ring $(BIN)/bindiff $(BIN)/convert $(BIN)/screen \
$(BIN)/nmerge $(BIN)/dbstruct $(BIN)/watchdog $(BIN)/tester $(BIN)/sysdpy \
$(BIN)/pmem $(BIN)/names $(BIN)/calendar $(BIN)/tickler2 $(BIN)/mergecal \
$(BIN)/countdown $(BIN)/lists $(BIN)/nutriwatch.exe $(BIN)/mergedb  \
$(BIN)/pickone $(BIN)/cvttime $(BIN)/fprint $(BIN)/xb $(BIN)/exec_test \
$(BIN)/cvtdate $(BIN)/checkdb $(BIN)/merge3b $(BIN)/ccount $(BIN)/pcount \
$(BIN)/stopwatch $(BIN)/printwrap $(BIN)/getfile $(BIN)/showfloat \
$(BIN)/extname $(BIN)/dscan $(BIN)/isnumeric $(BIN)/pydbase.py $(BIN)/crash \
$(BIN)/isvm $(BIN)/saya $(BIN)/disassemble $(BIN)/vdso_test $(BIN)/mlm.exe

INACTIVE_EXE = $(BINDIR)/bhi $(BINDIR)/alarm_do $(BINDIR)/trunc \
$(BINDIR)/strlen $(BINDIR)/pickdb \
$(BINDIR)/filer $(BINDIR)/dailyck $(BINDIR)/gsmtest $(BINDIR)/screensize \
$(BINDIR)/stopwatch $(BINDIR)/alarm2 $(BINDIR)/mailsort $(BINDIR)/build \
$(BINDIR)/clearcal $(BINDIR)/inirdr $(BINDIR)/dup $(BINDIR)/do \
$(BINDIR)/cmds $(BINDIR)/nmergecal \
$(BINDIR)/cvtate $(BINDIR)/split $(BINDIR)/rejoin  \
$(BINDIR)/catmail $(BINDIR)/crop $(BINDIR)/histogram \
$(BINDIR)/lsit $(BINDIR)/iozone $(BINDIR)/macbin $(BINDIR)/uufix \
$(BINDIR)/convert $(BINDIR)/pgcnt \
$(BINDIR)/pcount $(BINDIR)/ccount $(BINDIR)/file_exists $(BINDIR)/bike \
$(BINDIR)/proj2 \
$(BINDIR)/dial $(BINDIR)/codecnt $(BINDIR)/newspick $(BINDIR)/reset_modem \
$(BINDIR)/ex3 $(BINDIR)/l24 $(BINDIR)/getname $(BINDIR)/uucombine \
$(BINDIR)/noguard $(BINDIR)/tclck $(BINDIR)/tdepwd $(BINDIR)/phoneck \
$(BINDIR)/n2e $(BINDIR)/bldtele $(BINDIR)/phone $(BINDIR)/callcnt \
$(BINDIR)/amount $(BINDIR)/projects \
$(BINDIR)/sendelm $(BINDIR)/mailedit $(BINDIR)/savea $(BINDIR)/checka \
$(BINDIR)/share $(BINDIR)/xmenu $(BINDIR)/wotimer $(BINDIR)/ts \
$(BINDIR)/tele2eudora $(BINDIR)/cop

TEST_EXE = $(BINDIR)/lscrntst $(BINDIR)/hscrntst $(BINDIR)/cdatetst \
$(BINDIR)/chars $(BINDIR)/msgtest $(BINDIR)/ctimetst $(BINDIR)/tester \
$(BINDIR)/qacomp

KINDLE_EXE = $(BINDIR)/sysdpy $(BINDIR)/whatis $(BINDIR)/whereis \
$(TEST_EXE) $(BINDIR)/arm $(BINDIR)/bindiff $(BINDIR)/sd.exe \
$(BINDIR)/sd.sh $(BINDIR)/setpath $(BINDIR)/getver $(BINDIR)/dscan \
$(BINDIR)/merge $(BINDIR)/pmem

#  active is the default target
active:	 $(TEST_EXE) $(ACTIVE_EXE)

#
#  Standard targets
#

all: $(TEST_EXE) $(ACTIVE_EXE) $(INACTIVE_EXE)

test:  $(TEST_EXE) cdatetst.tst lscrntst.tst hscrntst.tst ctimetst.tst \
msgtest.tst chars.tst wwtest.tst pgcnt.tst

clean:
	-rm *.o
	-rm $(OBJDIR)/*.o

clobber: clean
	-rm $(ACTIVE_EXE)
	-rm $(INACTIVE_EXE)
	-rm $(TEST_EXE)

install:  $(RLSE_EXE)

setup:
	@echo CC = $(CC)
	@echo CXX = $(CXX)
	@echo CFLAGS = $(CFLAGS)
	@echo CXXFLAGS = $(CXXFLAGS)
	@echo LD = $(LD)
	@echo LDFLAGS = $(LDFLAGS)
	@echo OBJDIR = $(OBJDIR)
	@echo BINDIR = $(BINDIR)

kindle:  setup $(KINDLE_EXE)

# type 'make depend' to make C source file header dependencies
#depend:
#	makedepend $(XINCLUDES) \
#-I/usr/local/lib/g++-include $(C_SOURCES)

wwtest.tst:	$(BINDIR)/whatis
	BINDIR=$(BINDIR) ./testone.sh ./wwtest

cdatetst.tst:	$(BINDIR)/cdatetst
	./testone.sh $(BINDIR)/cdatetst 9/13/2005 -s

tarball:  
	-rm $(BINDIR)/gs.tgz
	cd $(BINDIR);tar zcvf gs.tgz *

$(BINDIR)/cpu_info_report.py:  cpu_info_report.py
	cp cpu_info_report.py $(BINDIR)

cpu_info_report.py:  cpu_info_report.i
	swig -python cpu_info_report.i

cpu_info_report_wrap.c:  cpu_info_report.i
	swig -python cpu_info_report.i

$(OBJDIR)/cpu_info_report_wrap.o:	cpu_info_report_wrap.c
	$(CC) -g -c -o $(OBJDIR)/cpu_info_report_wrap.o -fPIC $(PYTHON_INC) cpu_info_report_wrap.c

EXEC_TEST_OBJS = $(OBJDIR)/exec_test.o

$(BINDIR)/exec_test: $(EXEC_TEST_OBJS)
	$(CC) -g -o $(BINDIR)/exec_test $(EXEC_TEST_OBJS)

MLM_OBJS = $(OBJDIR)/mlm.o $(OBJDIR)/lscrn.o

$(BINDIR)/mlm.exe:  $(MLM_OBJS)
	$(CC) -g -o $(BINDIR)/mlm.exe $(MLM_OBJS)

DISASSEMBLE_OBJS = $(OBJDIR)/disassemble.o

$(BINDIR)/disassemble:  $(DISASSEMBLE_OBJS)
	$(CC) -o $(BINDIR)/disassemble $(DISASSEMBLE_OBJS)

CODE_ANALYZER_OBJS = $(OBJDIR)/code_analyzer.o

$(BINDIR)/code_analyzer:  $(CODE_ANALYZER_OBJS)
	$(CC) -o $(BINDIR)/code_analyzer $(CODE_ANALYZER_OBJS)

DUMP_TERMIOS_OBJS = $(OBJDIR)/dump_termios.o $(OBJDIR)/getachar.o \
$(OBJDIR)/lscrn.o

$(BINDIR)/dump_termios:  $(DUMP_TERMIOS_OBJS)
	$(CC) -o $(BINDIR)/dump_termios $(DUMP_TERMIOS_OBJS)

VDSO_TEST_OBJS = $(OBJDIR)/vdso_test.o

$(BINDIR)/vdso_test: $(VDSO_TEST_OBJS)
	$(CC) -g -static -o $(BINDIR)/vdso_test $(VDSO_TEST_OBJS)

CPU_INFO_REPORT_OBJS = $(OBJDIR)/cpu_info_report_wrap.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o \
$(OBJDIR)/datetime.o $(OBJDIR)/cdate.o $(OBJDIR)/rpad.o $(OBJDIR)/fileck.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/jday.o $(OBJDIR)/jdate.o $(OBJDIR)/weekday.o \
$(OBJDIR)/ch.o $(OBJDIR)/rpt.o $(OBJDIR)/rpttxt.o $(OBJDIR)/cpu_info_report.o \
$(OBJDIR)/getfilel.o $(OBJDIR)/getfileh.o \
$(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/box.o $(OBJDIR)/patcmp.o $(OBJDIR)/printer.o $(OBJDIR)/newpage.o  \
$(OBJDIR)/trim.o $(OBJDIR)/strupr.o $(OBJDIR)/dirpath.o

$(BINDIR)/_cpu_info_report.so: $(CPU_INFO_REPORT_OBJS)
	$(CC) -shared $(LDFLAGS) -o $(BINDIR)/_cpu_info_report.so $(CPU_INFO_REPORT_OBJS)

$(BINDIR)/pydbase.py:  pydbase.py
	cp pydbase.py $(BINDIR)

pydbase.py:  pydbase.i
	swig -python pydbase.i

pydbase_wrap.c:  pydbase.i
	swig -python pydbase.i

$(BINDIR)/patcmp.py:  patcmp.py
	cp patcmp.py $(BINDIR)

patcmp.py:  patcmp.i
	swig -python patcmp.i

patcmp_wrap.c:  patcmp.i
	swig -python patcmp.i

$(BINDIR)/_getfile.py:  _getfile.py
	cp _getfile.py $(BINDIR)

_getfile.py:  _getfile.i
	swig -python _getfile.i

_getfile_wrap.c:  _getfile.i
	swig -python _getfile.i

$(OBJDIR)/isvm.o: isvm.c
	$(CC) -c -g -o $(OBJDIR)/isvm.o isvm.c

ISVM_OBJS = $(OBJDIR)/isvm.o

$(BINDIR)/isvm: $(ISVM_OBJS)
#	$(CC) -static -g -o $(BINDIR)/isvm $(ISVM_OBJS)
	$(CC) -g -o $(BINDIR)/isvm $(ISVM_OBJS)
SHOWFLOAT_OBJS = $(OBJDIR)/showfloat.o

$(BINDIR)/showfloat:  $(SHOWFLOAT_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/showfloat $(SHOWFLOAT_OBJS)

CPUCLOCK_OBJS = $(OBJDIR)/cpuclock.o

$(BINDIR)/cpuclock:  $(CPUCLOCK_OBJS)
	$(CC) -o $(BINDIR)/cpuclock -lrt $(CPUCLOCK_OBJS)

WORDSIZE_OBJS = $(OBJDIR)/wordsize.o

$(BINDIR)/wordsize:  $(WORDSIZE_OBJS)
	$(CC) -o $(BINDIR)/wordsize $(WORDSIZE_OBJS)

SPAM_OBJS = $(OBJDIR)/spammodule.o

$(BINDIR)/spam.so:	$(SPAM_OBJS)
	$(CC) -shared $(PYTHON_FLAGS) -o $(BINDIR)/spam.so $(SPAM_OBJS)
    
$(OBJDIR)/spammodule.o:	spammodule.c
	$(CC) -g -c -o $(OBJDIR)/spammodule.o -fPIC $(PYTHON_INC) spammodule.c

GETFILE_SO_OBJS = $(OBJDIR)/_getfile_wrap.o $(OBJDIR)/getfileh.o \
$(OBJDIR)/getfilel.o \
$(OBJDIR)/dirpath.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/rpad.o $(OBJDIR)/box.o $(OBJDIR)/trim.o $(OBJDIR)/getachar.o \
$(OBJDIR)/patcmp.o $(OBJDIR)/settitle.o  $(OBJDIR)/gethostname.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/fileck.o $(OBJDIR)/ch.o \
$(OBJDIR)/getone.o $(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o


$(BINDIR)/__getfile.so:	$(GETFILE_SO_OBJS)
	$(CC) -shared $(PYTHON_FLAGS) -o $(BINDIR)/__getfile.so $(GETFILE_SO_OBJS)

$(OBJDIR)/_getfile_wrap.o:  _getfile_wrap.c
	$(CC) -g -c -o $(OBJDIR)/_getfile_wrap.o -fPIC $(PYTHON_INC) _getfile_wrap.c

PYDBASE_OBJS = $(OBJDIR)/pydbase_wrap.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o \
$(OBJDIR)/datetime.o $(OBJDIR)/cdate.o $(OBJDIR)/rpad.o $(OBJDIR)/fileck.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/jday.o $(OBJDIR)/jdate.o $(OBJDIR)/weekday.o \
$(OBJDIR)/ch.o

$(BINDIR)/_pydbase.so:	$(PYDBASE_OBJS)
	$(CC) -shared $(LDFLAGS) -o $(BINDIR)/_pydbase.so $(PYDBASE_OBJS)

$(OBJDIR)/pydbase_wrap.o:  pydbase_wrap.c
	$(CC) -g -c -o $(OBJDIR)/pydbase_wrap.o -fPIC $(PYTHON_INC) pydbase_wrap.c

PATCMP_OBJS = $(OBJDIR)/patcmp_wrap.o $(OBJDIR)/patcmp.o $(OBJDIR)/strupr.o \
$(OBJDIR)/ch.o

$(BINDIR)/_patcmp.so:	$(PATCMP_OBJS)
	$(CC) -shared $(LDFLAGS) -o $(BINDIR)/_patcmp.so $(PATCMP_OBJS)

$(OBJDIR)/patcmp_wrap.o:  patcmp_wrap.c
	$(CC) -g -c -o $(OBJDIR)/patcmp_wrap.o -fPIC $(PYTHON_INC) patcmp_wrap.c

$(BINDIR)/_cdate.py:  _cdate.py
	cp _cdate.py $(BINDIR)

_cdate.py:  _cdate.i
	swig -python _cdate.i

_cdate_wrap.c:  _cdate.i
	swig -python _cdate.i

_CDATE_OBJS = $(OBJDIR)/_cdate_wrap.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o \
$(OBJDIR)/datetime.o $(OBJDIR)/cdate.o $(OBJDIR)/rpad.o $(OBJDIR)/fileck.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/jday.o $(OBJDIR)/jdate.o $(OBJDIR)/weekday.o \
$(OBJDIR)/ch.o

$(BINDIR)/__cdate.so:	$(_CDATE_OBJS)
	$(CC) -shared $(LDFLAGS) -o $(BINDIR)/__cdate.so $(_CDATE_OBJS)

$(OBJDIR)/_cdate_wrap.o:  _cdate_wrap.c
	$(CC) -g -c -o $(OBJDIR)/_cdate_wrap.o -fPIC $(PYTHON_INC) _cdate_wrap.c

$(BINDIR)/_remote_pause.py:  _remote_pause.py
	cp _remote_pause.py $(BINDIR)

_remote_pause.py:  _remote_pause.i
	swig -python _remote_pause.i

_remote_pause_wrap.c:  _remote_pause.i
	swig -python _remote_pause.i

_REMOTE_PAUSE_OBJS = $(OBJDIR)/_remote_pause_wrap.o $(OBJDIR)/_remote_pause.o 

$(BINDIR)/__remote_pause.so:	$(_REMOTE_PAUSE_OBJS)
	$(CC) -shared $(LDFLAGS) -o $(BINDIR)/__remote_pause.so $(_REMOTE_PAUSE_OBJS)

$(OBJDIR)/_remote_pause_wrap.o:  _remote_pause_wrap.c
	$(CC) -g -c -o $(OBJDIR)/_remote_pause_wrap.o -fPIC $(PYTHON_INC) _remote_pause_wrap.c

_pylist.py:  _pylist.i
	swig -python _pylist.i

_pylist_wrap.c:  _pylist.i
	swig -python _pylist.i

$(OBJDIR)/_pylist_wrap.o:  _pylist_wrap.c
	$(CC) -g -c -o $(OBJDIR)/_pylist_wrap.o -fPIC $(PYTHON_INC) _pylist_wrap.c

_PYLIST_OBJS = $(OBJDIR)/_pylist_wrap.o $(OBJDIR)/listpy.o $(OBJDIR)/listutl.o \
$(OBJDIR)/listtxt.o $(OBJDIR)/lscrn.o $(OBJDIR)/hscrn.o $(OBJDIR)/ch.o \
$(OBJDIR)/rpad.o $(OBJDIR)/ztrim.o $(OBJDIR)/getachar.o $(OBJDIR)/settitle.o \
$(OBJDIR)/patcmp.o $(OBJDIR)/strupr.o \
$(OBJDIR)/gethostname.o

$(BINDIR)/__pylist.so:	$(_PYLIST_OBJS)
	$(CC) -shared $(LDFLAGS) -o $(BINDIR)/__pylist.so $(_PYLIST_OBJS)

$(BINDIR)/_pylist.py:  _pylist.py
	cp _pylist.py $(BINDIR)

$(BINDIR)/_cvttime.py:  _cvttime.py
	cp _cvttime.py $(BINDIR)

_cvttime.py:  _cvttime.i
	swig -python _cvttime.i

_cvttime_wrap.c:  _cvttime.i
	swig -python _cvttime.i

_CVTTIME_OBJS = $(OBJDIR)/_cvttime_wrap.o $(OBJDIR)/ctime.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/ch.o

$(BINDIR)/__cvttime.so:	$(_CVTTIME_OBJS)
	$(CC) -shared $(LDFLAGS) -o $(BINDIR)/__cvttime.so $(_CVTTIME_OBJS)

$(OBJDIR)/_cvttime_wrap.o:  _cvttime_wrap.c
	$(CC) -g -c -o $(OBJDIR)/_cvttime_wrap.o -fPIC $(PYTHON_INC) _cvttime_wrap.c

SAYA_OBJS = $(OBJDIR)/saya.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/rpad.o $(OBJDIR)/getachar.o $(OBJDIR)/ztrim.o

$(BINDIR)/saya:  $(SAYA_OBJS)
	$(CC) $(LDFLAGS) -o $(BINDIR)/saya $(SAYA_OBJS)

$(BINDIR)/_pyscrn.py:  _pyscrn.py
	cp _pyscrn.py $(BINDIR)

_pyscrn.py:  _pyscrn.i
	swig -python _pyscrn.i

_pyscrn_wrap.c:  _pyscrn.i
	swig -python _pyscrn.i

$(OBJDIR)/pyhscrn.o:  hscrn.c hscrn.h
	$(CC) -g -c -o $(OBJDIR)/pyhscrn.o -fPIC $(PYTHON_INC) -DPYTHON hscrn.c

_PYSCRN_OBJS = $(OBJDIR)/_pyscrn_wrap.o $(OBJDIR)/pyhscrn.o $(OBJDIR)/gettext.o \
$(OBJDIR)/getachar.o $(OBJDIR)/lscrn.o $(OBJDIR)/box.o $(OBJDIR)/rpad.o \
$(OBJDIR)/trim.o $(OBJDIR)/ztrim.o $(OBJDIR)/settitle.o \
$(OBJDIR)/gethostname.o $(OBJDIR)/getone.o

$(BINDIR)/__pyscrn.so:	$(_PYSCRN_OBJS)
	$(CC) -shared $(LDFLAGS) -o $(BINDIR)/__pyscrn.so $(_PYSCRN_OBJS)

$(OBJDIR)/_pyscrn_wrap.o:  _pyscrn_wrap.c
	$(CC) -g -c -o $(OBJDIR)/_pyscrn_wrap.o -fPIC $(PYTHON_INC) _pyscrn_wrap.c

$(BINDIR)/pscrn.pl:  pscrn.pl
	cp pscrn.pl $(BINDIR)

pscrn.py:  pscrn.i
	swig -perl5 pscrn.i

pscrn_wrap.c:  pscrn.i
	swig -perl5 pscrn.i

$(OBJDIR)/plhscrn.o:  hscrn.c hscrn.h
	$(CC) -g -c -o $(OBJDIR)/plhscrn.o -fPIC -DPYTHON hscrn.c

PSCRN_OBJS = $(OBJDIR)/pscrn_wrap.o $(OBJDIR)/plhscrn.o $(OBJDIR)/gettext.o \
$(OBJDIR)/getachar.o $(OBJDIR)/lscrn.o $(OBJDIR)/box.o $(OBJDIR)/rpad.o \
$(OBJDIR)/trim.o $(OBJDIR)/ztrim.o $(OBJDIR)/settitle.o \
$(OBJDIR)/gethostname.o

$(BINDIR)/pscrn.so:	$(PSCRN_OBJS)
	$(LD) -shared $(LDFLAGS) -E  -fstack-protector -L/usr/local/lib  -L/usr/lib/perl/5.10/CORE -lperl -ldl -lm -lpthread -lc -lcrypt -o $(BINDIR)/pscrn.so $(PSCRN_OBJS)

$(OBJDIR)/pscrn_wrap.o:  pscrn_wrap.c
	$(CC) -g -c -o $(OBJDIR)/pscrn_wrap.o -fPIC `perl -MExtUtils::Embed \
	-e ccopts` -I. pscrn_wrap.c

pdbase.pm:	pdbase.i
	swig -perl5 pdbase.i

pdbase_wrap.c:	pdbase.i
	swig -perl5 pdbase.i

$(OBJDIR)/pdbase_wrap.o:  pdbase_wrap.c dbase3.h dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o
	$(CC) -g -c -fPIC pdbase_wrap.c dbase3.c `perl -MExtUtils::Embed -e ccopts` -I.

$(BINDIR)/pdbase.so:	$(OBJDIR)/pdbase_wrap.o
	$(LD) -shared $(LDFLAGS) $(LDFLAGS) -o $(BINDIR)/$(BINDIR)/pdbase.so

$(BINDIR)/sd.sh:  sd.sh
	cp sd.sh $(BINDIR)

$(BINDIR)/setpath:  setpath
	cp setpath $(BINDIR)

$(BINDIR)/getver:  getver
	cp getver $(BINDIR)

$(BINDIR)/arm:	$(OBJDIR)/arm.o
	$(CC) $(LDFLAGS) -g -o $(BINDIR)/arm $(OBJDIR)/arm.o

#
#  executables
#

ISNUMERIC_OBJS = $(OBJDIR)/isnumeric.o 
$(BINDIR)/isnumeric:  $(ISNUMERIC_OBJS)
	$(CC) $(LDFLAGS) -g -o $(BINDIR)/isnumeric $(ISNUMERIC_OBJS)

DSCAN_OBJS = $(OBJDIR)/dscan.o $(OBJDIR)/getachar.o $(OBJDIR)/datetime.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o \
$(OBJDIR)/saybytes.o
$(BINDIR)/dscan:  $(DSCAN_OBJS)
	$(CC) $(LDFLAGS) -g -o $(BINDIR)/dscan $(DSCAN_OBJS)

EXTNAME_OBJS = $(OBJDIR)/extname.o
$(BINDIR)/extname:  $(EXTNAME_OBJS)
	$(CC) $(LDFLAGS) -g -o $(BINDIR)/extname $(EXTNAME_OBJS)

TESTER_OBJS = $(OBJDIR)/tester.o
$(BINDIR)/tester:  $(TESTER_OBJS)
	$(CC) $(LDFLAGS) -g -o $(BINDIR)/tester $(TESTER_OBJS)

PRINTWRAP_OBJS = $(OBJDIR)/printwrapc.o $(OBJDIR)/printwrapf.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/getword.o
$(BINDIR)/printwrap:  $(PRINTWRAP_OBJS)
	$(CC) $(LDFLAGS) -g -o $(BINDIR)/printwrap $(PRINTWRAP_OBJS)

KABOOM_OBJS = $(OBJDIR)/kaboom.o
$(BINDIR)/kaboom:	$(KABOOM_OBJS)
	$(CC) $(LDFLAGS) -g -o $(BINDIR)/kaboom $(KABOOM_OBJS)

SCREENSIZE_OBJS = $(OBJDIR)/screensize.o $(OBJDIR)/lscrn.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/rpad.o $(OBJDIR)/getachar.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o 

$(BINDIR)/screensize:	$(SCREENSIZE_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/screensize $(SCREENSIZE_OBJS)

PAUSE_OBJS = $(OBJDIR)/pause.o $(OBJDIR)/getachar.o $(OBJDIR)/grammar.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/ch.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o
$(BINDIR)/pause:	$(PAUSE_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/pause $(PAUSE_OBJS)

GSM_OBJS = $(OBJDIR)/gsmtest.o $(OBJDIR)/gsm.o
$(BINDIR)/gsmtest:  $(GSM_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/gsmtest $(GSM_OBJS)

SDB_OBJS = $(OBJDIR)/sdb.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o $(OBJDIR)/listutl.o \
$(OBJDIR)/listtxt.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/makename.o \
$(OBJDIR)/patcmp.o $(OBJDIR)/fileck.o $(OBJDIR)/ztrim.o $(OBJDIR)/cdate.o \
$(OBJDIR)/rpad.o $(OBJDIR)/datetime.o $(OBJDIR)/getachar.o $(OBJDIR)/ch.o \
$(OBJDIR)/dirpath.o \
$(OBJDIR)/strupr.o $(OBJDIR)/jday.o $(OBJDIR)/weekday.o $(OBJDIR)/jdate.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o
$(BINDIR)/sdb:  $(SDB_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/sdb $(SDB_OBJS)

PICKDB_OBJS = $(OBJDIR)/pickdb.o $(OBJDIR)/dbase3.o $(OBJDIR)/getone.o $(OBJDIR)/update.o $(OBJDIR)/fileck.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/cdate.o $(OBJDIR)/rpad.o $(OBJDIR)/datetime.o \
$(OBJDIR)/jday.o $(OBJDIR)/weekday.o $(OBJDIR)/ch.o $(OBJDIR)/jdate.o \
$(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/listutl.o \
$(OBJDIR)/listtxt.o $(OBJDIR)/patcmp.o $(OBJDIR)/strupr.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o

$(BINDIR)/pickdb:	${PICKDB_OBJS}
	${CC} -g $(LDFLAGS) -o $(BINDIR)/pickdb ${PICKDB_OBJS}

XB_OBJS = $(OBJDIR)/xb.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o \
$(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o $(OBJDIR)/fileck.o \
$(OBJDIR)/listutl.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/cdate.o $(OBJDIR)/rpad.o $(OBJDIR)/datetime.o \
$(OBJDIR)/jday.o $(OBJDIR)/weekday.o $(OBJDIR)/ch.o $(OBJDIR)/jdate.o \
$(OBJDIR)/patcmp.o $(OBJDIR)/strupr.o

$(BINDIR)/xb:	${XB_OBJS} dbase3.h
	${CC} -g $(LDFLAGS) -o $(BINDIR)/xb ${XB_OBJS}

CVTTIME_OBJS = $(OBJDIR)/cvttime.o $(OBJDIR)/ctime.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/ch.o

$(BINDIR)/cvttime:	${CVTTIME_OBJS}
	${CC} -g $(LDFLAGS) -o $(BINDIR)/cvttime ${CVTTIME_OBJS}

PICKONE_OBJS = $(OBJDIR)/pickone.o $(OBJDIR)/stripnl.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/ch.o $(OBJDIR)/rpad.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/strupr.o 


$(BINDIR)/pickone:	${PICKONE_OBJS}
	${CC} -g $(LDFLAGS) -o $(BINDIR)/pickone ${PICKONE_OBJS}

NUTRIWATCH_OBJS = $(OBJDIR)/nutriwatch.o $(OBJDIR)/hscrn.o $(OBJDIR)/getone.o \
$(OBJDIR)/dbase3.o $(OBJDIR)/update.o $(OBJDIR)/rpt.o $(OBJDIR)/cdate.o \
$(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o $(OBJDIR)/ztrim.o $(OBJDIR)/rpad.o \
$(OBJDIR)/patcmp.o $(OBJDIR)/lscrn.o $(OBJDIR)/fileck.o $(OBJDIR)/datetime.o \
$(OBJDIR)/trim.o $(OBJDIR)/rpttxt.o $(OBJDIR)/jday.o $(OBJDIR)/weekday.o \
$(OBJDIR)/ch.o $(OBJDIR)/getachar.o $(OBJDIR)/newpage.o $(OBJDIR)/jdate.o \
$(OBJDIR)/strupr.o $(OBJDIR)/get_parent.o \
$(OBJDIR)/printer.o $(OBJDIR)/getfilel.o $(OBJDIR)/getfileh.o \
$(OBJDIR)/dirpath.o $(OBJDIR)/box.o $(OBJDIR)/settitle.o \
$(OBJDIR)/gethostname.o

$(BINDIR)/nutriwatch.exe:	$(NUTRIWATCH_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/nutriwatch.exe $(NUTRIWATCH_OBJS)

PMEM_OBJS = $(OBJDIR)/pmem.o

$(BINDIR)/pmem:	${PMEM_OBJS}
	${CC} -g $(LDFLAGS) -o $(BINDIR)/pmem ${PMEM_OBJS}

SYSDPY_OBJS = $(OBJDIR)/sysdpy.o $(OBJDIR)/hscrn.o $(OBJDIR)/stripnl.o \
$(OBJDIR)/listutl.o $(OBJDIR)/getachar.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/getone.o $(OBJDIR)/rpad.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/ch.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o \
$(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o $(OBJDIR)/strupr.o \
$(OBJDIR)/dirpath.o $(OBJDIR)/box.o $(OBJDIR)/trim.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/fileck.o $(OBJDIR)/get_parent.o


$(BINDIR)/sysdpy:	$(SYSDPY_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/sysdpy $(SYSDPY_OBJS)

RUNMENU_OBJS = $(OBJDIR)/runmenu.o $(OBJDIR)/dbase3.o $(OBJDIR)/getone.o $(OBJDIR)/update.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/trim.o $(OBJDIR)/stripnl.o $(OBJDIR)/listutl.o \
$(OBJDIR)/getachar.o $(OBJDIR)/listtxt.o $(OBJDIR)/rpad.o $(OBJDIR)/fileck.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/cdate.o $(OBJDIR)/datetime.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/ch.o $(OBJDIR)/jday.o $(OBJDIR)/jdate.o $(OBJDIR)/weekday.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o \
$(OBJDIR)/patcmp.o $(OBJDIR)/strupr.o 


$(BINDIR)/runmenu:	$(RUNMENU_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/runmenu $(RUNMENU_OBJS)

THREADS_OBJS = $(OBJDIR)/threads.o
$(BINDIR)/threads:	$(THREADS_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/threads $(THREADS_OBJS) -lpthread

TIMECALC_OBJS = $(OBJDIR)/timecalc.o $(OBJDIR)/ctime.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/ch.o
$(BINDIR)/timecalc:	${TIMECALC_OBJS}
	${CC} -g $(LDFLAGS) -o $(BINDIR)/timecalc ${TIMECALC_OBJS}

CRASH_OBJS = $(OBJDIR)/crash.o
$(BINDIR)/crash:	${CRASH_OBJS}
	${CC} -g $(LDFLAGS) -o $(BINDIR)/crash ${CRASH_OBJS}

WATCHDOG_OBJS = $(OBJDIR)/watchdog.o
$(BINDIR)/watchdog:	$(WATCHDOG_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/watchdog $(WATCHDOG_OBJS)

DIFFTIME_OBJS = $(OBJDIR)/difftime.o
$(BINDIR)/difftime:	$(DIFFTIME_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/difftime $(DIFFTIME_OBJS)

TITLE_OBJS = $(OBJDIR)/title.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o 
$(BINDIR)/title:	$(TITLE_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/title $(TITLE_OBJS)

TESTACCUM_OBJS = $(OBJDIR)/testaccum.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o $(OBJDIR)/crdbase.o \
$(OBJDIR)/strupr.o $(OBJDIR)/fileck.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/cdate.o $(OBJDIR)/rpad.o $(OBJDIR)/datetime.o $(OBJDIR)/jday.o \
$(OBJDIR)/weekday.o $(OBJDIR)/ch.o $(OBJDIR)/jdate.o $(OBJDIR)/stripnl.o

$(BINDIR)/testaccum:	$(TESTACCUM_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/testaccum $(TESTACCUM_OBJS)

TESTSUM_OBJS = $(OBJDIR)/testsum.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o $(OBJDIR)/crdbase.o \
$(OBJDIR)/strupr.o $(OBJDIR)/fileck.o $(OBJDIR)/ztrim.o $(OBJDIR)/cdate.o \
$(OBJDIR)/rpad.o \
$(OBJDIR)/datetime.o $(OBJDIR)/jday.o $(OBJDIR)/weekday.o $(OBJDIR)/ch.o \
$(OBJDIR)/jdate.o $(OBJDIR)/stripnl.o

$(BINDIR)/testsum:	$(TESTSUM_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/testsum $(TESTSUM_OBJS)

BUILD_OBJS = $(OBJDIR)/build.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/getachar.o $(OBJDIR)/rpad.o $(OBJDIR)/fileck.o

$(BINDIR)/build:	$(BUILD_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/build $(BUILD_OBJS) $(LTERMCAP)

CLEARCAL_OBJS = $(OBJDIR)/clearcal.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/rpad.o $(OBJDIR)/ztrim.o $(OBJDIR)/getachar.o

$(BINDIR)/clearcal:  $(CLEARCAL_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/clearcal $(CLEARCAL_OBJS) -L/usr/dt/lib $(LTERMCAP) -lcsa

DO_OBJS = $(OBJDIR)/do.o $(OBJDIR)/getword.o $(OBJDIR)/getfilel.o \
$(OBJDIR)/getfileh.o $(OBJDIR)/trim.o $(OBJDIR)/lscrn.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/ch.o \
$(OBJDIR)/stripnl.o $(OBJDIR)/rpad.o $(OBJDIR)/ztrim.o $(OBJDIR)/getachar.o \
$(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o $(OBJDIR)/patcmp.o $(OBJDIR)/getone.o \
$(OBJDIR)/fileck.o $(OBJDIR)/strupr.o $(OBJDIR)/dirpath.o $(OBJDIR)/box.o

$(BINDIR)/do:	$(DO_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/do $(DO_OBJS) $(LTERMCAP)

CMDS_OBJS = $(OBJDIR)/cmds.o $(OBJDIR)/ztrim.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o \
$(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/getachar.o $(OBJDIR)/datetime.o $(OBJDIR)/cdate.o $(OBJDIR)/ch.o \
$(OBJDIR)/rpad.o $(OBJDIR)/fileck.o $(OBJDIR)/jday.o $(OBJDIR)/weekday.o \
$(OBJDIR)/jdate.o \
$(OBJDIR)/getone.o

$(BINDIR)/cmds:	$(CMDS_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/cmds $(CMDS_OBJS) $(LTERMCAP)

HISTOGRAM_OBJS = $(OBJDIR)/histogram.o $(OBJDIR)/listutl.o

$(BINDIR)/histogram: $(HISTOGRAM_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/histogram $(HISTOGRAM_OBJS)

CROP_OBJS = $(OBJDIR)/crop.o

$(BINDIR)/crop:	$(CROP_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/crop $(CROP_OBJS)

FPRINT_OBJS = $(OBJDIR)/fprint.o $(OBJDIR)/printer.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getfileh.o \
$(OBJDIR)/getfilel.o $(OBJDIR)/datetime.o $(OBJDIR)/newpage.o \
$(OBJDIR)/ztrim.o \
$(OBJDIR)/box.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/dirpath.o $(OBJDIR)/fileck.o $(OBJDIR)/strupr.o $(OBJDIR)/trim.o \
$(OBJDIR)/getone.o $(OBJDIR)/ch.o $(OBJDIR)/rpad.o $(OBJDIR)/settitle.o \
$(OBJDIR)/gethostname.o

$(BINDIR)/fprint:	 $(FPRINT_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/fprint $(FPRINT_OBJS) $(LTERMCAP)


TRUNCLINE_OBJS = $(OBJDIR)/truncline.o $(OBJDIR)/ztrim.o $(OBJDIR)/stripnl.o
$(BINDIR)/truncline:	$(TRUNCLINE_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/truncline $(TRUNCLINE_OBJS)

STRLEN_OBJS = $(OBJDIR)/strlen.o
$(BINDIR)/strlen:	 $(STRLEN_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/strlen $(STRLEN_OBJS)

TRUNC_OBJS = $(OBJDIR)/trunc.o
$(BINDIR)/trunc:	$(TRUNC_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/trunc $(TRUNC_OBJS)

CATMAIL_OBJS = $(OBJDIR)/catmail.o $(OBJDIR)/stripnl.o

$(BINDIR)/catmail:  $(CATMAIL_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/catmail $(CATMAIL_OBJS)

CTIMETST_OBJS = $(OBJDIR)/ctimetst.o $(OBJDIR)/myctime.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/ch.o $(OBJDIR)/stripnl.o

$(BINDIR)/ctimetst:  $(CTIMETST_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/ctimetst $(CTIMETST_OBJS)

DAILYCK_OBJS = $(OBJDIR)/dailyck.o $(OBJDIR)/getachar.o $(OBJDIR)/datetime.o \
$(OBJDIR)/makename.o $(OBJDIR)/dirpath.o $(OBJDIR)/lscrn.o

$(BINDIR)/dailyck:  $(DAILYCK_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/dailyck $(DAILYCK_OBJS) $(LTERMCAP)

FMEO_OBJS = $(OBJDIR)/fmeo.o $(OBJDIR)/fmeoall.o $(OBJDIR)/fmeotxt.o \
$(OBJDIR)/cryptxor.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/msgtxt.o \
$(OBJDIR)/listutl.o $(OBJDIR)/ch.o $(OBJDIR)/rpad.o $(OBJDIR)/getone.o \
$(OBJDIR)/listtxt.o $(OBJDIR)/datetime.o $(OBJDIR)/getachar.o \
$(OBJDIR)/ztrim.o \
$(OBJDIR)/setsec.o $(OBJDIR)/trim.o $(OBJDIR)/dbase3.o $(OBJDIR)/update.o $(OBJDIR)/crdbase.o \
$(OBJDIR)/cdate.o $(OBJDIR)/fileck.o $(OBJDIR)/strupr.o $(OBJDIR)/getfileh.o \
$(OBJDIR)/getfilel.o $(OBJDIR)/jday.o $(OBJDIR)/weekday.o $(OBJDIR)/jdate.o \
$(OBJDIR)/patcmp.o $(OBJDIR)/dirpath.o $(OBJDIR)/box.o $(OBJDIR)/settitle.o \
$(OBJDIR)/gethostname.o $(OBJDIR)/clipboard.o

$(BINDIR)/fmeo.exe:  $(FMEO_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/fmeo.exe $(FMEO_OBJS) $(LTERMCAP)

MSGTEST_OBJS = $(OBJDIR)/msgtest.o $(OBJDIR)/msgtxt.o $(OBJDIR)/getachar.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/hscrn.o $(OBJDIR)/rpad.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o 

$(BINDIR)/msgtest:  $(MSGTEST_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/msgtest $(MSGTEST_OBJS) $(LTERMCAP)

BINDIFF_OBJS = $(OBJDIR)/bindiff.o $(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o \
$(OBJDIR)/dirpath.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/rpad.o $(OBJDIR)/box.o $(OBJDIR)/trim.o $(OBJDIR)/getachar.o \
$(OBJDIR)/getone.o $(OBJDIR)/patcmp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/fileck.o $(OBJDIR)/regexp.o $(OBJDIR)/strupr.o $(OBJDIR)/ch.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o

$(BINDIR)/bindiff:  $(BINDIFF_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/bindiff $(BINDIFF_OBJS) $(LTERMCAP)

BINFIND_OBJS = $(OBJDIR)/binfind.o
$(BINDIR)/binfind:  $(BINFIND_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/binfind $(BINFIND_OBJS)

QACOMP_OBJS = $(OBJDIR)/qacomp.o $(OBJDIR)/patcmp.o $(OBJDIR)/regexp.o \
$(OBJDIR)/strupr.o $(OBJDIR)/ch.o
$(BINDIR)/qacomp:	 $(QACOMP_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/qacomp $(QACOMP_OBJS)


N2E_OBJS = $(OBJDIR)/n2e.o $(OBJDIR)/ztrim.o $(OBJDIR)/stripnl.o \
$(OBJDIR)/makename.o $(OBJDIR)/dirpath.o
$(BINDIR)/n2e:	$(N2E_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/n2e $(N2E_OBJS)

T2E_OBJS = $(OBJDIR)/tele2eudora.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o $(OBJDIR)/datetime.o \
$(OBJDIR)/cdate.o $(OBJDIR)/rpad.o $(OBJDIR)/fileck.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/weekday.o $(OBJDIR)/jday.o $(OBJDIR)/jdate.o $(OBJDIR)/ch.o
$(BINDIR)/tele2eudora:	$(T2E_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/tele2eudora $(T2E_OBJS)

TS_OBJS = ts.o datetime.o
$(BINDIR)/ts:	$(TS_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/ts $(TS_OBJS)

XBTSTTXT_OBJS = $(OBJDIR)/xbtsttxt.o $(OBJDIR)/xbase.o $(OBJDIR)/cmsgtxt.o \
$(OBJDIR)/except.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/getachar.o $(OBJDIR)/gettext.o $(OBJDIR)/fileck.o $(OBJDIR)/cdate.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/rpad.o $(OBJDIR)/datetime.o $(OBJDIR)/trim.o \
$(OBJDIR)/jday.o \
$(OBJDIR)/weekday.o $(OBJDIR)/ch.o $(OBJDIR)/jdate.o $(OBJDIR)/ztrim.o
$(BINDIR)/xbtsttxt:	$(XBTSTTXT_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/xbtsttxt $(XBTSTTXT_OBJS) $(LTERMCAP)


DBEXPORT_OBJS = $(OBJDIR)/dbexport.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o $(OBJDIR)/crdbase.o \
$(OBJDIR)/strupr.o $(OBJDIR)/fileck.o $(OBJDIR)/cdate.o	\
$(OBJDIR)/ztrim.o $(OBJDIR)/rpad.o $(OBJDIR)/datetime.o $(OBJDIR)/jday.o \
$(OBJDIR)/weekday.o $(OBJDIR)/ch.o $(OBJDIR)/jdate.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/getachar.o
$(BINDIR)/dbexport:  $(DBEXPORT_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/dbexport $(DBEXPORT_OBJS) $(LTERMCAP)

DBIMPORT_OBJS = $(OBJDIR)/dbimport.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o $(OBJDIR)/crdbase.o \
$(OBJDIR)/strupr.o $(OBJDIR)/fileck.o $(OBJDIR)/cdate.o	\
$(OBJDIR)/ztrim.o $(OBJDIR)/rpad.o $(OBJDIR)/datetime.o $(OBJDIR)/jday.o \
$(OBJDIR)/weekday.o $(OBJDIR)/ch.o $(OBJDIR)/jdate.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/getachar.o

$(BINDIR)/dbimport:  $(DBIMPORT_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/dbimport $(DBIMPORT_OBJS) $(LTERMCAP)



WOTIMER_OBJS = $(OBJDIR)/wotimer.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/getachar.o $(OBJDIR)/rpad.o $(OBJDIR)/ztrim.o

$(BINDIR)/wotimer:  $(WOTIMER_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/wotimer $(WOTIMER_OBJS) $(LTERMCAP)


FILER_OBJS = $(OBJDIR)/filer.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/cdate.o $(OBJDIR)/getachar.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o $(OBJDIR)/makename.o \
$(OBJDIR)/getone.o $(OBJDIR)/stripnl.o $(OBJDIR)/ztrim.o $(OBJDIR)/strupr.o \
$(OBJDIR)/fileck.o $(OBJDIR)/getword.o $(OBJDIR)/myctime.o $(OBJDIR)/rpad.o \
$(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o $(OBJDIR)/patcmp.o $(OBJDIR)/pattxt.o \
$(OBJDIR)/debuglog.o $(OBJDIR)/filertxt.o \
$(OBJDIR)/regexp.o $(OBJDIR)/mysystem.o $(OBJDIR)/datetime.o \
$(OBJDIR)/jday.o $(OBJDIR)/weekday.o $(OBJDIR)/ch.o \
$(OBJDIR)/jdate.o $(OBJDIR)/dirpath.o $(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o \
$(OBJDIR)/box.o $(OBJDIR)/trim.o

$(BINDIR)/filer:	$(FILER_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/filer $(FILER_OBJS) $(LTERMCAP)

RING_OBJS = $(OBJDIR)/ring.o

$(BINDIR)/ring:  $(RING_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/ring $(RING_OBJS)

CHECKA_OBJS = $(OBJDIR)/checka.o

$(BINDIR)/checka: $(CHECKA_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/checka $(CHECKA_OBJS)

SAVEA_OBJS = $(OBJDIR)/savea.o

$(BINDIR)/savea:	$(SAVEA_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/savea $(SAVEA_OBJS)

STOPWATCH_OBJS = $(OBJDIR)/stopwatch.o $(OBJDIR)/datetime.o \
$(OBJDIR)/getachar.o $(OBJDIR)/lscrn.o $(OBJDIR)/stripnl.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/hscrn.o $(OBJDIR)/rpad.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o

$(BINDIR)/stopwatch:	$(STOPWATCH_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/stopwatch $(STOPWATCH_OBJS) $(LTERMCAP)

MAILEDIT_OBJS = $(OBJDIR)/mailedit.o $(OBJDIR)/hscrn.o $(OBJDIR)/getachar.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/ztrim.o $(OBJDIR)/rpad.o $(OBJDIR)/getone.o \
$(OBJDIR)/stripnl.o

$(BINDIR)/mailedit:	$(MAILEDIT_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/mailedit $(MAILEDIT_OBJS) $(LTERMCAP)

SENDELM_OBJS = $(OBJDIR)/sendelm.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o $(OBJDIR)/strupr.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/ch.o $(OBJDIR)/fileck.o cdate.o \
$(OBJDIR)/rpad.o $(OBJDIR)/datetime.o $(OBJDIR)/jday.o $(OBJDIR)/weekday.o \
$(OBJDIR)/jdate.o

$(BINDIR)/sendelm:	$(SENDELM_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/sendelm $(SENDELM_OBJS)

MAILSORT_OBJS = $(OBJDIR)/mailsort.o $(OBJDIR)/getword.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/strupr.o $(OBJDIR)/myctime.o \
$(OBJDIR)/ch.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/setsec.o $(OBJDIR)/stripnl.o

$(BINDIR)/mailsort:  $(MAILSORT_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/mailsort $(MAILSORT_OBJS)

AMOUNT_OBJS = $(OBJDIR)/amount.o $(OBJDIR)/getword.o $(OBJDIR)/getini.o \
$(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o $(OBJDIR)/fileck.o $(OBJDIR)/stripnl.o \
$(OBJDIR)/ch.o

$(BINDIR)/amount: $(AMOUNT_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/amount $(AMOUNT_OBJS)

CALLCNT_OBJS = $(OBJDIR)/callcnt.o $(OBJDIR)/listutl.o

$(BINDIR)/callcnt:  $(CALLCNT_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/callcnt $(CALLCNT_OBJS)

SHARE_OBJS = $(OBJDIR)/share.o

$(BINDIR)/share:	$(SHARE_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/share $(SHARE_OBJS)

PHONE_OBJS = $(OBJDIR)/phone.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o $(OBJDIR)/cdate.o \
$(OBJDIR)/weekday.o $(OBJDIR)/jday.o $(OBJDIR)/datetime.o $(OBJDIR)/jdate.o \
$(OBJDIR)/ch.o $(OBJDIR)/ztrim.o $(OBJDIR)/rpad.o $(OBJDIR)/strupr.o \
$(OBJDIR)/fileck.o

$(BINDIR)/phone:	$(PHONE_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/phone $(PHONE_OBJS)

BLDTELE_OBJS = $(OBJDIR)/bldtele.o $(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o \
$(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o $(OBJDIR)/crdbase.o $(OBJDIR)/strupr.o $(OBJDIR)/dirpath.o \
$(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/rpad.o $(OBJDIR)/box.o \
$(OBJDIR)/trim.o $(OBJDIR)/getachar.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/fileck.o $(OBJDIR)/cdate.o $(OBJDIR)/ztrim.o $(OBJDIR)/datetime.o \
$(OBJDIR)/jday.o $(OBJDIR)/weekday.o $(OBJDIR)/ch.o $(OBJDIR)/jdate.o \
$(OBJDIR)/stripnl.o \
$(OBJDIR)/getone.o $(OBJDIR)/strupr.o

$(BINDIR)/bldtele:	$(BLDTELE_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/bldtele $(BLDTELE_OBJS) $(LTERMCAP)

ARGS_OBJS = $(OBJDIR)/args.o

$(BINDIR)/args:  $(ARGS_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/args $(ARGS_OBJS)

GETFILE_OBJS = $(OBJDIR)/getfile.o $(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o \
$(OBJDIR)/dirpath.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/rpad.o $(OBJDIR)/box.o $(OBJDIR)/trim.o $(OBJDIR)/getachar.o \
$(OBJDIR)/patcmp.o $(OBJDIR)/settitle.o  $(OBJDIR)/gethostname.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/fileck.o $(OBJDIR)/ch.o \
$(OBJDIR)/getone.o $(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o

$(BINDIR)/getfile:  $(GETFILE_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/getfile $(GETFILE_OBJS) $(LTERMCAP)

PHONECK_OBJS = $(OBJDIR)/phoneck.o $(OBJDIR)/datetime.o

$(BINDIR)/phoneck:  $(PHONECK_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/phoneck $(PHONECK_OBJS)

TDEPWD_OBJS = $(OBJDIR)/tdepwd.o

$(BINDIR)/tdepwd: $(TDEPWD_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/tdepwd $(TDEPWD_OBJS)

XSHOWFONTS_OBJS = $(OBJDIR)/xshowfonts.o

$(BINDIR)/xshowfonts:	 $(XSHOWFONTS_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/xshowfonts $(XSHOWFONTS_OBJS) $(XLIBS) -lXt -lX11 -lXmu -lXaw

XCHECK_OBJS = $(OBJDIR)/xcheck.o

$(BINDIR)/xcheck:	$(XCHECK_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/xcheck $(XCHECK_OBJS) $(XLIBS) -lX11

XMENU_OBJS = $(OBJDIR)/xmenu.o $(OBJDIR)/stripnl.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/strupr.o $(OBJDIR)/listutl.o $(OBJDIR)/ch.o

$(BINDIR)/xmenu:	$(XMENU_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/xmenu $(XMENU_OBJS) $(XLIBS) -lXm -lXt -lX11

RICODE_OBJS = $(OBJDIR)/ricode.o

$(BINDIR)/ricode: $(RICODE_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/ricode $(RICODE_OBJS)

COP_OBJS = $(OBJDIR)/cop.o

$(BINDIR)/cop:	$(COP_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/cop $(COP_OBJS)

PROJ2UP_OBJS = $(OBJDIR)/proj2up.o $(OBJDIR)/makename.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/datetime.o $(OBJDIR)/dirpath.o \
$(OBJDIR)/cdate.o $(OBJDIR)/rpad.o $(OBJDIR)/weekday.o $(OBJDIR)/jday.o \
$(OBJDIR)/jdate.o $(OBJDIR)/ch.o $(OBJDIR)/fileck.o $(OBJDIR)/mysystem.o \
$(OBJDIR)/getachar.o \
$(OBJDIR)/lscrn.o

$(BINDIR)/proj2up:	$(PROJ2UP_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/proj2up $(PROJ2UP_OBJS) $(LTERMCAP)

CHECKDB_OBJS = $(OBJDIR)/checkdb.o $(OBJDIR)/getachar.o $(OBJDIR)/crdbase.o \
$(OBJDIR)/dbase3.o $(OBJDIR)/update.o $(OBJDIR)/strupr.o $(OBJDIR)/getfileh.o \
$(OBJDIR)/getfilel.o $(OBJDIR)/stripnl.o $(OBJDIR)/getword.o \
$(OBJDIR)/datetime.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/cdate.o \
$(OBJDIR)/rpad.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o $(OBJDIR)/ch.o \
$(OBJDIR)/fileck.o $(OBJDIR)/trim.o $(OBJDIR)/ztrim.o $(OBJDIR)/weekday.o \
$(OBJDIR)/box.o \
$(OBJDIR)/jday.o $(OBJDIR)/dirpath.o $(OBJDIR)/jdate.o $(OBJDIR)/getone.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o

$(BINDIR)/checkdb:	$(CHECKDB_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/checkdb $(CHECKDB_OBJS) $(LTERMCAP)

AA_OBJS = $(OBJDIR)/aa.o $(OBJDIR)/ztrim.o $(OBJDIR)/stripnl.o $(OBJDIR)/ch.o \
$(OBJDIR)/fileck.o

$(BINDIR)/aa:	$(AA_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/aa $(AA_OBJS)

SD_OBJS = $(OBJDIR)/sd.o $(OBJDIR)/getachar.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/listutl.o $(OBJDIR)/rpad.o $(OBJDIR)/stripnl.o \
$(OBJDIR)/listtxt.o $(OBJDIR)/trim.o $(OBJDIR)/getone.o $(OBJDIR)/ch.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/dbase3.o $(OBJDIR)/update.o $(OBJDIR)/fileck.o $(OBJDIR)/cdate.o \
$(OBJDIR)/datetime.o $(OBJDIR)/jday.o $(OBJDIR)/weekday.o $(OBJDIR)/jdate.o \
$(OBJDIR)/crdbase.o $(OBJDIR)/strupr.o $(OBJDIR)/settitle.o \
$(OBJDIR)/gethostname.o $(OBJDIR)/patcmp.o 

$(BINDIR)/sd.exe:	$(SD_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/sd.exe $(SD_OBJS) $(LTERMCAP)

DUP_OBJS = $(OBJDIR)/dup.o

$(BINDIR)/dup:	$(DUP_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/dup $(DUP_OBJS)

TRIMCMD_OBJS = $(OBJDIR)/trimcmd.o $(OBJDIR)/ztrim.o $(OBJDIR)/stripnl.o

$(BINDIR)/trim:	$(TRIMCMD_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/trim $(TRIMCMD_OBJS);

TCLCK_OBJS = $(OBJDIR)/tclck.o $(OBJDIR)/weekday.o $(OBJDIR)/getachar.o \
$(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o $(OBJDIR)/datetime.o $(OBJDIR)/jday.o $(OBJDIR)/cdate.o \
$(OBJDIR)/rpad.o $(OBJDIR)/jdate.o $(OBJDIR)/ch.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/fileck.o $(OBJDIR)/lscrn.o $(OBJDIR)/hscrn.o $(OBJDIR)/getone.o \
$(OBJDIR)/myctime.o

$(BINDIR)/tclck:	$(TCLCK_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/tclck $(TCLCK_OBJS) $(LTERMCAP)

NOGUARD_OBJS = $(OBJDIR)/noguard.o

$(BINDIR)/noguard:  $(NOGUARD_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/noguard $(NOGUARD_OBJS)

INIRDR_OBJS = $(OBJDIR)/inirdr.o $(OBJDIR)/listtxt.o $(OBJDIR)/listutl.o \
$(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/rpad.o \
$(OBJDIR)/ch.o $(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o $(OBJDIR)/getone.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/box.o $(OBJDIR)/dirpath.o $(OBJDIR)/fileck.o \
$(OBJDIR)/trim.o \
$(OBJDIR)/stripnl.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o strupr.o

$(BINDIR)/inirdr: $(INIRDR_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/inirdr $(INIRDR_OBJS) $(LTERMCAP)

UUCOMBINE_OBJS = $(OBJDIR)/uucombine.o

$(BINDIR)/uucombine:	$(UUCOMBINE_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/uucombine $(UUCOMBINE_OBJS)

RAND_OBJS = $(OBJDIR)/rand.o $(OBJDIR)/datetime.o

$(BINDIR)/rand:	$(RAND_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/rand $(RAND_OBJS)

FIXDB_OBJS = $(OBJDIR)/fixdb.o $(OBJDIR)/getachar.o $(OBJDIR)/dbase3.o $(OBJDIR)/update.o \
$(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/getone.o $(OBJDIR)/datetime.o $(OBJDIR)/box.o \
$(OBJDIR)/dirpath.o $(OBJDIR)/cdate.o $(OBJDIR)/rpad.o $(OBJDIR)/fileck.o \
$(OBJDIR)/trim.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/weekday.o $(OBJDIR)/jday.o $(OBJDIR)/jdate.o \
$(OBJDIR)/ch.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/strupr.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o

$(BINDIR)/fixdb:	$(FIXDB_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/fixdb $(FIXDB_OBJS) $(LTERMCAP)

CLS_OBJS = $(OBJDIR)/cls.o $(OBJDIR)/lscrn.o

$(BINDIR)/cls:	$(CLS_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/cls $(CLS_OBJS) $(LTERMCAP)

LSIT_OBJS = $(OBJDIR)/lsit.o $(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o \
$(OBJDIR)/jdate.o $(OBJDIR)/getachar.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/dirpath.o $(OBJDIR)/rpad.o $(OBJDIR)/fileck.o \
$(OBJDIR)/trim.o $(OBJDIR)/box.o patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/ch.o $(OBJDIR)/getone.o $(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o

$(BINDIR)/lsit:	$(LSIT_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/lsit $(LSIT_OBJS) $(LTERMCAP)

WHEREIS_OBJS = $(OBJDIR)/whereis.o $(OBJDIR)/strupr.o $(OBJDIR)/dirpath.o \
$(OBJDIR)/stripnl.o $(OBJDIR)/ch.o

$(BINDIR)/whereis:  $(WHEREIS_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/whereis $(WHEREIS_OBJS)

IOZONE_OBJS = $(OBJDIR)/iozone.o

$(BINDIR)/iozone:	 $(IOZONE_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/iozone $(IOZONE_OBJS)

RESET_MODEM_OBJS = $(OBJDIR)/reset_modem.o

$(BINDIR)/reset_modem:	$(RESET_MODEM_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/reset_modem $(RESET_MODEM_OBJS)

NEWSPICK_OBJS = $(OBJDIR)/newspick.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/stripnl.o $(OBJDIR)/strupr.o \
$(OBJDIR)/ch.o

$(BINDIR)/newspick:  $(NEWSPICK_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/newspick $(NEWSPICK_OBJS)

SPLIT_OBJS = $(OBJDIR)/split.o $(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o \
$(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o \
$(OBJDIR)/dirpath.o $(OBJDIR)/rpad.o $(OBJDIR)/fileck.o $(OBJDIR)/trim.o \
$(OBJDIR)/box.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/ch.o \
$(OBJDIR)/getone.o $(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o

$(BINDIR)/split:	$(SPLIT_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/split $(SPLIT_OBJS) $(LTERMCAP)

REJOIN_OBJS = $(OBJDIR)/rejoin.o $(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o \
$(OBJDIR)/trim.o $(OBJDIR)/getachar.o $(OBJDIR)/hscrn.o $(OBJDIR)/box.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/fileck.o $(OBJDIR)/dirpath.o $(OBJDIR)/rpad.o \
$(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/ch.o $(OBJDIR)/getone.o \
$(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o

$(BINDIR)/rejoin: $(REJOIN_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/rejoin $(REJOIN_OBJS) $(LTERMCAP)

GETNAME_OBJS = $(OBJDIR)/getname.o $(OBJDIR)/makename.o $(OBJDIR)/getachar.o \
$(OBJDIR)/dirpath.o $(OBJDIR)/stripnl.o $(OBJDIR)/lscrn.o

$(BINDIR)/getname:	$(GETNAME_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/getname $(GETNAME_OBJS) $(LTERMCAP)

CHARS_OBJS = $(OBJDIR)/chars.o

$(BINDIR)/chars:	$(CHARS_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/chars $(CHARS_OBJS)

CVTDATE_OBJS = $(OBJDIR)/cvtdate.o $(OBJDIR)/cdate.o $(OBJDIR)/weekday.o \
$(OBJDIR)/datetime.o $(OBJDIR)/jday.o $(OBJDIR)/jdate.o $(OBJDIR)/ch.o \
$(OBJDIR)/getachar.o $(OBJDIR)/lscrn.o $(OBJDIR)/settitle.o \
$(OBJDIR)/gethostname.o

$(BINDIR)/cvtdate:	$(CVTDATE_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/cvtdate $(CVTDATE_OBJS) $(LTERMCAP)

L24_OBJS = $(OBJDIR)/l24.o

$(BINDIR)/l24:	$(L24_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/l24 $(L24_OBJS)

PGCNT_OBJS = $(OBJDIR)/pgcnt.o $(OBJDIR)/getachar.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o

$(BINDIR)/pgcnt:	$(PGCNT_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/pgcnt $(PGCNT_OBJS) $(LTERMCAP)

CONVERT_OBJS = $(OBJDIR)/convert.o $(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o \
$(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/dirpath.o \
$(OBJDIR)/getone.o $(OBJDIR)/rpad.o $(OBJDIR)/trim.o $(OBJDIR)/fileck.o \
$(OBJDIR)/getachar.o $(OBJDIR)/box.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/ch.o $(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o $(OBJDIR)/settitle.o \
$(OBJDIR)/gethostname.o

$(BINDIR)/convert:  $(CONVERT_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/convert $(CONVERT_OBJS) $(LTERMCAP)

UUFIX_OBJS = $(OBJDIR)/uufix.o

$(BINDIR)/uufix:	$(UUFIX_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/uufix $(UUFIX_OBJS)

MACBIN_OBJS = $(OBJDIR)/macbin.o $(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o \
$(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/jdate.o \
$(OBJDIR)/getachar.o $(OBJDIR)/box.o $(OBJDIR)/dirpath.o $(OBJDIR)/rpad.o \
$(OBJDIR)/fileck.o $(OBJDIR)/trim.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o \
$(OBJDIR)/listtxt.o $(OBJDIR)/ch.o $(OBJDIR)/getone.o $(OBJDIR)/strupr.o \
$(OBJDIR)/ztrim.o

$(BINDIR)/macbin:	 $(MACBIN_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/macbin $(MACBIN_OBJS) $(LTERMCAP)

BCONV_OBJS = $(OBJDIR)/bconv.o

$(BINDIR)/bconv:	$(BCONV_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/bconv $(BCONV_OBJS)

EX3_OBJS = $(OBJDIR)/ex3.o $(OBJDIR)/x10.o $(OBJDIR)/stringc.o

$(BINDIR)/ex3:  $(EX3_OBJS)
	CC1 -g $(LDFLAGS) -o $(BINDIR)/ex3 $(EX3_OBJS)

MYHEX_OBJS = $(OBJDIR)/myhex.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/getachar.o $(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o \
$(OBJDIR)/rpad.o $(OBJDIR)/ch.o $(OBJDIR)/box.o $(OBJDIR)/dirpath.o \
$(OBJDIR)/fileck.o $(OBJDIR)/trim.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/getone.o $(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/gethostname.o $(OBJDIR)/settitle.o

$(BINDIR)/myhex:	$(MYHEX_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/myhex $(MYHEX_OBJS) $(LTERMCAP)

PCOUNT_OBJS = $(OBJDIR)/pcount.o $(OBJDIR)/stpblk.o

$(BINDIR)/pcount:	 $(PCOUNT_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/pcount $(PCOUNT_OBJS)

CCOUNT_OBJS = $(OBJDIR)/ccount.o $(OBJDIR)/stpblk.o

$(BINDIR)/ccount:	 $(CCOUNT_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/ccount $(CCOUNT_OBJS)

CODECNT_OBJS = $(OBJDIR)/codecnt.o $(OBJDIR)/stpblk.o

$(BINDIR)/codecnt: $(CODECNT_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/codecnt $(CODECNT_OBJS)

CDATETST_OBJS = $(OBJDIR)/cdatetst.o $(OBJDIR)/cdate.o $(OBJDIR)/weekday.o \
$(OBJDIR)/jday.o $(OBJDIR)/jdate.o $(OBJDIR)/datetime.o $(OBJDIR)/ch.o \
$(OBJDIR)/holiday.o $(OBJDIR)/makename.o $(OBJDIR)/dirpath.o

$(BINDIR)/cdatetst:  $(CDATETST_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/cdatetst $(CDATETST_OBJS)

PROJECT2_OBJS = $(OBJDIR)/proj2.o $(OBJDIR)/dbase3.o	$(OBJDIR)/update.o $(OBJDIR)/datetime.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/cdate.o $(OBJDIR)/rpad.o \
$(OBJDIR)/weekday.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/jday.o \
$(OBJDIR)/jdate.o $(OBJDIR)/ch.o $(OBJDIR)/getachar.o $(OBJDIR)/myctime.o \
$(OBJDIR)/makename.o \
$(OBJDIR)/dirpath.o $(OBJDIR)/holiday.o $(OBJDIR)/rpt.o $(OBJDIR)/datecmp.o \
$(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o $(OBJDIR)/getone.o \
$(OBJDIR)/newpage.o \
$(OBJDIR)/trim.o $(OBJDIR)/box.o $(OBJDIR)/fileck.o $(OBJDIR)/crdbase.o \
$(OBJDIR)/strupr.o $(OBJDIR)/listtxt.o $(OBJDIR)/listutl.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/printer.o \
$(OBJDIR)/regexp.o $(OBJDIR)/proj2txt.o $(OBJDIR)/rpttxt.o $(OBJDIR)/msgtxt.o \
$(OBJDIR)/mysystem.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o 

$(BINDIR)/proj2:	$(PROJECT2_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/proj2 $(PROJECT2_OBJS) $(LTERMCAP)

BIKE_OBJS = $(OBJDIR)/bike.o $(OBJDIR)/rpt.o $(OBJDIR)/dbase3.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o $(OBJDIR)/getone.o $(OBJDIR)/update.o \
$(OBJDIR)/$(OBJDIR)/getachar.o $(OBJDIR)/datetime.o $(OBJDIR)/getfileh.o \
$(OBJDIR)/getfilel.o $(OBJDIR)/cdate.o $(OBJDIR)/newpage.o $(OBJDIR)/rpad.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/weekday.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/dirpath.o $(OBJDIR)/jday.o $(OBJDIR)/jdate.o $(OBJDIR)/ch.o \
$(OBJDIR)/fileck.o $(OBJDIR)/trim.o $(OBJDIR)/box.o $(OBJDIR)/myctime.o \
$(OBJDIR)/getone.o	\
$(OBJDIR)/patcmp.o $(OBJDIR)/rpttxt.o $(OBJDIR)/printer.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/strupr.o $(OBJDIR)/mysystem.o

$(BINDIR)/bike:  $(BIKE_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/bike $(BIKE_OBJS) $(LTERMCAP)

FILEE_OBJS = $(OBJDIR)/file_exists.o $(OBJDIR)/fileck.o

$(BINDIR)/file_exists: $(FILEE_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/file_exists $(FILEE_OBJS)

SCREEN_OBJS = $(OBJDIR)/screen.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/getachar.o $(OBJDIR)/box.o $(OBJDIR)/getone.o $(OBJDIR)/stripnl.o \
$(OBJDIR)/rpad.o $(OBJDIR)/getword.o $(OBJDIR)/trim.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o

$(BINDIR)/screen:	 $(SCREEN_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/screen $(SCREEN_OBJS) $(LTERMCAP)

PBASE_OBJS = $(OBJDIR)/pbase.o $(OBJDIR)/dbase3.o $(OBJDIR)/update.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/rpt.o $(OBJDIR)/getachar.o $(OBJDIR)/strupr.o \
$(OBJDIR)/datetime.o $(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o \
$(OBJDIR)/cdate.o $(OBJDIR)/getone.o $(OBJDIR)/rpad.o $(OBJDIR)/stripnl.o \
$(OBJDIR)/getword.o $(OBJDIR)/get_parent.o \
$(OBJDIR)/ch.o $(OBJDIR)/ztrim.o $(OBJDIR)/weekday.o $(OBJDIR)/box.o \
$(OBJDIR)/dirpath.o $(OBJDIR)/newpage.o $(OBJDIR)/jday.o $(OBJDIR)/fileck.o \
$(OBJDIR)/trim.o \
$(OBJDIR)/jdate.o $(OBJDIR)/crdbase.o $(OBJDIR)/patcmp.o $(OBJDIR)/rpttxt.o \
$(OBJDIR)/csv.o $(OBJDIR)/sortdb.o $(OBJDIR)/printer.o $(OBJDIR)/pbasecsv.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/holiday.o $(OBJDIR)/makename.o $(OBJDIR)/mysystem.o $(OBJDIR)/csv2.o \
$(OBJDIR)/msgtxt.o \
$(OBJDIR)/pbaseutl.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o

$(BINDIR)/pbase:	$(PBASE_OBJS) dbase3.h
	$(CXX) -g $(LDFLAGS) -o $(BINDIR)/pbase $(PBASE_OBJS) $(LTERMCAP)

DBSTRUCT_OBJS = $(OBJDIR)/dbstruct.o $(OBJDIR)/dbase3.o $(OBJDIR)/update.o $(OBJDIR)/datetime.o \
$(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o $(OBJDIR)/cdate.o \
$(OBJDIR)/rpad.o $(OBJDIR)/ztrim.o $(OBJDIR)/weekday.o $(OBJDIR)/getachar.o \
$(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/box.o $(OBJDIR)/dirpath.o \
$(OBJDIR)/jday.o \
$(OBJDIR)/jdate.o $(OBJDIR)/ch.o $(OBJDIR)/fileck.o $(OBJDIR)/trim.o \
$(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/getone.o $(OBJDIR)/strupr.o $(OBJDIR)/settitle.o \
$(OBJDIR)/gethostname.o

$(BINDIR)/dbstruct: $(DBSTRUCT_OBJS) dbase3.h
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/dbstruct $(DBSTRUCT_OBJS) $(LTERMCAP)

TICK2_OBJS = $(OBJDIR)/tick2.o $(OBJDIR)/tick2txt.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/gettext.o $(OBJDIR)/getachar.o \
$(OBJDIR)/makename.o $(OBJDIR)/datetime.o $(OBJDIR)/getone.o $(OBJDIR)/cdate.o \
$(OBJDIR)/rpad.o $(OBJDIR)/stripnl.o $(OBJDIR)/helpsub.o $(OBJDIR)/fileck.o \
$(OBJDIR)/setsec.o $(OBJDIR)/ch.o $(OBJDIR)/trim.o $(OBJDIR)/weekday.o \
$(OBJDIR)/patcmp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/regexp.o $(OBJDIR)/box.o $(OBJDIR)/dirpath.o \
$(OBJDIR)/jday.o $(OBJDIR)/jdate.o $(OBJDIR)/settitle.o \
$(OBJDIR)/gethostname.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/update.o $(OBJDIR)/holiday.o \
$(OBJDIR)/mysystem.o $(OBJDIR)/strupr.o $(OBJDIR)/datecmp.o

$(BINDIR)/tickler2:  $(TICK2_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/tickler2 $(TICK2_OBJS) $(LTERMCAP)

PROJ_OBJS = $(OBJDIR)/projects.o $(OBJDIR)/getachar.o $(OBJDIR)/makename.o \
$(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o $(OBJDIR)/datetime.o \
$(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/datecmp.o $(OBJDIR)/cdate.o \
$(OBJDIR)/getone.o $(OBJDIR)/newpage.o $(OBJDIR)/rpad.o $(OBJDIR)/fileck.o \
$(OBJDIR)/setsec.o \
$(OBJDIR)/ch.o $(OBJDIR)/trim.o $(OBJDIR)/weekday.o $(OBJDIR)/box.o \
$(OBJDIR)/dirpath.o $(OBJDIR)/jday.o $(OBJDIR)/jdate.o $(OBJDIR)/update.o \
$(OBJDIR)/holiday.o \
$(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o

$(BINDIR)/projects:  $(PROJ_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/projects $(PROJ_OBJS) $(LTERMCAP)

NAMES_OBJS = $(OBJDIR)/names.o $(OBJDIR)/namestxt.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/gettext.o $(OBJDIR)/rpt.o $(OBJDIR)/getachar.o \
$(OBJDIR)/makename.o $(OBJDIR)/datetime.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/pattxt.o $(OBJDIR)/rpttxt.o $(OBJDIR)/printer.o $(OBJDIR)/settitle.o \
$(OBJDIR)/regexp.o $(OBJDIR)/getone.o $(OBJDIR)/rpad.o \
$(OBJDIR)/stripnl.o $(OBJDIR)/helpsub.o $(OBJDIR)/fileck.o $(OBJDIR)/csv2.o \
$(OBJDIR)/namesimp.o \
$(OBJDIR)/ch.o $(OBJDIR)/ztrim.o $(OBJDIR)/trim.o $(OBJDIR)/getfileh.o \
$(OBJDIR)/getfilel.o $(OBJDIR)/box.o $(OBJDIR)/dirpath.o $(OBJDIR)/newpage.o \
$(OBJDIR)/update.o \
$(OBJDIR)/strupr.o $(OBJDIR)/setsec.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/mysystem.o $(OBJDIR)/msgtxt.o $(OBJDIR)/cdate.o $(OBJDIR)/jday.o \
$(OBJDIR)/jdate.o $(OBJDIR)/weekday.o $(OBJDIR)/gethostname.o

$(BINDIR)/names:	$(NAMES_OBJS)
	$(CXX) -g $(LDFLAGS) -o $(BINDIR)/names $(NAMES_OBJS) $(LTERMCAP)

LISTS_OBJS = $(OBJDIR)/lists.o $(OBJDIR)/rpt.o $(OBJDIR)/rpttxt.o \
$(OBJDIR)/datetime.o $(OBJDIR)/getachar.o $(OBJDIR)/makename.o \
$(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/hscrn.o $(OBJDIR)/liststxt.o \
$(OBJDIR)/msgtxt.o $(OBJDIR)/printer.o $(OBJDIR)/cdate.o $(OBJDIR)/jday.o \
$(OBJDIR)/weekday.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/getone.o $(OBJDIR)/rpad.o $(OBJDIR)/stripnl.o \
$(OBJDIR)/helpsub.o $(OBJDIR)/fileck.o $(OBJDIR)/setsec.o $(OBJDIR)/ch.o \
$(OBJDIR)/trim.o $(OBJDIR)/roman.o \
$(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o $(OBJDIR)/box.o $(OBJDIR)/dirpath.o \
$(OBJDIR)/newpage.o $(OBJDIR)/update.o $(OBJDIR)/mysystem.o \
$(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o $(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/settitle.o $(OBJDIR)/jdate.o $(OBJDIR)/getword.o $(OBJDIR)/datecmp.o \
$(OBJDIR)/gethostname.o $(OBJDIR)/gsm.o


$(BINDIR)/lists:	$(LISTS_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/lists $(LISTS_OBJS) $(LTERMCAP)

ALARM_OBJS = $(OBJDIR)/alarm.o $(OBJDIR)/myctime.o $(OBJDIR)/datetime.o \
$(OBJDIR)/ch.o $(OBJDIR)/ztrim.o

$(BINDIR)/alarm_do: $(ALARM_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/alarm_do $(ALARM_OBJS)

ALARM2_OBJS = $(OBJDIR)/alarm2.o $(OBJDIR)/myctime.o $(OBJDIR)/datetime.o \
$(OBJDIR)/ch.o $(OBJDIR)/ztrim.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/getachar.o $(OBJDIR)/rpad.o

$(BINDIR)/alarm2: $(ALARM2_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/alarm2 $(ALARM2_OBJS) $(LTERMCAP)

COUNTDOWN_OBJS = $(OBJDIR)/countdown.o $(OBJDIR)/myctime.o $(OBJDIR)/datetime.o \
$(OBJDIR)/ch.o $(OBJDIR)/ztrim.o $(OBJDIR)/getachar.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o

$(BINDIR)/countdown: $(COUNTDOWN_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/countdown $(COUNTDOWN_OBJS) $(LTERMCAP)

SAFERUN_OBJS = $(OBJDIR)/saferun.o 

$(BINDIR)/saferun: $(SAFERUN_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/saferun $(SAFERUN_OBJS) $(LTERMCAP)

BHI_OBJS = $(OBJDIR)/bhi.o $(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o \
$(OBJDIR)/stripnl.o $(OBJDIR)/trim.o $(OBJDIR)/getachar.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/box.o $(OBJDIR)/rpad.o $(OBJDIR)/fileck.o \
$(OBJDIR)/dirpath.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/ch.o \
$(OBJDIR)/getone.o $(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o


$(BINDIR)/bhi:  $(BHI_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/bhi $(BHI_OBJS) $(LTERMCAP)

CALENDAR_OBJS = $(OBJDIR)/cal2.o $(OBJDIR)/cal2txt.o $(OBJDIR)/weekday.o \
$(OBJDIR)/rpt.o $(OBJDIR)/rpttxt.o $(OBJDIR)/getachar.o \
$(OBJDIR)/makename.o $(OBJDIR)/cal2imp.o $(OBJDIR)/printer.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o \
$(OBJDIR)/datetime.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o $(OBJDIR)/getone.o \
$(OBJDIR)/cdate.o $(OBJDIR)/rpad.o $(OBJDIR)/stripnl.o $(OBJDIR)/getfileh.o \
$(OBJDIR)/getfilel.o $(OBJDIR)/newpage.o $(OBJDIR)/fileck.o $(OBJDIR)/setsec.o \
$(OBJDIR)/gettext.o $(OBJDIR)/jdate.o $(OBJDIR)/ch.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/trim.o \
$(OBJDIR)/helpsub.o $(OBJDIR)/box.o $(OBJDIR)/jday.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/myctime.o $(OBJDIR)/dirpath.o \
$(OBJDIR)/update.o $(OBJDIR)/holiday.o \
$(OBJDIR)/mysystem.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/getword.o $(OBJDIR)/datecmp.o $(OBJDIR)/strupr.o $(OBJDIR)/csv2.o \
$(OBJDIR)/msgtxt.o

$(BINDIR)/calendar:  $(CALENDAR_OBJS)
	$(CXX) -g $(LDFLAGS) -o $(BINDIR)/calendar $(CALENDAR_OBJS) $(LTERMCAP)

MERGE3B_OBJS = $(OBJDIR)/merge3b.o $(OBJDIR)/getachar.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/stripnl.o $(OBJDIR)/ch.o $(OBJDIR)/trim.o \
$(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o $(OBJDIR)/box.o $(OBJDIR)/rpad.o \
$(OBJDIR)/fileck.o $(OBJDIR)/dirpath.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/setsec.o \
$(OBJDIR)/listtxt.o $(OBJDIR)/getone.o $(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o

$(BINDIR)/merge3b: $(MERGE3B_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/merge3b $(MERGE3B_OBJS) $(LTERMCAP)

MERGE3_OBJS = $(OBJDIR)/merge3.o $(OBJDIR)/getachar.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/stripnl.o $(OBJDIR)/ch.o $(OBJDIR)/trim.o \
$(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o $(OBJDIR)/box.o $(OBJDIR)/rpad.o \
$(OBJDIR)/fileck.o $(OBJDIR)/dirpath.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/setsec.o \
$(OBJDIR)/listtxt.o $(OBJDIR)/getone.o $(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o

$(BINDIR)/merge3: $(MERGE3_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/merge3 $(MERGE3_OBJS) $(LTERMCAP)

MERGECAL_OBJS = $(OBJDIR)/mergecal.o $(OBJDIR)/getachar.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/stripnl.o $(OBJDIR)/ch.o $(OBJDIR)/trim.o \
$(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o $(OBJDIR)/box.o $(OBJDIR)/rpad.o \
$(OBJDIR)/fileck.o $(OBJDIR)/dirpath.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/setsec.o \
$(OBJDIR)/listtxt.o $(OBJDIR)/getone.o $(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o

$(BINDIR)/mergecal: $(MERGECAL_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/mergecal $(MERGECAL_OBJS) $(LTERMCAP)

NMERGECAL_OBJS = $(OBJDIR)/nmergecal.o $(OBJDIR)/getachar.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/stripnl.o $(OBJDIR)/ch.o $(OBJDIR)/trim.o \
$(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o $(OBJDIR)/box.o $(OBJDIR)/rpad.o \
$(OBJDIR)/fileck.o $(OBJDIR)/dirpath.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/setsec.o \
$(OBJDIR)/listtxt.o $(OBJDIR)/getone.o $(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o

$(BINDIR)/nmergecal: $(NMERGECAL_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/nmergecal $(NMERGECAL_OBJS) $(LTERMCAP)

NMERGE_OBJS = $(OBJDIR)/nmerge.o $(OBJDIR)/getachar.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/stripnl.o $(OBJDIR)/ch.o $(OBJDIR)/trim.o \
$(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o $(OBJDIR)/box.o $(OBJDIR)/rpad.o \
$(OBJDIR)/fileck.o $(OBJDIR)/dirpath.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/setsec.o \
$(OBJDIR)/listtxt.o $(OBJDIR)/getone.o $(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o

$(BINDIR)/nmerge: $(NMERGE_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/nmerge $(NMERGE_OBJS) $(LTERMCAP)

MERGE_OBJS = $(OBJDIR)/merge.o $(OBJDIR)/getachar.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/stripnl.o $(OBJDIR)/ch.o $(OBJDIR)/trim.o \
$(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o $(OBJDIR)/box.o $(OBJDIR)/rpad.o \
$(OBJDIR)/fileck.o $(OBJDIR)/dirpath.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/setsec.o \
$(OBJDIR)/listtxt.o $(OBJDIR)/getone.o $(OBJDIR)/strupr.o $(OBJDIR)/ztrim.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o

$(BINDIR)/merge:	$(MERGE_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/merge $(MERGE_OBJS) $(LTERMCAP)

MERGEDB_OBJS = $(OBJDIR)/mergedb.o $(OBJDIR)/getachar.o $(OBJDIR)/hscrn.o \
$(OBJDIR)/lscrn.o $(OBJDIR)/stripnl.o $(OBJDIR)/ch.o $(OBJDIR)/trim.o \
$(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o $(OBJDIR)/box.o $(OBJDIR)/rpad.o \
$(OBJDIR)/fileck.o $(OBJDIR)/dirpath.o $(OBJDIR)/dbase3.o $(OBJDIR)/update.o $(OBJDIR)/crdbase.o \
$(OBJDIR)/strupr.o \
$(OBJDIR)/datetime.o $(OBJDIR)/cdate.o $(OBJDIR)/ztrim.o $(OBJDIR)/weekday.o \
$(OBJDIR)/jday.o $(OBJDIR)/jdate.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/setsec.o \
$(OBJDIR)/listtxt.o $(OBJDIR)/getone.o $(OBJDIR)/settitle.o \
$(OBJDIR)/gethostname.o

$(BINDIR)/mergedb: $(MERGEDB_OBJS) dbase3.h 
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/mergedb $(MERGEDB_OBJS) $(LTERMCAP)

$(OBJDIR)/wwtxt.o: wwtxt.c
	$(CC) $(CFLAGS) -c -g -D_FILE_OFFSET_BITS=64 -o $(OBJDIR)/wwtxt.o wwtxt.c 

$(OBJDIR)/whatis.o: whatis.c
	$(CC) $(CFLAGS) -c -g -D_FILE_OFFSET_BITS=64 -o $(OBJDIR)/whatis.o whatis.c

WHATIS_OBJS = $(OBJDIR)/whatis.o $(OBJDIR)/wwibm.o $(OBJDIR)/wwtdm.o \
$(OBJDIR)/wwmac1.o $(OBJDIR)/wwmac2.o $(OBJDIR)/wwunix.o $(OBJDIR)/strupr.o \
$(OBJDIR)/getfileh.o $(OBJDIR)/getfilel.o $(OBJDIR)/stripnl.o \
$(OBJDIR)/getword.o $(OBJDIR)/trim.o $(OBJDIR)/hscrn.o $(OBJDIR)/lscrn.o \
$(OBJDIR)/ch.o \
$(OBJDIR)/fileck.o $(OBJDIR)/getachar.o $(OBJDIR)/box.o $(OBJDIR)/rpad.o \
$(OBJDIR)/dirpath.o $(OBJDIR)/patcmp.o \
$(OBJDIR)/regexp.o $(OBJDIR)/listutl.o $(OBJDIR)/listtxt.o \
$(OBJDIR)/getone.o $(OBJDIR)/wwtxt.o $(OBJDIR)/ztrim.o $(OBJDIR)/settitle.o \
$(OBJDIR)/gethostname.o
$(BINDIR)/whatis:	 $(WHATIS_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/whatis $(WHATIS_OBJS) $(LTERMCAP)

LSCRNTST_OBJS = $(OBJDIR)/lscrntst.o $(OBJDIR)/lscrn.o $(OBJDIR)/getachar.o \
$(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o $(OBJDIR)/datetime.o

$(BINDIR)/lscrntst:  $(LSCRNTST_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/lscrntst $(LSCRNTST_OBJS) $(LTERMCAP)

HSCRNTST_OBJS = $(OBJDIR)/hscrntst.o $(OBJDIR)/hscrn.o $(OBJDIR)/gettext.o \
$(OBJDIR)/getachar.o $(OBJDIR)/lscrn.o $(OBJDIR)/box.o $(OBJDIR)/rpad.o \
$(OBJDIR)/trim.o $(OBJDIR)/ztrim.o $(OBJDIR)/settitle.o $(OBJDIR)/gethostname.o

$(BINDIR)/hscrntst:  $(HSCRNTST_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/hscrntst $(HSCRNTST_OBJS) $(LTERMCAP)

DIAL_OBJS = $(OBJDIR)/dial.o $(OBJDIR)/ch.o $(OBJDIR)/getini.o \
$(OBJDIR)/ztrim.o $(OBJDIR)/stripnl.o $(OBJDIR)/strupr.o $(OBJDIR)/fileck.o

$(BINDIR)/dial:  $(DIAL_OBJS)
	$(CC) -g $(LDFLAGS) -o $(BINDIR)/dial $(DIAL_OBJS)


#
#  compiles
#

$(OBJDIR)/merge3.o:  merge3.c merge.c

$(OBJDIR)/mergedb.o:	mergedb.c merge.c

$(OBJDIR)/mergecal.o:	 mergecal.c merge.c

$(OBJDIR)/nmergecal.o:  nmergecal.c merge.c

$(OBJDIR)/merge3b.o:  merge3b.c merge.c

$(OBJDIR)/nmerge.o:  nmerge.c merge.c

$(OBJDIR)/clearcal.o:	 clearcal.c
	$(CC) $(CFLAGS) -c -g -o $(OBJDIR)/clearcal.o -I/usr/dt/share/include clearcal.c

$(OBJDIR)/xmenu.o:	xmenu.c
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/xmenu.o -g $(XINCLUDES) xmenu.c

$(OBJDIR)/xshowfonts.o:	xshowfonts.c
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/xshowfonts.o -g $(XINCLUDES) xshowfonts.c

$(OBJDIR)/myctime.o:	ctime.c
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/myctime.o -g ctime.c


