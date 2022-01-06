#ifndef scope_INCLUDED
#define scope_INCLUDED

#include "numtypes.h"

#define node struct node
typedef struct scope {
	#define scope struct scope
	scope* parent;
	node* global;
	u32 ref_count;
	u32 tag;
	#undef scope
} scope;
#include "node.h"

scope* scope_new(node*);
scope* scope_borrow(scope*);
scope* scope_unref(scope*);
scope* scope_unref_from(scope*, void*);
scope* scope_in_scope(scope*, scope*);
scope* global_scope();
scope* global_scope_do_op();
scope* global_scope_unref();
scope* root_node_scope();
scope* root_node_scope_do_op();
scope* root_node_scope_unref();
bool scope_is_in(scope*, scope*);
void scope_print_bytes_with_indent(scope*, fast_u8);
#undef node

#endif
