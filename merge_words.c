#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define WORDS_N_MIN 2
#define BUFFER_SIZE 65536

typedef struct letter_s letter_t;
typedef struct node_s node_t;

struct letter_s {
	int symbol;
	node_t *next;
};

struct node_s {
	int len_max;
	int len_min;
	int letters_n;
	letter_t *letters;
};

typedef struct {
	int *symbols;
	int symbols_size;
	int symbols_n;
	node_t *node;
	int duplicate;
}
word_t;

typedef struct {
	word_t *word;
	int symbol;
}
choice_t;

static node_t *new_node(void);
static letter_t *new_letter(node_t *, int);
static void set_letter(letter_t *, int);
static void sort_node(node_t *);
static int compare_letters(const void *, const void *);
static int set_word(word_t *);
static void merge_words(int, node_t *);
static void print_solution(void);
static void choose_symbol(int, node_t *, word_t *);
static int add_choice(int, word_t *, int);
static void set_choice(choice_t *, word_t *, int);
static int add_symbol(word_t *, int);
static void free_words(int);
static void free_node(node_t *);

static int words_n, choices_size, choices_max, symbols_min;
static node_t *node_root;
static word_t *words;
static choice_t *choices;

int main(int argc, char *argv[]) {
	char *end, buffer[BUFFER_SIZE+1];
	int i;
	FILE *fd;
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <number of words> <path_to_dictionary>\n", argv[0]);
		fflush(stderr);
		return EXIT_FAILURE;
	}
	words_n = (int)strtol(argv[1], &end, 10);
	if (*end || words_n < WORDS_N_MIN) {
		fprintf(stderr, "Invalid number of words\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	node_root = new_node();
	if (!node_root) {
		return EXIT_FAILURE;
	}
	fd = fopen(argv[2], "r");
	if (!fd) {
		fprintf(stderr, "Could not open dictionary\n");
		fflush(stderr);
		free_node(node_root);
		return EXIT_FAILURE;
	}
	while (fgets(buffer, BUFFER_SIZE+1, fd)) {
		node_t *node = node_root;
		for (i = 0; isalpha((int)buffer[i]); ++i);
		if (i == 0 || buffer[i] != '\n') {
			fprintf(stderr, "Invalid word in dictionary\n");
			fflush(stderr);
			fclose(fd);
			free_node(node_root);
			return EXIT_FAILURE;
		}
		buffer[i] = '\0';
		for (i = 0; buffer[i]; ++i) {
			int symbol = tolower((int)buffer[i]), j;
			letter_t *letter;
			for (j = 0; j < node->letters_n && node->letters[j].symbol != symbol; ++j);
			if (j < node->letters_n) {
				letter = node->letters+j;
			}
			else {
				letter = new_letter(node, symbol);
				if (!letter) {
					fclose(fd);
					free_node(node_root);
					return EXIT_FAILURE;
				}
			}
			node = letter->next;
			if (!node) {
				node = new_node();
				if (!node) {
					fclose(fd);
					free_node(node_root);
					return EXIT_FAILURE;
				}
				letter->next = node;
			}
		}
		for (i = 0; i < node->letters_n && node->letters[i].symbol != '\n'; ++i);
		if (i == node->letters_n && !new_letter(node, '\n')) {
			fclose(fd);
			free_node(node_root);
			return EXIT_FAILURE;
		}
	}
	fclose(fd);
	sort_node(node_root);
	words = malloc(sizeof(word_t)*(size_t)words_n);
	if (!words) {
		fprintf(stderr, "Could not allocate memory for words\n");
		fflush(stderr);
		free_node(node_root);
		return EXIT_FAILURE;
	}
	for (i = words_n; i--; ) {
		if (!set_word(words+i)) {
			free_words(i);
			free_node(node_root);
			return EXIT_FAILURE;
		}
	}
	choices = malloc(sizeof(choice_t));
	if (!choices) {
		fprintf(stderr, "Could not allocate memory for choices\n");
		fflush(stderr);
		free_words(words_n);
		free_node(node_root);
		return EXIT_FAILURE;
	}
	choices_size = 1;
	add_choice(0, NULL, '\n');
	choices_max = 0;
	symbols_min = 0;
	merge_words(1, node_root);
	free(choices);
	free_words(words_n);
	free_node(node_root);
	return EXIT_SUCCESS;
}

static node_t *new_node(void) {
	node_t *node = malloc(sizeof(node_t));
	if (!node) {
		fprintf(stderr, "Could not allocate memory for node\n");
		fflush(stderr);
		return NULL;
	}
	node->letters_n = 0;
	node->letters = NULL;
	return node;
}

static letter_t *new_letter(node_t *node, int symbol) {
	if (node->letters_n > 0) {
		letter_t *letters = realloc(node->letters, sizeof(letter_t)*(size_t)(node->letters_n+1));
		if (!letters) {
			fprintf(stderr, "Could not reallocate memory for letters\n");
			fflush(stderr);
			free(node->letters);
			node->letters_n = 0;
			return NULL;
		}
		node->letters = letters;
	}
	else {
		node->letters = malloc(sizeof(letter_t));
		if (!node->letters) {
			fprintf(stderr, "Could not allocate memory for letters\n");
			fflush(stderr);
			return NULL;
		}
	}
	set_letter(node->letters+node->letters_n, symbol);
	++node->letters_n;
	return node->letters+node->letters_n-1;
}

static void set_letter(letter_t *letter, int symbol) {
	letter->symbol = symbol;
	letter->next = NULL;
}

static void sort_node(node_t *node) {
	node->len_max = 0;
	if (node->letters_n > 0) {
		int i;
		node->len_min = BUFFER_SIZE;
		qsort(node->letters, (size_t)node->letters_n, sizeof(letter_t), compare_letters);
		for (i = node->letters_n; i--; ) {
			if (node->letters[i].next) {
				sort_node(node->letters[i].next);
				if (node->letters[i].next->len_max > node->len_max) {
					node->len_max = node->letters[i].next->len_max;
				}
				if (node->letters[i].next->len_min < node->len_min) {
					node->len_min = node->letters[i].next->len_min;
				}
			}
		}
		if (node->letters_n > 1 || node->letters[0].symbol != '\n') {
			++node->len_max;
		}
		node->len_min = node->letters[0].symbol != '\n' ? node->len_min+1:0;
	}
	else {
		node->len_min = 0;
	}
}

static int compare_letters(const void *a, const void *b) {
	const letter_t *letter_a = (const letter_t *)a, *letter_b = (const letter_t *)b;
	return letter_a->symbol-letter_b->symbol;
}

static int set_word(word_t *word) {
	word->symbols = malloc(sizeof(int));
	if (!word->symbols) {
		fprintf(stderr, "Could not allocate memory for word->symbols\n");
		fflush(stderr);
		return 0;
	}
	word->symbols_size = 1;
	word->symbols_n = 0;
	word->node = node_root;
	return 1;
}

static void merge_words(int choices_n, node_t *node) {
	int len_max_sum, len_min_sum, i;
	if (choices_n+node->len_max < choices_max) {
		return;
	}
	len_max_sum = words[0].node->len_max;
	len_min_sum = words[0].node->len_min;
	for (i = 1; i < words_n; ++i) {
		int j;
		for (j = 0; j < i && words[j].node != words[i].node; ++j);
		words[i].duplicate = j < i;
		if (words[i].duplicate && words[i].node->letters_n == 1 && words[i].node->letters[0].symbol == '\n') {
			return;
		}
		len_max_sum += words[i].node->len_max;
		len_min_sum += words[i].node->len_min;
	}
	if (len_min_sum > node->len_max || choices_n+len_max_sum < choices_max) {
		return;
	}
	if (choices_n+node->len_max == choices_max) {
		int symbols_hi = words[0].symbols_n+words[0].node->len_max, symbols_sum_lo;
		if (symbols_hi <= symbols_min) {
			return;
		}
		if (words[0].symbols_n+words[0].node->len_min <= symbols_min) {
			symbols_sum_lo = symbols_min+1;
		}
		else {
			symbols_sum_lo = words[0].symbols_n+words[0].node->len_min;
		}
		for (i = 1; i < words_n; ++i) {
			if (words[i].symbols_n+words[i].node->len_max < symbols_hi) {
				symbols_hi = words[i].symbols_n+words[i].node->len_max;
				if (symbols_hi <= symbols_min) {
					return;
				}
			}
			if (words[i].symbols_n+words[i].node->len_min <= symbols_min) {
				symbols_sum_lo += symbols_min+1;
			}
			else {
				symbols_sum_lo += words[i].symbols_n+words[i].node->len_min;
			}
		}
		if (symbols_sum_lo >= choices_max) {
			return;
		}
	}
	if (choices_n >= choices_max && node->letters[0].symbol == '\n' && words[0].node->letters[0].symbol == '\n') {
		int symbols_lo = words[0].symbols_n;
		for (i = 1; i < words_n && !words[i].duplicate && words[i].node->letters[0].symbol == '\n'; ++i) {
			if (words[i].symbols_n < symbols_lo) {
				symbols_lo = words[i].symbols_n;
			}
		}
		if (i == words_n) {
			if (choices_n > choices_max) {
				choices_max = choices_n;
				symbols_min = symbols_lo;
				print_solution();
			}
			else {
				if (symbols_lo > symbols_min) {
					symbols_min = symbols_lo;
					print_solution();
				}
			}
		}
	}
	choose_symbol(choices_n, node, words);
	for (i = 1; i < words_n; ++i) {
		if (!words[i].duplicate) {
			choose_symbol(choices_n, node, words+i);
		}
	}
}

static void print_solution(void) {
	int i;
	printf("Total Length %d\nMinimum Length %d\n", choices_max-1, symbols_min);
	for (i = 1; i < choices_max; ++i) {
		putchar(choices[i].symbol);
	}
	puts("");
	for (i = 0; i < words_n; ++i) {
		int j;
		for (j = 1; j < choices_max; ++j) {
			putchar(choices[j].word == words+i ? '*':'.');
		}
		putchar(' ');
		for (j = 0; j < words[i].symbols_n; ++j) {
			putchar(words[i].symbols[j]);
		}
		puts("");
	}
	fflush(stdout);
}

static void choose_symbol(int choices_n, node_t *node, word_t *word) {
	int letters_idx_min = word->node->letters[0].symbol == '\n' ? 1:0, letters_idx = 0, i;
	for (i = letters_idx_min; i < word->node->letters_n; ++i) {
		while (letters_idx < node->letters_n && node->letters[letters_idx].symbol < word->node->letters[i].symbol) {
			++letters_idx;
		}
		if (letters_idx < node->letters_n && node->letters[letters_idx].symbol == word->node->letters[i].symbol && add_choice(choices_n, word, node->letters[letters_idx].symbol) && add_symbol(word, node->letters[letters_idx].symbol)) {
			node_t *word_node = word->node;
			word->node = word->node->letters[i].next;
			merge_words(choices_n+1, node->letters[letters_idx].next);
			word->node = word_node;
			--word->symbols_n;
		}
	}
}

static int add_choice(int choices_n, word_t *word, int symbol) {
	if (choices_n == choices_size) {
		choice_t *choices_tmp = realloc(choices, sizeof(choice_t)*(size_t)(choices_size+1));
		if (!choices_tmp) {
			fprintf(stderr, "Could not reallocate memory for choices\n");
			fflush(stderr);
			return 0;
		}
		choices = choices_tmp;
		++choices_size;
	}
	set_choice(choices+choices_n, word, symbol);
	return 1;
}

static void set_choice(choice_t *choice, word_t *word, int symbol) {
	choice->word = word;
	choice->symbol = symbol;
}

static int add_symbol(word_t *word, int symbol) {
	if (word->symbols_n == word->symbols_size) {
		int *symbols = realloc(word->symbols, sizeof(int)*(size_t)(word->symbols_size+1));
		if (!symbols) {
			fprintf(stderr, "Could not reallocate memory for word->symbols\n");
			fflush(stderr);
			return 0;
		}
		word->symbols = symbols;
		++word->symbols_size;
	}
	word->symbols[word->symbols_n++] = symbol;
	return 1;
}

static void free_words(int words_max) {
	int i;
	for (i = words_max; i--; ) {
		free(words[i].symbols);
	}
	free(words);
}

static void free_node(node_t *node) {
	if (node->letters_n > 0) {
		int i;
		for (i = node->letters_n; i--; ) {
			if (node->letters[i].next) {
				free_node(node->letters[i].next);
			}
		}
		free(node->letters);
	}
	free(node);
}
