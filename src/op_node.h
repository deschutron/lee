#ifndef op_node_INCLUDED
#define op_node_INCLUDED

#include "numtypes.h"
#include "node.h"
#include "mininode.h"
#include "scope.h"

node* op_node_new(u32, node*, node*);
node* node_apply_opr(node*, node*, node*);
u32 op_cat_op(u32);
node*(* node_op_f(u32 op))(node*, node*);
node* op_node_seng(node*, node*);
node* node_add(node*, node*);
mininode node_mini_add(node*, node*);
node* node_sub(node*, node*);
node* node_mul(node*, node*);
node* node_div(node*, node*);
node* node_apply(node*, node*);
node* node_eq(node*, node*);
node* node_and(node*, node*);
node* node_or(node*, node*);
node* node_xor(node*, node*);
node* node_implies(node*, node*);
node* node_is_implied_by(node*, node*);
node* node_iff(node*, node*);
node* node_roi(node*, node*);
node* node_while(node*, node*);
node* node_gith(node*, node*);
node* node_semic(node*, node*);
node* node_lambda(node*, node*);
node* node_list_comma(node*, node*);
node* node_set_comma(node*, node*);
node* node_pipe(node*, node*);
node* node_cat(node*, node*);
node* node_believe(node*, node*);
node* node_push(node*, node*);
node* node_union(node*, node*);
node* op_node_op_append(node*, node*);
node* op_node_op_push(node*, node*);
node* node_nbas(node*, node*);
bool node_bytes_eq(node*, node*);
bool node_bytes_eq_mini(node*, mininode);
#define node_ikXt_op_funcs(x) \
	bool node_ikmt_##x(node*, node*, bool);\
	static inline bool node_ikt_##x(node* this, node* that) {\
		return node_ikmt_##x(this, that, false);\
	}\
	static inline bool node_iknt_##x(node* this, node* that) {\
		return !node_ikmt_##x(this, that, true);\
	}\
	static void DUMMY_F()
#define node_ikXt_op_mini_funcs(x) \
	bool node_ikmt_##x##_mini(node*, mininode, scope*, bool);\
	static inline bool node_ikt_##x##_mini(\
		node* this, mininode that, scope* sc\
	) {\
		return node_ikmt_##x##_mini(this, that, sc, false);\
	}\
	static inline bool node_iknt_##x##_mini(\
		node* this, mininode that, scope* sc\
	) {\
		return !node_ikmt_##x##_mini(this, that, sc, true);\
	}\
	static void DUMMY_F()
node_ikXt_op_funcs(eq);
node_ikXt_op_mini_funcs(eq);
node_ikXt_op_funcs(logi_eq);
node_ikXt_op_funcs(ge);
node_ikXt_op_funcs(sort_ge);
node_ikXt_op_funcs(be_in);
node_ikXt_op_funcs(contain);
node_ikXt_op_funcs(define);

#endif
