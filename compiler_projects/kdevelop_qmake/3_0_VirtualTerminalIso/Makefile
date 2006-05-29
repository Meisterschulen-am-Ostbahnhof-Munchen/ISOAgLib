#############################################################################
# Makefile for building: 3_0_VirtualTerminalIso
# Project:               3_0_VirtualTerminalIso
#############################################################################

####### Project specific variables
TARGET = 3_0_VirtualTerminalIso
ISOAGLIB_PATH = ../../..
APP_INC = -I../../../IsoAgLib_Examples/tutorial/3_Terminal/3_0_VirtualTerminalIso 

PROJ_DEFINES = -DSYSTEM_PC -DPRJ_USE_AUTOGEN_CONFIG=config_3_0_VirtualTerminalIso.h


first: all

####### Files
SOURCES = ../../../xgpl_src/IsoAgLib/comm/SystemMgmt/impl/monitoritem_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/SystemMgmt/impl/identitem_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/SystemMgmt/impl/baseitem_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/SystemMgmt/impl/istate_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/SystemMgmt/impl/systemmgmt_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/SystemMgmt/ISO11783/impl/isomonitor_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/SystemMgmt/ISO11783/impl/isosystempkg_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/SystemMgmt/ISO11783/impl/isoitem_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/SystemMgmt/ISO11783/impl/isoname_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/SystemMgmt/ISO11783/impl/isorequestpgn_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/Scheduler/impl/scheduler_c.cpp  \
		../../../xgpl_src/IsoAgLib/util/impl/cancustomer_c.cpp  \
		../../../xgpl_src/IsoAgLib/util/impl/canpkg_c.cpp  \
		../../../xgpl_src/IsoAgLib/util/impl/canpkgext_c.cpp  \
		../../../xgpl_src/IsoAgLib/util/impl/util_funcs.cpp  \
		../../../xgpl_src/IsoAgLib/util/liberr_c.cpp  \
		../../../xgpl_src/supplementary_driver/driver/datastreams/volatilememory_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/Multipacket/impl/multireceive_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/Multipacket/impl/multisend_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/Multipacket/impl/streamchunk_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/Multipacket/impl/multisendpkg_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/Multipacket/impl/chunk_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/Multipacket/impl/stream_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectsoftkeymask_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectworkingset_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectellipse_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectpicturegraphic_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectlinearbargraph_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/isoterminal_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectinputstring_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectbutton_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectinputnumber_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectmeter_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectauxiliaryfunction_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectfontattributes_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobject_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectdatamask_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectauxiliaryinput_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectinputlist_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectarchedbargraph_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectmacro_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectalarmmask_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectstringvariable_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectline_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectoutputstring_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/isoterminalpkg_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectinput_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectfillattributes_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectnumbervariable_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectlineattributes_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectinputattributes_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectinputboolean_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectrectangle_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectcontainer_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectstring_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectobjectpointer_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectoutputnumber_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectpolygon_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/impl/vtobjectkey_c.cpp  \
		../../../xgpl_src/IsoAgLib/comm/ISO_Terminal/ivtobject_c.cpp  \
		../../../xgpl_src/IsoAgLib/hal/pc/can/msq_helper.cpp  \
		../../../xgpl_src/IsoAgLib/hal/pc/can/target_extension_can_client.cpp  \
		../../../xgpl_src/IsoAgLib/hal/pc/can/hal_can_interface.cpp  \
		../../../xgpl_src/IsoAgLib/hal/pc/system/system_target_extensions.cpp  \
		../../../xgpl_src/IsoAgLib/driver/can/impl/filterbox_c.cpp  \
		../../../xgpl_src/IsoAgLib/driver/can/impl/msgobj_c.cpp  \
		../../../xgpl_src/IsoAgLib/driver/can/impl/ident_c.cpp  \
		../../../xgpl_src/IsoAgLib/driver/can/impl/canio_c.cpp  \
		../../../xgpl_src/IsoAgLib/driver/system/impl/system_c.cpp  \
		../../../IsoAgLib_Examples/tutorial/3_Terminal/3_0_VirtualTerminalIso/3_0_VirtualTerminalIso.cpp  


#Special Sources for CAN Server
SOURCES_SERVER = ../../../xgpl_src/IsoAgLib/hal/pc/can/target_extension_can_server_A1.cpp \
		../../../xgpl_src/IsoAgLib/hal/pc/can/msq_helper.cpp \
		../../../xgpl_src/IsoAgLib/hal/pc/can/can_server_helper.cpp

