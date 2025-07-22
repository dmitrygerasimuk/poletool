# Makefile for poletool (dictionary converter)

CC = gcc
SRC = poletool.c
OUT = poletool

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    ICONV_FLAG = -liconv
else
    ICONV_FLAG =
endif

CFLAGS = -Wall -O2

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC) $(ICONV_FLAG)

clean:
	rm -f $(OUT)