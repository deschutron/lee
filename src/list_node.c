#include "lee.h"
#include "shared_shortcuts.h"

node* list_node_cons(node* this, node* that) {
	if (!this) return
		borrow(node_EMPTY_LIST);
	if (!that) return
		box(this);
	if (that->as != stru32z("(,")) return
		cat(box(this), that);
	return lyse(that,
		box_node_new(stru32z("(,"),
			comma_cons(this, borrow_l(that))
		)
	);
}
node* node_comma_cons(node* this, node* that) {
	if (!this) return
		err_node_new("Can't construct list contents without head", NULL);
	if (!that) return
		this;
	return op_node_new(stru32z("`,"), this, that);
}
node* list_node_from_array(usz len, node** xs) {
	if (!len) return
		borrow(node_EMPTY_LIST);
	return node_comma_from_array(len, xs);
}
static node* node_comma_from_array_recur(usz len, node** xs, node* rv) {
	if (!len) return
		rv;
	return node_comma_from_array_recur(
		len - 1,
		&xs[1], 
		op_node_new(stru32z("`,"),
			*xs,
			rv
		)
	);
}
node* node_comma_from_array(usz len, node** xs) {
	if (!len) return
		err_node_new("Can't construct list contents from empty array", NULL);
	return node_comma_from_array_recur(len - 1, &xs[1], *xs);
}
node* list_node_first(node* this) {
	assert(this->ref_count);
	if (ikt_eq(this, node_EMPTY_LIST)) return
		err_node_new("No primary value", this);
	return lyse_with_note(this,
		node_comma_first(borrow_l(this)),
		"lsnofi"
	);
}
node* node_comma_first(node* this) {
	if (
		this->as &&
		this->as != stru32z("...")
		||
		this->op != stru32z(",") &&
		this->op != stru32z("`,") &&
		this->op != stru32z("&,") &&
		this->op != stru32z(":=") &&
		this->op != stru32z("::")
	) return
		this;
	if (this->as == stru32z("...")) return lyse(this,
		borrow_l(this)
	);
	if (this->op == ',') return
		box_node_new(stru32z("*_"), this);
	if (this->op == stru32z("&,")) return
		box_node_new(stru32z("*_"), this);
	node* l,* si;
	if (this->op == stru32z(":=")) return
		ikt_eq(l = borrow_l(this), node_ZERO) ?
			lyse(l, lyse(this,
				borrow_r(this)
			)) :
			lyse(l,
				box_node_new(stru32z("*_"), this)
			);
	if (this->op == stru32z("::")) {
		if (
			ikt_eq(l = borrow_l(this), node_ZERO) &&
			ikt_eq(si = size(borrow_r(this)), node_ONE)
		) return lyse(l, lyse(si, lyse(this,
			first(borrow_r(this))
		)));
	}
	if (this->op != stru32z("`,")) return
		box_node_new(stru32z("*_"), this);
	return node_comma_first(lyse(this, borrow_l(this)));
}
node* list_node_rest(node* this) {
	assert(this);
	if (ikt_eq(this, node_EMPTY_LIST)) return
		box_node_new(stru32z("rs"), this);
	return lyse_with_note(this,
		list(node_comma_rest(borrow_l(this))),
		"lsnors"
	);
}
node* node_comma_rest(node* this) {
	if (has_op_as(this) && this->op == stru32z("&,")) return
		box_node_new(stru32z("..._"),
			box_node_new(stru32z("rs"),
				box_node_new(stru32z("(,"),
					this
				)
			)
		);
	if (has_op_as(this) && this->op == stru32z("`,")) return lyse(this,
		borrow_r(this)
	);
	node* var,* label,* rv;
	scope* sc;
	if (has_op_as(this) && (
		this->op == stru32z(":=") ||
		this->op == stru32z("::") ||
		this->op == stru32z(":|")
	)) {
		var = borrow_l(this);
		label = borrow_l(var);
		sc = (scope*)*var->psr;
		if (is_in_scope(this, sc) && ikt_eq(label, node_ZERO))
			rv = box_node_new(stru32z("..._"),
				borrow(node_EMPTY_LIST)
			);
		else
			rv = this;
		unref(lyse(var, label));
		return rv;
	}
	return lyse(this,
		box_node_new(stru32z("..._"), borrow(node_EMPTY_LIST))
	);
}
/** This is a cat of this and that */
node* list_node_cat(node* this, node* that) {
	node* til;
	if (that->as == '"') {
		if ((til = borrow_l(this))->as == stru32z("num")) {
		}
		if ((til = borrow_l(this))->as == stru32z("'") && til->len == 1) {
		}
	}
	if (that->as == stru32z("(,")) return lyse(this, lyse(that,
		box_node_new(stru32z("(,"),
			comma_cat(borrow_l(this), borrow_l(that))
		)
	));
	return op_node_new(stru32z(".||"), this, that);
}
node* node_comma_cat(node* this, node* that) {
	if (
		this->as &&
		this->as != stru32z("long") &&
		this->as != stru32z("lonr") &&
		this->as != stru32z("miop") &&
		this->as != stru32z("...")
		||
		this->op != stru32z("`,")
	) return
		op_node_new(stru32z("`,"), this, that);
	if (this->as == stru32z("...")) return
		box_node_new(stru32z("..."),
			cat(this->l, box_node_new(stru32z("(,"), that))
		);
	return node_comma_cat(
		lyse(this, borrow_l(this)),
		comma_cat(borrow_r(this), that)
	);
}
node* node_comma_list(node* this) {
	if (has_op_as(this) && this->op == ',') return lyse(this,
		op_node_new(stru32z("`,"),
			comma_list(borrow_l(this)),
			comma_list(borrow_r(this))
		)
	);
	return this;
}
node* node_comma_set(node* this) {
	if (has_op_as(this) && this->op == ',') return lyse(this,
		op_node_new(stru32z("&,"),
			comma_set(borrow_l(this)),
			comma_set(borrow_r(this))
		)
	);
	return this;
}
node* list_node_add(node* this, node* that) {
	if (
		ikt_eq(this, node_EMPTY_LIST) &&
		ikt_eq(that, node_EMPTY_LIST)
	) return
		op_node_new('*',
			borrow(node_TWO),
			borrow(node_EMPTY_LIST)
		);
	if (that->as == stru32z("(,")) return
		box_node_new(stru32z("(,"),
			comma_add(this, that)
		);
	return op_node_new('+', this, that);
}
node* node_comma_add(node* this, node* that) {
	if (!this->as && this->op == stru32z("`,")) {
		if (has_op_as(that) && that->op == stru32z("`,")) return
			lyse(this, lyse(that,
				comma_cat(
					add(borrow_l(this), borrow_l(that)),
					comma_add(borrow_r(this), borrow_r(that))
				)
			));
	}
	return op_node_new('+', this, that);
}
node* list_node_push(node* this, node* that) {
	if (ikt_eq(this, node_EMPTY_LIST)) return lyse(this,
		list(that)
	);
	return lyse(this,
		box_node_new(stru32z("(,"), comma_push(borrow_l(this), that))
	);
}
node* node_comma_push(node* this, node* that) {
	return op_node_new(stru32z("`,"), that, this);
}
node* list_node_size(node* this) {
	if (ikt_be_empty(this)) return lyse(this,
		borrow(node_ZERO)
	);
	if (this->l_type == 'm') return lyse(this,
		comma_size(borrow(this->ml), borrow(this->sc))
	);
	return lyse(this,
		comma_size(borrow(this->l))
	);
}
static node* mininode_comma_size_recur(mininode this, node* rv);
static node* comma_size_recur(node* this, node* rv) {
	if (this->as == stru32z("..._")) return lyse(this,
		add(rv, size(borrow_l(this)))
	);
	if (this->as == stru32z("+_")) return lyse(this,
		comma_size_recur(borrow_l(this), rv)
	);
	if (!has_op_as(this) ||
		this->op != stru32z("`,") &&
		this->op != stru32z("&,")
	) return lyse(this,
		inc(rv)
	);
	if (this->l_type == 'm') return lyse(this,
		mininode_comma_size_recur(this->ml, rv)
	);
	return comma_size_recur(
		lyse(this, borrow_r(this)),
		add(rv, comma_size(borrow(this->l)))
	);
}
node* node_comma_size(node* this) {
	return comma_size_recur(this, borrow(node_ZERO));
}
static node* mininode_comma_size_recur(mininode this, node* rv) {
	if (this.as == stru32z("..._")) return lyse(this,
		add(rv, size(borrow(this.l)))
	);
	if (this.as != stru32z("+_")) return lyse(this,
		inc(rv)
	);
	return comma_size_recur(this.l, rv);
}
node* mininode_comma_size(mininode this, scope* sc) {
	if (sc) unref(sc);
	return mininode_comma_size_recur(this, borrow(node_ZERO));
}
node* list_node_length(node* this) {
	if (ikt_be_empty(this)) return lyse(this,
		borrow(node_ZERO)
	);
	return lyse(this,
		comma_length(borrow_l(this))
	);
}
static node* comma_length_recur(node* this, node* rv) {
	bool is_op;
	if ((is_op = has_op_as(this)) && (
		this->op == stru32z(":=") ||
		this->op == stru32z("::") ||
		this->op == stru32z(":|")
	)) return lyse(this,
		rv
	);
	if (this->as == stru32z("+_")) return lyse(this,
		comma_length_recur(borrow_l(this), rv)
	);
	if (this->as == stru32z("..._")) return lyse(this,
		add(rv, length(borrow_l(this)))
	);
	if (!is_op ||
		this->op != stru32z("`,") &&
		this->op != stru32z("&,")
	) return lyse(this,
		inc(rv)
	);
	return comma_length_recur(
		lyse(this, borrow_r(this)),
		add(rv, comma_length(borrow_l(this)))
	);
}
node* node_comma_length(node* this) {
	return comma_length_recur(this, borrow(node_ZERO));
}
node* mininode_comma_length(mininode this, scope* sc) {
	if (sc) unref(sc);
	return comma_length_recur(restore(this, sc), borrow(node_ZERO));
}
node* node_union(node* this, node* that) {
	if (ikt_eq(this, that)) return lyse(that,
		this
	);
	if (ikt_eq(this, node_EMPTY_SET)) return lyse(this,
		that
	);
	if (ikt_eq(that, node_EMPTY_SET)) return lyse(that,
		this
	);
	return op_node_new(stru32z("∪"), this, that);
}
node* list_node_mul(node* this, node* that) {
	return op_node_new('*', this, that);
}
bool list_node_ikmt_be_constant(node* this, bool maybe) {
	if (ikt_eq(this, node_EMPTY_LIST)) return
		true;
	node* l;
	bool rv;
	rv = ikmt_be_constant(borrow_l(this), maybe);
	unref(l);
	return rv;
}
bool node_comma_ikmt_be_constant(node* this, bool maybe) {
	if (this->as != stru32z("`,")) return
		ikmt_be_constant(this, maybe);
	node* l,* r;
	bool rv;
	l = borrow_l(this);
	r = borrow_r(this);
	rv = ikmt_be_constant(l, maybe) && ikmt_be_constant(r, maybe);
	unref(lyse(l, r));
	return rv;
}
bool list_node_ikmt_be_finite(node* this, bool maybe) {
	if (ikt_eq(this, node_EMPTY_LIST)) return
		true;
	node* l;
	bool rv;
	return
		ikmt_be_finite(first(this), maybe) &&
		ikmt_be_finite(rest(this), maybe);
}
