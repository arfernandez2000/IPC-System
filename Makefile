CC=gcc
GCCFLAGS = -g -Wall -std=c99 -Wextra
GCCLIBS = -lrt -lpthread
EXT_FILES =  src/errors.c 
MASTER = src/master src/view src/slave


all: $(MASTER)

$(MASTER): %: %.c
	@$(CC) $(GCCFLAGS) $(EXT_FILES) -o $@ $< $(GCCLIBS)

check:
	@cppcheck --quiet --enable=all --force --inconclusive src
pvs-studio:
	@pvs-studio-analyzer trace -- make
	@pvs-studio-analyzer analyze
	@plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log

.PHONY: clean
clean:
	@rm -rf $(MASTER)
