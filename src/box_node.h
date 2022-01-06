#ifndef box_node_INCLUDED
#define box_node_INCLUDED

#include "node.h"
#include "scope.h"
#include "mininode.h"

node* box_node_new(u32, node*);
static u32 bxr_s_as(u8*);
node* node_apply_bxr(node*, node*);
node* box_node_seng(node*, node*);
node*(* node_box_f(u32))(node*);
node* node_nop(node*);
node* node_neg(node*);
node* node_inv(node*);
node* node_not(node*);
node* node_first(node*);
node* node_box(node*);
node* node_list(node*);
node* node_set(node*);
node* node_spread(node*);
node* node_range_forever(node*);
node* node_range_from_zero(node*);
node* node_dec(node*);
node* node_inc(node*);
node* node_size(node*);
mininode node_mini_size(node*);
node* node_length(node*);
node* node_rest(node*);
node* node_let(node*);
node* node_have(node*);
node* node_pub(node*);
node* node_cx_block(node*);
node* node_label(node*);
#define node_ikXt_box_funcs(x) \
	bool node_ikmt_##x(node*, bool);\
	static inline bool node_ikt_##x(node* this) {\
		return node_ikmt_##x(this, false);\
	}\
	static inline bool node_iknt_##x(node* this) {\
		return !node_ikmt_##x(this, true);\
	}\
	static void DUMMY_F()
node_ikXt_box_funcs(be_true);
node_ikXt_box_funcs(be_finite);
node_ikXt_box_funcs(be_constant);
node_ikXt_box_funcs(ze);
node_ikXt_box_funcs(have_labels);
node_ikXt_box_funcs(zelen);
node_ikXt_box_funcs(be_empty);
node_ikXt_box_funcs(have_first_value);
node_ikXt_box_funcs(contain_eval_instr);

#endif
