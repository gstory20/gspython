#
#  Makefile
#
#  Glenn Story -- 3/10/93
#
#  This is my general unix makefile
#

INCLS = gs.h gslib.h

BIN ?= /usr/local/bin

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
PYTHON_INC ?= -I/usr/include/python3.9	

#
#  Ubuntu once set an obsolescent cc command for CC
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

.SUFFIXES : .o .c .cpp

$(OBJDIR)/%.o :: %.c
	$(CC) $(CFLAGS) -c -g -o $(OBJDIR)/$*.o $<

$(OBJDIR)/%.o : %.cpp 
	$(CXX) $(CXXFLAGS) -c -g -o $(OBJDIR)/$*.o $<

$(BIN)/%:: $(BINDIR)/%
	install $(BINDIR)/$* $(BIN)

%.tst:: $(BINDIR)/%
	./testone.sh $(BINDIR)/$*

ACTIVE_EXE = $(BINDIR)/_pydbase.so $(BINDIR)/pydbase.py $(BINDIR)/__pyscrn.so \
$(BINDIR)/_pyscrn.py $(BINDIR)/__cdate.so $(BINDIR)/_cdate.py \
$(BINDIR)/__cvttime.so $(BINDIR)/_cvttime.py \
$(BINDIR)/_pylist.py $(BINDIR)/__pylist.so \
$(BINDIR)/__getfile.so $(BINDIR)/_getfile.py \
$(BINDIR)/_cpu_info_report.so $(BINDIR)/cpu_info_report.py \
$(BINDIR)/_patcmp.so $(BINDIR)/patcmp.py \
$(BINDIR)/__remote_pause.so $(BINDIR)/_remote_pause.py

RLSE_EXE =  \
$(BIN)/_pydbase.so $(BIN)/_pyscrn.py $(BIN)/__pyscrn.so $(BIN)/__cdate.so \
$(BIN)/__remote_pause.so $(BIN)/_remote_pause.py \
$(BIN)/_cpu_info_report.so $(BIN)/cpu_info_report.py \
$(BIN)/_patcmp.so $(BIN)/patcmp.py \
$(BIN)/_cdate.py $(BIN)/_cvttime.py $(BIN)/__cvttime.so \
$(BIN)/_pylist.py $(BIN)/__pylist.so \
$(BIN)/_getfile.py $(BIN)/__getfile.so \
$(BIN)/pydbase.py 

#  active is the default target
active:	 $(ACTIVE_EXE)

#
#  Standard targets
#

clean:
	-rm *.o
	-rm $(OBJDIR)/*.o

clobber: clean
	-rm $(ACTIVE_EXE)

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


cpu_info_report.py:  cpu_info_report.i
	swig -python cpu_info_report.i

cpu_info_report_wrap.c:  cpu_info_report.i
	swig -python cpu_info_report.i

$(OBJDIR)/cpu_info_report_wrap.o:	cpu_info_report_wrap.c
	$(CC) -g -c -o $(OBJDIR)/cpu_info_report_wrap.o -fPIC $(PYTHON_INC) cpu_info_report_wrap.c

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

