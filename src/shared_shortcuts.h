#include "lee.h"

#define method_IJ(x, f, I, J) _Generic(x,\
	I:        I##_##f,\
	const I:  I##_##f,\
	J:        J##_##f,\
	const J:  J##_##f\
)
#define method_IX(x, f, I, X) _Generic(x,\
	I:        I##_##f,\
	const I:  I##_##f,\
	X*:       X##_##f,\
	const X*: X##_##f\
)
#define method_XY(x, f, X, Y) _Generic(x,\
	X*:       X##_##f,\
	const X*: X##_##f,\
	Y*:       Y##_##f,\
	const Y*: Y##_##f\
)
#define method_IXY(x, f, I, X, Y) _Generic(x,\
	I:        I##_##f,\
	const I:  I##_##f,\
	X*:       X##_##f,\
	const X*: X##_##f,\
	Y*:       Y##_##f,\
	const Y*: Y##_##f\
)
#define method_XD(x, f, X) _Generic(x,\
	X*:       X##_##f,\
	const X*: X##_##f,\
	default:  f\
)
#define array_method_XY(x, f, X, Y) _Generic(x,\
	X**:      X##_array_##f,\
	const X**:X##_array_##f,\
	Y**:      Y##_array_##f,\
	const Y**:Y##_array_##f,\
)
#define method_n(x, f)     node_##f
#define method_m(x, f)     mininode_##f
#define method_nX(x, f, X) method_XY(x, f, node, X)
#define method_nD(x, f, X) method_XD(x, f, node)
#define method_ns(x, f)    method_XY(x, f, node, scope)
#define method_mn(x, f)    method_IX(x, f, mininode, node)
#define method_mns(x, f)   method_IXY(x, f, mininode, node, scope)
#define array_method_mn(x, f) array_method_XY(x, f, minonode, node)

#define add node_add
#define and(x, y) node_and((x), (y))
#define apply node_apply
#define apply_bxr node_apply_bxr
#define apply_opr node_apply_opr
#define borrow_all node_array_borrow_all
#define borrow_l node_borrow_l
#define borrow_r node_borrow_r
#define bytes_eq node_bytes_eq
#define bytes_eq_mini node_bytes_eq_mini
#define bytes_nz mininode_bytes_nz
#define bytes_ze mininode_bytes_ze
#define can_be_minimized node_can_be_minimized
#define cat(x, y) node_cat((x), (y))
#define comma_add node_comma_add
#define comma_cat node_comma_cat
#define comma_cons node_comma_cons
#define comma_first node_comma_first
#define comma_list node_comma_list
#define comma_push node_comma_push
#define comma_rest node_comma_rest
#define comma_self_assoc mininode_comma_self_assoc
#define comma_self_cstraint mininode_comma_self_cstraint
#define comma_set node_comma_set
#define data_byte_count node_data_byte_count
#define dec node_dec
#define define node_define
#define define_all node_define_all
#define div(x, y) node_div((x), (y))
#define envary_with_all_of node_envary_with_all_of
#define envary_with_roots node_envary_with_roots
#define eq node_eq
#define fixed_len_numstr node_fixed_len_numstr
#define fixed_len_numstr_in_base node_fixed_len_numstr_in_base
#define gith node_gith
#define has_op_as node_has_op_as
#define have node_have
#define iff node_iff
#define implies node_implies
#define inc node_inc
#define is_implied_by node_is_implied_by
#define is_in scope_is_in
#define is_in_scope node_is_in_scope
#define is_near node_is_near
#define lambda node_lambda
#define let(x) node_let((x))
#define list_comma node_list_comma
#define lyse_with_note node_lyse_with_note
#define make node_make
#define mini_first node_mini_first
#define mini_neg node_mini_neg
#define mini_nop node_mini_nop
#define mini_sub node_mini_sub
#define minimize node_minimize
#define mul node_mul
#define near node_near
#define near_rs_ node_near_rs_
#define nearby_addr node_nearby_addr
#define numstr node_numstr
#define numstr_in_base node_numstr_in_base
#define or(x, y) node_or((x), (y))
#define uni(x, y) node_union((x), (y))
#define pipe(x, y) node_pipe((x), (y))
#define print_bytes_with_indent_sc node_print_bytes_with_indent_sc
#define pub(x) node_pub((x))
#define push(x, y) node_push((x), (y))
#define rest(x) node_rest((x))
#define restore mininode_restore
#define roi(x, y) node_roi((x), (y))
#define self_assoc mininode_self_assoc
#define self_cstraint mininode_self_cstraint
#define semic(x, y) node_semic((x), (y))
#define seng_subst node_seng_subst
#define seng_subst_all node_seng_subst_all
#define set(x) node_set((x))
#define set_comma node_set_comma
#define sub(x, y) node_sub((x), (y))
#define with_scope node_with_scope
#define with_vad node_with_vad
#define xor(x) node_xor((x))
#define believe node_believe
#define mini_sub node_mini_sub
#define cx_block node_cx_block
#define nbas node_nbas
#define parse(x, ...) lee_parser_parse((x) ,##__VA_ARGS__)
#define destroy(x, ...) lee_parser_destroy((x) ,##__VA_ARGS__)
#define borrow(x)              method_mns((x), borrow)((x))
#define box(x)                 method_mn((x), box)((x))
#define can_be_printed(x)      method_IJ((x), can_be_printed, u32, u64)((x))
#define can_be_printed_like_identifier(x) \
	method_IJ((x), can_be_printed_like_identifier, u32, u64)((x))
