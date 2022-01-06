#ifndef mini_op_node_INCLUDED
#define mini_op_node_INCLUDED

#include "node.h"
#include "scope.h"
#include "mininode.h"

node* mini_op_node_new(u32, mininode, mininode);
node* mini_op_node_new_from_nodes(u32, node*, node*);
mininode mininode_mini_add(mininode, scope*, node*);
mininode mininode_mini_add_mini(mininode, mininode, scope*);
node*(* mininode_op_f_mini(u32 op))(mininode, mininode, scope*);
node* mini_op_node_seng(node*, node*);
#define mininode_ikXt_op_funcs(x) \
	bool mininode_ikmt_##x(mininode, scope*, node*, bool);\
	static inline bool mininode_ikt_##x(mininode this, scope* sc, node* that) {\
		return mininode_ikmt_##x(this, sc, that, false);\
	}\
	static inline bool mininode_iknt_##x(mininode this, scope* sc, node* that){\
		return !mininode_ikmt_##x(this, sc, that, true);\
	}\
	static void DUMMY_F()
mininode_ikXt_op_funcs(eq);
mininode_ikXt_op_funcs(ge);
#define mininode_ikXt_op_mini_funcs(x) \
	bool mininode_ikmt_##x##_mini(mininode, mininode, scope*, bool);\
	static inline bool mininode_ikt_##x##_mini(\
		mininode this, mininode that, scope* sc\
	) {\
		return mininode_ikmt_##x##_mini(this, that, sc, false);\
	}\
	static inline bool mininode_iknt_##x##_mini(\
		mininode this, mininode that, scope* sc\
	) {\
		return !mininode_ikmt_##x##_mini(this, that, sc, true);\
	}\
	static void DUMMY_F()
mininode_ikXt_op_mini_funcs(eq);
mininode_ikXt_op_mini_funcs(ge);
mininode_ikXt_op_mini_funcs(sort_ge);

#endif
