#ifndef mini_box_node_INCLUDED
#define mini_box_node_INCLUDED

#include "numtypes.h"
#include "node.h"
#include "mininode.h"
#include "scope.h"

#define static_mini_box_node(x, y) &(node){\
	.as = (x),\
	.op = 0,\
	.data_type = 'm'+'_'*256,\
	.len = 1,\
	.mem_type = 'S',\
	.ref_count = 1,\
	.sc = NULL,\
	.vads = NULL,\
	.ml = (y),\
	.pb_note = (void*)(' '*0x03030302 + 0x02090d13) /* "Smib" */,\
	.p_unref_from = NULL\
}
node* node_MINUS_ONE;

node* mini_box_node_new(u32, mininode);
node* mini_box_node_seng(node*, node*);
node*(* node_mini_box_f(u32))(mininode, scope*);
node* mininode_nop(mininode, scope*);
node* mininode_neg(mininode, scope*);
node* mininode_inv(mininode, scope*);
node* mininode_first(mininode, scope*);
node* mininode_not(mininode, scope*);
node* mininode_list(mininode, scope*);
node* mininode_spread(mininode, scope*);
node* mininode_box(mininode, scope*);
node* mininode_range_from_zero(mininode, scope*);
node* mininode_range_forever(mininode, scope*);
node* mininode_self_assoc(mininode, scope*);
node* mininode_self_cstraint(mininode, scope*);
node* mininode_comma_self_assoc(mininode, scope*);
node* mininode_comma_self_cstraint(mininode, scope*);
node* mininode_size(mininode, scope*);
mininode mininode_mini_size(mininode, scope*);
node* mininode_length(mininode, scope*);
#define mininode_ikXt_box_funcs(x) \
	bool mininode_ikmt_##x(mininode, scope*, bool);\
	static inline bool mininode_ikt_##x(mininode this, scope* sc) {\
		return mininode_ikmt_##x(this, sc, false);\
	}\
	static inline bool mininode_iknt_##x(mininode this, scope* sc) {\
		return !mininode_ikmt_##x(this, sc, true);\
	}\
	static void DUMMY_F()
mininode_ikXt_box_funcs(ze);
mininode_ikXt_box_funcs(be_empty);
mininode_ikXt_box_funcs(be_true);
mininode_ikXt_box_funcs(have_labels);
mininode_ikXt_box_funcs(be_constant);
mininode_ikXt_box_funcs(be_finite);

#endif
