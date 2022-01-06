#include "lee.h"
#include "shared_shortcuts.h"

node* node_MINUS_ONE = static_mini_box_node('-'+'_'*256, mininode_ONE);

node* mini_box_node_new(u32 as, mininode l) {
	printf("mini_box_node_new(%u, '", as);
	print_rimek_bytes(&l, sizeof(mininode));
	printf("')\n");
	assert(l.footer);
	assert(l.len <= sizeof(void*));
	return nm_node_new(as, 0, NULL, &l, bytes_nz(l));
}
node* mini_box_node_seng(node* this, node* vads) {
	if (!this->len) return
		err_node_new("No arg to evalue box node", this);
	node*(* mini_f)(mininode, scope*);
	node*(* f)(node*);
	scope* sc;
	mininode msl;
	msl = mini_seng(borrow(*this->ms), borrow(this->sc), vads);
	if (
		msl.as == stru32z("+_") &&
		msl.l->as == stru32z("ERR")
	) return lyse(this,
		msl.l
	);
	sc = borrow(this->sc);
	unref(this);
	if (msl.as == stru32z("ERR")) return
		restore(msl, sc);
	mini_f = node_mini_box_f(this->as);
	if (mini_f) return
		(*mini_f)(msl, sc);
	f = node_box_f(this->as);
	if (f) return
		(*f)(restore(msl, sc));
	return this;
}
node*(* node_mini_box_f(u32 as))(mininode, scope*) {
	return
		as == stru32z("+_") ?
			&mininode_nop :
		as == stru32z("-_") ?
			&mininode_neg :
		as == stru32z("//_") ?
			&mininode_inv :
		as == stru32z("*_") ?
			&mininode_first :
		as == stru32z("!_") ?
			&mininode_not :
		as == stru32z("(,") ?
			&mininode_list :
		as == stru32z("..._") ?
			&mininode_spread :
		as == stru32z("&_") ?
			&mininode_box :
		as == stru32z(".._") ?
			&mininode_range_forever :
		as == stru32z("_..") ?
			&mininode_range_from_zero :
		as == stru32z("_:=:") ?
			&mininode_comma_self_assoc :
		as == stru32z("_:::") ?
			&mininode_comma_self_cstraint :
		as == stru32z(";:=:") ?
			&mininode_self_assoc :
		as == stru32z(";:::") ?
			&mininode_self_cstraint :
		as == stru32z("|_|") ?
			&mininode_size :
		as == stru32z("len") ?
			&mininode_length :
			NULL;
}
node* mininode_nop(mininode this, scope* sc) {
	return restore(this, sc);
}
node* mininode_neg(mininode this, scope* sc) {
	return
		this.as == stru32z("-_") ? unref(sc),
			this.l :
		ikt_eq_mini(this, mininode_ZERO, sc) ? lyse(this, (unref(sc),
			borrow(node_ZERO)
		)) :
		with_scope(
			mini_box_node_new(stru32z("-_"), this),
			sc
		);
}
node* mininode_inv(mininode this, scope* sc) {
	return
		ikt_eq_mini(this, mininode_ONE, sc) ? lyse(this,
			borrow(node_ONE)
		) : this.as == stru32z("//_") ?
			this.l :
		inv(restore(this, sc));
}
node* mininode_first(mininode this, scope* sc) {
	return
		this.as == stru32z("(,") ?
			comma_first(this.l) :
		this.as == '"' ?
			blob_node_new('\'', this.s, 1) :
		this.as == stru32z("..._") ?
			first(first(this.l)) :
		this.as == stru32z("_..") ?
			this.l :
		this.as == stru32z(".._") ? lyse(this,
			borrow(node_ZERO)
		) :
			first(restore(this, sc));
}
node* mininode_not(mininode this, scope* sc) {
	return
		this.as == stru32z("!_") ? unref(sc),
			this.l 
		:
		ikt_be_true(this, sc) ? lyse(this, (unref(sc),
			borrow(node_FALSE)
		)) :
		iknt_be_true(this, sc) ? lyse(this, (unref(sc),
			borrow(node_TRUE)
		)) :
		with_scope(
			mini_box_node_new(stru32z("!_"), this),
			sc
		);
}
node* mininode_list(mininode this, scope* sc) {
	return
		this.as == stru32z("..._") ? unref(sc),
			this.l :
			with_scope(
				mini_box_node_new(stru32z("(,"), this),
				sc
			);
}
node* mininode_spread(mininode this, scope* sc) {
	return
		this.as == stru32z("(,") ? unref(sc),
			this.l :
		ikt_eq_mini(mini_size(this, sc), mininode_ONE, sc) ? unref(sc),
			this.l :
			spread(restore(this, sc));
}
node* mininode_box(mininode this, scope* sc) {
	if (this.as == stru32z("..._")) return unref(sc),
		this.l;
	if (this.as == '\'' && this.len == 1) return unref(sc),
		blob_node_new('"', this.s, 1);
	node* rv;
	rv = box_node_new(stru32z("(,"), borrow(node_VOID));
	rv->l_type = 'm';
	rv->ml = this;
	rv->sc = sc;
	unref(node_VOID);
	return rv;
}
node* mininode_range_from_zero(mininode this, scope* sc) {
	return unref(sc),
		ikt_ge_mini(mininode_ZERO, this, sc) ?
			borrow(node_EMPTY_LIST) :
		ikt_eq_mini(this, mininode_ONE, sc) ?
			box(borrow(node_ONE)) :
		range_from_zero(restore(this, borrow(sc)));
}
node* mininode_range_forever(mininode this, scope* sc) {
	return with_scope(
		mini_box_node_new(stru32z("_.."), this),
		sc
	);
}
node* mininode_self_assoc(mininode this, scope* sc) {
	return with_scope(
		op_node_new(stru32z("::="), this.l, borrow(this.l)),
		sc
	);
}
node* mininode_self_cstraint(mininode this, scope* sc) {
	return with_scope(
		op_node_new(stru32z(":::"), this.l, borrow(this.l)),
		sc
	);
}
node* mininode_comma_self_assoc(mininode this, scope* sc) {
	return with_scope(
		op_node_new(stru32z(":="), this.l, borrow(this.l)),
		sc
	);
}
node* mininode_comma_self_cstraint(mininode this, scope* sc) {
	return with_scope(
		op_node_new(stru32z("::"), this.l, borrow(this.l)),
		sc
	);
}
node* mininode_size(mininode this, scope* sc) {
	return
		this.as == '"' ? unref(sc),
			blob_node_new('\'', &this.len, !!this.len) :
		this.as == stru32z("&_") ? unref(sc),
			borrow(node_ONE) :
		this.as == stru32z(".._") && (
			this.l->as == stru32z("num") ||
			this.l->as == '\''
		) ? unref(sc),
			this.l :
			size(restore(this, sc));
}
mininode mininode_mini_size(mininode this, scope* sc) {
	return
		this.as == '"' ? unref(sc),
			blob_mininode('\'', &this.len, !!this.len) :
		this.as == stru32z("&_") ? unref(sc),
			mininode_ONE :
		this.as == stru32z(".._") && (
			this.l->as == stru32z("num") ||
			this.l->as == '\''
		) ? unref(sc),
			minimize(this.l, sc) :
			mini_size(restore(this, sc));
}
node* mininode_length(mininode this, scope* sc) {
	return
		this.as == '"' ? unref(sc),
			blob_node_new('\'', &this.len, !!this.len) :
		this.as == stru32z("&_") ? unref(sc),
			borrow(node_ONE) :
			length(restore(this, sc));
}
bool mininode_ikmt_ze(mininode this, scope* sc, bool maybe) {
	return ikmt_eq_mini(this, mininode_ZERO, sc, maybe) ||
		ikmt_ze(restore(this, sc), maybe);
}
bool mininode_ikmt_be_empty(mininode this, scope* sc, bool maybe) {
	return ikmt_eq_mini(this, mininode_EMPTY_LIST, sc, maybe) ||
		ikmt_be_empty(restore(this, sc), maybe);
}
bool mininode_ikmt_be_true(mininode this, scope* sc, bool maybe) {
	return
		ikt_eq_mini(this, mininode_TRUE, sc) ?
			true :
		ikt_eq_mini(this, mininode_FALSE, sc) ?
			false :
		this.as == '!' ?
			!ikmt_be_true(this.l, !maybe) :
			ikmt_be_true(restore(this, sc), maybe);
}
bool mininode_ikmt_be_constant(mininode this, scope* sc, bool maybe) {
	return
		this.as == stru32z("num") ||
		this.as == '\'' ||
		this.as == '"' ||
		this.as == stru32z("boo") ||
		this.as == stru32z("(,") &&
			comma_ikt_be_constant(this.l) ||
		maybe;
}
