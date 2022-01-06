#include "lee.h"
#include "shared_shortcuts.h"

/** long op nodes using truncated addresses of nearby nodes. */
node* lonr_op_node_new(u32 op, node* l, node** rs, fast_u8 len) {
	u16 near_rs[len - 1];
	fast_u8 i;
	if (len == 2) return
		op_node_new(op, l, *rs);
	for (i = 0; i < min(len, node_near_rs_BUFLEN); i++) {
		if (is_near(rs[i], l))
			near_rs[i] = node_nearby_addr(rs[i]);
		else return
			long_op_node_new(op, l, rs, len);
	}
	return nN_node_new(stru32z("lonr"), op, l, near_rs, len);
}
node* lonr_op_node_from_near_rs(u32 op, node* l, u16* near_rs, fast_u8 len) {
	fast_u8 i;
	if (len == 2) return
		op_node_new(op, l, near(l, *near_rs));
	return nN_node_new(stru32z("lonr"), op, l, near_rs, len);
}
bool node_is_near(node* this, node* that) {
	return
		(uptr)this&1 ?
			false :
		~((uptr)this | 65535) == ~((uptr)that      | 65535) ||
		~((uptr)this | 65535) == ~((uptr)node_VOID | 65535);
}
node* node_near(node* this, u16 addr) {
	return (node*)(
		addr&1 ?
			(uptr)node_VOID & ~(uptr)65535 | (addr - 1) :
			(uptr)this &      ~(uptr)65535 |  addr
	);
}
u16 node_nearby_addr(node* this) {
	return (u16)(
		((uptr)this&65535) +
		(~((uptr)this | 65535) == ~((uptr)node_VOID | 65535))
	);
}
node* lonr_op_node_borrow_r(node* this) {
	if (this->len == 3) return
		op_node_new(this->op,
			borrow(near(this->l, *this->near_rs)),
			borrow(near(this->l, this->near_rs[1]))
		);
	if (*this->near_rs&1) {
		if (this->len == 2) return
			near(NULL, *this->near_rs);
		if (this->len == 3) return
			op_node_new(this->op,
				near(NULL, *this->near_rs),
				near(this->l, this->near_rs[1])
			);
		if (!(this->near_rs[1]&1)) return
			op_node_new(this->op,
				near(NULL, *this->near_rs),
				lonr_op_node_from_near_rs(this->op,
					NULL,
					&this->near_rs[1],
					this->len - 2
				)
			);
		node** rs;
		fast_u8 i, rlen;
		rlen = this->len - 1;
		for (i = 0; i < rlen; i++)
			rs[i] = near(this->l, this->near_rs[i]);
		return long_op_node_new(this->op, NULL, rs, rlen);
	}
	return nN_node_new(stru32z("lonr"), this->op,
		near(this->l, *this->near_rs),
		&this->near_rs[1],
		this->len - 1
	);
}
node* node_near_rs_(node* this, fast_u8 i) {
	return near(this->l, i);
}
node* lonr_op_node_seng(node* this, node* vads) {
	return seng(op_node_new(this->op, borrow_l(this), borrow_r(this)), vads);
}
