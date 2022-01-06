#ifndef var_node_INCLUDED
#define var_node_INCLUDED

#include "numtypes.h"
#include "node.h"
#include "scope.h"
#include "mininode.h"

node* var_node_new(node* label, scope* sc, node* parent, u32 op);
node* slash_var_node_believe(node* this, node* that);
node* dot_var_node_believe(node* this, node* that);
node* var_node_believe(node* this, node* that, u32 op);
node*(* var_node_believe_box_f(u32 as))(node*, node*);
node*(* var_node_believe_op_f(u32 op))(node*, node*, node*);
node* var_node_believe_eq(node* var, node* l, node* r);
node* var_node_believe_is_in(node* var, node* l, node* r);
node* var_node_believe_and(node* var, node* l, node* r);

#endif
