#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "btree.h"
#include <sys/types.h>
#include "sha1.h"


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

    return 0;
}
