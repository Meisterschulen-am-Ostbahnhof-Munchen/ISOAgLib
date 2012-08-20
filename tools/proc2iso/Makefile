# GNU Makefile for compiling proc2iso

ISOAGLIB := ../..
ISOAGLIBRARY := $(ISOAGLIB)/library
TARGET := proc2iso
SRCS := src/proc2iso.cpp $(ISOAGLIBRARY)/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.cpp $(ISOAGLIBRARY)/xgpl_src/IsoAgLib/util/impl/util_funcs.cpp $(ISOAGLIBRARY)/xgpl_src/IsoAgLib/util/impl/flexiblebytestrings.cpp
OBJS := ${SRCS:.cpp=.o} 
DEPS := ${SRCS:.cpp=.dep} 
XDEPS := $(wildcard ${DEPS}) 

CCFLAGS = -O2 -Wall -Wextra -DAPP_NO_THREADS -DXML_USE_NO_THREADS -DHAL_PATH_ISOAGLIB=IsoAgLib/hal/pc -DOPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN -DCAN_INSTANCE_CNT=1 -D_CRT_SECURE_NO_WARNINGS -I$(ISOAGLIBRARY)/xgpl_src
LDFLAGS =
LIBS = -lxerces-c

.PHONY: all clean distclean 
all:: ${TARGET} 

ifneq (${XDEPS},) 
include ${XDEPS} 
endif 

${TARGET}: ${OBJS} 
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS} 

${OBJS}: %.o: %.cpp %.dep 
	${CC} ${CCFLAGS} -o $@ -c $< 

${DEPS}: %.dep: %.cpp Makefile 
	${CC} ${CCFLAGS} -MM $< > $@ 

clean: 
	-rm -f *~ *.o ${TARGET} 
	-rm -f ${XDEPS}
	-rm -f ${OBJS}

distclean: clean
