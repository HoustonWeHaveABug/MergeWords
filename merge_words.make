MERGE_WORDS_C_FLAGS=-O2 -std=c89 -Wpedantic -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings -Wswitch-default -Wswitch-enum -Wbad-function-cast -Wstrict-overflow=5 -Wundef -Wlogical-op -Wfloat-equal -Wold-style-definition

merge_words: merge_words.o
	gcc -o merge_words merge_words.o

merge_words.o: merge_words.c merge_words.make
	gcc -c ${MERGE_WORDS_C_FLAGS} -o merge_words.o merge_words.c

clean:
	rm -f merge_words merge_words.o
