#define word_size 20
#define key_length 40
#define node_size 850

typedef struct kp {     /* key pair */
    char key[key_length + 1];      /* character array used to store each key of size key_length, add 1 for null termination */
    int occurences;            /* used to count the number of occurences of a password */
} kp;

typedef struct tnode {  /* b-tree node */
    int n;                   /* the number of keys currently stored in this node */
    kp *keys;         /* pointer to data containing the keys, should be initialized with malloc */
    char **children;         /* pointer to data containing pointers to children */
                             /* these will be paths to files containing particular nodes, which will have to be opened */
                             /* and loaded into memory and cast to tnode type */
    int leaf;                /* 1 == current node is a leaf node, 0 otherwise*/
} tnode;

tnode *allocate_node(int keys, int leaf);
void free_tnode(tnode *node);
void write_tnode(tnode *node, char *filename);
tnode *read_tnode(char *filename);
void load_key_into_node(tnode *node, char *line, char *delimiter, int key_pos);
tnode *build_b_tree(char *filename);
kp *search_b_tree(tnode *x, char key[41]);
kp *search_b_tree_helper(tnode *child, char key[41]);