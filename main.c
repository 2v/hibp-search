#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>
#include "btree.h"

int main() {
    printf("Running!\n");

    // let's create a test tnode with some random fields filled in
    tnode *node = allocate_node(100, 0);
    for(int i = 0; i < node->n; i++) {
        strcpy(node->keys[i].key, "00000000A8DAE4228F821FB418F59826079BF368");
        node->keys[i].occurences = node->n-i-1;
    }

    for(int i = 0; i < node->n; i++) {
        strcpy(node->children[i], "test_1");
    }


    write_tnode(node, "test2.txt");
    free_tnode(node);

    tnode *new_node = read_tnode("test2.txt");

    printf("node keys: %i\n", new_node->n);

    printf("contents of first key: %s, %i, \ncontents of second key: %s, %i, \ncontents of last key: %s, %i \n", 
        new_node->keys[0].key, new_node->keys[0].occurences, 
        new_node->keys[2].key, new_node->keys[2].occurences,
        new_node->keys[new_node->n-1].key, new_node->keys[new_node->n-1].occurences);

    free_tnode(new_node);

    // tnode *test_node = build_b_tree("pwned-passwords-sha1-ordered-by-hash-v7.txt");
    // write_tnode(test_node, "dat/r.txt");
    // free_tnode(test_node);

    /* check time taken to search for a hashed password */
    double time_spent = 0.0;

    clock_t beg = clock();

    tnode *test_node = read_tnode("dat/r.txt");

    kp *test = search_b_tree(test_node, "E8C76907C6F240F1A5AEB2C07EFD988839FB2C12");

    free_tnode(test_node);
    
    clock_t end = clock();
    time_spent += (double) (end - beg) / CLOCKS_PER_SEC;

    printf("key with value: %s and occurence: %i found in time %f\n", test->key, test->occurences, time_spent);
    free(test);

    return 0;
}