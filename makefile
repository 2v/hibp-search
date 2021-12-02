all: search build_b_tree build_b_tree_leak

search: search.c btree.c btree.h sha1.c sha1.h
	gcc -o search search.c btree.c sha1.c -I.

build_b_tree: build_b_tree.c btree.c btree.h sha1.c sha1.h
	gcc -o build_b_tree build_b_tree.c btree.c sha1.c -I.

build_b_tree_leak: build_b_tree.c btree.c btree.h sha1.c sha1.h
	gcc -o build_b_tree_leak build_b_tree.c btree.c sha1.c -g -fsanitize=address -fsanitize=leak