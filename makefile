EXEC_FILE = main
C_FILES = main.c reading.c data_parsing.c code_parsing.c coding.c other_parsing.c structs_methods.c
H_FILES = assembler.h

O_FILES = $(C_FILES:.c=.o)

all: $(EXEC_FILE)
$(EXEC_FILE): $(O_FILES) 
	gcc -Wall -ansi -pedantic $(O_FILES) -o $(EXEC_FILE) 
%.o: %.c $(H_FILES)
	gcc -Wall -ansi -pedantic -c -o $@ $<
clean:
	rm -f *.o $(EXEC_FILE)