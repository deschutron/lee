#include "lee.h"
#include "shared_shortcuts.h"

node* lomi_op_node_new(u32 op, mininode l, mininode* xs, fast_u8 len) {
	return
		len <  2 ?
			err_node_new("nup", NULL) :
		bytes_ze(l) ?
			lomi_op_node_new(op, *xs, &xs[1], len) :
		len == 2 ?
			mini_op_node_new(op, l, *xs) :
		op_cat_op(op) ?
			op_node_new(op_cat_op(op),
				mini_op_node_new(op, l, *xs),
				len == 3 ?
					restore(xs[1], NULL) :
					lomi_op_node_new(op, xs[1], &xs[2], len - 1)
			) :
			err_node_new("nup", NULL);
}
