#ifndef empty_node_INCLUDED
#define empty_node_INCLUDED

#include "numtypes.h"
#include "node.h"

_Pragma("GCC diagnostic ignored \"-Winitializer-overrides\"")
#define static_empty_node(x_as) &(node){\
	.as = (x_as),\
	.op = 0,\
	.data_type = '_'*257,\
	.len = 0,\
	.mem_type = 'S',\
	.ref_count = 1,\
	.sc = NULL,\
	.vads = NULL,\
	.l = NULL,\
	.r = NULL,\
	.p_borrow = NULL,\
	.p_unref_from = NULL\
}
node* node_VOID;
node* node_ZERO;
node* node_EMPTY_STRING;
node* node_EMPTY_LIST;
node* node_EMPTY_SET;

node* empty_node_new(u32);

#endif
