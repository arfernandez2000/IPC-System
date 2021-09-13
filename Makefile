CC=gcc
GCCFLAGS = -g -Wall -std=c99 -Wextra
GCCLIBS = -lrt -lpthread
EXT_FILES =  src/errors.c 
MASTER = master view slave


all: $(MASTER)

$(MASTER): %: src/%.c
	@$(CC) $(GCCFLAGS) $(EXT_FILES) -o $@ $< $(GCCLIBS)

check:
	cppcheck --quiet --enable=all --force --inconclusive src
pvs:
	pvs-studio-analyzer trace -- make
	pvs-studio-analyzer analyze
	plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log
format: 
	clang-format -style=file --sort-includes --Werror -i ./src/*.c ./src/*.h

clean:
	@rm -rf $(MASTER) results.txt
.PHONY: all check pvs clean format
