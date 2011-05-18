# This Makefile is based on the Apache Xerces-Makefile
# find the original licensing information below here:

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

###################################################################
#                    IMPORTANT NOTE                               #
###################################################################
# If you are going to do the OS390BATCH build, make sure you have #
# the OS390BATCH environment variable set.                        #
#                                                                 #
#   export OS390BATCH=1                                           #
#                                                                 #
###################################################################

XERCESCROOT=/usr/include/xercesc
LD_LIBRARY_PATH=/usr/lib/xercesc/

PLATFORM = LINUX
COMPILER = g++
GCC = yes
GXX = yes
STRIP = strip
CP = cp
CXXFLAGS = -g -O2 -Wall -DAPP_NO_THREADS -DXML_USE_NO_THREADS -DUSE_FREE_IMAGE_LIB -fno-builtin -Wshadow -Wcast-qual -Wcast-align -Woverloaded-virtual -Wpointer-arith
CFLAGS   = -g -O2 -Wall -DAPP_NO_THREADS -DXML_USE_NO_THREADS -DUSE_FREE_IMAGE_LIB -fno-builtin -Wshadow -Wcast-qual -Wcast-align -Woverloaded-virtual -Wpointer-arith
LDFLAGS =
THREADS = none
EXTRA_LIBS =
PREFIX = /usr

include version.incl
include Makefile.incl

APP_NAME=vt2iso

OUTDIR = obj
OBJS = ${OUTDIR}/vt2iso.o ${OUTDIR}/vt2isoimagebase_c.o ${OUTDIR}/vt2isoimagefreeimage_c.o ${OUTDIR}/vt2iso-defines.o ${OUTDIR}/vt2iso-globals.o
myLIBRARY_NAMES = ${LIBRARY_NAMES} ../libs/freeimage/libfreeimage.a -lpthread
SRC = src

all: makedir ${APP_NAME} 

makedir:
	-mkdir -p $(OUTDIR)

${APP_NAME}: ${OBJS}
	${LINK} ${PLATFORM_LIB_LINK_OPTIONS} ${OBJS} -o $@ ${LIBRARY_SEARCH_PATHS} ${myLIBRARY_NAMES} ${EXTRA_LINK_OPTIONS}
	${STRIP} ${APP_NAME}

$(OUTDIR)/vt2iso.o: ${SRC}/vt2iso.cpp ${SRC}/vt2isoimagebase_c.h ${SRC}/vt2isoimagefreeimage_c.h
	${CC} ${INCLUDES} ${CMP} -o $(OUTDIR)/vt2iso.o ${SRC}/vt2iso.cpp

$(OUTDIR)/vt2isoimagebase_c.o: ${SRC}/vt2isoimagebase_c.h ${SRC}/vt2isoimagebase_c.cpp
	${CC} ${INCLUDES} ${CMP} -o $(OUTDIR)/vt2isoimagebase_c.o ${SRC}/vt2isoimagebase_c.cpp

$(OUTDIR)/vt2isoimagefreeimage_c.o: ${SRC}/vt2isoimagebase_c.h ${SRC}/vt2isoimagefreeimage_c.cpp ${SRC}/vt2isoimagefreeimage_c.h
	${CC} ${INCLUDES} ${CMP} -o $(OUTDIR)/vt2isoimagefreeimage_c.o ${SRC}/vt2isoimagefreeimage_c.cpp

${OUTDIR}/vt2iso-defines.o: ${SRC}/vt2iso-defines.hpp ${SRC}/vt2iso-defines.cpp
	${CC} ${INCLUDES} ${CMP} -o $(OUTDIR)/vt2iso-defines.o ${SRC}/vt2iso-defines.cpp

${OUTDIR}/vt2iso-globals.o: ${SRC}/vt2iso-globals.hpp ${SRC}/vt2iso-globals.cpp
	${CC} ${INCLUDES} ${CMP} -o $(OUTDIR)/vt2iso-globals.o ${SRC}/vt2iso-globals.cpp

clean:
	rm -f ${OBJS} ${APP_NAME}

distclean:	clean
	rm -f Makefile

