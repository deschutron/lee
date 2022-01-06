#ifndef box_mininode_INCLUDED
#define box_mininode_INCLUDED

#include "node.h"
#include "mininode.h"

mininode box_mininode(u32, node*);
node* box_mininode_seng(mininode, scope*, node*);
mininode box_mininode_mini_seng(mininode, scope*, node*);
mininode(* mininode_box_f(u32))(node*);
mininode node_mini_nop(node*);
mininode node_mini_neg(node*);
mininode node_mini_first(node*);

#endif
