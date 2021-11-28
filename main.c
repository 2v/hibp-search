#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define word_size 20
#define key_length 40

// let infinity string = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
const char *inf_key = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";

typedef struct kp {     /* key pair */
    char key[key_length + 1];      /* character array used to store each key of size key_length, add 1 for null termination */
    int occurences;            /* used to count the number of occurences of a password */
} kp;

typedef struct tnode {
    int n;                   /* the number of keys currently stored in this node */
    struct kp *keys;         /* pointer to data containing the keys, should be initialized with malloc */
    char **children;         /* pointer to data containing pointers to children */
                             /* these will be paths to files containing particular nodes, which will have to be opened
                             /* and loaded into memory and cast to tnode type */
    int leaf;                /* 1 == current node is a leaf node, 0 otherwise*/
} tnode;

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
    /* TODO: could lead to memory leaks, check each one individually */
    if (ret->keys == NULL || ret->children == NULL ) {
        free (ret);
        return NULL;
    }

    return ret;
}

/* free a tnode from memory */
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

/* read a node from a file */
tnode *read_tnode(char *filename) {
    tnode *node = malloc(sizeof(tnode));
    FILE *file = fopen(filename, "rb");
    if (file != NULL) {
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

void load_data_into_node(tnode *node, char *line, char *delimiter, int key_pos) {
    /* tokenizing the line into key and occurence pair */
    char *key = strtok(line, delimiter);
    char *occurence_string = strtok(NULL, delimiter); 
    int occurence = atoi(occurence_string);

    printf("key: %s, occurence: %i\n", key, occurence);

    strcpy(node->keys[key_pos].key, key);
    node->keys[key_pos].occurences = occurence;
}

/* takes the input file and parses it into a b_tree, returning the root */
tnode *build_b_tree(char *filename) {
    FILE *file = fopen(filename, "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if (file == NULL)
        exit(EXIT_FAILURE);

    int node_size = 46;

    /* routine to fill the B-tree, each node being of size node_size */
    tnode *root = allocate_node(node_size, 0);
    for(int i = 0; i < node_size; i++) {

        tnode *c = allocate_node(node_size, 0);
        for(int j = 0; j < node_size; j++) {

            tnode *cc = allocate_node(node_size, 1);
            for(int k = 0; k < node_size; k++) {

                if ((read = getline(&line, &len, file)) != -1) {
                    load_data_into_node(cc, line, ":", k);
                } else {
                    strcpy(cc->keys[k].key, inf_key);
                    cc->keys[k].occurences = 0;
                    break;
                }
            }

            /* once the node is full, write it to a file with an identifiable name */
            char *node_filename = malloc(13 * sizeof(char));
            sprintf(node_filename, "dat/c%ic%i.txt", i, j);
            write_tnode(cc, node_filename);
            strcpy(c->children[j], node_filename);
            free_tnode(cc);
            free(node_filename);

            if ((read = getline(&line, &len, file)) != -1) {
                load_data_into_node(c, line, ":", j);
            } else {
                strcpy(c->keys[j].key, inf_key);
                c->keys[j].occurences = 0;
                break;
            }
        }

        /* once the node is full, write it to a file with an identifiable name */
        char *node_filename = malloc(11 * sizeof(char));
        sprintf(node_filename, "dat/c%i.txt", i);
        write_tnode(c, node_filename);
        strcpy(root->children[i], node_filename);
        free_tnode(c);
        free(node_filename);

        if ((read = getline(&line, &len, file)) != -1) {
            load_data_into_node(root, line, ":", i);
        } else {
            strcpy(root->keys[i].key, inf_key);
            root->keys[i].occurences = 0;
            break;
        }
   }

    fclose(file);
    if (line)
        free(line);

    return root;
}

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

    tnode *test_node = build_b_tree("pwned-passwords-test.txt");


    free_tnode(test_node);

    return 0;
}