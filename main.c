#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define word_size 20

typedef struct kp {     /* key pair */
    unsigned long key;      /* 8 byte unsigned long which will store key value */
    char password[20];         /* character array used to store the plaintext password */
} kp;

typedef struct tnode {
    int n;                   /* the number of keys currently stored in this node */
    struct kp *keys;         /* pointer to data containing the keys, should be initialized with malloc */
    char **children;         /* pointer to data containing pointers to children */
                             /* these will be paths to files containing particular nodes, which will have to be opened
                             /* and loaded into memory and cast to tnode type */
    int leaf;                /* 1 == current node is a leaf node, 0 otherwise*/
} tnode;

/* we need a data type to store the key and a pointer to the 
sateilite data (plaintext password) associated with key */

/* instantiates a new node */
struct tnode *allocate_node(int keys, int leaf) {
    tnode *ret = malloc (sizeof (tnode));
    if (ret == NULL)
        return NULL;

    ret->n = keys;
    ret->keys = malloc (keys * sizeof(kp));
    ret->children = malloc (keys * sizeof(char *));
    for(int i = 0; i < keys; i++) {
        /* call malloc once for ever key to create space for a path to a child node */
        ret->children[i] = malloc (word_size * sizeof(char));
    }
    
    ret->leaf = leaf;

    /* check to ensure that malloc was successful*/
    /* could lead to memory leaks, check each one individually */
    if (ret->keys == NULL || ret->children == NULL ) {
        free (ret);
        return NULL;
    }

    return ret;
}

// free a tnode from memory
void free_tnode(tnode *node) {
    int n = node->n;
    for(int i = 0; i < n; i++) {
        /* free all data allocated to array elements */
        free(node->children[i]);
    }
    free(node->keys);
    free(node->children); // error here after reading the node from file
    free(node);
}

/* write a node to a file */
void write_tnode(tnode *node, char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file != NULL) {
        // write the struct itself to the file
        fwrite(node, sizeof(tnode), 1, file);

        // write all of the keypairs to the file
        fwrite(node->keys, node->n * sizeof(kp), 1, file);
        
        // write the children data to the file
        fwrite(node->children, node->n * sizeof(char *), 1, file);

        // write each string stored in children to the file
        for(int i = 0; i < node->n; i++) {
            fwrite(node->children[i], word_size * sizeof(char), 1, file);
        }

        fclose(file);
    }
}
// strcpy(object->day,"Good day");

/* read a node from a file */
tnode *read_tnode(char *filename) {
    tnode *node = malloc(sizeof(tnode));
    FILE *file = fopen(filename, "rb");
    printf("here");
    if (file != NULL) {
        printf("here");
        fread(node, sizeof(tnode), 1, file);


        // read all of the keypairs to the node
        node->keys = malloc(node->n * sizeof(kp));
        fread(node->keys, node->n * sizeof(kp), 1, file);
        
        // read the children data to the node
        node->children = malloc(node->n * sizeof(char *));
        fread(node->children, node->n * sizeof(char *), 1, file);

        // write each string stored in children to the file
        // issue with this segment, at the 7th iteration blacking out and seg fault
        for(int i = 0; i < node->n; i++) {
            node->children[i] = malloc(word_size * sizeof(char));
            fread(node->children[i], word_size * sizeof(char), 1, file);
        }

        fclose(file);
    }
    return node;
}


int main() {
    printf("Running!\n");
    // FILE *file = fopen("test", "wb");
    // if (file != NULL) {
    //     fwrite("test0", 5, 1, file);
    //     fwrite("test1", 5, 1, file);
    //     fwrite("test2", 5, 1, file);
    //     fclose(file);
    // }

    // let's create a test tnode with some random fields filled in
    tnode *node = allocate_node(20, 0);
    for(int i = 0; i < node->n; i++) {
        node->keys[i].key = i;
        strcpy(node->keys[i].password, "test123456");
    }

    for(int i = 0; i < node->n; i++) {
        strcpy(node->children[i], "test_1");
    }


    write_tnode(node, "test2.txt");
    free_tnode(node);

    tnode *new_node = read_tnode("test2.txt");

    //printf("node keys %i", new_node->n);

    // printf("contents of first key: %ld, %s, contents of second key: %ld, %s \n", 
    //     new_node->keys[0].key, new_node->keys[0].password, 
    //     new_node->keys[1].key, new_node->keys[1].password); 

    free_tnode(new_node);

    return 0;
}