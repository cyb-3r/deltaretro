CC = cc
CFLAGS = -Wall -Wextra -std=c99
DFLAGS = -g
RFLAGS = -O2
NAME = program

INC_D = ./include
SRC_D = ./src
LIB_D = ./lib
OUT_D = ./bin

DBD = $(OUT_D)/debug
RLD = $(OUT_D)/release

LIB = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL $(LIB_D)/libraylib.a $(LIB_D)/toml.a
SRC = $(SRC_D)/*.c
DBG = $(DBD)/$(NAME)
RLS = $(RLD)/$(NAME)

all: release debug

$(DBD) $(RLD):
	mkdir -p $@

release: $(RLD)
	$(CC) $(CFLAGS) $(RFLAGS) $(SRC) -o $(RLS) $(LIB)

debug: $(DBD)
	$(CC) -DDEBUG $(CFLAGS) $(DFLAGS) $(SRC) -o $(DBG) $(LIB)

clean:
	rm -rf ./bin/*

.PHONY: all release debug clean
