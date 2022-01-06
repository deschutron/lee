#ifndef long_op_node_INCLUDED
#define long_op_node_INCLUDED

#include "numtypes.h"
#include "node.h"

node* long_op_node_new(u32, node*, node**, fast_u8);
node** node_array_borrow_all(node**, fast_u8);
node* long_op_node_seng(node*, node*);

#endif
