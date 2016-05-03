ARCH=$(shell arch)
ifeq ($(ARCH),armv7l)
TARGET ?= rpi2
endif
TARGET ?= x86
CC ?= gcc

CFLAGS=-O2 -g -Wall -Werror -Wextra -pedantic -std=c99 -Ilib
LDFLAGS=-O2 -g

LT_CC:=libtool --tag=CC --mode=compile $(CC)
LT_CC_DEP:=$(CC)
LT_LD:=libtool --tag=CC --mode=link $(CC)

MKDIR=mkdir -p

CC=${LT_CC}
CC_DEP=${LT_CC_DEP}
LD=${LT_LD}




ifeq ($(TARGET),rpi2)
endif

ifeq ($(TARGET),x86)
endif

CFLAGS+= $(TARGETCFLAGS)
LDFLAGS+= $(TARGETCFLAGS)


LIB_DIR=lib
LIB_DEP_DIR=dep_lib
LIB_OBJ_DIR=obj_lib
SRC_DIR=src
SRC_DEP_DIR=dep_src
SRC_OBJ_DIR=obj_src

FS_LIB=$(wildcard ${LIB_DIR}/*.c)
FS_SRC=$(wildcard ${SRC_DIR}/*.c)
FS_OBJ_LIB=$(FS_LIB:${LIB_DIR}/%.c=${LIB_OBJ_DIR}/%.lo)
FS_OBJ_SRC=$(FS_SRC:${SRC_DIR}/%.c=${SRC_OBJ_DIR}/%.lo)
FS_OBJ=$(FS_OBJ_SRC) $(FS_OBJ_LIB)
FS_DEP_LIB=$(FS_LIB:${LIB_DIR}/%.c=${LIB_DEP_DIR}/%.d)
FS_DEP_SRC=$(FS_SRC:${SRC_DIR}/%.c=${SRC_DEP_DIR}/%.d)
FS_DEP=$(FS_DEP_SRC) $(FS_DEP_LIB)

.SUFFIXES: .c .d

.PHONY: all
all: library tools test

.PHONY: clean
clean::
	-find . -type f -name '*.a' -print -delete
	-find . -type f -name '*.d' -print -delete
	-find . -type f -name '*.o' -print -delete
	-find . -type f -name '*.la' -print -delete
	-find . -type f -name '*.lo' -print -delete
	-find . -type f -name '*.so' -print -delete
	-find . -type d -name '.libs' -print | xargs rm -rv
	-rm -rf bin/*

.PHONY: test
test: tools
	bin/sha1dcsum tools/test/*
	bin/sha1dcsum_partialcoll tools/test/*

.PHONY: tools
tools: sha1dcsum sha1dcsum_partialcoll

.PHONY: sha1dcsum
sha1dcsum: bin/sha1dcsum

.PHONE: sha1dcsum_partialcoll
sha1dcsum_partialcoll: bin/sha1dcsum
	cp bin/sha1dcsum bin/sha1dcsum_partialcoll
	
.PHONY: library
library: bin/libdetectcoll.la

bin/libdetectcoll.la: $(FS_OBJ_LIB)
	${LD} ${CFLAGS} $(FS_OBJ_LIB) -o bin/libdetectcoll.la

bin/sha1dcsum: $(FS_OBJ_SRC) library
	${LD} ${CFLAGS} $(FS_OBJ_SRC) $(FS_OBJ_LIB) -Lbin -ldetectcoll -o bin/sha1dcsum 


${SRC_DEP_DIR}/%.d: ${SRC_DIR}/%.c
	${MKDIR} $(shell dirname $@) && $(CC_DEP) $(CFLAGS) -M -MF $@ $<

${SRC_OBJ_DIR}/%.lo ${SRC_OBJ_DIR}/%.o: ${SRC_DIR}/%.c ${SRC_DEP_DIR}/%.d
	${MKDIR} $(shell dirname $@) && $(CC) $(CFLAGS) -o $@ -c $<


${LIB_DEP_DIR}/%.d: ${LIB_DIR}/%.c
	${MKDIR} $(shell dirname $@) && $(CC_DEP) $(CFLAGS) -M -MF $@ $<

${LIB_OBJ_DIR}/%.lo ${LIB_OBJ_DIR}/%.o: ${LIB_DIR}/%.c ${LIB_DEP_DIR}/%.d
	${MKDIR} $(shell dirname $@) && $(CC) $(CFLAGS) -o $@ -c $<

-include $(FS_DEP)
