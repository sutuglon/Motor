# set INSTALLDIR to your target folder
LIBDESTIN = U:\DEV\lib
INCLUDEDESTIN = U:\DEV\include
LIBNAME=libCPNG.a

all-after : ${LIBDESTIN}\libCPNG.a $(INCLUDEDESTIN)\readimg.h

${LIBDESTIN}\libCPNG.a: .\output\cpng.a
	copy .\output\cpng.a ${LIBDESTIN}\libCPNG.a
	
$(INCLUDEDESTIN)\readimg.h: readimg.h
	copy readimg.h $(INCLUDEDESTIN)\readimg.h
