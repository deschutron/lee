#include "lee.h"
#include "shared_shortcuts.h"

node* mini_op_node_new(u32 op, mininode l, mininode r) {
	mininode ms[2];
	ms[0] = l;
	ms[1] = r;
	return nm_node_new(
		stru32z("miop"), op,
		NULL, ms,
		bytes_nz(l) + bytes_nz(r)
	);
}
node* mini_op_node_new_from_nodes(u32 op, node* l, node* r) {
	mininode ms[2];
	fast_u8 i = 0;
	node* l_arg = NULL;
	if (l) {
		if (can_be_minimized(l, NULL))
			ms[i++] = minimize(l, NULL);
		else
			l_arg = l;
	}
	if (r)
		ms[i] = minimize(r, NULL);
	return nm_node_new(
		stru32z("miop"), op,
		l_arg, ms,
		!!l + !!r
	);
}
mininode mininode_mini_add(mininode this, scope* sc, node* that) {
	if (ikt_ze(this, sc)) return unref(this),
		minimize(that, borrow(sc));
	if (ikt_ze(that)) return unref(that),
		this;
	return mini_add(restore(this, sc), that);
}
mininode mininode_mini_add_mini(mininode this, mininode that, scope* sc) {
	if (ikt_ze(this, sc)) return unref(this),
		that;
	if (ikt_ze(that, sc)) return unref(that),
		this;
	if (that.as == stru32z("-_")) return unref(that),
		minimize(sub(restore(that, sc), this.l), sc);
	if (this.as == stru32z("-_")) return unref(that),
		minimize(sub(this.l, restore(that, sc)), sc);
	if (
		this.as == '\'' && that.as == '\'' &&
		this.len < sizeof(uptr) && that.len < sizeof(uptr)
	) {
		uptr z;
		u8* zs;
		fast_u8 i, zlen;
		z = (uptr)this.p + (uptr)that.p;
		zs = (u8*)&z;
		zlen = sizeof(uptr);
		for (i = zlen; --i && !zs[i]; --zlen);
		return blob_mininode('\'', zs, zlen);
	}
	return minimize(mini_op_node_new('+', this, that), sc);
}
node* mininode_add_mini(mininode this, mininode that, scope* sc) {
	if (ikt_ze(this, sc)) return unref(this),
		restore(that, sc);
	if (ikt_ze(that, sc)) return unref(that),
		restore(this, sc);
	if (that.as == stru32z("-_")) return lyse(that,
		sub(restore(that, sc), this.l)
	);
	if (this.as == stru32z("-_")) return lyse(that,
		sub(this.l, restore(that, sc))
	);
	if (
		this.as == '\'' && that.as == '\'' &&
		this.len < sizeof(uptr) && that.len < sizeof(uptr)
	) {
		uptr z;
		u8* zs;
		fast_u8 i, zlen;
		z = (uptr)this.p + (uptr)that.p;
		zs = (u8*)&z;
		zlen = sizeof(uptr);
		for (i = zlen; --i && !zs[i]; --zlen);
		return blob_node_new('\'', zs, zlen);
	}
	return mini_op_node_new('+', this, that);
}
node*(* mininode_op_f_mini(u32 op))(mininode, mininode, scope*) {
	#define match(x) op == x
	#define match_s op == stru32z
	return
		match('+') ?  &mininode_add_mini :
		              NULL;
	#undef match_s
	#undef match
}
node* mini_op_node_seng(node* this, node* vads) {
	node*(* f)(mininode, mininode, scope*);
	f = mininode_op_f_mini(this->op);
	if (!f) return
		op_node_seng(
			op_node_new(this->op,
				this->l_type == 'n' ? this->l : restore(this->ml, this->sc),
				restore(*this->msr, this->sc)
			),
			vads
		);
	if (this->len < 2) return
		err_node_new("Not enough args to seng mini op node", this);
	mininode sl, sr;
	scope* sc;
	sl = mini_seng(borrow(this->ml), this->sc, vads);
	sr = mini_seng(borrow(*this->msr), this->sc, vads);
	sc = borrow(this->sc);
	unref(this);
	if (sl.as == stru32z("ERR")) return
		restore(sl, sc);
	if (sr.as == stru32z("ERR")) return
		restore(sr, sc);
	return (*f)(sl, sr, sc);
}
bool mininode_ikmt_eq(mininode this, scope* sc, node* that, bool maybe) {
	if (
		this.as == that->as &&
		this.type == that->l_type &&
		this.len == that->len
	) return
		true;
	return ikmt_eq(restore(this, sc), that, maybe);
}
bool mininode_ikmt_ge(mininode this, scope* sc, node* that, bool maybe) {
	if (
		this.as == that->as &&
		this.type == that->l_type &&
		this.len == that->len
	) return
		true;
	if (
		this.as == '\'' && that->as == '\'' &&
		this.len >= that->len
	) return
		(uptr)this.p >= (uptr)*that->ps;
	return ikmt_ge(restore(this, sc), that, maybe);
}
bool mininode_ikmt_eq_mini(
	mininode this, mininode that, scope* sc, bool maybe
) {
	if (this.footer == that.footer && this.p == that.p) return
		true;
	return ikmt_eq(this, sc, restore(that, sc), maybe);
}
bool mininode_ikmt_ge_mini(
	mininode this, mininode that, scope* sc, bool maybe
) {
	if (this.footer == that.footer && this.p == that.p) return
		true;
	if (
		ikt_ze(this, sc) &&
		(that.as == stru32z("num") || that.as == '\'')
	) return
		true;
	if (this.as == '\'' && that.as == '\'') return
		(uptr)this.p >= (uptr)that.p;
	return ikmt_ge(restore(this, sc), restore(that, sc), maybe);
}
bool mininode_ikmt_sort_ge_mini(
	mininode this, mininode that, scope* sc, bool maybe
) {
	return
		this.footer > that.footer ?
			true :
		that.footer > this.footer ?
			false :
		this.p > that.p;
}
