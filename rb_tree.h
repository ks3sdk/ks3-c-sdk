#ifndef _KS3_SDK_C_RB_TREE_H_
#define _KS3_SDK_C_RB_TREE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>
#include <string.h> 

typedef char* key_type;
typedef char* data_type;

typedef enum color_t { 
	RED = 0, 
	BLACK = 1
} color_t;

typedef struct {
	char* key;
	char* value;
} key_value;

typedef struct rb_node_t {
	key_value* kv;
	color_t color;
	struct rb_node_t* left;
    struct rb_node_t* right;
    struct rb_node_t* parent;    
} rb_node_t;

rb_node_t* rb_insert_compare(key_value* kv, rb_node_t* root,
	int (*compare_func)(key_value*, key_value*));
rb_node_t* rb_search_compare(key_value* kv, rb_node_t* root,
	int (*compare_func)(key_value*, key_value*));
rb_node_t* rb_erase_compare(key_value* kv, rb_node_t* root,
	int (*compare_func)(key_value*, key_value*));
rb_node_t* rb_erase_all(rb_node_t* root);

#ifdef __cplusplus
}
#endif

#endif  // _KS3_SDK_C_RB_TREE_H_