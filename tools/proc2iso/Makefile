# Generated automatically from Makefile.in by configure.
#
# The Apache Software License, Version 1.1
#
# Copyright (c) 1999-2000 The Apache Software Foundation.  All rights
# reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
#
# 3. The end-user documentation included with the redistribution,
#    if any, must include the following acknowledgment:
#       "This product includes software developed by the
#        Apache Software Foundation (http://www.apache.org/)."
#    Alternately, this acknowledgment may appear in the software itself,
#    if and wherever such third-party acknowledgments normally appear.
#
# 4. The names "Xerces" and "Apache Software Foundation" must
#    not be used to endorse or promote products derived from this
#    software without prior written permission. For written
#    permission, please contact apache\@apache.org.
#
# 5. Products derived from this software may not be called "Apache",
#    nor may "Apache" appear in their name, without prior written
#    permission of the Apache Software Foundation.
#
# THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
# ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
# USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
# ====================================================================
#
# This software consists of voluntary contributions made by many
# individuals on behalf of the Apache Software Foundation, and was
# originally based on software copyright (c) 1999, International
# Business Machines, Inc., http://www.ibm.com .  For more information
# on the Apache Software Foundation, please see
# <http://www.apache.org/>.
#
#
# $Id: Makefile.in,v 1.12 2002/08/19 15:13:37 tng Exp $
#

XERCESCROOT := /usr/include/xercesc
LD_LIBRARY_PATH := /usr/lib/xercesc/
ISOAGLIB := ../..
ISOAGLIBRARY := $(ISOAGLIB)/library
PLATFORM := LINUX
COMPILER := g++
GCC := yes
GXX := yes
CXXFLAGS :=  -g  -Wall -DAPP_NO_THREADS -DXML_USE_NO_THREADS -DSYSTEM_PC -DOPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN -I$(ISOAGLIBRARY)/xgpl_src
CFLAGS :=    -g  -Wall -DAPP_NO_THREADS -DXML_USE_NO_THREADS -DSYSTEM_PC -DOPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN -I$(ISOAGLIBRARY)/xgpl_src
LDFLAGS :=
THREADS := none
EXTRA_LIBS :=
PREFIX := /usr

include version.incl
include Makefile.incl

# Make list of objects for given list of cpp files:
OBJECTS = $(addprefix $(OBJ_DIR)/,$(addsuffix .o,$(basename $(notdir $(1)))))

APP_NAME := proc2iso
IS_X86 = $(filter i%86,$(1))
IS_X86LINUX = $(and $(call IS_X86,$(1)),$(filter Linux,$(1)))
IS_WIN32 = $(and $(call IS_X86,$(1)),$(filter MINGW32_NT%,$(1)))
SYSTEM := $(shell uname -a)
OSDIRPART := $(if $(call IS_X86LINUX,$(SYSTEM)),x86linux,$(if $(call IS_WIN32,$(SYSTEM)),win32,anyos))
OBJ_DIR := objects_$(OSDIRPART)
EXEC :=	$(ISOAGLIB)/bin/$(OSDIRPART)
SOURCES := \
	src/proc2iso.cpp \
	$(ISOAGLIBRARY)/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.cpp \
	$(ISOAGLIBRARY)/xgpl_src/IsoAgLib/util/impl/util_funcs.cpp \
	$(ISOAGLIBRARY)/xgpl_src/IsoAgLib/util/iliberr_c.cpp
OBJS := $(call OBJECTS,$(SOURCES))
myLIBRARY_NAMES = $(LIBRARY_NAMES) -lpthread

all: $(EXEC)/$(APP_NAME) $(EXEC)/proc2iso.xsd

## Dependencies will not be calculated. Use this ``force'' as
## prerequisite to force compilation.
.PHONY: force

$(OBJ_DIR):
	-mkdir -p $(OBJ_DIR)

$(EXEC)/$(APP_NAME): $(OBJS)
	$(LINK) $(PLATFORM_LIB_LINK_OPTIONS) $(OBJS) -o $@ $(LIBRARY_SEARCH_PATHS) $(myLIBRARY_NAMES) $(EXTRA_LINK_OPTIONS)

$(EXEC)/proc2iso.xsd: bin/proc2iso.xsd
	cp -p $< $@

define OBJECT_RULE
$$(call OBJECTS,$(1)): $(1) force | $(OBJ_DIR)
	$(CC) $(INCLUDES) $(CMP) -o $$@ $$<
endef

$(foreach SRC,$(SOURCES),$(eval $(call OBJECT_RULE,$(SRC))))

clean:
	-rm -f $(OBJS)
	-rmdir $(OBJ_DIR)

distclean: clean 
	-rm -f $(EXEC)/$(APP_NAME)
