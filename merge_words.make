MERGE_WORDS_C_FLAGS=-O2 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings

merge_words: merge_words.o
	gcc -o merge_words merge_words.o

merge_words.o: merge_words.c merge_words.make
	gcc -c ${MERGE_WORDS_C_FLAGS} -o merge_words.o merge_words.c

clean:
	rm -f merge_words merge_words.o
