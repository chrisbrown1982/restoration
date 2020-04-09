SUBDIRS=`find . -maxdepth 1 -type d | grep -v ^[.]\$$`

build :
	@for f in ${SUBDIRS}; do   if [ -f $$f/Makefile ] ;\
	then echo "[PROCESS] $$f" ;  make -C $$f $@ > /dev/null 2>&1 || echo " !!! Error.. !!! "  ;\
	else echo "[SKIP]    $$f" ;  fi ; done

test :
	@for f in ${SUBDIRS}; do   if [ -f $$f/Makefile ] ;\
	then echo "[PROCESS] $$f" ;  make -C $$f $@ > /dev/null 2>&1 || echo " !!! Error.. !!! "  ;\
	else echo "[SKIP]    $$f" ;  fi ; done

clean :
	@for f in ${SUBDIRS}; do   if [ -f $$f/Makefile ] ;\
	then echo "[PROCESS] $$f" ;  make -C $$f $@ > /dev/null 2>&1 || echo " !!! Error.. !!! "  ;\
	else echo "[SKIP]    $$f" ;  fi ; done

