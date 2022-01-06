#include "lee.h"
#include "shared_shortcuts.h"

mininode mininode_of(
	u32 as,
	node* l, u8* s, void* p,
	fast_u8 len
) {
	mininode rv;
	fprintf(stderr, "▵");
	rv.as = as;
	if (l && len) {
		if (len >= 2) return
			err_mininode("nup");
		rv.type = 'n';
		rv.l = l;
		rv.len = 1;
	} else if (s) {
		if (len >= mininode_s_BUFLEN) return
			box_mininode(stru32z("+_"),
				blob_node_new(as, s, len)
			);
		if (len) {
			*(void**)rv.s = *(void**)s;
			for (fast_u8 i = len; i < mininode_s_BUFLEN; i++)
				rv.s[i] = 0;
		} else
			*(void**)rv.s = NULL;
		rv.type = 's';
		rv.len = len;
	} else if (p && len) {
		if (len >= 2) return
			err_mininode("nup");
		rv.type = 'p';
		rv.p = p;
		rv.len = 1;
	} else {
		if (len) return
			err_mininode("nup");
		rv.type = '_';
		rv.p = NULL;
	}
	#ifdef ENV64
		rv.tag = 0;
	#endif
	return rv;
}
/* mininodes only need to be borrowed or unreffed if they
   contain child nodes.
   mininodes that have child nodes _always_ have to be
   borrowed and unreffed.
   If there's ever any doubt, borrow and unref your mininodes.
 */
mininode mininode_borrow(mininode this) {
	fprintf(stderr, "▵");
	mininode rv;
	rv = this;
	if (this.type == 'n')
		rv.l = borrow(this.l);
	return rv;
}
mininode mininode_unref(mininode this) {
	return unref_with_note_from(this, "mini unr", NULL);
}
mininode mininode_unref_with_note(mininode this, char* note) {
	return unref_with_note_from(this, note, NULL);
}
mininode mininode_unref_with_note_from(mininode this, char* note, void* start) {
	mininode rv;
	fprintf(stderr, "▿");
	rv = this;
	if (this.type == 'n') {
		if (this.l->ref_count == 1)
			rv = mininode_NULL;
		unref_with_note_from(this.l, note, start);
	}
	return rv;
}
node* mininode_lyse(mininode this, node* that) {
	fprintf(stderr, "▿");
	if (this.type == 'n')
		unref_with_note_from(this.l, "mini lys", NULL);
	return that;
}
mininode mininode_mini_lyse(mininode this, mininode that) {
	unref(this);
	return that;
}
mininode mininode_clone(mininode this) {
	fprintf(stderr, "▵");
	if (this.type == 'n')
		borrow(this.l);
	return this;
}
mininode mininode_lyse_and_clone(mininode this) {
	return this;
}
usz mininode_data_byte_count(mininode this) {
	return
		!this.len        ? 0 :
		this.type == 'n' ? sizeof(node*) :
		this.type == 's' ? this.len :
		this.type == 'p' ? sizeof(void*) :
		mininode_s_BUFLEN /* sizeof(void*) */;
}
void mininode_print_type_name(mininode this) {
	if (!this.as) return
		(void)printf("[0]");
	if (u32_can_be_printed_like_identifier(this.as))
		printf("%.4s", (char*)&this.as);
	else if (u32_can_be_printed(this.as))
		printf("[%u]", this.as);
}
void mininode_print(mininode this, scope* sc) {
	print_type_name(this);
	putchar(40);
	if (this.type == 's') {
		putchar('"');
		for (fast_u8 i = 0; i < min(this.len, mininode_s_BUFLEN); i++)
			putchar(this.s[i]);
		putchar('"');
	} else if (this.type == 'n')
		print(this.l);
	else if (this.type == 'p')
		printf("object at %lu", (long)this.p);
	if (sc) {
		putchar('|');
		putchar('S'+(4*((u8)sc& 8)));
		putchar('C'+(2*((u8)sc&16)));
	}
	putchar(41);
}
void mininode_print_bytes(mininode this) {
	print_bytes_with_indent(this, 0);
}
void mininode_print_bytes_with_indent(mininode this, int indent) {
	u8* bys;
	fast_u8 i;
	int addr_sz, undent;
	bys = (u8*)&this;
	undent = 15 - indent;
	addr_sz = min((int)sizeof(node*), undent);
	printf("\x1b[36m");
	for (i = 0; i < addr_sz; i++)
		putchar("mininode        "[i]);
	printf("\x1b[0m");
	for (i = addr_sz; i < undent; i++)
		putchar(32);
	for (i = 0; i < indent; i++)
		printf("░");
	putchar(32);
	for (i = 0; i < sizeof(mininode); i++)
		print_rimek_byte(bys[i]);
	putchar(10);
	if (this.type == 'n')
		print_bytes_with_indent(this.l, indent + 1);
}
node* mininode_restore(mininode this, scope* sc) {
	node* rv;
	rv =
		this.as == stru32z("+_") ?
			this.l :
		this.type == '_' ?
			empty_node_new(this.as) :
		this.type == 'n' ?
			box_node_new(this.as, this.l) :
		this.type == 's' ?
			blob_node_new(this.as, this.s, this.len) :
		this.type == 'p' ?
			p_box_node_new(this.as, this.p, NULL, NULL) :
			err_node_new("Can't restore m-node:typ",
				blob_node_new('"', (u8*)&this, sizeof(mininode))
			);
	return
		sc ?
			with_scope(rv, sc) :
			rv;
}
node* mininode_in_scope(mininode this, scope* sc) {
	mininode next;
	next = this;
	if (this.type == 'n')
		next.l = in_scope(this.l, sc);
	return restore(next, sc);
}
node* mininode_seng(mininode this, scope* sc, node* vads) {
	if (!this.as) return
		err_node_new("senging meaningless node", restore(this, sc));
	if (
		this.as == '"' ||
		this.as == stru32z("'") ||
		this.as == stru32z("num")
	) return
		restore(this, sc);
	if (node_box_f(this.as)) return
		box_mininode_seng(this, sc, vads);
	return seng(restore(this, sc), vads);
}
mininode mininode_mini_seng(mininode this, scope* sc, node* vads) {
	if (!this.as) return
		mini_nop(err_node_new("senging meaningless node", restore(this, sc)));
	if (
		this.as == '"' ||
		this.as == stru32z("'") ||
		this.as == stru32z("num")
	) return
		this;
	if (node_box_f(this.as)) return
		box_mininode_mini_seng(this, sc, vads);
	return minimize(seng(restore(this, sc), vads), sc);
}
node* mininode_vars(mininode this, scope* sc) {
	return
		this.type == 'n' ?
			vars(this.l) :
			borrow(node_EMPTY_LIST);
}
node* mininode_envary(mininode this, scope* sc, node* vars) {
	if (this.as == stru32z("roo")) return
		var_node_new(
			leaf_node_new('"', (char*)this.s),
			borrow(root_node_scope()),
			borrow(root_node_scope()->global),
			*this.s
		);
	if (this.as == '[') return
		var_node_new(
			this.l,
			borrow(sc ?: global_scope()),
			borrow((sc ?: global_scope())->global),
			'/'
		);
	return restore(this, sc);
}
bool mininode_ikmt_have_labels(mininode this, scope* sc, bool _) {
	if (this.type != 'n') return
		false;
	return ikt_have_labels(this.l);
}
bool mininode_bytes_ze(mininode this) {
	return !this.p && !this.footer;
}
bool mininode_bytes_nz(mininode this) {
	return this.p || this.footer;
}
