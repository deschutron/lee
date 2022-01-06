#include "lee.h"
#include "shared_shortcuts.h"

node* node_new(
	u32 as, u32 op,
	node* l, node** xs, u8* s, void** ps, mininode* ms, u16* near_xs,
	fast_u8 len, void*(* p_borrow)(void*), void*(* p_unref_from)(void*, void*),
	scope* sc, node* vads
) {
	node* rv;
	if (s && len > node_s_BUFLEN - !!l*(sizeof(node*) - 1)) return
		blob_cat_op(as) ?
			op_node_new(blob_cat_op(as),
				node_new(
					as, op,
					l, xs, s, ps, ms, near_xs,
					node_s_BUFLEN, p_borrow, p_unref_from,
					sc, vads
				),
				node_new(
					as, op,
					l, xs, &s[node_s_BUFLEN], ps, ms, near_xs,
					len - node_s_BUFLEN, p_borrow, p_unref_from,
					sc, vads
				)
			) :
			assert(false),
			err_node_new("Too much data for blob n",
				blob_node_new(as, s, node_s_BUFLEN)
			);
	if (xs && len > node_xs_BUFLEN) return
		as == stru32z("long") && op_cat_op(op) ?
			op_node_new(op_cat_op(op),
				node_new(
					as, op,
					l, xs, s, ps, ms, near_xs,
					node_xs_BUFLEN, p_borrow, p_unref_from,
					sc, vads
				),
				len - node_xs_BUFLEN == 1 ?
					xs[node_xs_BUFLEN] :
					node_new(
						len - node_xs_BUFLEN == 2 ? 0 : as, op,
						l, &xs[node_xs_BUFLEN], s, ps, ms, near_xs,
						len - node_xs_BUFLEN, p_borrow, p_unref_from,
						sc, vads
					)
			) :
			err_node_new("Too many sub-nodes", NULL);
	if (ps && len > node_ps_BUFLEN) return
		err_node_new("Too many pointers", NULL);
	if (ms && len > node_ms_BUFLEN) return
		err_node_new("Too many mininodes", NULL);
	if (near_xs && len > node_near_rs_BUFLEN) return
		err_node_new("Too many nearby nodes", NULL);
	fprintf(stderr, "▲");
	rv = malloc(sizeof(node));
	rv->mem_type = 'h';
	rv->ref_count = 1;
	*(u32*)&rv->s[node_s_BUFLEN - 16] = stru32("new");
	return make(rv,
		as, op,
		l, xs, s, ps, ms, near_xs,
		len, p_borrow, p_unref_from,
		sc, vads
	);
}
node* node_make(node* this,
	u32 as, u32 op,
	node* l, node** xs, u8* s, void** ps, mininode* ms, u16* near_xs,
	fast_u8 len, void*(* p_borrow)(void*), void*(* p_unref_from)(void*, void*),
	scope* sc, node* vads
) {
	assert(this->ref_count == 1);
	this->as = as;
	this->op = op;
	this->len = len;
	this->sc = sc;
	this->vads = NULL;
	this->l = l;
	this->l_type =
		!len ? '_' :
		l    ? 'n' :
		xs   ? 'n' :
		s    ? 's' :
		ps   ? 'p' :
		ms   ? 'm' :
		0;
	if (!this->l_type) return
		err_node_make(this, "Unsupported node type", clone(this));
	this->r = NULL;
	this->r_type =
		len < 2 ? '_' :
		!!xs + !!s + !!ps + !!ms + !!near_xs >= 2 ?
		           0  :
		xs      ? 'n' :
		s       ? 's' :
		ps      ? 'p' :
		ms      ? 'm' :
		near_xs ? 'N' :
		           0;
	if (!this->r_type) return
		err_node_make(this, "Unsupported node type", clone(this));
	// make nm nodes work as mm nodes by making *this->ms be nop(this->l)
	if (this->data_type == stru16("nm"))
		this->ml.footer = mininode_NOP_FOOTER;
	#define copy_them(ys, ysr, T) \
		memcpy(\
			l ?\
				this->ysr :\
				this->ys,\
			ys,\
			sizeof(T)*min(\
				len - !!l,\
				l ?\
					node_##ysr##_BUFLEN :\
					node_##ys##_BUFLEN\
			)\
		)
	if (xs)
		copy_them(xs, rs,  node*);
	if (s)
		copy_them(s,  sr,  char);
	if (ps) {
		copy_them(ps, psr, void*);
		this->p_borrow = p_borrow;
		this->p_unref_from = p_unref_from;
	}
	if (ms)
		copy_them(ms, msr, mininode);
	if (near_xs)
		memcpy(
			this->near_rs,
			near_xs,
			2*min(len - !!l, node_near_rs_BUFLEN)
		);
	#undef copy_them
	return this;
}
node* ns_node_new(u32 as, u32 op, node* l, u8* s, fast_u8 len) {
	return node_new(as, 0,
		l, NULL, s, NULL, NULL, NULL,
		len, NULL, NULL,
		NULL, NULL
	);
}
node* ns_node_make(node* this, u32 as, u32 op, node* l, u8* s, fast_u8 len) {
	return node_make(this,
		as, 0,
		l, NULL, s, NULL, NULL, NULL,
		len, NULL, NULL,
		NULL, NULL
	);
}
node* err_node_new(char* msg, node* clue) {
	return ns_node_new(stru32z("ERR"), 0,
		clue, (u8*)msg, 1 + strnlen(msg, node_s_BUFLEN + 1)
	);
}
node* err_node_make(node* this, char* msg, node* clue) {
	return ns_node_make(this,
		stru32z("ERR"), 0,
		clue, (u8*)msg, strnlen(msg, node_s_BUFLEN) + 1
	);
}
node* p_box_node_new(
	u32 as,
	void* p,
	void*(* p_borrow)(void*), void*(* p_unref_from)(void*, void*)
) {
	return pp_node_new(
		as, 0,
		&p,
		1, p_borrow, p_unref_from
	);
}
node* pp_node_new(
	u32 as, u32 op,
	void** ps,
	fast_u8 len, void*(* p_borrow)(void*), void*(* p_unref_from)(void*, void*)
) {
	return node_new(
		as, op,
		NULL, NULL, NULL, ps, NULL, NULL,
		len, p_borrow, p_unref_from,
		NULL, NULL
	);
}
node* np_node_new(
	u32 as, u32 op,
	node* l, void** psr,
	fast_u8 len, void*(* p_borrow)(void*), void*(* p_unref_from)(void*, void*)
) {
	return node_new(
		as, op,
		l, NULL, NULL, psr, NULL, NULL,
		len, p_borrow, p_unref_from,
		NULL, NULL
	);
}
node* nN_node_new(u32 as, u32 op, node* l, u16* near_xs, fast_u8 len) {
	return node_new(
		as, op,
		l, NULL, NULL, NULL, NULL, near_xs,
		len, NULL, NULL,
		NULL, NULL
	);
}
node* nm_node_new(u32 as, u32 op, node* l, mininode* ms, fast_u8 len) {
	return node_new(
		as, op,
		l, NULL, NULL, NULL, ms, NULL,
		len, NULL, NULL,
		NULL, NULL
	);
}
node* node_borrow(node* this) {
	fprintf(stderr, "△");
	assert(this);
	assert(!((uptr)this->sc&1));
	assert(this->as != stru32z("[sc") ||
		(void*)this->p_unref_from == (void*)scope_unref_from
	);
	if (this->mem_type == 'S') return
		this;
	if (!this->ref_count) {
		fprintf(stderr, "borrowing unrefd node\n");
		fflush(stderr);
		print_bytes(this);
		assert(this->ref_count);
	}
	if (!~this->ref_count) return
		clone(this);
	/** If it's a non-borrowable pointer node,
	    return a new nop node pointing to it. */
	if (this->l_type == 'p' || this->r_type == 'p')
		if (!this->p_borrow && this->p_unref_from) return
			box_node_new(stru32z("+_"), this);
	this->ref_count++;
	return this;
}
static void write_unref_note(node* this, char* note) {
	int offset;
	offset = max(0,
		(int)data_byte_count(this) - (node_s_BUFLEN - 8)
	);
	if (offset < 8)
		strncpy(
			(char*)&this->s[node_s_BUFLEN - 8 + offset],
			&note[offset],
			8 - offset
		);
}
void node_flush_freeands() {
	node_unref_with_note_from(NULL, (char*)(u32*)&"flus", (void*)3);
}
static void free_them(node** freeands, const fast_u8 count) {
	if (!count) return;
	strcpy((char*)&(*freeands)->s[node_s_BUFLEN - 16], "freed");
	free(*freeands);
	free_them(&freeands[1], count - 1);
}
node* node_unref_with_note_from(node* this, char* note, void* start) {
	static const fast_u8 freeand_buflen = 16;
	static node* freeands[freeand_buflen] = {
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
	};
	static fast_u8 freeand_count = 0;
	assert(this);
	assert(this->as != stru32z("[sc") ||
		(void*)this->p_unref_from == (void*)scope_unref_from
	);
	if (!this) {
		/*
			This is a sneaky bit here.
			I (deschutron) would like to have a way to "flush" the list of
			things to be freed - free them all at once.
			This is my idea for performance critical code to
			get collection out of its way - first it flushes the freeands list,
			so it's not interrupted by calls to free
			until that list is full again.
			But the freeands list is stored as static vars in this method,
			to keep them close by where they're needed, so then
			I'm tunnelling the flush message through as otherwise-invalid
			values for this method's parameters.
		*/
		if ((uptr)start == 3 && *(u32*)note == stru32z("flus")) return
			free_them(freeands, freeand_count),
			err_node_new("Nodes freed", NULL);
		fprintf(stderr, "No node to unref! (%s)\n", note);
		return err_node_new("No node to unref!", leaf_node_new('"', note));
	}
	if ((void*)this == start) return
		this;
	if (!this->ref_count) {
		fprintf(stderr, "double freeing! (note = \"%s\")\n", note);
		print_bytes(this);
		assert(this->ref_count);
	}
	if (this->mem_type == 'S' || -- this->ref_count) {
		fprintf(stderr, "▽");
		return this;
	}
	fprintf(stderr, "▼");
	node* stot;
	node* tail_call_l = NULL;
	fast_u8 i, rlen;
	stot = start ?: this;
	if (this->l_type == 'n') {
		tail_call_l = this->l;
		rlen = this->len - 1;
		if (this->r_type == 'n')
			for (i = 0; i < rlen; i++)
				unref_with_note_from(this->rs[i], note ?: "rs unr", stot);
		else if (this->r_type == 'p' && this->p_unref_from)
			for (i = 0; i < rlen; i++)
				(*this->p_unref_from)(this->psr[i], start);
		else if (this->r_type == 'N')
			for (i = 0; i < rlen; i++)
				unref_with_note_from(
					near(this->l, this->near_rs[i]),
				"Nrsunr", stot);
		else if (this->r_type == 'm')
			for (i = 0; i < rlen; i++)
				unref_with_note_from(this->msr[i], note ?: "mrsunr", stot);
	} else if (this->l_type == 'p' && this->p_unref_from)
		for (i = 0; i < this->len; i++)
			(*this->p_unref_from)(this->ps[i], start);
	  else if (this->r_type == 'm')
		for (i = 0; i < this->len; i++)
			unref_with_note_from(this->ms[i], note ?: "ms unr", stot);
	if (this->vads)
		unref_from(this->vads, stot);
	if (this->sc)
		unref_from(this->sc, stot);
	if (note)
		write_unref_note(this, note);
	strcpy((char*)&this->s[node_s_BUFLEN - 16], "freeand");
	if (this->mem_type == 's') return
		NULL;
	freeands[freeand_count++] = this;
	if (freeand_count == freeand_buflen) {
		free_them(freeands, freeand_buflen);
		freeand_count = 0;
	}
	if (!tail_call_l) return
		NULL;
	return unref_with_note_from(tail_call_l, note ?: "l  unr", stot);
}
node* node_unref_with_note(node* this, char* note) {
	return unref_with_note_from(this, note, NULL);
}
node* node_unref_from(node* this, void* start) {
	return unref_with_note_from(this, NULL, start);
}
node* node_unref(node* this) {
	return unref_with_note_from(this, NULL, NULL);
}
node* node_lyse(node* this, node* that) {
	unref_with_note_from(this, "lysed", NULL);
	return that;
}
mininode node_mini_lyse(node* this, mininode that) {
	unref_with_note_from(this, "mlysd", NULL);
	return that;
}
node* node_lyse_with_note(node* this, node* that, char* note) {
	unref_with_note_from(this, note, NULL);
	return that;
}
#define mut
static node* clone_r_data(node* this, mut node* that) {
	fast_u8 i;
	that->r = NULL;
	if (this->r_type == '_');
	else if (this->r_type == 'n')
		for (i = 1; i < this->len; i++)
			that->xs[i] = borrow(this->xs[i]);
	else if (this->r_type == 's') {
		for (i = 0; i < this->len - 1; i++)
			that->sr[i] = this->sr[i];
		if (i != node_sr_BUFLEN)
			that->sr[i] = 0;
	}else if(this->r_type == 'p')
		for (i = 0; i < this->len - 1; i++)
			that->psr[i] = this->p_borrow ?
				(*this->p_borrow)(this->psr[i]) :
				this->psr[i];
	else if (this->r_type == 'm')
		for (i = 0; i < this->len - 1; i++)
			that->msr[i] = borrow(this->msr[i]);
	else if (this->r_type == 'N')
		for (i = 0; i < this->len - 1; i++)
			that->near_rs[i] = nearby_addr(
				borrow(near(this->l, this->near_rs[i]))
			);
	else return
		err_node_new("clone_r_data: nup", that);
	return that;
}
static node* clone_data(node* this, mut node* that) {
	if (this->data_type == stru16("__"))
		that->l =
		that->r = NULL;
	  else if (this->l_type == 'n') {
		that->l = borrow(this->l);
		return clone_r_data(this, that);
	} else if (this->l_type == 's') {
		fast_u8 i;
		if (this->len < 2*sizeof(node*))
			that->l =
			that->r = NULL;
		for (i = 0; i < (this->len+3)/4; i++)
			((u32*)that->s)[i] = ((u32*)this->s)[i];
		if (this->len != node_s_BUFLEN)
			that->s[this->len] = 0;
	} else if (this->l_type == 'p')
		for (fast_u8 i = 0; i < this->len; i++)
			that->ps[i] = this->p_borrow ?
				(*this->p_borrow)(this->ps[i]) :
				this->ps[i];
	  else if (this->l_type == 'm')
		for (fast_u8 i = 0; i < this->len; i++)
			that->ms[i] = borrow(this->ms[i]);
	  else return
		err_node_new("clone_data: nup", that);
	return that;
}
node* node_clone(node* this) {
	node* rv;
	fprintf(stderr, "◮");
	rv = malloc(sizeof(node));
	rv->asop = this->asop;
	rv->data_type = this->data_type;
	rv->len = this->len;
	rv->mem_type = 'h';
	rv->ref_count = 1;
	rv->sc = this->sc;
	rv->vads = this->vads;
	if (this->l_type == 'p' || this->r_type == 'p') {
		rv->p_borrow = this->p_borrow;
		rv->p_unref_from = this->p_unref_from;
	} else
		*(u16*)&rv->s[node_s_BUFLEN - 16] = stru16("cl");
	return clone_data(this, rv);
}
node* node_lyse_and_clone(node* this) {
	return this->ref_count == 1 && this->mem_type != 'S' ?
		this :
		lyse_with_note(this, clone(this), "l&c");
}
fast_u8 node_data_unit_byte_count(u8 type) {
	return
		type == '_' ? 0 :
		type == 's' ? 1 :
		type == 'N' ? 2 :
		type == 'n' ? sizeof(node*) :
		type == 'p' ? sizeof(void*) :
		type == 'm' ? sizeof(mininode) :
		node_s_BUFLEN;
}
fast_u8 node_data_byte_count(node* this) {
	return
		!this->len ?
			0 :
		this->l_type == this->r_type || this->r_type == '_' ?
			this->len*node_data_unit_byte_count(this->l_type) :
			node_data_unit_byte_count(this->l_type) +
				(this->len - 1)*node_data_unit_byte_count(this->r_type);
}
bool node_can_be_minimized(node* this, scope* sc) {
	return
		(!this->sc || this->sc == sc) &&
		!this->op &&
		data_byte_count(this) < sizeof(void*) && (
			this->l_type != 'p' ||
			!this->p_borrow && !this->p_unref_from
		);
}
mininode node_minimize(node* this, scope* sc) {
	mininode rv;
	rv =
		this->sc && this->sc != sc ?
			box_mininode(stru32z("+_"), borrow(this)) :
		!this->len ?
			empty_mininode(this->as) :
		this->l_type == 'n' ?
			box_mininode(this->as, borrow(this->l)) :
		this->l_type == 's' ?
			blob_mininode(this->as, this->s, this->len) :
		this->l_type == 'p' && !this->p_unref_from && !this->p_borrow ?
			mininode_of(this->as, NULL, NULL, *this->ps, 1) :
			box_mininode(stru32z("+_"), borrow(this));
	unref(this);
	return rv;
}
void node_print_type_name(node* this) {
	char* ch_view;
	if (!this->asop) return
		(void)printf("[0]");
	if (this->as) {
		if (u64_can_be_printed_like_identifier(this->asop))
			printf("%.8s", (char*)&this->asop);
		else if (u64_can_be_printed(this->asop))
			printf("[\"%.8s\"]", (char*)&this->asop);
		else
			printf("[%lu]", this->asop);
	} else if (u32_can_be_printed(this->op))
		printf("op[\"%.4s\"]", (char*)&this->op);
	else
		printf("op[%u]", this->op);
}
void node_print(node* this) {
	static int depth = 0;
	fast_u8 i;
	if (!this) return
		(void)printf("ₒ");
	if (++depth > 1e2) {
		fprintf(stderr, "too deep in printing (%d)\n", depth);
		depth = 0;
		return;
	}
	print_type_name(this);
	putchar(40);
	if (this->l_type == 's') {
		u8 b;
		fast_u8 usl;
		usl = 0;
		putchar('"');
		for (i = 0; i < min(this->len, node_s_BUFLEN); i++) {
			b = (u8)this->s[i];
			if (
				b < 32 ||
				b == 127 ||
				!usl && b >= 128 && b < 192 ||
				usl && b >= 192
			) {
				printf("\x1b[32m");
				print_rimek_byte(b);
				printf("\x1b[0m");
			} else
				putchar(b);
			usl = usl ? usl - 1 :
				(b >= 192) +
				(b >= 224) +
				(b >= 240);
		}
		putchar('"');
	} else if (this->l_type == 'n') {
		print(this->l);
		if (this->r_type != '_')
			putchar(',');
		if (this->r_type == 'n')
			for (i = 1; i != this->len; i++)
				print(this->xs[i]),
				i != this->len - 1 && printf(", ");
		else if (this->r_type == 's') 
			printf("\"%s\"", this->sr);
		else if (this->r_type == 'p') {
			printf("object%s at ", this->len == 2 ? "" : "s");
			for (i = 0; i != this->len - 1; i++)
				printf("%lu", (u64)this->psr[i]),
				i != this->len - 1 && printf(", ");
		}else if(this->r_type == 'm')
			for (i = 0; i != this->len - 1; i++)
				print(this->msr[i], this->sc);
		else if (this->r_type == 'N')
			for (i = 0; i != this->len - 1; i++)
				print(near(this->l, this->near_rs[i]));
	} else if (this->l_type == 'p')
		printf("object at %lu", (u64)this->ps[0]);
	 else if  (this->l_type == 'm')
		for (i = 0; i != this->len - 1; i++)
			print(this->ms[i], this->sc);
	if (this->sc) {
		putchar('|');
		if ((uptr)this->sc&1) 
			printf("\x1b[31;1m");
		putchar('S'+(4*((u8)this->sc& 8)));
		putchar('C'+(2*((u8)this->sc&16)));
		if ((uptr)this->sc&1) 
			printf("\x1b[0m");
	}
	putchar(41);
	--depth;
}
void node_print_bytes_with_indent_sc(
	node* this, int indent, scope* prev_sc
) {
	static int depth = 0;
	u8* bys;
	scope* next_sc;
	fast_u8 i;
	bys = (u8*)this;
	if (++depth > 1e2)
		return (void)fprintf(stderr, "too deep in printing\n");
	printf("\x1b[36m");
	for (i = 0; i < min((int)sizeof(node*), 15 - indent); i++)
		print_rimek_byte(((char*)&this)[i]);
	printf("\x1b[0m");
	for (; i < 15 - indent; i++)
		putchar(32);
	for (fast_u8 i = 0; i < indent; i++)
		printf("░");
	putchar(32);
	if (!this)
		return (void)putchar(10);
	next_sc = this->sc ?: prev_sc;
	for (i = 0; i < sizeof(node); i++)
		print_rimek_byte(bys[i]);
	putchar(10);
	i = 0;
	if (this->l_type == 'n' && this->len) {
		print_bytes_with_indent_sc(this->l, indent + 1, next_sc);
		if (this->r_type == 'n')
			for (++i; i < this->len; i++)
				print_bytes_with_indent_sc(this->xs[i], indent + 1, next_sc);
		if (this->r_type == 'p' && this->len >= 2 &&
			this->p_unref_from == (void* (*)(void*, void*))&scope_unref_from
		)
			for (; i < this->len - 1; i++) {
				scope* r_sc;
				r_sc = (scope*)this->psr[i];
				if (r_sc != prev_sc) {
					print_bytes_with_indent(r_sc, indent + 1);
					next_sc = next_sc ?: r_sc;
				}
			}
	}
	if (this->sc && this->sc != prev_sc)
		print_bytes_with_indent(this->sc, indent + 1);
	if (this->r_type != 'p' && this->vads && this->vads != node_EMPTY_LIST)
		print_bytes_with_indent_sc(this->vads, indent + 1, next_sc);
	--depth;
}
void node_print_bytes_with_indent(node* this, int indent) {
	node_print_bytes_with_indent_sc(this, indent, NULL);
}
void node_print_bytes(node* this) {
	print_bytes_with_indent(this, 0);
}
node* node_with_scope(node* this, scope* sc) {
	node* rv;
	if (!sc) return
		this;
	rv = lyse_and_clone(this);
	rv->sc = sc;
	return rv;
}
node* node_in_scope(node* this, scope* sc) {
	node* rv;
	if (!sc) return
		this;
	rv = lyse_and_clone(this);
	rv->sc = this->sc ?
		in_scope(rv->sc, borrow(sc)) :
		sc;
	if (this->l_type == 'n')
		rv->l = in_scope(rv->l, borrow(sc));
	if (this->r_type != 'n') return
		rv;
	for (fast_u8 i = 1, len = this->len; i < len; i++)
		rv->xs[i] = in_scope(rv->xs[i], borrow(sc));
	return rv;
}
bool node_is_in_scope(node* this, scope* sc) {
	return is_in(this->sc, sc);
}
node* node_borrow_l(node* this) {
	assert(this->ref_count);
	return
		this->l_type == 'n' ?
			borrow(this->l) :
		this->l_type == 'm' ?
			restore(*this->ms, this->sc) :
			NULL;
}
node* node_borrow_r(node* this) {
	return
		this->len < 2 ?
			NULL :
		this->r_type == 'n' ?
			this->as == stru32z("long") ?
				long_op_node_new(this->op, NULL,
					borrow_all(&this->xs[1], this->len - 1),
					this->len - 1
				) :
				borrow(this->r) :
		this->l_type == 'n' ?
			this->r_type == 'm' ?
				restore(*this->msr, this->sc) :
			this->r_type == 'N' ?
				lonr_op_node_borrow_r(this) :
				NULL :
		this->data_type == stru16("mm") ?
			restore(this->ms[1], this->sc) :
		NULL;
}
node* node_seng(node* this, node* vads) {
	assert(vads);
	if (!this) return
		err_node_new("nothing to seng", lyse(vads, NULL));
	if (!this->asop) return
		err_node_new("senging meaningless node", lyse(vads, this));
	if (!this->as && (
		this->op == stru32z("v/") || this->op == stru32z("v."))
	) return
		node_seng_subst_all(this, vads);
	if (!this->op) {
		if (this->l_type == 'n' && node_box_f(this->as)) return
			box_node_seng(this, vads);
		if (this->l_type == 'm' && (
			node_mini_box_f(this->as) ||
			node_box_f(this->as)
		)) return
			mini_box_node_seng(this, vads);
		if (this->l_type == 's') return
			blob_node_seng(this, vads);
	}
	if (this->as == stru32z("long") && node_op_f(this->op)) return
		long_op_node_seng(this, vads);
	if (this->as == stru32z("lonr") && node_op_f(this->op)) return
		lonr_op_node_seng(this, vads);
	if (this->as == stru32z("miop")) return
		mini_op_node_seng(this, vads);
	if (!this->as && node_op_f(this->op)) return
		op_node_seng(this, vads);
	unref(vads);
	return this;
}
node* node_seng_subst_all(node* this, node* vads) {
	printf("sa\n");
	printf("vads = ");
	print(vads);
	puts("");
	if (ikt_be_empty(vads)) return lyse(vads,
		this
	);
	return node_seng_subst_all(
		node_seng_subst(this, first(borrow(vads))),
		rest(vads)
	);
}
node* node_seng_subst(node* this, node* vad) {
	if (ikt_eq(vad, node_TRUE)) return
		this;
	if (ikt_eq(vad, node_FALSE)) return
		err_node_new("Contradiction!", this);
	if (!has_op_as(vad)) {
		print_bytes(vad);
		assert(has_op_as(vad));
	}
	printf("su");
	bool is_n;
	node* vad_l;
	vad_l = borrow_l(vad);
	if (
		vad->op == stru32z("::=") &&
		ikt_eq(this, vad_l)
	) return lyse(this, lyse(vad, lyse(vad_l,
		borrow_r(vad)
	)));
	if (
		(vad->op == stru32z(":::") || vad->op == stru32z("::|")) &&
		ikt_eq(this, vad_l)
	) return lyse(this, lyse(vad, lyse(vad_l,
		borrow_r(vad)
	)));
	return lyse(vad_l, with_vad(this, vad));
}
node* node_with_vad(node* this, node* vad) {
	node* rv;
	rv = lyse_and_clone(this);
	rv->vads = rv->vads ?
		push(rv->vads, vad) :
		list(vad);
	return rv;
}
node* node_vars(node* this) {
	if (this->as == stru32z("hav")) return lyse(this,
		node_had_vars(borrow_l(this), borrow(this->sc))
	);
	if (this->as == stru32z("pub")) return lyse(this,
		node_had_vars(borrow_l(this), borrow(global_scope()))
	);
	if (this->as == stru32z("ERR")) return lyse(this,
		borrow(node_EMPTY_LIST)
	);
	fast_u8 i;
	if (this->l_type == 'n') {
		if (((this->r_type + 1)&'~') == 'n') return lyse(this,
			cat(vars(borrow(this->l)), vars(borrow_r(this)))
		);
		return lyse(this,
			vars(borrow(this->l))
		);
	}
	if (this->l_type == 'm') {
		if (this->r_type == 'm') return lyse(this,
			cat(vars(borrow_l(this)), vars(borrow_r(this)))
		);
		return lyse(this,
			vars(borrow_l(this))
		);
	}
	return borrow(node_EMPTY_LIST);
}
static node* had_vars_recur(node* this, node* vars) {
	if (this->as == '[') return
		push(vars,
			var_node_new(
				borrow_l(this),
				borrow(this->sc),
				borrow((this->sc ?: global_scope())->global),
				'/'
			)
		);
	if (this->op == stru32z("v/") || this->op == stru32z("v.")) return
		push(vars, this);
	node* virs;
	/*
		_ n s p m N */
	/* __ */
	if (!this->len) return
		vars;
	/* n_ */
	if (this->len == 1 && this->l_type == 'n') return
		had_vars_recur(borrow(this->l), vars);
	/* m_ */
	if (this->len == 1 && this->l_type == 'm') return
		had_vars_recur(borrow_l(this), vars);
	/* sX, pS */
	if (this->l_type == 's' || this->l_type == 'p') return
		vars;
	/* nn */
	if (this->r_type == 'n') {
		node* virs;
		virs = borrow(node_EMPTY_LIST);
		for (fast_u8 i = 0; i < this->len; i++)
			virs = had_vars_recur(this->xs[i], virs);
		return lyse(this,
			cat(virs, vars)
		);
	}
	/* ns, np */
	if (this->r_type == 's' || this->r_type == 'p') return
		had_vars_recur(borrow(this->l), vars);
	/* nm */
	if (this->r_type == 'n' || this->r_type == 'm') {
		node* virs;
		virs = borrow(node_EMPTY_LIST);
		for (fast_u8 i = 0; i < this->len; i++)
			virs = had_vars_recur(restore(this->ms[i], this->sc), virs);
		return lyse(this,
			cat(virs, vars)
		);
	}
	/* nN */
	if (this->r_type == 'N') {
		node* virs;
		virs = had_vars_recur(borrow(this->l), vars);
		for (fast_u8 i = 0; i < this->len - 1; i++)
			virs = had_vars_recur(borrow(near_rs_(this, i)), virs);
		return lyse(this,
			cat(virs, vars)
		);
	}
	return err_node_new("Problem collecting had vars", this);
}
node* node_had_vars(node* this, scope* sc) {
	return had_vars_recur(this, borrow(node_EMPTY_LIST));
}
node* node_envary_with_all_of(node* expr, node* vas) {
	static node* prev_expr = NULL,* prev_vas = NULL;
	assert(expr != prev_expr || vas != prev_vas);
	assert(vas->as != stru32z("(,") || vas->l->as != '"');
	prev_expr = expr; prev_vas = vas;
	if (!iknt_be_empty(vas)) return
		envary_with_roots(expr);
	node* bvas;
	bvas = borrow(vas);
	return node_envary_with_all_of(
		envary(expr, first(vas)),
		rest(bvas)
	);
}
node* node_envary_with_roots(node* expr) {
	if (expr->as == '[') {
		node* rv;
		rv = lyse_and_clone(expr);
		// Introducing node type '[BR' - the broken label
		// A label that doesn't refer to any declared variable.
		rv->as = stru32z("[BR");
		return rv;
	}
	if (expr->as != stru32z("roo")) return
		expr;
	scope* root_sc;
	root_sc =
		*expr->s == (u8)'.' ?
			global_scope() :
		*expr->s == (u8)'/' ?
			root_node_scope() :
			NULL;
	if (!root_sc) return
		err_node_new("can't envary with roots", expr);
	return
		var_node_new(
			leaf_node_new('"', (char*)expr->s),
			borrow(root_sc),
			borrow(root_sc->global),
			'/'
		);
}
node* node_envary(node* expr, node* va) {
	node* la;
	node* la_name,* la_parent;
	scope* la_scope;
	node* va_name,* va_parent;
	scope* va_scope;
	bool p;
	u8 va_pathsep;
	la = NULL;
	assert(va->as != stru32z("(,"));
	if (!expr->len || ((expr->l_type + 1)&'~') != 'n') return lyse(va,
		expr
	);
	va_pathsep = ((u8*)&va->op)[1];  // . 2nd char in "v." or "v/"
	if (has_op_as(expr) && expr->op == va_pathsep) {
		la = borrow_r(expr);
		la_parent = expr->l;
	} else if (expr->as == '[' && va_pathsep == '/') {
		printf("e[");
		la = borrow(expr);
		la_parent = la->sc->global;
	}
	if (la) {
		la_name = borrow_l(la);
		la_scope = la->sc;
		va_name = va->l->l;
		va_scope = (scope*)*va->l->psr;
		va_parent = borrow_r(va);
		p =
			ikt_eq(la_name, va_name) &&
			la_scope == va_scope &&
			ikt_eq(la_parent, va_parent);
		unref(la);
		unref(la_name);
		unref(va_parent);
		if (p) return lyse(expr,
			va
		); else return lyse(va,
			expr
		);
	}
	if (expr->as == stru32z("hav")) return lyse(expr,
		envary(borrow_l(expr), va)
	);
	node* rv;
	fast_u8 i, rlen;
	rv = lyse_and_clone(expr);
	if (expr->l_type == 'n') {
		rv->l = envary(expr->l, borrow(va));
		rlen = expr->len - 1;
		if (expr->r_type == 'n')
			for (i = 0; i < rlen; i++)
				rv->rs[i] = envary(rv->rs[i], borrow(va));
		else if (expr->r_type == 'm')
			for (i = 0; i < rlen; i++)
				rv->msr[i] =
					minimize(
						envary(rv->msr[i], expr->sc, borrow(va)),
						expr->sc
					);
		else if (expr->r_type == 'N') {
			node* rs[expr->len];
			for (i = 0; i < rlen; i++)
				rs[i] = envary(near_rs_(expr, i), borrow(va));
			rv = lyse(rv,
				lonr_op_node_new(expr->op, expr->l, rs, expr->len)
			);
		}
	} else if (expr->l_type == 'm')
		for (i = 0; i < rv->len; i++)
			rv->ms[i] =
				minimize(envary(rv->ms[i], expr->sc, borrow(va)), expr->sc);
	unref(va);
	return rv;
}
static node* define_all_loop(node* vas, node* expr, node* dvas) {
	static node* prev_vas = NULL,* prev_expr = NULL,* prev_dvas = NULL;
	assert(vas != prev_vas || expr != prev_expr || dvas != prev_dvas);
	assert(dvas != prev_dvas || expr != prev_expr || vas->as != stru32z("rs"));
	prev_vas = vas; prev_expr = expr; prev_dvas = dvas;
	if (ikt_be_empty(vas)) return
		dvas;
	node* bvas,* bexpr;
	bvas = borrow(vas);
	bexpr = borrow(expr);
	node* rv,* first_defn;
	first_defn = define(first(vas), expr);
	if (ikt_eq(first_defn, node_TRUE))
		rv = define_all_loop(
			rest(bvas),
			bexpr,
			dvas
		);
	else if (ikt_eq(first_defn, node_FALSE))
		rv = err_node_new("Contradiction!", expr);
	else
		rv = define_all_loop(
			rest(bvas),
			bexpr,
			cat(dvas, first_defn)
		);
	assert(rv);
	assert(rv->ref_count);
	return rv;
}
node* node_define_all(node* vas, node* expr) {
	printf("da");
	assert(vas);
	assert(expr);
	return define_all_loop(vas, expr, borrow(node_EMPTY_LIST));
}
node* node_define(node* va, node* expr) {
	if (expr->as == stru32z("let")) return lyse(expr,
		box(believe(va, borrow_l(expr)))
	);
	// iterate through all the nodes in expr,
	// calling va.define on each of them.
	node* rv,* defn;
	node* vads;
	fast_u8 rlen, i;
	vads = borrow(node_EMPTY_LIST);
	/*printf("node_define: expr = ");
	print(expr);
	puts("");*/
	if (expr->l_type == 'n') {
		vads = ikt_be_true(defn = define(borrow(va), borrow(expr->l))) ?
			lyse(defn, vads) :
			cat(vads, defn);
		rlen = expr->len - 1;
		if (expr->r_type == 'n')
			for (i = 0; i < rlen; i++)
				vads = ikt_be_true(
					defn = define(borrow(va), borrow(expr->rs[i]))
				) ?
					lyse(defn, vads) :
					cat(vads, defn);
		else if (expr->r_type == 'm')
			for (i = 0; i < rlen; i++)
				vads =
					ikt_be_true(defn =
						define(borrow(va), restore(expr->msr[i], expr->sc))
					) ?
						lyse(defn, vads) :
						cat(vads, defn);
		else if (expr->r_type == 'N')
			for (i = 0; i < rlen; i++)
				vads = ikt_be_true(
					defn = define(
						borrow(va),
						borrow(near(expr->l, expr->near_rs[i]))
					)
				) ?
					lyse(defn, vads) :
					cat(vads, defn);
	} else if (expr->l_type == 'm')
		for (i = 0; i < expr->len; i++)
			vads =
				ikt_be_true(
					defn = define(borrow(va), restore(expr->ms[i], expr->sc))
				) ?
					lyse(defn, vads) :
					cat(vads, defn);
	return lyse(expr, lyse(va,
		vads
	));
}
bool node_has_op_as(node* this) {
	return
		!this->as ||
		this->as == stru32z("long") ||
		this->as == stru32z("lonr") ||
		this->as == stru32z("miop");
}