#Special Rules for CAN Server
OBJ_DIR_SERVER  = objects_server
OBJECTS_SERVER = $(patsubst %.cpp,$(OBJ_DIR_SERVER)/%.o,$(notdir $(SOURCES_SERVER)))
DEPS_SERVER := $(patsubst %.o,%.d,$(OBJECTS_SERVER))

$(OBJ_DIR_SERVER)/%.d :
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -MM $(filter %/$(patsubst $(OBJ_DIR_SERVER)/%.d,%,$@).cpp,$(SOURCES_SERVER)) 2>/dev/null | \
	perl -p -e 's!^([^:]+:)!$(OBJ_DIR_SERVER)/$$1!' > $@

$(OBJ_DIR_SERVER)/%.o : $(filter %/$(patsubst $(OBJ_DIR_SERVER)/%.d,%,$@).cpp,$(SOURCES_SERVER)) $(OBJ_DIR_SERVER)/%.d
	$(CXX) $(CXXFLAGS) $(INCPATH)  -o $@ -c $(filter %/$(patsubst $(OBJ_DIR_SERVER)/%.o,%,$@).cpp,$(SOURCES_SERVER))

include $(DEPS_SERVER)

can_server: $(OBJECTS_SERVER)
	$(LINK) $(LFLAGS) -o can_server $(OBJECTS_SERVER) $(LIBS)
####### Some Standard Rules
CC       = gcc
CXX      = g++
STRIP	 = strip
LEX      = flex
YACC     = yacc
CFLAGS   = -pipe -Wall -W -g -fno-builtin -Wall -Woverloaded-virtual -Wstrict-prototypes -Wpointer-arith $(PROJ_DEFINES)
CXXFLAGS = -pipe -Wall -W -g -fno-builtin -Wall -Woverloaded-virtual -Wstrict-prototypes -Wpointer-arith $(PROJ_DEFINES)
LEXFLAGS =
YACCFLAGS= -d
INCPATH  = -I. -I$(ISOAGLIB_PATH)/xgpl_src $(APP_INC) $(BIOS_INC)
LINK     = g++
LFLAGS   = -pthread
LIBS     = $(SUBLIBS)  $(BIOS_LIB)
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

OBJ_DIR  = objects

####### Build Rules

OBJECTS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SOURCES)))
DEPS := $(patsubst %.o,%.d,$(OBJECTS))

# $(OBJ_DIR)/%.d :
# 	$(CXX) -c $(CXXFLAGS) $(INCPATH) -MM -MT $(patsubst %.d,%.o,$@)  $(filter %/$(patsubst $(OBJ_DIR)/%.d,%,$@).cpp,$(SOURCES)) > $@

$(OBJ_DIR)/%.d :
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -MM $(filter %/$(patsubst $(OBJ_DIR)/%.d,%,$@).cpp,$(SOURCES)) 2>/dev/null | \
	perl -p -e 's!^([^:]+:)!$(OBJ_DIR)/$$1!' > $@

include $(DEPS)

####### Implicit rules

.SUFFIXES: .c .o .cpp .cc .cxx .C

$(OBJ_DIR)/%.o : $(filter %/$(patsubst $(OBJ_DIR)/%.d,%,$@).cpp,$(SOURCES)) $(OBJ_DIR)/%.d
	$(CXX) $(CXXFLAGS) $(INCPATH)  -o $@ -c $(filter %/$(patsubst $(OBJ_DIR)/%.o,%,$@).cpp,$(SOURCES))

####### Build rules
deps : $(DEPS)

all: can_server Makefile $(TARGET)
$(TARGET):  $(OBJECTS)
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

dist:
	@mkdir -p objects/$(TARGET) && $(COPY_FILE) --parents $(SOURCES) $(HEADERS) $(FORMS) $(DIST) objects/$(TARGET)/ && ( cd `dirname objects/$(TARGET)` && $(TAR) vtserver_rte.tar vtserver_rte && $(GZIP) vtserver_rte.tar ) && $(MOVE) `dirname objects/vtserver_rte`/vtserver_rte.tar.gz . && $(DEL_FILE) -r objects/vtserver_rte

yaccclean:
lexclean:
clean:
	-$(DEL_FILE) $(OBJECTS) $(DEPS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) $(TARGET)


FORCE:

####### Compile


####### Install

install: can_server

uninstall: can_server

