LIBNAME =             dexh1
STATIC_LIB :=         lib${LIBNAME}.a
SHARED_LIB :=         lib${LIBNAME}.so

SRC :=                $(wildcard *.c)
SRC_HEADERS :=        $(wildcard *.h)
OBJS :=               $(patsubst %.c, %.o, ${SRC})

PREFIX :=             /usr/local

BUILD_LIB_PATH :=     build/lib
BUILD_HEADERS_PATH := build/include

CFLAGS +=             -O3 -s
SAFEFLAGS :=          -Wall -Werror -pedantic -std=iso9899:1990


all: ${STATIC_LIB} ${SHARED_LIB} ${SRC_HEADERS}

install: ${STATIC_LIB} ${SHARED_LIB}
	mkdir -p ${PREFIX}/include/${LIBNAME}

	cp -r ${BUILD_LIB_PATH}/${STATIC_LIB} ${BUILD_LIB_PATH}/${SHARED_LIB} ${PREFIX}/lib/
	cp -r ${SRC_HEADERS} ${PREFIX}/include/${LIBNAME}

uninstall:
	rm -rf ${PREFIX}/lib/${STATIC_LIB}
	rm -rf ${PREFIX}/lib/${SHARED_LIB}
	
	rm -rf ${PREFIX}/include/${LIBNAME}

${SRC_HEADERS}:
	mkdir -p ${BUILD_HEADERS_PATH}
	cp -r ${SRC_HEADERS} ${BUILD_HEADERS_PATH}

${STATIC_LIB}: ${OBJS}
	mkdir -p ${BUILD_LIB_PATH}

	ar rcs ${BUILD_LIB_PATH}/$@ ${OBJS}

	rm -rf *.o

%.o: %.c
	${CC} ${LDFLAGS} ${CFLAGS} ${SAFEFLAGS} -static -c $< -o $@

${SHARED_LIB}: ${SRC}
	mkdir -p ${BUILD_LIB_PATH}
	
	${CC} ${LDFLAGS} ${CFLAGS} ${SAFEFLAGS} -shared -fPIC -o ${BUILD_LIB_PATH}/$@ $^

clean:
	rm -rf *.o ./build

.PHONY: all clean install uninstall
