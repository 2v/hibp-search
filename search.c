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
        printf("search requires a string argument to search for. \nUsage: ./search <plaintext_password>\n");
        return 0;
    }

    SHA1_CTX sha; uint8_t results[20]; char *buf; int n;

    n = strlen(argv[1]); SHA1Init(&sha); SHA1Update(&sha, (uint8_t *)argv[1], n); SHA1Final(results, &sha);

    char buffer[41];
    /* Print the digest as one long hex value */ 
    for (n = 0; n < 20; n++)
        sprintf(buffer + n*2, "%02x", results[n]);

    buffer[40] = '\0';

    /* convert string to uppercase */
    for (int i = 0; buffer[i] != '\0'; i++)
        buffer[i] = toupper(buffer[i]);
   
    /* check time taken to search for a hashed password */
    double t = 0.0;
    clock_t beg = clock();
    tnode *root = read_tnode("dat/r.txt");
    kp *result = search_b_tree(root, buffer); 
    free_tnode(root);
    clock_t end = clock();
    t += (double) (end - beg) / CLOCKS_PER_SEC;

    if(result->occurences <= 0) {
        printf("key not found in database!\n");
    } else {
        printf("key with value: %s and occurrence: %i found in time %f\n", result->key, result->occurences, t);
    }

    free(result);

    return 0;
}