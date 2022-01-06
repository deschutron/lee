#include "lee.h"
#include "shared_shortcuts.h"

node* long_op_node_new(u32 op, node* l, node** xs, fast_u8 len) {
	return node_new(
		(len >= 2)*stru32z("long"), op,
		l, xs, NULL, NULL, NULL, NULL,
		len, NULL, NULL,
		NULL, NULL
	);
}
node** node_array_borrow_all(node** these, fast_u8 N) {
	fast_u8 i;
	node* botei;
	for (i = 0; i < N; i++) {
		botei = borrow(these[i]);
		/* if cur was on its final reference
		   or it has p_unref_from and not p_borrow */
		if (these[i] != botei)
			/*
			   This function is a bit sneaky.
			   If borrowing a node results in a new node object being created,
			   it has to put the new node object in the list for its
			   recipient.
			   The node donor has to have the same list though, because
			   the array is not actually being copied.
			   So then we unref the borrowed node and borrow the copy again.
			   Then the donor has the new node, but it has the same Lee
			   value as the old one, and it has the right value for ref_count.
			*/
			these[i] = lyse(these[i], borrow(botei));
	}
	return these;
}
node* long_op_node_seng(node* this, node* vads) {
	return seng(op_node_new(this->op, borrow_l(this), borrow_r(this)), vads);
}
