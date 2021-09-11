CC=gcc
GCCFLAGS = -g -Wall -std=c99 -Wextra
GCCLIBS = -lrt -lpthread
EXT_FILES =  src/errors.c 
MASTER = src/master src/view src/slave


all: $(MASTER)

$(MASTER): %: %.c
	@$(CC) $(GCCFLAGS) $(EXT_FILES) -o $@ $< $(GCCLIBS)

.PHONY: clean
clean:
	@rm -rf $(MASTER)