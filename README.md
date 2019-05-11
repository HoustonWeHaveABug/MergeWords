# MergeWords

Solution to Daily Programmer Idea https://www.reddit.com/r/dailyprogrammer_ideas/comments/axeqqn/intermediate_two_words_collide/ (bonus part).

The program finds the longest word in a dictionary formed from n merged words, keeping the letters of these words in same order. At least d letters of each merged words must be alterned with letters from other words (d is called below the minimal merging degree). If d is equal to 1 then merged words can simply be concatenated.

The program takes 3 arguments on command line:

- Number of words to merge (>= 2)

- Minimal merging degree (>= 1)

- Path to dictionary (list of words containing only letters, all converted to lowercase by the program).

The program creates a trie from the dictionary and in each node, stores the maximal number of letters that can be added to the current word. Then a backtracking is executed on the trie (this structure allows a lot of pruning to be done). Any new optimal (or tied with optimal) word found is printed on the fly. If a new word is found and has the same length as the current optimum, it becomes the new optimum if the total merging degree of the merged words is greater than the one of the current optimum.

Running times for minimal merging degree = 2 using dictionary https://raw.githubusercontent.com/dolph/dictionary/master/enable1.txt (including the creation of the trie):

n=2: 0.3s

n=3: 0.3s

n=4: 1.1s

n=5: 2m31s

n=6: 8h09m
