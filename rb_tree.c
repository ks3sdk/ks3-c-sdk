
#include "rb_tree.h"

static rb_node_t* rb_new_node(key_value* kv) {
    rb_node_t* node = (rb_node_t *) malloc (sizeof(struct rb_node_t)); 

    if (!node) {
        printf("malloc error!\n");
        exit(-1);
    }

	node->kv = kv;
    return node; 
}

static void free_rb_node(rb_node_t* node) {
    if (node == NULL) {
        return;
    }
    if (node->kv) {
		if (node->kv->key) {
			free(node->kv->key);
		}
		if (node->kv->value) {
			free(node->kv->value);
		}
		free(node->kv);
    }
    free(node);
    node = NULL;
}

static rb_node_t* rb_rotate_left(rb_node_t *node, rb_node_t *root) {
    rb_node_t* right = node->right;
    if ((node->right = right->left)) {
        right->left->parent = node;
    }

    right->left= node;
    if ((right->parent = node->parent)) {   
        if (node == node->parent->right) {
            node->parent->right = right;
        } else {
            node->parent->left = right;
        }
    } else {
        root = right;
    } 

    node->parent = right;
    return root; 
} 

static rb_node_t* rb_rotate_right(rb_node_t* node, rb_node_t* root) {
    rb_node_t *left = node->left;
    if ((node->left = left->right)) {
        left->right->parent = node;
    }
    left->right = node;

    if ((left->parent = node->parent)) { 
        if (node == node->parent->right) {
            node->parent->right = left;           
        } else {
            node->parent->left = left;
        } 
    } else {
        root = left;
    }

    node->parent = left;
    return root;
}

static rb_node_t* rb_search_auxiliary_compare(
	key_value* kv, rb_node_t* root, rb_node_t** save,
	int (*compare_func)(key_value*, key_value*)) {
    int ret;
    rb_node_t* node = root;
    rb_node_t* parent = NULL;

    while (node) {
        parent = node;
		ret = (*compare_func)(node->kv, kv);
        if (ret > 0) {
            node = node->left;
        } else if (ret < 0) {
            node = node->right;
        } else {
			return node;
        }
    }

    if (save) {
        *save= parent;
    }

    return NULL;
}

rb_node_t* rb_search_compare(key_value* kv, rb_node_t* root,
	int (*compare_func)(key_value*, key_value*)) {
		return rb_search_auxiliary_compare(kv, root, NULL, compare_func);
}

static rb_node_t *rb_insert_rebalance(rb_node_t* node, rb_node_t* root) {
    rb_node_t *parent, *gparent, *uncle, *tmp;

    while ((parent = node->parent) && (parent->color == RED)) {
        gparent = parent->parent;
        if (parent == gparent->left) {
            uncle = gparent->right;
            if (uncle && uncle->color == RED) {
                uncle->color = BLACK;
                parent->color = BLACK;

                gparent->color = RED;
                node = gparent;
            } else {               
                if (parent->right == node) {
                    root = rb_rotate_left(parent, root);
                    tmp = parent;
                    parent = node;
                    node = tmp;
                }
                parent->color = BLACK;
                gparent->color = RED;
                root = rb_rotate_right(gparent, root);
            }
        } else {
            uncle = gparent->left;

            if (uncle && uncle->color == RED) {
                uncle->color = BLACK;

                parent->color = BLACK;
                gparent->color = RED;
                node = gparent;
            } else {
                if (parent->left == node) {
                    root = rb_rotate_right(parent, root);
                    tmp = parent;
                    parent = node;
                    node = tmp;
                }
                parent->color = BLACK;

                gparent->color = RED;
                root = rb_rotate_left(gparent, root);
            }
        }
    }

    root->color = BLACK;
    return root;
}

rb_node_t* rb_insert_compare(key_value* kv,
	rb_node_t* root, int (*compare_func)(key_value*, key_value*)) {
    rb_node_t* parent = NULL;
    rb_node_t* node;
    int cmp_ret;

    parent = NULL;
	if ((node = rb_search_auxiliary_compare(kv, root, &parent, compare_func))) {
        return root;
    }
    node = rb_new_node(kv);
    node->parent = parent;
    node->left = node->right = NULL;
    node->color = RED;

    if (parent) {
		cmp_ret = (*compare_func)(parent->kv, kv);
        if (cmp_ret > 0) {
            parent->left = node; 
        } else {
            parent->right = node;
        }
    } else {
        root = node;
    }

    return rb_insert_rebalance(node, root);
} 

