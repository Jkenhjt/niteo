LIBNAME           := niteo
STATIC_LIB        := lib${LIBNAME}.a
SHARED_LIB        := lib${LIBNAME}.so

SRC               := $(wildcard src/*.c)
SRC_HEADERS       := $(wildcard src/*.h)
OBJS              := $(patsubst %.c, %.o, ${SRC})

SAFEFLAGS         += -std=iso9899:2011 -Wall -Werror -pedantic
CFLAGS            += -O3 -s -ffast-math

ARFLAGS            = rcs

PREFIX            := /usr/local

BUILD_DIR         := build
BUILD_LIB_DIR     := ${BUILD_DIR}/lib
BUILD_HEADERS_DIR := ${BUILD_DIR}/include

DEPENDENCIES_FOLDERS := deps/dexh1

LIBS              := -ldexh1


all: deps ${STATIC_LIB} ${SHARED_LIB} ${SRC_HEADERS}

install: ${STATIC_LIB} ${SHARED_LIB} ${SRC_HEADERS}
	mkdir -p ${PREFIX}/include/${LIBNAME}

	cp -r ${BUILD_LIB_DIR}/${STATIC_LIB} ${PREFIX}/lib
	cp -r ${BUILD_LIB_DIR}/${SHARED_LIB} ${PREFIX}/lib

	cp -r $(addprefix ${BUILD_HEADERS_DIR}/, $(notdir ${SRC_HEADERS})) ${PREFIX}/include/${LIBNAME}

uninstall:
	rm -rf ${PREFIX}/lib/${STATIC_LIB}
	rm -rf ${PREFIX}/lib/${SHARED_LIB}
	
	rm -rf ${PREFIX}/include/${LIBNAME}

deps:
	${MAKE} -C ${DEPENDENCIES_FOLDERS}

${STATIC_LIB}: ${OBJS} ${BUILD_LIB_DIR}
	${AR} ${ARFLAGS} ${BUILD_LIB_DIR}/$@ ${OBJS}

	rm -rf ${OBJS}

${SRC_HEADERS}: ${BUILD_HEADERS_DIR}
	cp -r ${SRC_HEADERS} ${BUILD_HEADERS_DIR}

%.o: %.c
	${CC} ${LDFLAGS} ${CFLAGS} ${SAFEFLAGS} -static -c $< -o $@

${SHARED_LIB}: ${SRC} | ${BUILD_LIB_DIR}
	${CC} ${LDFLAGS} ${CFLAGS} ${SAFEFLAGS} -shared -fPIC $^ -o ${BUILD_LIB_DIR}/$@

${BUILD_LIB_DIR}: ${BUILD_DIR}
${BUILD_HEADERS_DIR}: ${BUILD_DIR}

${BUILD_DIR}:
	mkdir -p ${BUILD_DIR} ${BUILD_LIB_DIR} ${BUILD_HEADERS_DIR}

clean:
	rm -rf ${OBJS} ${BUILD_DIR}
	${MAKE} -C ${DEPENDENCIES_FOLDERS} clean

.PHONY: all install uninstall clean deps
