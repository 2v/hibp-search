#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "btree.h"
#include <sys/types.h>
#include "sha1.h"

const char *inf_key = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";

/* instantiates a new node */
tnode *allocate_node(int keys, int leaf) {
    tnode *ret = malloc (sizeof (tnode));
    if (ret == NULL)
        return NULL;

    ret->n = keys;
    ret->keys = malloc (keys * sizeof(kp));
    if (ret->keys == NULL) {
        free (ret);
        return NULL;
    }
    ret->children = malloc (keys * sizeof(char *));
    if (ret->children == NULL) {
        free(ret->keys);
        free (ret);
        return NULL;
    }
 
    for(int i = 0; i < keys; i++) {
        ret->children[i] = malloc (word_size * sizeof(char));
    }
    
    ret->leaf = leaf;

    return ret;
}

/* free a tnode from memory */
void free_tnode(tnode *node) {
    int n = node->n;

    /* free all data allocated to child array elements */
    for(int i = 0; i < n; i++) {
        char *child = node->children[i];
        free(child);
    }

    free(node->keys);
    free(node->children);
    free(node);
}

/* write a node to a file */
void write_tnode(tnode *node, char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file != NULL) {
        /* write the struct itself to the file */
        fwrite(node, sizeof(tnode), 1, file);

        /* write all of the keypairs to the file */
        fwrite(node->keys, node->n * sizeof(kp), 1, file);
        
        /* write the children data to the file */
        fwrite(node->children, node->n * sizeof(char *), 1, file);

        /* write each string stored in children to the file */
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

        /* read all of the keypairs to the node */
        node->keys = malloc(node->n * sizeof(kp));
        fread(node->keys, node->n * sizeof(kp), 1, file);
        
        /* read the children data to the node */
        node->children = malloc(node->n * sizeof(char *));
        fread(node->children, node->n * sizeof(char *), 1, file);

        /* write each string stored in children to the file */
        for(int i = 0; i < node->n; i++) {
            node->children[i] = malloc(word_size * sizeof(char));
            fread(node->children[i], word_size * sizeof(char), 1, file);
        }

        fclose(file);
    }
    return node;
}

/*  loads a key pair into a node at the given key_pos. 
    uses strtok to break up the given line into two tokens
    seperated by the given delimiter */
void load_key_into_node(tnode *node, char *line, char *delimiter, int key_pos) {
    char *key = strtok(line, delimiter);
    if (key == NULL)
        return;

    char *occurence_string = strtok(NULL, delimiter); 
    if (occurence_string == NULL)
        return;

    int occurence = atoi(occurence_string);

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

    /* routine to fill the B-tree, each node being of size node_size */
    tnode *root = allocate_node(node_size, 0);
    for(int i = 0; i < node_size; i++) {
        float p = (float)i/(float)node_size*100;
        printf("Progress %.3f%%\n", p);
        tnode *c = allocate_node(node_size, 0);
        for(int j = 0; j < node_size; j++) {

            tnode *cc = allocate_node(node_size, 1);
            for(int k = 0; k < node_size; k++) {

                if ((read = getline(&line, &len, file)) != -1) {
                    load_key_into_node(cc, line, ":", k);
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
                load_key_into_node(c, line, ":", j);
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
            load_key_into_node(root, line, ":", i);
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

/* search the b-tree starting from root node x and find key pair from key */
kp *search_b_tree(tnode *root, char key[41]) {
    kp *ret = malloc(sizeof(kp));

    int i = 0;
    while(i < root->n && strcmp(key, root->keys[i].key) > 0 
            && strcmp(root->keys[i].key, inf_key) != 0) {
        i = i + 1;
    }

    if(i < root->n && strcmp(key, root->keys[i].key) == 0) {
        strcpy(ret->key, root->keys[i].key);
        ret->occurences = root->keys[i].occurences;
        return ret;
   } else if(root->leaf) {
        strcpy(ret->key, "NOT FOUND");
        ret->occurences = -1;
        return ret;
    } else {
        free(ret);
        tnode *child = read_tnode(root->children[i]);
        search_b_tree_helper(child, key);
    }
}


/* only difference between search_b_tree is that child argument is always freed before recursive step*/
kp *search_b_tree_helper(tnode *child, char key[41]) {
    kp *ret = malloc(sizeof(kp));

    int i = 0;
    while(i < child->n && strcmp(key, child->keys[i].key) > 0 
            && strcmp(child->keys[i].key, inf_key) != 0) {
        i = i + 1;
    }

    if(i < child->n && strcmp(key, child->keys[i].key) == 0) {
        strcpy(ret->key, child->keys[i].key);
        ret->occurences = child->keys[i].occurences;
        free_tnode(child);
        return ret;
   } else if(child->leaf) {
        strcpy(ret->key, "NOT FOUND");
        ret->occurences = -1;
        free_tnode(child);
        return ret;
    } else {
        tnode *c = read_tnode(child->children[i]);
        /* free the child node passed as argument since it will no longer be used */
        free_tnode(child);
        free(ret);
        search_b_tree_helper(c, key);
    }
}

