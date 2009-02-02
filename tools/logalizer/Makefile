#############################################################################
# Makefile for building: logalizer
# Project:               logalizer
#############################################################################

####### Project specific variables
TARGET = ../../bin/logalizer
APP_INC = -Isrc 
COMPILER_BINARY_PRE = 


first: all

####### Files
SOURCES_APP = src/logalizer.cpp  

####### Some Standard Rules
SUBLIBS  = -lrt
CC       = $(COMPILER_BINARY_PRE)gcc
CXX      = $(COMPILER_BINARY_PRE)g++
STRIP	 = $(COMPILER_BINARY_PRE)strip
LEX      = flex
YACC     = yacc
CFLAGS   = -pipe -O -Wall -Wextra -g -fno-builtin -fno-exceptions -fno-exceptions -Winit-self -Wmissing-include-dirs -Wshadow -Wcast-qual -Wcast-align -Wconversion -Winline -Woverloaded-virtual -Wpointer-arith
CXXFLAGS = -pipe -O -Wall -Wextra -g -fno-builtin -fno-exceptions -fno-exceptions -Winit-self -Wmissing-include-dirs -Wshadow -Wcast-qual -Wcast-align -Wconversion -Winline -Woverloaded-virtual -Wpointer-arith
LEXFLAGS =
YACCFLAGS= -d
INCPATH  = -Isrc $(APP_INC)
LINK     = $(COMPILER_BINARY_PRE)g++
#LFLAGS   = -pthread
LIBS     = $(SUBLIBS)
AR       = ar cqs
RANLIB   =
TAR      = tar -cf
GZIP     = gzip -9f
COPY     = cp -f
COPY_FILE= $(COPY)
COPY_DIR = $(COPY) -r
INSTALL_FILE= $(COPY_FILE)
INSTALL_DIR = $(COPY_DIR)
DEL_FILE = rm -f
SYMLINK  = ln -sf
DEL_DIR  = rmdir
MOVE     = mv -f
CHK_DIR_EXISTS= test -d
MKDIR    = mkdir -p

####### Output directory

OBJ_DIR_APP  = obj

####### Build Rules for Application Part

OBJECTS_APP = $(patsubst %.cpp,$(OBJ_DIR_APP)/%.o,$(notdir $(SOURCES_APP)))
DEPS_APP := $(patsubst %.o,%.d,$(OBJECTS_APP))

$(OBJ_DIR_APP)/%.d :
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -MM $(filter %/$(patsubst $(OBJ_DIR_APP)/%.d,%,$@).cpp,$(SOURCES_APP)) 2>/dev/null | \
	perl -p -e 's!^([^:]+:)!$(OBJ_DIR_APP)/$$1!' > $@

include $(DEPS_APP)

####### Implicit rules

.SUFFIXES: .c .o .cpp .cc .cxx .C

$(OBJ_DIR_APP)/%.o : $(filter %/$(patsubst $(OBJ_DIR_APP)/%.d,%,$@).cpp,$(SOURCES_APP)) $(OBJ_DIR_APP)/%.d
	$(CXX) $(CXXFLAGS) $(INCPATH)  -o $@ -c $(filter %/$(patsubst $(OBJ_DIR_APP)/%.o,%,$@).cpp,$(SOURCES_APP))

####### Build rules
deps : $(DEPS_APP)

prepare :
	$(MKDIR) $(OBJ_DIR_APP)

all: prepare deps Makefile $(TARGET)
$(TARGET):  $(OBJECTS_APP)
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS_APP) $(LIBS)
	$(STRIP) $(TARGET)

yaccclean:
lexclean:
clean:
	-$(DEL_FILE) $(OBJECTS_APP) $(DEPS_APP)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET)


FORCE:

####### Compile


####### Install

uninstall: 

