#include "lee.h"
#include "shared_shortcuts.h"

node* op_node_new(u32 op, node* l, node* r) {
	return node_new(
		0, op,
		l, &r, NULL, NULL, NULL, NULL,
		2, NULL, NULL,
		NULL, NULL
	);
}
node* node_apply_opr(node* o, node* x, node* y) {
	if (o->as == stru32z("bo")) return lyse(o,
		apply_opr(borrow_r(o),
			apply_bxr(borrow_l(o), x),
			y
		)
	);
	if (o->as == stru32z("opr")) {
		node*(* f)(node*, node*);
		f = node_op_f(*(u32*)o->s);
		unref_with_note(o, "op opr");
		node* EMLI;
		EMLI = borrow(borrow(node_EMPTY_LIST));
		return f ?
			(*f)(seng(x, EMLI), seng(y, EMLI)) :
			op_node_new(*(u32*)o->s, x, y);
	}
	return op_node_new(stru32z("o "),
		o,
		box_node_new(stru32z("(,"),
			op_node_new(',', x, y)
		)
	);
}
u32 op_cat_op(u32 op) {
	if (
		op == '+' ||
		op == '*' ||
		op == stru32z(".||") ||
		op == stru32z("&&") ||
		op == stru32z("||") ||
		op == stru32z("<!>") ||
		op == stru32z("<=>") ||
		op == ' ' ||
		op == ';' ||
		op == stru32z("`,") ||
		op == stru32z("&,")
	) return
		op;
	return 0;
}
node* op_node_seng(node* this, node* vads) {
	node*(* f)(node*, node*);
	f = node_op_f(this->op);
	if (!f) return
		this;
	if (this->data_type != stru16("nn") || this->len < 2) return
		err_node_new("Not enough args to seng op node", this);
	node* sl,* sr;
	sl = seng(borrow(this->l), borrow(vads));
	sr = seng(borrow(this->r), vads);
	unref(this);
	if (sl->as == stru32z("ERR")) return
		sl;
	if (sr->as == stru32z("ERR")) return
		sr;
	return (*f)(sl, sr);
}
node*(* node_op_f(u32 op))(node*, node*) {
	#define match(x) op == x
	#define match_s op == stru32z
	return
		match_s(".||") ?  &node_cat :
		match('+') ?      &node_add :
		match('-') ?      &node_sub :
		match('*') ?      &node_mul :
		match_s("//") ?   &node_div :
		match(' ') ?      &node_apply :
		match('=') ?      &node_eq :
		match_s("&&") ?   &node_and :
		match_s("||") ?   &node_or :
		match_s("<!>") ?  &node_xor :
		match_s("==>") ?  &node_implies :
		match_s("<==") ?  &node_is_implied_by :
		match_s("<=>") ?  &node_iff :
		match_s("`;") ?   &node_roi :
		match_s("&;") ?   &node_while :
		match('|') ?      &node_gith :
		match(';') ?      &node_semic :
		match_s("=>") ?   &node_lambda :
		match_s("`,") ?   &node_list_comma :
		match_s("&,") ?   &node_set_comma :
		match_s(".|") ?   &node_pipe :
		match_s("nbas") ? &node_nbas :
		                  NULL;
	#undef match_s
	#undef match
}
node* node_add(node* this, node* that) {
	if (ikt_ze(this)) return lyse(this,
		that
	);
	if (ikt_ze(that)) return lyse(that,
		this
	);
	if (that->as == stru32z("-_")) return lyse(that,
		sub(this, borrow_l(that))
	);
	if (this->as == stru32z("-_")) return lyse(this,
		sub(that, borrow_l(this))
	);
	if (
		this->as == '\'' && that->as == '\'' &&
		this->len <= sizeof(uptr) - 1 &&
		this->len <= sizeof(uptr) - 1
	) {
		uptr z;
		u8* zs;
		fast_u8 i, zlen;
		z = *(uptr*)this->s + *(uptr*)that->s;
		zs = (u8*)&z;
		zlen = max(this->len, that->len) + 1;
		for (i = zlen; --i && !zs[i]; --zlen);
		return lyse(this, lyse(that,
			blob_node_new('\'', zs, zlen)
		));
	}
	return op_node_new('+', this, that);
}
mininode node_mini_add(node* this, node* that) {
	if (ikt_ze(this)) return unref(this),
		minimize(that, NULL);
	if (ikt_ze(that)) return unref(that),
		minimize(this, NULL);
	if (that->as == stru32z("-_")) return unref(that),
		minimize(sub(this, borrow_l(that)), NULL);
	if (this->as == stru32z("-_")) return unref(this),
		minimize(sub(that, borrow_l(this)), NULL);
	return minimize(add(this, that), NULL);
}
node* node_sub(node* this, node* that) {
	if (ikt_ze(this)) return lyse(this,
		neg(that)
	);
	if (ikt_ze(that)) return lyse(that,
		this
	);
	if (this->as == stru32z("-_")) return lyse(this,
		neg(add(borrow_l(this), that))
	);
	if (that->as == stru32z("-_")) return lyse(that,
		add(this, borrow_l(that))
	);
	if (
		ikt_eq(this, that) &&
		ikt_be_finite(this) &&
		ikt_be_finite(that)
	) return lyse(this, lyse(that,
		borrow(node_ZERO)
	));
	if (
		this->as == '\'' && that->as == '\'' &&
		this->len <= sizeof(ll_unt) - 1 &&
		that->len <= sizeof(ll_unt) - 1
	) {
		ll_int z;
		u8* zs;
		fast_u8 i, zlen;
		z = *(ll_int*)this->s - *(ll_int*)that->s;
		z = z >= 0 ?: -z;
		zs = (u8*)&z;
		zlen = max(this->len, that->len) + 1;
		for (i = zlen; --i && !zs[i]; --zlen);
		return z >= 0 ?
			blob_node_new('\'', zs, zlen) :
			neg(blob_node_new('\'', zs, zlen));
	}
	return op_node_new('-', this, that);
}
node* node_mul(node* this, node* that) {
	if (ikt_eq(this, node_ONE)) return lyse(this,
		that
	);
	if (ikt_eq(that, node_ONE)) return lyse(that,
		this
	);
	if (ikt_be_finite(this) && ikt_ze(that)) return
		lyse(that, lyse(this,
			borrow(node_ZERO)
		));
	if (ikt_be_finite(that) && ikt_ze(this)) return
		lyse(this, lyse(that,
			borrow(node_ZERO)
		));
	if (this->as == stru32z("-_")) return lyse(this,
		neg(mul(borrow_l(this), that))
	);
	if (that->as == stru32z("-_")) return lyse(that,
		neg(mul(this, borrow_l(that)))
	);
	if (this->as == stru32z("//_")) return lyse(this,
		div(that, borrow_l(this))
	);
	if (that->as == stru32z("//_")) return lyse(that,
		div(this, that)
	);
	/*if (this->as == stru32z("num") && this->as == stru32z("num")) {
		node* ni,* na;
		ni = fixed_len_numstr(borrow(this), sizeof(ll_unt));
		if (ni->as == stru32z("ERR")) return lyse(ni,
			op_node_new('*', this, that)
		);
		na = fixed_len_numstr(borrow(that), sizeof(ll_unt));
		if (na->as == stru32z("ERR")) return lyse(ni, lyse(na,
			op_node_new('*', this, that)
		));
		return lyse(this, lyse(that,
			mul(ni, na)
		));
	}*/
	if (this->as == '\'' && that->as == '\'') {
		node* rv;
		fast_u8 i;
		uptr zs;
		if (this->len + that->len > sizeof(uptr)) return
			op_node_new('*', this, that);
		zs = (*(uptr*)this->s)*(*(uptr*)that->s);
		rv = blob_node_new('\'', (u8*)&zs, this->len + that->len);
		for (i = rv->len; --i && !rv->s[i]; --rv->len);
		return lyse(this, lyse(that,
			rv
		));
	}
	if (ikt_eq(this, borrow(node_EMPTY_LIST))) return lyse(this, lyse(that,
		borrow(node_EMPTY_LIST)
	));
	if (ikt_eq(this, node_TWO)) return lyse(this,
		add(that, borrow(that))
	);
	if (ikt_eq(that, node_TWO)) return lyse(that,
		add(this, borrow(this))
	);
	if (that->as == stru32z("(,")) return
		list_node_mul(this, that);
	if (that->as == '"' && that->len == 1) return lyse(that,
		box(mul(this, blob_node_new('\'', that->s, 1)))
	);
	if (that->as == '"' && that->len == 1) return
		push(mul(this, first(that)), mul(this, rest(that)));
	return op_node_new('*', this, that);
}
node* node_div(node* this, node* that) {
	if (ikt_eq(this, node_ONE)) return lyse(this,
		inv(that)
	);
	if (ikt_eq(that, node_ONE)) return lyse(that,
		this
	);
	if (ikt_ze(this) && iknt_ze(that)) return lyse(that,
		this
	);
	if (this->as == stru32z("-_")) return lyse(this,
		neg(div(borrow_l(this), that))
	);
	if (that->as == stru32z("-_")) return lyse(that,
		neg(div(this, borrow_l(that)))
	);
	if (
		this->as == stru32z("//_") &&
		iknt_ze(this) && ikt_be_finite(this)
	) return lyse(this,
		inv(mul(borrow_l(this), that))
	);
	if (
		that->as == stru32z("//_") &&
		iknt_ze(that) && ikt_be_finite(that)
	) return lyse(that,
		mul(this, borrow_l(that))
	);
	node* til,* tir;
	til =
	tir = NULL;
	if (
		has_op_as(this) && this->op == '*' &&
		ikt_eq((tir = borrow_r(this)), that)
	) return lyse(that, lyse(this, lyse(tir,
		borrow_l(this)
	)));
	if (
		has_op_as(this) && this->op == '*' &&
		ikt_eq((til = borrow_l(this)), that)
	) return lyse(that, lyse(this, lyse(til,
		borrow_r(this)
	)));
	if (til) unref(til);
	if (tir) unref(tir);
	return op_node_new(stru32z("//"), this, that);
}
node* node_apply(node* this, node* that) {
	if (has_op_as(this) && this->op == stru32z("=>")) return lyse(this,
		seng_subst(this,
			op_node_new(stru32z("::="), borrow_l(this), borrow_r(this))
		)
	);
	return op_node_new(' ', this, that);
}
node* node_eq(node* this, node* that) {
	if (ikt_eq(this, that)) return lyse(this, lyse(that,
		borrow(node_TRUE)
	));
	if (iknt_eq(this, that)) return lyse(this, lyse(that,
		borrow(node_FALSE)
	));
	node* til,* tir;
	til =
	tir = NULL;
	if (
		has_op_as(this) && this->op == '+' &&
		ikt_be_finite(til = borrow_l(this))
	) return lyse(this,
		eq(borrow_r(this), sub(that, til))
	);
	if (
		has_op_as(this) && this->op == '+' &&
		ikt_be_finite(tir = borrow_r(this))
	) return lyse(this,
		eq(borrow_l(this), sub(that, tir))
	);
	if (
		has_op_as(this) && this->op == '-' &&
		ikt_be_finite(til = borrow_l(this))
	) return lyse(this,
		eq(neg(borrow_r(this)), sub(that, til))
	);
	if (
		has_op_as(this) && this->op == '-' &&
		ikt_be_finite(tir = borrow_r(this))
	) return lyse(this,
		eq(borrow_l(this), add(tir, this))
	);
	if (
		has_op_as(this) && this->op == '*' &&
		ikt_be_finite(til = borrow_l(this)) &&
		iknt_ze(til)
	) return lyse(this,
		eq(borrow_r(this), div(that, til))
	);
	if (
		has_op_as(this) && this->op == stru32z("//") &&
		ikt_be_finite(tir = borrow_r(this)) &&
		iknt_ze(tir)
	) return lyse(this,
		eq(borrow_r(this), mul(that, tir))
	);
	if (tir) unref(tir);
	if (til) unref(til);
	return op_node_new('=', this, that);
}
node* node_and(node* this, node* that) {
	if (ikt_be_true(this)) return lyse(this,
		that
	);
	if (ikt_be_true(that)) return lyse(that,
		this
	);
	if (iknt_be_true(this) || iknt_be_true(that)) return lyse(this, lyse(that,
		borrow(node_FALSE)
	));
	if (ikt_logi_eq(this, that)) return lyse(this,
		that
	);
	return op_node_new(stru32z("&&"), this, that);
}
node* node_or(node* this, node* that) {
	if (iknt_be_true(this)) return lyse(this,
		that
	);
	if (iknt_be_true(that)) return lyse(that,
		this
	);
	if (ikt_be_true(this) || ikt_be_true(that)) return lyse(this, lyse(that,
		borrow(node_TRUE)
	));
	if (ikt_logi_eq(this, that)) return lyse(this,
		that
	);
	return op_node_new(stru32z("||"), this, that);
}
node* node_xor(node* this, node* that) {
	if (ikt_logi_eq(this, that)) return lyse(this, lyse(that,
		borrow(node_FALSE)
	));
	if (iknt_logi_eq(this, that)) return lyse(this, lyse(that,
		borrow(node_TRUE)
	));
	return box_node_new('!', op_node_new(stru32z("<=>"), this, that));
}
node* node_implies(node* this, node* that) {
	return or(that, not(this));
}
node* node_is_implied_by(node* this, node* that) {
	return or(this, not(this));
}
node* node_iff(node* this, node* that) {
	if (ikt_logi_eq(this, that)) return lyse(this, lyse(that,
		borrow(node_TRUE)
	));
	if (iknt_logi_eq(this, that)) return lyse(this, lyse(that,
		borrow(node_FALSE)
	));
	return op_node_new(stru32z("<=>"), this, that);
}
node* node_roi(node* this, node* that) {
	return op_node_new(stru32z("`;"), this, that);
}
node* node_while(node* this, node* that) {
	return op_node_new(stru32z("&;"), this, that);
}
node* node_gith(node* this, node* that) {
	return first(cx_block(semic(let(that), this)));
}
node* node_semic(node* this, node* that) {
	if (
		!iknt_contain_eval_instr(this) ||
		this->as == '~'
	) return
		op_node_new(';', this, that);
	return lyse(this,
		that
	);
}
node* node_lambda(node* this, node* that) {
	if (ikt_be_constant(this)) return
		op_node_new(stru32z("=>"), borrow(node_VOID), that);
	return op_node_new(stru32z("=>"), this, that);
}
node* node_list_comma(node* this, node* that) {
	return op_node_new(stru32z("`,"), this, that);
}
node* node_set_comma(node* this, node* that) {
	if (ikt_eq(this, that)) return lyse(this,
		that
	);
	if (iknt_sort_ge(this, that)) return
		op_node_new(stru32z("&,"), that, this);
	return op_node_new(stru32z("&,"), this, that);
}
node* node_pipe(node* this, node* that) {
	return apply(that, this);
}
node* node_cat(node* this, node* that) {
	if (ikt_eq(this, node_EMPTY_LIST)) return
		that;
	if (ikt_eq(that, node_EMPTY_LIST)) return
		this;
	if (
		this->as == '"' && that->as == '"' &&
		this->len + that->len <= node_s_BUFLEN
	) {
		node* rv;
		u8* t;
		rv = lyse_and_clone(this);
		t = &rv->s[this->len];
		for (fast_u8 i = 0; i < that->len; i++)
			t[i] = that->s[i];
		rv->len += that->len;
		return lyse(that, rv);
	}
	if (
		this->as == '"' &&
		that->as == stru32z("(,") &&
		that->len == 1 && that->l_type == 'n'
	) {
		node* tal;
		tal = borrow_l(that);
		if (tal->as == stru32z("num") && (
			tal->len <= 2 ||
			tal->len == 3 &&
				 (100*(*tal->s & 15) +
				   10*(tal->s[1]&15) +
				       tal->s[2]&15)
				< 256
		)) return
			cat(this, fixed_len_numstr(tal, 1));
		if (tal->as == '\'') {
			if (!tal->len) return lyse(tal,
				cat(this, blob_node_new('"', (u8*)"", 0))
			);
			if (tal->len == 1) return lyse(tal,
				cat(this, blob_node_new('"', (u8*)"\0", 1))
			);
		}
		unref(tal);
		return op_node_new(stru32z(".||"), this, that);
	}
	if (this->as == stru32z("(,")) return
		list_node_cat(this, that);
	if (
		this->as == '"' &&
		has_op_as(that) &&
		that->op == stru32z(".||") && that->len >= 2 &&
		that->l->as == '"'
	) return lyse(that,
		cat(cat(this, borrow_l(this)), borrow_r(that))
	);
	if (
		!this->as && this->op == stru32z(".||") &&
		!that->as && that->op == stru32z(".||")
	) {
		/*node* xs[4] = {
			borrow(this->l), borrow(this->r),
			borrow(that->l), borrow(that->r)
		};*/
		node* xs[4];
		xs[0] = borrow(this->l);
		xs[1] = borrow(this->r);
		xs[2] = borrow(that->l);
		xs[3] = borrow(that->r);
		return lyse(this, lyse(that,
			long_op_node_new(stru32z(".||"), NULL, xs, 4)
		));
	};
	if (!this->as && this->op == stru32z(".||")) {
		/*node** xs;
		xs = (node**){
			borrow(this->l), borrow(this->r), that
		};*/
		node* xs[3];
		xs[0] = borrow(this->l);
		xs[1] = borrow(this->r);
		xs[2] = that;
		return lyse(this,
			long_op_node_new(stru32z(".||"), NULL, xs, 3)
		);
	}
	return op_node_new(stru32z(".||"), this, that);
}
node* node_believe(node* this, node* that) {
	if (has_op_as(this) && this->op == stru32z("v/")) return
		slash_var_node_believe(this, that);
	if (has_op_as(this) && this->op == stru32z("v.")) return
		dot_var_node_believe(this, that);
	if (!this->len) return lyse(that,
		this
	);
	/*if (((this->l_type + 1)&'~') == 'n') {
		node* rv,* til;
		fast_u8 i, rlen;
		rv = lyse_and_clone(this);
		rv->l = lyse(til,
			believe(til = borrow_l(this), that)
		);
		if (this->len == 1) return lyse(that,
			rv
		);
		rlen = this->len - 1;
		if (this->r_type == 'n')
			for (i = 0; i < rlen; i++)
				rv->rs[i] = believe(rv->rs[i], that);
		else if (this->r_type == 'm')
			for (i = 0; i < rlen; i++)
				rv->msr[i] = minimize(believe(rv->rs[i], that), NULL);
		} else if (this->r_type == 'N') {
			node** bers;
			bool is_far = false;
			for (i = 0; i < rlen; i++) {
				bers[i] = believe(near(this->l, this->near_rs[i]), that);
				if (!is_near(bers[i]))
					is_far = true;
			}
			if (is_far) return lyse(rv,
				long_op_node_new(this->as, this->l, bers, this->len)
			);
			for (i = 0; i < rlen; i++)
				rv->near_rs[i] = bers[i];
		}
		return lyse(that,
			rv
		);
	}*/
	return op_node_new(stru32z("bli"), this, that);
}
node* node_push(node* this, node* that) {
	if (ikt_eq(this, node_EMPTY_LIST)) return lyse(this,
		list(that)
	);
	if (this->as == stru32z("(,")) return
		list_node_push(this, that);
	if (this->as == '"' && that->as == stru32z("'")) {
		node* rv;
		rv = blob_node_new('"', this->s - 1, this->len + 1);
		*rv->s = *that->s;
		return rv;
	}
	return cat(list(that), this);
}
node* op_node_op_append(node* this, node* that) {
	node* rs[3];
	fast_u8 len;
	rs[0] = borrow(this->r);
	if (!that->as && that->op == this->op) {
		rs[1] = borrow(that->l);
		rs[2] = borrow(that->r);
		len = 4;
	} else {
		rs[1] = that;
		len = 3;
	}
	return lyse(this,
		long_op_node_new(this->op, borrow(this->l), rs, len)
	);
}
node* op_node_op_push(node* this, node* that) {
	return
		!that->as && that->op == this->op ?
			op_node_op_append(that, this)
		: lyse(this,
			long_op_node_new(this->op,
				that,
				borrow_all(this->xs, this->len),
				this->len + 1
			)
		);
}
node* node_nbas(node* this, node* that) {
	if (
		ikt_ge(leaf_node_new('\'', "$"), this) &&
		this->as == '\''
	) return lyse(this,
		numstr_in_base(that, *this->s)
	);
	return op_node_new(stru32z("nbas"), this, that);
}
bool node_bytes_eq(node* this, node* that) {
	return
		this->asop == that->asop &&
		this->data_type == that->data_type &&
		this->len == that->len &&
		this->vads == that->vads && (
			!this->sc || !that->sc ||
			this->sc == that->sc
		) && (
			!this->len ||
			!memcmp(this->s, that->s, data_byte_count(this))
		);
}
bool node_bytes_eq_mini(node* this, mininode that) {
	return
		this->asop == (u64)that.as &&
		this->l_type == that.type && (
			this->r_type == this->l_type ||
			this->r_type == '_'
		) &&
		this->len == that.len && (
			!this->len ||
			!memcmp(this->s, that.s, data_byte_count(this))
		);
}
bool node_ikmt_eq(node* this, node* that, bool maybe) {
	return
		this == that ||
		this->asop == that->asop && (
			this->l_type == 'n' && this->len == 1 &&
			that->l_type == 'n' && that->len == 1 &&
			ikt_eq(this->l, that->l)
		) ||
		bytes_eq(this, that) ?:
		this->as == stru32z("num") && that->as == '\'' ?
			ikmt_eq(numstr(this), that, maybe) :
		this->as == '\'' && that->as == stru32z("num") ?
			ikmt_eq(this, numstr(that), maybe) :
		this->as == '\'' && that->as == '\'' ||
		this->as == '"' && that->as == '"' ||
		this->as == stru32z("num") && that->as == stru32z("num") ?
			bytes_eq(this, that) :
			maybe;
}
bool node_ikmt_eq_mini(node* this, mininode that, scope* sc, bool maybe) {
	return
		that.as == '+' && this == that.l ||
		this->asop == (u64)that.as && (
			this->l_type == 'n' && this->len == 1 &&
			that.type == 'n' &&
			ikt_eq(this->l, that.l)
		) ||
		bytes_eq_mini(this, that) ?:
		this->as == stru32("num") && that.as == '\'' ?
			ikmt_eq(numstr(this), restore(that, sc), maybe) :
		this->as == '\'' && that.as == '\'' ||
		this->as == '"' && that.as == '"' ||
		this->as == stru32z("num") && that.as == stru32z("num") ?
			bytes_eq_mini(this, that) :
			maybe;
}
bool node_ikmt_logi_eq(node* this, node* that, bool maybe) {
	return
		ikt_eq(this, that) ||
		ikt_be_true(this) && ikt_be_true(that) ||
		iknt_be_true(this) && iknt_be_true(that) ||
		maybe;
}
bool node_ikmt_ge(node* this, node* that, bool maybe) {
	return
		ikt_eq(this, that) ||
		(this->as == stru32z("num") || this->as == '\'') &&
			ikt_ze(that) ?:
		this->as == '\'' && that->as == '\'' ?
			this->len >  that->len ||
			this->len == that->len &&
				*(uptr*)&this->s[this->len - sizeof(uptr)] >=
				*(uptr*)&that->s[this->len - sizeof(uptr)] :
			maybe;
}
bool node_ikmt_sort_ge(node* this, node* that, bool maybe) {
	fast_u8 kge;
	kge =
		ikt_eq(this, that) ? 1 :
		cmp(this->asop, that->asop) ?:
		cmp(this->data_type, that->data_type) ?:
		cmp(this->len, that->len) ?:
		!this->len;
	fast_u8 i = 0, len = this->len;
	switch (this->data_type) {
	case 'n'*257:
		for (; i != len; i++) {
			if (iknt_sort_ge(that->xs[i], this->xs[i])) return
				true;
			if (iknt_sort_ge(this->xs[i], that->xs[i])) return
				false;
		}
		return true;
	case 'm'*257:
	case 'n'+'m'*256:
		/*for (; i != len; i++) {
			if (iknt_sort_ge(that->ms[i], this->ms[i]), this->sc) return
				true;
			if (iknt_sort_ge(this->ms[i], that->ms[i]), this->sc) return
				false;
		}
		return true;*/
		return maybe;
	case 's'*257:
		return !memcmp(this->s, that->s, this->len);
	case 'p'*257:
		return !memcmp(this->ps, that->ps, this->len*sizeof(void*));
	case 'n'+'s'*256:
		return
			iknt_sort_ge(that->l, this->l) ||
			!iknt_sort_ge(this->l, that->l) &&
			!memcmp(this->sr, that->sr, this->len - 1);
	case 'n'+'p'*256:
		if (iknt_sort_ge(that->l, this->l)) return
			true;
		if (iknt_sort_ge(this->l, that->l)) return
			false;
		return !memcmp(this->ps, that->ps, (this->len - 1)*sizeof(void*));
	case 'n'+'N'*256:
		if (iknt_sort_ge(that->l, this->l)) return
			true;
		if (iknt_sort_ge(this->l, that->l)) return
			false;
		for (; i != len; i++) {
			if (iknt_sort_ge(near_rs_(that, i), near_rs_(this, i))) return
				true;
			if (iknt_sort_ge(near_rs_(this, i), near_rs_(that, i))) return
				false;
		}
		return true;
	default:
		return true;
	}
}
bool node_ikmt_contain(node* this, node* that, bool maybe) {
	return ikmt_be_in(that, this, maybe);
}
bool node_ikmt_be_in(node* this, node* that, bool maybe) {
	return
		ikt_eq(this, first(borrow(that))) ||
		ikt_be_in(this, rest(that)) ||
		maybe &&
			!ikt_be_empty(that);
}
bool node_ikmt_define(node* this, node* that, bool maybe) {
	return maybe;
}
