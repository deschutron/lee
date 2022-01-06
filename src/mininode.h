#ifndef mininode_INCLUDED
#define mininode_INCLUDED

#include "numtypes.h"
#include "util.h"
#include "scope.h"
#include "node.h"

#define mininode_s_BUFLEN sizeof(void*)
#define node struct node
typedef struct mininode {
	union {
		node* l;
		u8 s[mininode_s_BUFLEN];
		void* p;
	};
	union {
		struct {
			#ifdef ENV64
				u32 as;
				u8 type;
				u8 len;
				u16 tag;
			#else
				u32 as : 22;
				u32 type : 7;
				u32 len : 3;
			#endif
		};
		uptr footer;
	};
} mininode;
#ifdef ENV64
	#define mininode_MAX_AS UINT32_MAX
	#define mininode_SET_TAG_TO_ZERO .tag = 0
	#define mininode_NOP_FOOTER (*(uptr*)"+_\0\0n\1\0\0")
#else
	#define mininode_MAX_AS ('@'*65536 - 1)
	#define mininode_SET_TAG_TO_ZERO
	#define mininode_NOP_FOOTER (*(uptr*)"+_\x80\x9f")
#endif

mininode mininode_of(u32, node*, u8*, void*, fast_u8);
mininode mininode_borrow(mininode);
mininode mininode_unref(mininode);
mininode mininode_unref_with_note(mininode, char*);
mininode mininode_unref_with_note_from(mininode, char*, void*);
node* mininode_lyse(mininode, node*);
mininode mininode_mini_lyse(mininode, mininode);
mininode mininode_clone(mininode);
mininode mininode_lyse_and_clone(mininode);
usz minonode_data_byte_count(mininode);
void mininode_print_type_name(mininode);
void mininode_print(mininode, scope*);
void mininode_print_bytes(mininode);
void mininode_print_bytes_with_indent(mininode, int);
node* mininode_restore(mininode, scope*);
node* mininode_in_scope(mininode, scope*);
node* mininode_seng(mininode, scope*, node*);
mininode mininode_mini_seng(mininode, scope*, node*);
node* mininode_vars(mininode, scope*);
node* mininode_envary(mininode, scope*, node*);
bool mininode_bytes_ze(mininode);
bool mininode_bytes_nz(mininode);
#undef node

#endif