#define clone(x)               method_mn((x), clone)((x))
#define borrow(x)              method_mns((x), borrow)((x))
#define box(x)                 method_mn((x), box)((x))
#define clone(x)               method_mn((x), clone)((x))
#define envary(x, ...)         method_mn((x), envary)((x) ,##__VA_ARGS__)
#define first(x, ...)          method_mn((x), first)((x) ,##__VA_ARGS__)
#define in_scope(x, y)         method_mns((x),in_scope)((x), (y))
#define inv(x, ...)            method_mn((x), inv)((x) ,##__VA_ARGS__)
#define length(x, ...)         method_mn((x), length)((x) ,##__VA_ARGS__)
#define list(x, ...)           method_mn((x), list)((x) ,##__VA_ARGS__)
#define lyse(x, y)             method_mn((x), lyse)((x), (y))
#define lyse_and_clone(x, ...) method_mn((x), lyse_and_clone)((x),##__VA_ARGS__)
#define mini_lyse(x, y)        method_mn((x), mini_lyse)((x), (y))
#define neg(x, ...)            method_mn((x), neg)((x) ,##__VA_ARGS__)
#define nop(x, ...)            method_mn((x), nop)((x) ,##__VA_ARGS__)
#define not(x, ...)            method_mn((x), not)((x) ,##__VA_ARGS__)
#define print(x, ...)          method_mn((x), print)((x) ,##__VA_ARGS__)
#define print_bytes(x)         method_mn((x), print_bytes)((x))
#define print_bytes_with_indent(x, y) \
	method_mns((x), print_bytes_with_indent)((x), (y))
#define print_bytes_with_indent(x, y) \
	method_mns((x), print_bytes_with_indent)((x), (y))
#define print_bytes_with_indent(x, y) \
	method_mns((x), print_bytes_with_indent)((x), (y))
