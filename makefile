# makefile
#	Makefile for rf.x rpcgen
#
# This makefile generates the following files using rpcgen:
#	rf.h
#	rf_clnt.c
#	rf_xdr.c
#	rf_svc.c
#
# It also generates the following object files:
#	rf_clnt.o, rf_xdr.o, rf_svc.o, rfsvcfn.o, and rftest.o
#
# The source files for the above object files are:
#	rf_clnt.c, rf_xdr.c, rf_svc.c, rfsvcfn.c, and rftest.c and rf.h
# It generates rpc client executable: rfclient
#	and server executables: rfserver
#
# To generate the executables issue the following make commands:
# For rfclient:	make rfclient
# For rfserver: make rfserver
#

all: rfserver rfclient
	make rfclient
	make rfserver

full: rfserver rfclient
	make clean
	make rfclient
	make rfserver

rfserver: rf_svc.o rf_xdr.o rfsvcfn.o
	cc $? -o rfserver -lnsl

rfclient: rftest.o rf_clnt.o rf_xdr.o rf.x
	cc rf_clnt.o rf_xdr.o rftest.o -o rfclient -lnsl

rf.h: rf.x
	echo '#include <time.h>' > $@
	rpcgen -h rf.x >>$@

rf_clnt.c: rf.x
	rpcgen -l rf.x >$@

rf_xdr.c: rf.x
	rpcgen -c rf.x >$@

rf_svc.c: rf.x
	rpcgen -s udp rf.x >$@

rf_clnt.o: rf_clnt.c rf.h rf.x
	cc -g -c $*.c

rf_svc.o: rf_svc.c rf.h rf.x
	cc -g -c $*.c

rf_xdr.o: rf_xdr.c rf.h rf.x
	cc -g -c $*.c

rfsvcfn.o: rfsvcfn.c rf.h rf.x
	cc -g -c $*.c

rftest.o: rftest.c rf.h rf.x
	cc -g -c $*.c

clean: 
	@echo "	Clean before building."
	rm -f rfserver rfclient rf.h rf_clnt.c rf_xdr.c rf_svc.c rf_clnt.o rf_svc.o rf_xdr.o rfsvcfn.o rftest.o

