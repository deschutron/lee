#include "lee.h"
#include "shared_shortcuts.h"

#define boo 'b'+('o'+ 'o'*256)*256
node* node_TRUE = static_blob_node(boo, "true" "\0\0\0", 4);
node* node_FALSE = static_blob_node(boo, "false" "\0\0", 5);
node* node_ONE = static_blob_node('\'', "\1" "\0\0\0\0\0\0", 1);
node* node_TWO = static_blob_node('\'', "\2" "\0\0\0\0\0\0", 1);
node* node_X_STRING = static_blob_node('"', "x" "\0\0\0\0\0\0", 1);
node* node_Y_STRING = static_blob_node('"', "y" "\0\0\0\0\0\0", 1);
node* node_DOT_STRING = static_blob_node('"', "." "\0\0\0\0\0\0", 1);
node* node_SLASH_STRING = static_blob_node('"', "/" "\0\0\0\0\0\0", 1);
#undef boo

node* blob_node_new(u32 as, u8* s, fast_u8 len) {
	return node_new(
		as, 0,
		NULL, NULL, s, NULL, NULL, NULL,
		len, NULL, NULL,
		NULL, NULL
	);
}
node* blob_node_make(node* this, u32 as, u8* s, fast_u8 len) {
	return node_make(this,
		as, 0,
		NULL, NULL, s, NULL, NULL, NULL,
		len, NULL, NULL,
		NULL, NULL
	);
}
node* leaf_node_new(u32 as, const char* s) {
	node* rv,* next;
	rv = node_new(as, 0,
		NULL, NULL, (u8*)"", NULL, NULL, NULL,
		0, NULL, NULL,
		NULL, NULL
	);
	if (!s || !*s) return
		rv;
	u8* t;
	t = memccpy(rv->s, s, 0, node_s_BUFLEN);
	rv->len = t ?
		(u8)(t - rv->s - 1) :
		node_s_BUFLEN;
	rv->l_type = rv->len ? 's' : '_';
	if (!t && leaf_node_cat_op(as) && s[node_s_BUFLEN]) return
		op_node_new(leaf_node_cat_op(as),
			rv,
			leaf_node_new(as, &s[node_s_BUFLEN])
		);
	return rv;
}
u32 leaf_node_cat_op(u32 as) {
	return
		as == '"' ?
			stru32z(".||") :
			0;
}
node* node_numstr(node* this) {
	return node_fixed_len_numstr(this, ~(unt)0);
}
node* node_numstr_in_base(node* this, fast_u8 ba) {
	return node_fixed_len_numstr_in_base(this, ~(unt)0, ba);
}
node* node_fixed_len_numstr(node* this, unt len) {
	return fixed_len_numstr_in_base(this, len, 10);
}
static node* with_as(node* this, u32 as) {
	this->as = as;
	return this;
}
node* node_fixed_len_numstr_in_base(node* this, unt len, fast_u8 ba) {
	node* rv;
	ll_unt x;
	x = strtoull((char*)this->s, NULL, ba);
	if (errno) return
		~len ?
			err_node_new("can't read the number",
				list(list_comma(
					this,
					blob_node_new('\'', (u8*)&errno, sizeof(errno))
				))
			) :
			ba == 10 ?
				with_as(this, stru32z("num")) :
				with_as(
					op_node_new(stru32z("nbas"),
						blob_node_new('\'', &ba, 1),
						this
					),
					'"'
				);
	rv = lyse_and_clone(this);
	rv->as = '\'';
	*(ll_unt*)rv->s = x;
	fast_u8 i, nlen;
	if (~len) {
		nlen = len;
		for (i = sizeof(ll_unt); --i >= nlen;)
			if (rv->s[i]) return
				err_node_new("number too long for fixed length", rv);
		rv->s[nlen] = 0;
		i++;
	} else
		i    =
		nlen = sizeof(ll_unt);
	for (; --i && !rv->s[i]; --nlen);
	rv->len = nlen;
	return rv;
}
u32 blob_cat_op(u32 as) {
	if (as == '"') return
		stru32z(".||");
	return 0;
}
node* blob_node_seng(node* this, node* vads) {
	unref(vads);
	if (this->as == stru32z("num")) return
		numstr(this);
	return this;
}
