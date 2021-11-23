#include <stdio.h>
#include <stdlib.h>

struct kp {     /* key pair */
    unsigned long key;      /* 8 byte unsigned long which will store key value */
    char* password;         /* character array used to store the plaintext password */
};

struct tnode {
    int n;                   /* the number of keys currently stored in this node */
    struct kp *keys;         /* pointer to data containing the keys, should be initialized with malloc */
    char **children;         /* pointer to data containing pointers to children */
                             /* these will be paths to files containing particular nodes, which will have to be opened
                             /* and loaded into memory and cast to tnode type */
    int leaf;                /* 1 == current node is a leaf node, 0 otherwise*/
};

/* we need a data type to store the key and a pointer to the 
sateilite data (plaintext password) associated with key */

/* instantiates a new node */
struct tnode *new_node(int keys, int leaf) {
    struct tnode *ret = malloc (sizeof (struct tnode));
    if (ret == NULL)
        return NULL;

    ret->n = keys;
    ret->keys = malloc (keys * sizeof (struct kp));
    ret->children = malloc (keys * sizeof(char *));
    ret->leaf = leaf;

    /* check to ensure that malloc was successful*/
    if (ret->keys == NULL || ret->children == NULL ) {
        free (ret);
        return NULL;
    }

    return ret;
}

/* gets a node from a file */
struct tnode *read_node_from_disc(char *path) {
}

/* write a node to a file */
struct tnode *write_node_to_disc(char *path) {
}

int main() {
    printf("Running!\n");

    return 0;
}