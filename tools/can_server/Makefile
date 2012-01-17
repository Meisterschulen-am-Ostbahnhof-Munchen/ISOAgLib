#############################################################################
# Makefile for building: can_server_[sock/msq]_{can-card}
# Project:               can_server
# Help:                  make help
#############################################################################

.PHONY: all deps clean distclean first help

ISOAGLIB_PATH := ../..

PROJ_DEFINES_MSQ := -DCAN_DRIVER_MESSAGE_QUEUE
PROJ_DEFINES_SOCKET := -DCAN_DRIVER_SOCKET

# Files
SOURCES_MSQ := \
  src/can_server_msq.cpp \
	$(ISOAGLIB_PATH)/library/xgpl_src/IsoAgLib/hal/pc/can/msq_helper.cpp

SOURCES_SOCKET := src/can_server_sock.cpp

SOURCES_MSQ_NO_CARD := \
	src/can_server_common.cpp \
	src/can_filtering.cpp \
	$(SOURCES_MSQ) \
	src/devices/can_device_no_card.cpp

SOURCES_SOCKET_NO_CARD := \
	src/can_server_common.cpp \
	src/can_filtering.cpp \
	$(SOURCES_SOCKET) \
	src/devices/can_device_no_card.cpp


SOURCES_MSQ_PCAN := \
	src/can_server_common.cpp \
	src/can_filtering.cpp \
	$(SOURCES_MSQ) \
	src/devices/can_device_pcan.cpp

SOURCES_SOCKET_PCAN := \
	src/can_server_common.cpp \
	src/can_filtering.cpp \
	$(SOURCES_SOCKET) \
	src/devices/can_device_pcan.cpp


SOURCES_MSQ_SOCKETCAN := \
	src/can_server_common.cpp \
	src/can_filtering.cpp \
	$(SOURCES_MSQ) \
	src/devices/can_device_socketcan.cpp

SOURCES_SOCKET_SOCKETCAN := \
	src/can_server_common.cpp \
	src/can_filtering.cpp \
	$(SOURCES_SOCKET) \
	src/devices/can_device_socketcan.cpp


SOURCES_MSQ_RTE := \
	src/can_server_common.cpp \
	src/can_filtering.cpp \
	$(SOURCES_MSQ) \
	src/devices/can_device_rte.cpp

SOURCES_SOCKET_RTE := \
	src/can_server_common.cpp \
	src/can_filtering.cpp \
	$(SOURCES_SOCKET) \
	src/devices/can_device_rte.cpp

# SOURCES_ALL_MSQ/SOCKET are used for the target "clean"
SOURCES_ALL_MSQ := \
	src/can_server_common.cpp \
	src/can_filtering.cpp \
	$(SOURCES_MSQ) \
	src/devices/can_device_no_card.cpp \
	src/devices/can_device_pcan.cpp	\
	src/devices/can_device_socketcan.cpp	\
	src/devices/can_device_rte.cpp \

SOURCES_ALL_SOCKET := \
	src/can_server_common.cpp \
	src/can_filtering.cpp \
	$(SOURCES_SOCKET) \
	src/devices/can_device_no_card.cpp \
	src/devices/can_device_pcan.cpp	\
	src/devices/can_device_socketcan.cpp	\
	src/devices/can_device_rte.cpp \


LFLAGS_RTE := -L/usr/local/lib -lrte_client -lfevent

# Definition of compiler binary prefix corresponding to selected target
# can be set either here or given as an environment variable outside of this Makefile
#COMPILER_BINARY_PRE :=

CC       := $(COMPILER_BINARY_PRE)gcc
CXX      := $(COMPILER_BINARY_PRE)g++
STRIP    := $(COMPILER_BINARY_PRE)strip

# flags common to compiler and linker:
CLFLAGS   := -pthread

GCC_VERSION := $(shell $(CXX) -dumpversion)
EXTRA_CFLAGS := $(if $(findstring ^2.,^$(GCC_VERSION).),,-Wextra -Winit-self -Wmissing-include-dirs)
INCPATH  := -I. -Isrc -I$(ISOAGLIB_PATH)/library/xgpl_src/IsoAgLib/hal/pc/can -I$(ISOAGLIB_PATH)/library/xgpl_src
COMPILERFLAGS = -pipe -O $(CLFLAGS) -Wall -g $(EXTRA_CFLAGS) -fno-builtin -fno-exceptions -Wshadow -Wcast-qual -Wcast-align -Woverloaded-virtual -Wpointer-arith -DPRJ_USE_AUTOGEN_CONFIG=config_can_server.h -DSYSTEM_WITH_ENHANCED_CAN_HAL $(INCPATH) $(1)

CFLAGS = $(call COMPILERFLAGS,) $(1)
CXXFLAGS = $(call COMPILERFLAGS,) $(1)

LINK     := $(COMPILER_BINARY_PRE)g++
DEL_FILE := rm -f
DEL_DIR  := rmdir
MKDIR    := mkdir -p

