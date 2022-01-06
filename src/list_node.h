#ifndef list_node_INCLUDED
#define list_node_INCLUDED

#include <stdbool.h>
#include "numtypes.h"
#include "node.h"
#include "mininode.h"
#include "scope.h"

node* list_node_cons (node*, node*);
node* node_comma_cons(node*, node*);
node* list_node_from_array (usz, node**);
node* node_comma_from_array(usz, node**);
node* list_node_first (node*);
node* node_comma_first(node*);
node* list_node_rest (node*);
node* node_comma_rest(node*);
node* list_node_cat (node*, node*);
node* node_comma_cat(node*, node*);
node* node_comma_list(node*);
node* node_comma_set(node*);
node* list_node_add (node*, node*);
node* node_comma_add(node*, node*);
node* list_node_push (node*, node*);
node* node_comma_push(node*, node*);
node* list_node_size(node*);
node* node_comma_size(node*);
node* mininode_comma_size(mininode, scope*);
node* list_node_length(node*);
node* mininode_comma_length(mininode, scope*);
node* node_comma_length(node*);
node* list_node_mul(node*, node*);
#define list_node_ikXt_box_funcs(x) \
	bool list_node_ikmt_##x(node*, bool);\
	static inline bool list_node_ikt_##x(node* this) {\
		return list_node_ikmt_##x(this, false);\
	}\
	static inline bool list_node_iknt_##x(node* this) {\
		return !list_node_ikmt_##x(this, true);\
	}\
	static void DUMMY_F()
#define node_comma_ikXt_box_funcs(x) \
	bool node_comma_ikmt_##x(node*, bool);\
	static inline bool node_comma_ikt_##x(node* this) {\
		return node_comma_ikmt_##x(this, false);\
	}\
	static inline bool node_comma_iknt_##x(node* this) {\
		return !node_comma_ikmt_##x(this, true);\
	}\
	static void DUMMY_F()
#define node_comma_ikXt_op_funcs(x) \
	bool node_comma_ikmt_##x(node*, node*, bool);\
	static inline bool node_comma_ikt_##x(node* this, node* that) {\
		return node_comma_ikmt_##x(this, that, false);\
	}\
	static inline bool node_comma_iknt_##x(node* this, node* that) {\
		return !node_comma_ikmt_##x(this, that, true);\
	}\
	static void DUMMY_F()
list_node_ikXt_box_funcs(be_constant);
list_node_ikXt_box_funcs(be_finite);
node_comma_ikXt_box_funcs(be_constant);
node_comma_ikXt_box_funcs(be_finite);

#endif
