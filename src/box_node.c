#include "lee.h"
#include "shared_shortcuts.h"

node* box_node_new(u32 as, node* l) {
	return node_new(
		as, 0,
		l, NULL, NULL, NULL, NULL, NULL,
		1, NULL, NULL,
		NULL, NULL
	);
}
static u32 bxr_s_as(u8* s) {
	assert(*(u32*)s < stru32z("\0\0\0\1"));
	u8 as_s[4];
	*(u32*)as_s = *(u32*)s;
	as_s[strnlen((char*)s, 3)] = '_';
	return *(u32*)as_s;
}
node* node_apply_bxr(node* o, node* x) {
	if (o->as == stru32z("opr")) {
		node*(* f)(node*);
		u32 as;
		as = bxr_s_as(o->s);
		f = node_box_f(as);
		unref_with_note(o, "bx opr");
		return f ?
			(*f)(seng(x, borrow(node_EMPTY_LIST))) :
			box_node_new(as, x);
	}
	if (!has_op_as(o) || o->op != stru32z("bo")) {
		print(o);
		return op_node_new(stru32z("b "), o, x);
	}
	return
		node_apply_bxr(
			lyse(o, borrow(o->l)),
			apply_bxr(borrow_r(o), x)
		);
}
node* box_node_seng(node* this, node* vads) {
	node*(* f)(node*);
	if (this->l_type == 'm') return
		mini_box_node_seng(this, vads);
	if (!this->len) return
		err_node_new("Not enough args to seng box node", this);
	if (this->l_type != 'n') return
		err_node_new("Trying to seng non-box node as box node", this);
	f = node_box_f(this->as);
	node* sl;
	scope* sc;
	sc = this->sc ? borrow(this->sc) : NULL;
	sl = seng(borrow(this->l), vads);
	unref(this);
	if (sl->as == stru32z("ERR")) return
		sl;
	if (!f) return
		box_node_new(this->as, sl);
	return with_scope(
		(*f)(sl),
		sc
	);
}
node*(* node_box_f(u32 as))(node*) {
	#define match as == stru32z
	return
		match("+_")   ? &node_nop :
		match("-_")   ? &node_neg :
		match("//_")  ? &node_inv :
		match("&_")   ? &node_box :
		match("*_")   ? &node_first :
		match("..._") ? &node_spread :
		match("[")    ? &node_label :
		match("(,")   ? &node_list :
		match("{,")   ? &node_set :
		match("!_")   ? &node_not :
		match(".._")  ? &node_range_forever :
		match("_..")  ? &node_range_from_zero :
		match("|_|")  ? &node_size :
		match("len")  ? &node_length :
		match("ɪ" )   ? &node_inc :
		match("ᴅ"  )  ? &node_dec :
		match("rs")   ? &node_rest :
		match("(:")   ? &node_cx_block :
		match("let")  ? &node_let :
		match("hav")  ? &node_have :
		match("pub")  ? &node_pub :
			NULL;
	#undef match
}
node* node_nop(node* this) {
	return this;
}
node* node_label(node* this) {
	return box_node_new('[', this);
}
node* node_neg(node* this) {
	return
		this == node_MINUS_ONE ? lyse(this,
			borrow(node_ONE)
		) :
		this->as == stru32z("-_") ? lyse(this,
			borrow_l(this)
		) :
		has_op_as(this) && this->op == '-' ? lyse(this,
			sub(borrow_r(this), borrow(this->l))
		) :
		ikt_ze(this) ? lyse(this,
			borrow(node_ZERO)
		) :
		box_node_new(stru32z("-_"), this);
}
node* node_inv(node* this) {
	if (ikt_eq(this, node_ONE)) return
		this;
	if (this->as == stru32z("//_")) return lyse(this,
		this->l
	);
	if (has_op_as(this) && this->op == stru32z("//")) return lyse(this,
		div(borrow_r(this), borrow(this->l))
	);
	return box_node_new(stru32z("//_"), this);
}
node* node_not(node* this) {
	if (this->as == stru32z("!_")) return lyse(this,
		borrow_l(this)
	);
	if (ikt_be_true(this)) return lyse(this,
		borrow(node_FALSE)
	);
	if (iknt_be_true(this)) return lyse(this,
		borrow(node_TRUE)
	);
	if (
		(!this->as || this->as == stru32z("miop")) &&
		this->op == stru32z("!=")
	) {
		node* rv;
		rv = lyse_and_clone(this);
		rv->op = '=';
		return rv;
	}
	if (has_op_as(this) && this->op == stru32z("!=")) return
		op_node_new('=', borrow_l(this), borrow_r(this));
	if (
		(!this->as || this->as == stru32z("miop")) &&
		this->op == stru32z("!:")
	) {
		node* rv;
		rv = lyse_and_clone(this);
		rv->op = ':';
		return rv;
	}
	if (has_op_as(this) && this->as == stru32z("!:")) return lyse(this,
		op_node_new(':', borrow_l(this), borrow_r(this))
	);
	if (has_op_as(this) && this->op == '<') return lyse(this,
		op_node_new(stru32z(">="), borrow_l(this), borrow_r(this))
	);
	return box_node_new('!', this);
}
node* node_first(node* this) {
	assert(this->ref_count);
	if (this->as == stru32z("(,") || ikt_eq(this, node_EMPTY_LIST)) return
		list_node_first(this);
	if (has_op_as(this) && this->op == stru32z("`,")) return
		comma_first(this);
	node* l,* r;
	if (has_op_as(this) && this->op == stru32z("w/")) {
		if (ikt_have_first_value(r = borrow_r(this))) return
			first(this->r);
		if (iknt_have_first_value(r)) return
			first(this->l);
		return box_node_new(stru32z("*_"), this);
	}
	if (this->as == '"') return
		blob_node_new(stru32z("'"), this->s, 1);
	return box_node_new(stru32z("*_"), this);
}
node* node_box(node* this) {
	if (this->as == '\'' && this->len == 1) {
		node* rv;
		rv = lyse_and_clone(this);
		rv->as = '"';
		return rv;
	}
	if (can_be_minimized(this, NULL)) return
		mini_box_node_new(stru32z("(,"), minimize(this, this->sc));
	return box_node_new(stru32z("(,"), this);
}
node* node_list(node* this) {return
	this->as == stru32z("..._") ? lyse(this,
		borrow_l(this)
	) :
	box_node_new(stru32z("(,"), comma_list(this));
}
node* node_set(node* this) {return
	box_node_new(stru32z("(,"), comma_set(this));
}
node* node_spread(node* this) {
	if (this->as == stru32z("(,")) return lyse(this,
		borrow_l(this)
	);
	return box_node_new(stru32z("..._"), this);
}
node* node_range_forever(node* this) {
	/* This should return an iterator of some sort. */
	return box_node_new(stru32z("_.."), this);
}
node* node_range_from_zero(node* this) {
	if (ikt_ge(node_ZERO, this)) return lyse(this,
		borrow(node_EMPTY_LIST)
	);
	if (ikt_eq(this, node_ONE)) return lyse(this,
		box(borrow(node_ZERO))
	);
	if (ikt_eq(this, node_TWO)) return lyse(this,
		list(list_comma(borrow(node_ZERO), borrow(node_ONE)))
	);
	if (this->as == stru32z("num")) return
		push(this, range_from_zero(dec(this)));
	if (this->as == '\'') return
		push(this, range_from_zero(dec(this)));
	return box_node_new(stru32z(".._"), this);
}
node* node_dec(node* this) {
	if (this == node_ONE) return lyse(this,
		borrow(node_ZERO)
	);
	if (ikt_eq(this, node_ZERO)) return lyse(this,
		borrow(node_MINUS_ONE)
	);
	if (this->as == '\'' && this->len < sizeof(void*)) {
		node* rv;
		rv = lyse_and_clone(this);
		--rv->pl;
		return rv;
	}
	return sub(this, borrow(node_ONE));
}
node* node_inc(node* this) {
	if (this == node_ZERO) return lyse(this,
		borrow(node_ONE)
	);
	if (ikt_eq(this, node_MINUS_ONE)) return lyse(this,
		borrow(node_ZERO)
	);
	if (this->as == '\'' && this->len < sizeof(void*) && !~(uptr)this->pl) {
		node* rv;
		rv = lyse_and_clone(this);
		++rv->pl;
		return rv;
	}
	return add(this, borrow(node_ONE));
}
node* node_size(node* this) {
	if (ikt_eq(this, node_EMPTY_LIST)) return lyse(this,
		borrow(node_ZERO)
	);
	if (this->as == stru32z("(,")) return
		list_node_size(this);
	if (this->as == stru32z("w/")) return lyse(this,
		add(size(borrow_l(this)), size(borrow_r(this)))
	);
	if (this->as == '"') return lyse(this,
		blob_node_new('\'', &this->len, 1)
	);
	return box_node_new(stru32z("|_|"), this);
}
mininode node_mini_size(node* this) {
	mininode rv;
	if (ikt_eq(this, node_EMPTY_LIST)) return unref(this),
		mininode_ZERO;
	if (this->as == stru32z("(,")) return
		minimize(list_node_size(this), NULL);
	if (this->as == stru32z("w/")) return unref(this),
		mini_add_mini(
			mini_size(borrow_l(this)),
			mini_size(borrow_r(this)),
		NULL);
	if (this->as == '"') return
		rv = blob_mininode('\'', &this->len, 1),
		unref(this),
		rv;
	return box_mininode(stru32z("|_|"), this);
}
node* node_length(node* this) {
	if (ikt_eq(this, node_EMPTY_LIST)) return lyse(this,
		borrow(node_ZERO)
	);
	if (this->as == stru32z("(,")) return
		list_node_length(this);
	if (this->as == stru32z("w/")) return lyse(this,
		add(size(borrow_l(this)), size(borrow_r(this)))
	);
	if (this->as == '"') return
		blob_node_new('\'', &this->len, 1);
	return box_node_new(stru32z("len"), this);
}
node* node_rest(node* this) {
	if (ikt_eq(this, node_EMPTY_LIST)) return
		box_node_new(stru32z("rs"), this);
	if (this->as == stru32z("(,")) return
		list_node_rest(this);
	if (this->as == '"') {
		if (this->ref_count == 1) {
			fast_u8 i;
			--this->len;
			for (i = 0; i < this->len; i++)
				this->s[i] = this->s[i + 1];
			return this;
		}
		return blob_node_new('"', &this->s[1], this->len - 1);
	}
	return box_node_new(stru32z("rs"), this);
}
node* node_cx_block(node* this) {
	return box_node_new(stru32z("(:"), this);
}
node* node_let(node* this) {
	if (ikt_be_true(this)) return lyse(this,
		borrow(node_TRUE)
	);
	if (iknt_be_true(this)) return
		err_node_new("Contradiction!", box_node_new(stru32z("let"), this));
	return box_node_new(stru32z("let"), this);
}
node* node_have(node* this) {
	if (ikt_be_constant(this)) return
		this;
	return box_node_new(stru32z("hav"), this);
}
node* node_pub(node* this) {
	if (ikt_be_constant(this)) return
		this;
	return box_node_new(stru32z("pub"), this);
}
bool bool_xor(bool this, bool that) {
	return !!this ^ !!that;
}
bool node_ikmt_be_true(node* this, bool maybe) {
	if (ikt_eq(this, node_TRUE)) return
		true;
	if (ikt_eq(this, node_FALSE)) return
		false;
	if (this->as == '!') return
		this->as == 'm' ?
			!ikmt_be_true(*this->ms, this->sc, !maybe) :
			!ikmt_be_true(this->l, !maybe);
	if (this->as == stru32z("miop")) {
		mininode ml, mr;
		ml = this->l_type == 'm' ? this->ml : minimize(this->l, NULL);
		mr = *this->msr;
		if (this->op == '=') return
			ikmt_eq_mini(ml, mr, this->sc, maybe);
		if (this->op == stru32z(">=")) return
			ikmt_ge_mini(ml, mr, this->sc, maybe);
		if (this->op == stru32z("&&")) return
			ikmt_be_true(ml, this->sc, maybe) &&
			ikmt_be_true(mr, this->sc, maybe);
		if (this->op == stru32z("||")) return
			ikmt_be_true(ml, this->sc, maybe) ||
			ikmt_be_true(mr, this->sc, maybe);
		if (this->op == stru32z("<=>")) return bool_xor(
			!ikmt_be_true(ml, this->sc, maybe),
			 ikmt_be_true(mr, this->sc, maybe)
		);
	}
	if (this->vads && ikt_contain(this->vads, this)) return
		true;
	if (!this->as && this->op == '=') return
		ikmt_eq(borrow_l(this), borrow_r(this), maybe);
	node* l,* r;
	if (has_op_as(this)) {
		l = borrow_l(this);
		r = borrow_r(this);
		bool rv;
		rv =
		this->op == stru32z(">=") ?
			ikmt_ge(l, r, maybe) :
		this->op == ':' ?
			ikmt_be_in(l, r, maybe) :
		this->op == stru32z("&&") ?
			ikmt_be_true(l, maybe) && ikmt_be_true(r, maybe) :
		this->op == stru32z("||") ?
			ikmt_be_true(l, maybe) || ikmt_be_true(r, maybe) :
		this->op == stru32z("<=>") ?
			!ikmt_be_true(l, maybe)^!!ikmt_be_true(r, maybe) :
			maybe;
		unref(lyse(l, r));
		return rv;
	}
	return maybe;
}
bool node_ikmt_be_finite(node* this, bool maybe) {
	if (this->as == stru32z("num")) return
		true;
	if (this->as == '\'') return
		true;
	if (this->as == '"') return
		true;
	if (ikt_eq(this, node_EMPTY_LIST)) return
		true;
	if (this->as == stru32z("(,")) return
		list_node_ikmt_be_finite(this, maybe);
	node* l;
	bool rv;
	if (this->as == stru32z("-_")) {
		rv = ikmt_be_finite(l = borrow_l(this), maybe);
		unref(l);
		return rv;
	}
	if (this->as == stru32z("//_")) {
		rv = !ikmt_be_finite(l = borrow_l(this), !maybe);
		unref(l);
		return rv;
	}
	return maybe;
}
bool node_ikmt_be_constant(node* this, bool maybe) {
	return
		this->as == stru32z("num") ||
		this->as == '\'' ||
		this->as == '"' ||
		this->as == stru32z("boo") ||
		this->as == stru32z("(,") &&
			list_node_ikmt_be_constant(this, maybe) ||
		this->as == stru32z("`,") &&
			comma_ikmt_be_constant(this, maybe) ||
		maybe &&
			this->as != stru32z("v/") &&
			this->as != stru32z("v.");
}
bool node_ikmt_ze(node* this, bool maybe) {
	return
		this->as == stru32z("num") && *this->s == '0' && this->len == 1 ||
		this->as == '\'' && !this->len ||
		ikmt_eq(this, node_ZERO, maybe);
}
bool node_ikmt_have_labels(node* this, bool maybe) {
	if (!this->len) return
		false;
	if (this->as == '[') return
		true;
	if (this->as == stru32z("roo")) return
		true;
	if (
		!this->as ||
		this->as == stru32z("miop") ||
		this->as == stru32z("long") ||
		this->as == stru32z("lonr")
	)if(this->op == '.' || this->op == '/' || this->op == '#') return
		true;
	if (this->l_type == 'n') {
		if (ikt_have_labels(this->l)) return
			true;
		if (this->len < 2) return
			false;
		fast_u8 i;
		if (this->r_type == 'n')
			for (i = 1; i < this->len; i++)
				if (ikt_have_labels(this->xs[i])) return
					true;
		if (this->r_type == 'm')
			for (i = 0; i < this->len - 1; i++)
				if (ikt_have_labels(this->msr[i], this->sc)) return
					true;
		if (this->r_type == 'N')
			for (i = 0; i < this->len - 1; i++)
				if (ikt_have_labels(near(this->l, this->near_rs[i]))) return
					true;
		return false;
	}
	if (this->l_type == 'm')
		for (fast_u8 i = 0; i < this->len; i++)
			if (ikt_have_labels(this->ms[i], this->sc)) return
				true;
	return false;
}
bool node_ikmt_zelen(node* this, bool maybe) {
	node* l;
	if (ikt_eq(this, node_EMPTY_LIST)) return
		true;
	if (this->as == stru32z("(,") && this->len) {
		if (has_op_as(l = borrow_l(this)) && l->op == stru32z("`,")) return
			false;
		return maybe;
	}
	return maybe;
}
bool node_ikmt_be_empty(node* this, bool maybe) {
	if (ikt_eq(this, node_EMPTY_LIST)) return
		true;
	if (this->as == stru32z("(,")) return
		!this->len;
	if (this->as == '"') return
		!this->len;
	return maybe;
}
bool node_ikmt_have_first_value(node* this, bool maybe) {
	node* l;
	if (ikt_be_empty(this)) return
		false;
	if (this->as == '"') return
		true;
	if (
		this->as == stru32z("(,") &&
		(l = borrow_l(this))->as != stru32z("`,") &&
		l->as != stru32z("&,") &&
		l->as != stru32z("..._") && (
			!has_op_as(l) || (
				l->as != stru32z(":=") &&
				l->as != stru32z("::") &&
				l->as != stru32z(":|")
			)
		)) return
			true;
	return maybe;
}
bool node_ikmt_contain_eval_instr(node* this, bool maybe) {
	if (
		this->as == '\'' ||
		this->as == '"' ||
		this->as == stru32z("num") ||
		this->as == stru32z("boo")
	) return
		false;
	if (
		this->as == stru32z("hav") ||
		this->as == stru32z("pub") ||
		this->as == stru32z("let")
	) return
		true;
	mininode mini_ret_str, mini_yield_str, mini_yeet_str;
	mini_ret_str = leaf_mininode('"', "ret");
	mini_yield_str = leaf_mininode('"', "yield");
	mini_yeet_str = leaf_mininode('"', "yeet");
	if (
		has_op_as(this) && this->op == stru32z("v.") &&
		this->l->as == stru32z("[sc") && (
			ikt_eq_mini(this->l->l, mini_ret_str, NULL) ||
			ikt_eq_mini(this->l->l, mini_yield_str, NULL) ||
			ikt_eq_mini(this->l->l, mini_yeet_str, NULL)
		)
	) return
		maybe;
	if (this->as == '[' && (
		this->l_type == 'm' && ikt_eq_mini(this->ml, mini_ret_str, NULL) ||
		this->l_type == 'n' && ikt_eq_mini(this->l,  mini_ret_str, NULL) ||
		this->l_type == 'm' && ikt_eq_mini(this->ml, mini_yield_str, NULL) ||
		this->l_type == 'n' && ikt_eq_mini(this->l,  mini_yield_str, NULL) ||
		this->l_type == 'm' && ikt_eq_mini(this->ml, mini_yeet_str, NULL) ||
		this->l_type == 'n' && ikt_eq_mini(this->l,  mini_yeet_str, NULL)
	)) return
		maybe;
	/**
		And then you go down through all of the other things inside this
		node to see if it's true for any of them.
		(not yet implemented)
	 */
	if (this->l_type == 'n' && (
		this->len == 1 || this->r_type == 'p' || this->r_type == 's'
	)) return
		node_ikmt_contain_eval_instr(this->l, maybe);
	if (this->data_type == stru16("nn") && this->len == 2) return
		node_ikmt_contain_eval_instr(this->l, maybe) ||
		node_ikmt_contain_eval_instr(this->r, maybe);
	return maybe;
}
