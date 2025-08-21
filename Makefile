CC 	    := cc
SOURCES := $(wildcard src/*.c)
OBJECTS := ${SOURCES:.c=.o}
CFLAGS 	:= -Wall -Wextra -std=c99

DFLAGS 	:= -DDEBUG -g
RFLAGS 	:= -O2

BIN     := program
I_PATH  := include
L_PATH  := lib
B_PATH  := bin
ASSETS  := resources

DIR_DEBUG 	:= ${B_PATH}/debug
DIR_RELEASE := ${B_PATH}/release

FRMK := $(addprefix -framework ,CoreVideo IOKit Cocoa GLUT OpenGL)
LIBS := toml.a
DPCS := ${FRMK} -lraylib $(addprefix ${L_PATH}/,${LIBS})

DBG := ${DIR_DEBUG}/${BIN}
RLS := ${DIR_RELEASE}/${BIN}

release: CFLAGS += ${RFLAGS}
debug: CFLAGS += ${DFLAGS}

%.o: %.c
	${CC} ${CFLAGS} -I${I_PATH} -o $@ -c $<

${DIR_DEBUG} ${DIR_RELEASE}:
	mkdir -p $@

release: ${OBJECTS} | ${DIR_RELEASE}
	${CC} ${CFLAGS} ${OBJECTS} -o ${RLS} ${DPCS}

debug: ${OBJECTS} | ${DIR_DEBUG}
	${CC} ${CFLAGS} ${OBJECTS} -o ${DBG} ${DPCS}
	cp -r ${ASSETS} config.toml ${DIR_DEBUG}/

clean:
	${RM} -r ${OBJECTS} ${DIR_DEBUG}/* ${DIR_RELEASE}/*

.PHONY: release debug clean