#define print_type_name(x)     method_mn((x), print_type_name)((x))
#define range_forever(x, ...)  method_mn((x), range_forever)(x ,##__VA_ARGS__)
#define range_from_zero(x, ...) \
	method_mn((x), range_from_zero)((x) ,##__VA_ARGS__)
#define range_from_zero(x, ...) \
	method_mn((x), range_from_zero)((x) ,##__VA_ARGS__)
#define seng(x, ...)           method_mn((x), seng)((x) ,##__VA_ARGS__)
#define mini_seng(x, ...)      method_m((x), mini_seng)((x) ,##__VA_ARGS__)
#define size(x, ...)           method_mn((x), size)((x) ,##__VA_ARGS__)
#define mini_size(x, ...)      method_mn((x), mini_size)((x) ,##__VA_ARGS__)
#define mini_add(x, ...)       method_mn((x), mini_add)((x) ,##__VA_ARGS__)
#define mini_add_mini(x, ...)  method_m((x), mini_add_mini)((x) ,##__VA_ARGS__)
#define comma_size(x, ...)     method_mn((x), comma_size)((x) ,##__VA_ARGS__)
#define comma_length(x, ...)   method_mn((x), comma_length)((x) ,##__VA_ARGS__)
#define spread(x, ...)         method_mn((x), spread)((x) ,##__VA_ARGS__)
#define unref(x)               method_mns((x),unref)((x))
#define unref_from(x, y)       method_ns((x), unref_from)((x), (y))
#define unref_with_note(x, y)  method_mn((x), unref_with_note)((x), (y))
#define unref_with_note_from(x, y, z) \
	method_mn((x), unref_with_note_from)((x), (y), (z))
#define unref_with_note_from(x, y, z) \
	method_mn((x), unref_with_note_from)((x), (y), (z))
#define vars(x, ...)           method_mn((x), vars)((x) ,##__VA_ARGS__)
#define had_vars(x, ...)       method_n((x), had_vars)((x) ,##__VA_ARGS__)
#define ikmt_eq(x, ...)        method_mn((x), ikmt_eq)((x) ,##__VA_ARGS__)
#define ikt_eq(x, ...)         ikmt_eq((x) ,##__VA_ARGS__, false)
#define iknt_eq(x, ...)        (!ikmt_eq((x) ,##__VA_ARGS__, true))
#define ikmt_eq_mini(x, ...)   method_mn((x), ikmt_eq_mini)((x) ,##__VA_ARGS__)
#define ikt_eq_mini(x, ...)    ikmt_eq_mini((x) ,##__VA_ARGS__, false)
#define iknt_eq_mini(x, ...)   (!ikmt_eq_mini((x) ,##__VA_ARGS__, true))
#define ikmt_ge_mini(x, ...)   method_m((x), ikmt_ge_mini)((x) ,##__VA_ARGS__)
#define ikt_ge_mini(x, ...)    ikmt_ge_mini((x) ,##__VA_ARGS__, false)
#define iknt_ge_mini(x, ...)   (!ikmt_ge_mini((x) ,##__VA_ARGS__, true))
#define ikmt_ge(x, ...)        method_mn((x), ikmt_ge)((x) ,##__VA_ARGS__)
#define ikt_ge(x, ...)         ikmt_ge((x) ,##__VA_ARGS__, false)
#define iknt_ge(x, ...)        (!ikmt_ge((x) ,##__VA_ARGS__, true))
#define ikmt_ze(x, ...)        method_mn((x), ikmt_ze)((x) ,##__VA_ARGS__)
#define ikt_ze(x, ...)         ikmt_ze((x) ,##__VA_ARGS__, false)
#define iknt_ze(x, ...)        (!ikmt_ze((x) ,##__VA_ARGS__, true))
#define ikmt_sort_ge(x, ...)   method_n((x), ikmt_sort_ge)((x) ,##__VA_ARGS__)
#define ikt_sort_ge(x, ...)    ikmt_sort_ge((x) ,##__VA_ARGS__, false)
#define iknt_sort_ge(x, ...)   (!ikmt_sort_ge((x) ,##__VA_ARGS__, true))
#define ikmt_be_true(x, ...)   method_mn((x), ikmt_be_true)((x) ,##__VA_ARGS__)
#define ikt_be_true(x, ...)    ikmt_be_true((x) ,##__VA_ARGS__, false)
#define iknt_be_true(x, ...)   (!ikmt_be_true((x) ,##__VA_ARGS__, true))
#define ikmt_be_constant(x,...) \
	method_mn((x), ikmt_be_constant)((x) ,##__VA_ARGS__)
#define ikt_be_constant(x,...) ikmt_be_constant((x) ,##__VA_ARGS__, false)
#define iknt_be_constant(x,...)(!ikmt_be_constant((x) ,##__VA_ARGS__, true))
#define ikmt_be_finite(x, ...) method_mn((x),ikmt_be_finite)((x) ,##__VA_ARGS__)
#define ikt_be_finite(x, ...)  ikmt_be_finite((x) ,##__VA_ARGS__, false)
#define iknt_be_finite(x, ...) (!ikmt_be_finite((x) ,##__VA_ARGS__, true))
#define ikmt_be_empty(x, ...)  method_mn((x), ikmt_be_empty)((x) ,##__VA_ARGS__)
#define ikt_be_empty(x, ...)   ikmt_be_empty((x) ,##__VA_ARGS__, false)
#define iknt_be_empty(x, ...)  (!ikmt_be_empty((x) ,##__VA_ARGS__, true))
#define ikmt_have_first_value(x, ...) \
	method_n((x), ikmt_have_first_value)((x) ,##__VA_ARGS__)
#define ikt_have_first_value(x, ...) \
	ikmt_have_first_value((x) ,##__VA_ARGS__, false)
#define iknt_have_first_value(x, ...) \
	(!ikmt_have_first_value((x) ,##__VA_ARGS__, true))
#define ikmt_have_labels(x, ...) \
	method_mn((x), ikmt_have_labels)((x) ,##__VA_ARGS__)
#define ikt_have_labels(x, ...) \
	ikmt_have_labels((x) ,##__VA_ARGS__, false)
#define iknt_have_labels(x, ...) \
	(!ikmt_have_labels((x) ,##__VA_ARGS__, true))
#define ikmt_be_in(x, ...)     method_n((x), ikmt_be_in)((x) ,##__VA_ARGS__)
#define ikt_be_in(x, ...)      ikmt_be_in((x) ,##__VA_ARGS__, false)
#define iknt_be_in(x, ...)     (!ikmt_be_in((x) ,##__VA_ARGS__, true))
#define ikmt_contain(x, ...)   method_n((x), ikmt_contain)((x) ,##__VA_ARGS__)
#define ikt_contain(x, ...)    ikmt_contain((x) ,##__VA_ARGS__, false)
#define iknt_contain(x, ...)   (!ikmt_contain((x) ,##__VA_ARGS__, true))
#define ikmt_define(x, ...)    method_n((x), ikmt_define)((x) ,##__VA_ARGS__)
#define ikt_define(x, ...)     ikmt_define((x) ,##__VA_ARGS__, false)
#define iknt_define(x, ...)    (!ikmt_define((x) ,##__VA_ARGS__, true))
#define comma_ikmt_be_constant(x,...) \
	method_n((x), comma_ikmt_be_constant)((x) ,##__VA_ARGS__)
#define comma_ikt_be_constant(x) comma_ikmt_be_constant((x), false)
#define comma_iknt_be_constant(x,...) \
	(!comma_ikmt_be_constant((x) ,##__VA_ARGS__, true))
#define comma_ikmt_be_finite(x, ...) method_n((x),comma_ikmt_be_finite)((x) ,##__VA_ARGS__)
#define comma_ikt_be_finite(x, ...)  comma_ikmt_be_finite((x) ,##__VA_ARGS__, false)
#define comma_iknt_be_finite(x, ...) \
		(!comma_ikmt_be_finite((x) ,##__VA_ARGS__, true))
#define ikmt_logi_eq(x, ...)   method_n((x), ikmt_logi_eq)((x) ,##__VA_ARGS__)
#define ikt_logi_eq(x, ...)    ikmt_logi_eq((x) ,##__VA_ARGS__, false)
#define iknt_logi_eq(x, ...)   (!ikmt_logi_eq((x) ,##__VA_ARGS__, true))
#define ikmt_contain_eval_instr(x, ...) \
	method_n((x), ikmt_contain_eval_instr)((x) ,##__VA_ARGS__)
#define ikt_contain_eval_instr(x, ...) \
	ikmt_contain_eval_instr((x) ,##__VA_ARGS__, false)
#define iknt_contain_eval_instr(x, ...) \
	!ikmt_contain_eval_instr((x) ,##__VA_ARGS__, true)
