all: assembler

assembler: assembler.o pre_assembler.o utils.o data_struct.o errors.o first_pass.o tables.o
	gcc -ansi -Wall -pedantic -o assembler assembler.o pre_assembler.o utils.o data_struct.o errors.o first_pass.o tables.o

assembler.o: assembler.c pre_assembler.h first_pass.h data_struct.h
	gcc -ansi -Wall -pedantic -c assembler.c -o assembler.o

pre_assembler.o: pre_assembler.c pre_assembler.h utils.h data_struct.h errors.h
	gcc -ansi -Wall -pedantic -c pre_assembler.c -o pre_assembler.o

utils.o: utils.c utils.h errors.h data_struct.h
	gcc -ansi -Wall -pedantic -c utils.c -o utils.o

data_struct.o: data_struct.c data_struct.h utils.h errors.h
	gcc -ansi -Wall -pedantic -c data_struct.c -o data_struct.o

errors.o: errors.c errors.h
	gcc -ansi -Wall -pedantic -c errors.c -o errors.o

first_pass.o: first_pass.c first_pass.h utils.h errors.h tables.h
	gcc -ansi -Wall -pedantic -c first_pass.c -o first_pass.o

tables.o: tables.c tables.h errors.h
	gcc -ansi -Wall -pedantic -c tables.c -o tables.o

clean:
	rm *.o *~ assembler *.am