static rb_node_t* rb_erase_rebalance(rb_node_t* node,
        rb_node_t* parent, rb_node_t* root) {
    rb_node_t *other, *o_left, *o_right;

    while ((!node || node->color == BLACK) && node != root) {       
        if (parent->left == node) {
            other = parent->right;

            if (other->color == RED) {
                other->color = BLACK;
                parent->color = RED;
                root = rb_rotate_left(parent, root);
                other = parent->right;
            }

            if ((!other->left || other->left->color == BLACK)
                    && (!other->right || other->right->color == BLACK)) {
                other->color = RED;
                node = parent;
                parent = node->parent;
            } else {
                if (!other->right || other->right->color == BLACK) {
                    if ((o_left = other->left)) {
                        o_left->color = BLACK;
                    }

                    other->color = RED;
                    root = rb_rotate_right(other, root);
                    other = parent->right;
                }

                other->color = parent->color;
                parent->color = BLACK;
                if (other->right) {
                    other->right->color = BLACK;
                }

                root = rb_rotate_left(parent, root);
                node =root;
                break;
            }
        } else {
            other = parent->left;
            if (other->color == RED) {
                other->color = BLACK;
                parent->color = RED;

                root = rb_rotate_right(parent, root);
                other = parent->left;
            }
            if ((!other->left || other->left->color == BLACK)
                    && ( !other->right || other->right->color == BLACK)) {
                other->color = RED;
                node = parent;
                parent = node->parent;
            } else {
                if (!other->left || other->left->color == BLACK) {
                    if ((o_right = other->right)) {
                        o_right->color = BLACK;
                    }
                    other->color = RED;

                    root = rb_rotate_left(other, root);
                    other = parent->left;
                }

                other->color = parent->color;
                parent->color = BLACK;

                if (other->left) {
                    other->left->color = BLACK;
                }
                root = rb_rotate_right(parent, root);
                node = root;
                break;
            }
        }
    }

    if (node) {
        node->color =BLACK;
    }

    return root;
} 

rb_node_t* rb_erase_compare(key_value* kv, rb_node_t* root,
	int (*compare_func)(key_value*, key_value*)) {
		rb_node_t *child, *parent, *old, *left, *node; 
    color_t color; 

    if (!(node = rb_search_auxiliary_compare(kv, root,
		NULL, compare_func))) {
        printf("key %d is not exist!\n"); 
        return root; 
    }

    old = node;
    if (node->left && node->right) {
        node = node->right;
        while ((left = node->left) != NULL) {
            node =left;     
        }
        child = node->right;
        parent = node->parent;
        color = node->color;
        if (child) {
            child->parent = parent;
        }

        if (parent) {
            if (parent->left == node) {
                parent->left = child;
            } else {
                parent->right = child;
            }
        } else {
            root = child;
        }

        if (node->parent == old) {
            parent = node;
        }
        node->parent = old->parent;
        node->color = old->color;

        node->right = old->right;
        node->left = old->left;

        if (old->parent) {
            if (old->parent->left == old) {
                old->parent->left = node;
            } else {
                old->parent->right = node;
            }
        } else {
            root = node;
        }

        old->left->parent = node; 
        if (old->right) {
            old->right->parent = node;
        }
    } else {
        if (!node->left) {
            child = node->right;
        } else if(!node->right) {
            child = node->left;
        }
        parent = node->parent;

        color = node->color;

        if (child) {
            child->parent = parent;
        }
        if (parent) {
            if (parent->left == node) {
                parent->left = child;
            } else {
                parent->right = child;
            }
        } else {
            root = child;
        }
    }

    // free space
    free_rb_node(old);
    if (color == BLACK) {
        root= rb_erase_rebalance(child, parent, root);
    }
    return root;
}

rb_node_t* rb_erase_all(rb_node_t* root) {
	if (root == NULL) {
		return;
	}
	if (root->left != NULL) {
		rb_erase_all(root->left);
	}
	if (root->right != NULL) {
		rb_erase_all(root->right);
	}
	free_rb_node(root);
}
