#ifndef lonr_op_node_INCLUDED
#define lonr_op_node_INCLUDED

#include "numtypes.h"
#include "node.h"
#include "scope.h"
#include "mininode.h"

node* lonr_op_node_new(u32, node*, node**, fast_u8);
bool node_is_near(node*, node*);
node* node_near(node*, u16);
u16 node_nearby_addr(node*);
node* lonr_op_node_borrow_r(node*);
node* node_near_rs_(node*, fast_u8);
node* lonr_op_node_seng(node*, node*);

#endif
