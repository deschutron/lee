#ifndef node_INCLUDED
#define node_INCLUDED

#include <stdbool.h>
#include "numtypes.h"
#include "util.h"
#include "scope.h"
#include "mininode.h"

#define node_s_BUFLEN    32                    // 32, 32
#define node_xs_BUFLEN  (32/sizeof(node*))     //  4,  8
#define node_ps_BUFLEN  (32/sizeof(void*) - 2) //  2,  6
#define node_ms_BUFLEN  (32/sizeof(mininode))  //  2,  4
#define node_sr_BUFLEN  (32 - sizeof(node*))   // 24, 28
#define node_rs_BUFLEN  (node_xs_BUFLEN - 1)   //  3,  7
#define node_psr_BUFLEN (node_ps_BUFLEN - 1)   //  1,  5
#define node_near_rs_BUFLEN (node_sr_BUFLEN/2) // 12, 14
#define node_msr_BUFLEN (node_ms_BUFLEN - 1)   //  1,  3
typedef struct node {
	#define node     struct node
	#define scope    struct scope
	#define mininode struct mininode
	// ENV64, ENV32
	//  0,  0
	union {
		struct {
			u32 as;
			u32 op;
		};
		u64 asop;
	};
	//  8,  8
	union {
		struct {
			u8 l_type; /* : ..."_nmsp" */
			u8 r_type; /* : ..."_nNmsp" */
		};
		u16 data_type;
	};
	u8 len;
	u8 mem_type;
	u32 ref_count;
	// 16, 16
	scope* sc;
	// 24, 20
	node* vads;
	// 32, 24
	#ifndef ENV64
		u64 gap;
	#endif
	// 32, 32
	union {
		// node*, ...
		struct {
			node* l;
			union {
				node* r;
				node* rs[node_rs_BUFLEN];
				u8 sr[node_sr_BUFLEN];
				u16 near_rs[node_near_rs_BUFLEN];
			};
		};
		node* xs[node_xs_BUFLEN];
		u8 s[node_s_BUFLEN];
		// void*, ...
		struct {
			union {
				struct {
					void* pl;
					void* psr[node_psr_BUFLEN];
				};
				void* ps[node_ps_BUFLEN];
			};
			union {
				void*(* p_borrow)(void*);
				void* pb_note;
			};
			void*(* p_unref_from)(void*, void*);
		};
		// mininode, ...
		mininode ms[node_ms_BUFLEN];
		struct {
			mininode ml;
			mininode msr[node_msr_BUFLEN];
		};
	};
	// 64, 64
	#undef node
	#undef scope
	#undef mininode
} node;
/*
	l   is always xs[0]
	r   is always xs[1]
	rs  is always &xs[1]
	msr is always &ms[1]
	psr is always &ps[1]
	sr  is never  &s[1]
	sr  is always &s[sizeof(node*)]
	near_rs always starts at the posi of &xs[1]
	Iterating through the full array for a data type always
	iterates  through the RHS array for it.
*/

node* node_new(
	u32, u32,
	node*, node**, u8*, void**, mininode*, u16*,
	fast_u8, void*(*)(void*), void*(*)(void*, void*),
	scope*, node*
);
node* node_make(node*,
	u32, u32,
	node*, node**, u8*, void**, mininode*, u16*,
	fast_u8, void*(*)(void*), void*(*)(void*, void*),
	scope*, node*
);
node* ns_node_new(u32, u32, node*, u8*, fast_u8);
node* ns_node_make(node*, u32, u32, node*, u8*, fast_u8);
node* err_node_new(char*, node*);
node* err_node_make(node*, char*, node*);
node* p_box_node_new(u32, void*, void*(*)(void*), void*(*)(void*, void*));
node* pp_node_new(
	u32, u32,
	void**,
	fast_u8, void*(*)(void*), void*(*)(void*, void*)
);
node* np_node_new(
	u32, u32,
	node*, void**,
	fast_u8, void*(*)(void*), void*(*)(void*, void*)
);
node* nN_node_new(u32, u32, node*, u16*, fast_u8);
node* nm_node_new(u32, u32, node*, mininode*, fast_u8);
node* node_borrow(node*);
node* node_unref_with_note_from(node*, char*, void*);
node* node_unref_with_note(node*, char*);
node* node_unref_from(node*, void*);
node* node_unref(node*);
node* node_lyse(node*, node*);
mininode node_mini_lyse(node*, mininode);
node* node_lyse_with_note(node*, node*, char*);
node* node_clone(node*);
node* node_lyse_and_clone(node*);
fast_u8 node_data_unit_byte_count(u8);
fast_u8 node_data_byte_count(node*);
bool node_can_be_minimized(node*, scope*);
mininode node_minimize(node*, scope*);
void node_print_type_name(node*);
void node_print(node*);
void node_print_bytes_with_indent_sc(node*, int, scope*);
void node_print_bytes_with_indent(node*, int);
void node_print_bytes(node*);
node* node_with_scope(node*, scope*);
node* node_in_scope(node*, scope*);
bool node_is_in_scope(node*, scope*);
node* node_borrow_l(node*);
node* node_borrow_r(node*);
node* node_seng(node*, node*);
node* node_seng_subst_all(node*, node*);
node* node_seng_subst(node*, node*);
node* node_with_vad(node*, node*);
node* node_vars(node*);
node* node_had_vars(node*, scope*);
node* node_envary_with_all_of(node*, node*);
node* node_envary_with_roots(node*);
node* node_envary(node*, node*);
node* node_define_all(node*, node*);
node* node_define(node*, node*);
node* node_believe(node*, node*);
bool node_has_op_as(node*);

#endif
