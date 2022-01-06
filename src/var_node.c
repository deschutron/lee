#include "lee.h"
#include "shared_shortcuts.h"

node* var_node_new(node* label, scope* sc, node* parent, u32 op) {
	assert(sc);
	assert(parent);
	assert(!((uptr)label->sc&1));
	return op_node_new('v'+ op*256,
		np_node_new(stru32z("[sc"), 0,
			label, (void**)&sc,
			1 + !!sc,
			(void* (*)(void*))&scope_borrow,
			(void* (*)(void*, void*))&scope_unref_from
		),
		parent
	);
}
node* slash_var_node_believe(node* this, node* that) {
	return var_node_believe(this, that, stru32z("v/"));
}
node* dot_var_node_believe(node* this, node* that) {
	return var_node_believe(this, that, stru32z("v."));
}
node* var_node_believe(node* this, node* that, u32 op) {
	node*(*f )(node*, node*);
	node*(*g )(node*, node*, node*);
	f = var_node_believe_box_f(that->as);
	g = var_node_believe_op_f(that->op);
	if (that->as == stru32z("ERR")) return lyse(this,
		that
	);
	if (iknt_be_true(that)) return
		err_node_new("Contradiction!", op_node_new('|', this, that));
	if (f) return lyse(that,
		(*f)(this, borrow_l(that))
	);
	if (g) return lyse(that,
		(*g)(this, borrow(that->l), borrow_r(that))
	);
	return op_node_new(stru32z("::|"), this, that);
}
node*(* var_node_believe_box_f(u32 as))(node*, node*) {
	return NULL;
}
node*(* var_node_believe_op_f(u32 op))(node*, node*, node*) {
	return
		op == '=' ?
			var_node_believe_eq :
		op == ':' ?
			var_node_believe_is_in :
		op == stru32z("&&") ?
			var_node_believe_and :
			NULL;
}
node* var_node_believe_eq(node* var, node* l, node* r) {
	if (ikt_eq(l, var)) return lyse(l,
		op_node_new(stru32z("::="), var, r)
	);
	if (ikt_eq(r, var)) return lyse(r,
		op_node_new(stru32z("::="), var, l)
	);
	return borrow(node_TRUE);
}
node* var_node_believe_is_in(node* var, node* l, node* r) {
	if (ikt_eq(l, var)) return lyse(l,
		op_node_new(stru32z(":::"), var, r)
	);
	if (ikt_eq(r, var)) return lyse(r,
		op_node_new(stru32z("::|"), var,
			op_node_new(':', l, borrow(var))
		)
	);
	return borrow(node_TRUE);
}
node* var_node_believe_and(node* var, node* l, node* r) {
	return var_node_believe(
		var_node_believe(var, l, '/'),
		r,
		'/'
	);
}