OBJ_DIR = objects_$(1)
OBJECTS = $(patsubst %.cpp,$(call OBJ_DIR,$(1))/%.o,$(notdir $(2)))
DEPS = $(patsubst %.o,%.d,$(call OBJECTS,$(1),$(2)))
EXEC_DIR := $(ISOAGLIB_PATH)/bin/x86linux
PRE_EXEC = $(addprefix ,$(1))
TARGETS := $(call PRE_EXEC,can_server_msq_no_card can_server_socket_no_card can_server_msq_pcan can_server_socket_pcan can_server_msq_socketcan can_server_socket_socketcan can_server_msq_rte can_server_socket_rte)

# Rules

first: $(call PRE_EXEC,can_server_msq_no_card can_server_socket_no_card)

help:
	@printf 'Invocation Examples:\n'
	@printf '$$ make %s\n' all '' $(TARGETS)

all: $(TARGETS)

define RULES
$$(call OBJ_DIR,$(1)) :
	$(MKDIR) $$(call OBJ_DIR,$(1))

$$(call OBJ_DIR,$(1))/%.d : | $$(call OBJ_DIR,$(1))
	$(CXX) -c $$(call CXXFLAGS,$(3)) -MM $$(filter %/$$(patsubst $$(call OBJ_DIR,$(1))/%.d,%,$$@).cpp,$(2)) 2>/dev/null | \
	perl -p -e 's!^([^:]+:)!$$(call OBJ_DIR,$(1))/$$$$1!' > $$@

$$(call OBJ_DIR,$(1))/%.o : $$(filter %/$$(patsubst $$(call OBJ_DIR,$(1))/%.d,%,$$@).cpp,$(2)) $$(call OBJ_DIR,$(1))/%.d 
	$(CXX) $$(call CXXFLAGS,$(3)) -o $$@ -c $$(filter %/$$(patsubst $$(call OBJ_DIR,$(1))/%.o,%,$$@).cpp,$(2))

include $$(call DEPS,$(1),$(2))
endef

$(eval $(call RULES,msq,$(SOURCES_ALL_MSQ),$(PROJ_DEFINES_MSQ)))
$(eval $(call RULES,socket,$(SOURCES_ALL_SOCKET),$(PROJ_DEFINES_SOCKET)))

$(call PRE_EXEC,can_server_msq_no_card): $(call OBJECTS,msq,$(SOURCES_MSQ_NO_CARD))
	$(LINK) -o $@ $^ $(CLFLAGS) -lrt
	$(STRIP) $@

$(call PRE_EXEC,can_server_socket_no_card): $(call OBJECTS,socket,$(SOURCES_SOCKET_NO_CARD))
	$(LINK) -o $@ $^ $(CLFLAGS) -lrt
	$(STRIP) $@

$(call PRE_EXEC,can_server_msq_pcan): $(call OBJECTS,msq,$(SOURCES_MSQ_PCAN))
	$(LINK) -o $@ $^ $(CLFLAGS) -lrt
	$(STRIP) $@

$(call PRE_EXEC,can_server_socket_pcan): $(call OBJECTS,socket,$(SOURCES_SOCKET_PCAN))
	$(LINK) -o $@ $^ $(CLFLAGS) -lrt
	$(STRIP) $@

$(call PRE_EXEC,can_server_msq_socketcan): $(call OBJECTS,msq,$(SOURCES_MSQ_SOCKETCAN))
	$(LINK) -o $@ $^ $(CLFLAGS) -lrt
	$(STRIP) $@

$(call PRE_EXEC,can_server_socket_socketcan): $(call OBJECTS,socket,$(SOURCES_SOCKET_SOCKETCAN))
	$(LINK) -o $@ $^ $(CLFLAGS) -lrt
	$(STRIP) $@

$(call PRE_EXEC,can_server_msq_rte): $(call OBJECTS,msq,$(SOURCES_MSQ_RTE))
	$(LINK) -o $@ $^ $(CLFLAGS) $(LFLAGS_RTE) -lrt
	$(STRIP) $@

$(call PRE_EXEC,can_server_socket_rte): $(call OBJECTS,socket,$(SOURCES_SOCKET_RTE))
	$(LINK) -o $@ $^ $(CLFLAGS) $(LFLAGS_RTE) -lrt
	$(STRIP) $@

deps: $(call DEPS,msq,$(SOURCES_ALL_MSQ)) $(call DEPS,socket,$(SOURCES_ALL_SOCKET))

clean:
	-$(DEL_FILE) $(call OBJECTS,msq,$(SOURCES_ALL_MSQ)) $(call DEPS,msq,$(SOURCES_ALL_MSQ))
	-$(DEL_FILE) $(call OBJECTS,socket,$(SOURCES_ALL_SOCKET)) $(call DEPS,socket,$(SOURCES_ALL_SOCKET))
	-$(DEL_DIR) $(call OBJ_DIR,msq)
	-$(DEL_DIR) $(call OBJ_DIR,socket)

distclean: clean
	-$(DEL_FILE) $(TARGETS)
