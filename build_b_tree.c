#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include "sha1.h"
#include "btree.h"
 
int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("build_b_tree requires a path to the HIBP file to parse. \nUsage: ./build_b_tree <colon_delimited_HIBP_database>\n");
        return 0;
    }
  
    /* check time taken to search for a hashed password */
    double t = 0.0;
    clock_t beg = clock();

    tnode *root = build_b_tree(argv[1], "dat");
    write_tnode(root, "dat/r.bin"); // must be written to dat/r.bin so that search method can find it
    free_tnode(root);

    clock_t end = clock();
    t += (double) (end - beg) / CLOCKS_PER_SEC;

    return 0;
}