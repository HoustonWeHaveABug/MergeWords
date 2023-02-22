# MergeWords

Solution to Daily Programmer Idea https://www.reddit.com/r/dailyprogrammer_ideas/comments/axeqqn/intermediate_two_words_collide/ (bonus part).

The program finds the longest word in a dictionary formed from n merged words, keeping the letters of these words in same order.

The program takes 2 arguments on the command line:

- Number of words to merge (n >= 2)

- Path to dictionary (list of words containing only letters, all converted to lowercase by the program).

The program creates a trie from the dictionary and in each node, it stores the minimal and maximal number of letters that can be added to the current word. Then a backtracking is executed on the trie (this structure allows a lot of pruning to be done). Any new optimal word found is printed on the fly. If a new word is found and has the same length as the current optimum, it becomes the new optimum if the minimum length of the merged words is greater than the one of the current optimum.

Running times using dictionary https://raw.githubusercontent.com/dolph/dictionary/master/enable1.txt (including the creation of the trie):

n=2: 0.3s

n=3: 0.3s

n=4: 0.4s

n=5: 1.5s

n=6: 30.0s

n=7: 1m31s
