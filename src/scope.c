#include "lee.h"
#include "shared_shortcuts.h"

scope* scope_new(node* global) {
	scope* rv;
	rv = malloc(sizeof(scope));
	rv->ref_count = 1;
	rv->parent = NULL;
	rv->global = global;
	rv->tag = 0;
	return rv;	
}
scope* scope_borrow(scope* this) {
	assert(this);
	assert((uptr)this->parent != 34);
	this->ref_count++;
	this->tag += 256;
	return this;
}
scope* scope_unref(scope* this) {
	return unref_from(this, NULL);
}
scope* scope_unref_from(scope* this, void* start) {
	void* stot;
	assert(this);
	if (this == (scope*)start) return
		this->ref_count ? this : NULL;
	assert(this->ref_count);
	if (--this->ref_count) return
		this;
	stot = start ?: this;
	if (this->parent)
		unref_from(this->parent, stot);
	if (this->global)
		unref_from(this->global, stot);
	free(this);
	return NULL;
}
scope* scope_in_scope(scope* this, scope* that) {
	if (!that || !this || this == that) return
		this;
	for (scope* x = this; x->parent; x = x->parent) {
		if (x->parent == that || x->parent == this) return
			unref(that), this;
		x->parent = that;
	}
	this->tag += 65536;
	return unref(that), this;
}
scope* global_scope() {
	return global_scope_do_op(stru32z("get"));
}
scope* global_scope_do_op(u32 op) {
	static scope* g = NULL;
	if (op == stru32z("nref")) return
		g ? unref(g) : NULL;
	if (op != stru32z("get")) return
		NULL;
	if (g) return
		g;
	g = scope_new(NULL);
	g->global = var_node_new(
		borrow(node_DOT_STRING),
		g,
		borrow(node_EMPTY_LIST),
		'/'
	);
	return g;
}
scope* global_scope_unref() {
	return global_scope_do_op(stru32z("nref"));
}
scope* root_node_scope() {
	return root_node_scope_do_op(stru32z("get"));
}
scope* root_node_scope_unref() {
	return root_node_scope_do_op(stru32z("nref"));
}
scope* root_node_scope_do_op(u32 op) {
	static scope* r = NULL;
	if (op == stru32z("nref")) return
		r ? unref(r) : NULL;
	if (op != stru32z("get")) return
		NULL;
	if (r) return
		r;
	r = scope_new(NULL);
	r->global = var_node_new(
		borrow(node_SLASH_STRING),
		r,
		borrow(node_EMPTY_LIST),
		'/'
	);
	return r;
}
bool scope_is_in(scope* this, scope* that) {
	return
		!that ?
			false :
		!this ?
			false :
		this == that ?
			true :
		scope_is_in(this, that->parent);
}
void scope_print_bytes_with_indent(scope* this, fast_u8 indent) {
	fast_u8 i;
	printf("\x1b[36m");
	for (i = 0; i < min((int)sizeof(scope*), 15 - indent); i++)
		print_rimek_byte(((char*)&this)[i]);
	printf("\x1b[0m");
	for (; i < 15; i++)
		printf("%s", i >= 15 - indent ? "░" : " ");
	putchar(32);
	print_rimek_bytes(this, sizeof(scope));
	putchar(10);
}
