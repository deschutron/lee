#ifndef blob_node_INCLUDED
#define blob_node_INCLUDED

#define static_blob_node(x, y, z) &(node){\
	.ref_count = 1,\
	.mem_type = 'S',\
	.data_type = 's'*257,\
	.as = x,\
	.op = 0,\
	.s = y,\
	.len = z,\
	.sc = NULL,\
	.vads = NULL\
}

node* node_TRUE;
node* node_FALSE;
node* node_ONE;
node* node_TWO;
node* node_X_STRING;
node* node_Y_STRING;
node* node_DOT_STRING;
node* node_SLASH_STRING;

node* blob_node_new(u32, u8*, fast_u8);
node* blob_node_make(node*, u32, u8*, fast_u8);
node* leaf_node_new(u32, const char*);
u32 leaf_node_cat_op(u32);
node* node_numstr(node*);
node* node_numstr_in_base(node*, fast_u8);
node* node_fixed_len_numstr(node*, unt);
node* node_fixed_len_numstr_in_base(node*, unt, fast_u8);
u32 blob_cat_op(u32);
node* blob_node_seng(node*, node*);

#endif
