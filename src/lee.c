#include "lee.h"
#include "shared_shortcuts.h"
node* the_program;

int main(int argc, char** args) {
	/*
		This implementation of Lee is made for
		32-bit or 64-bit computers, with
		function pointers no larger than void* pointers, and
		little-endian numbers.
	*/
	static_assert(
		sizeof(void*(* )(void*)) <= sizeof(void*),
		"function pointers being larger than void pointers is not supported"
	);
	static_assert(
		#ifdef ENV64
			sizeof(void*) == 8,
		#else
			sizeof(void*) == 4,
		#endif
		"failed to detect void* size (util.h)"
	);
	/*static_assert(
		IS_LITTLE_ENDIAN,
		"non-little-endian platforms are not supported"
	);*/
	lee_parser* pa;
	node* x,* sx,* vas,* vsx,* dvas;
	fast_u8 iter_count;
	pa = lee_parser_new(NULL);
	while (parse(pa, NULL))
		putchar('.');
	putchar(10);
	x = the_program;
	print(x);
	puts("");
	print_bytes(x);
	puts("\nseng'd:");
	sx = seng(x, borrow(node_EMPTY_SET));
	puts("");
	print(sx);
	puts("");
	print_bytes(sx);
	iter_count = 3;
	while (ikt_have_labels(sx) && iter_count--) {
		puts("vars:");
		vas = vars(borrow(sx));
		assert(vas->ref_count);
		puts("");
		print(vas);
		puts("");
		print_bytes(vas);
		puts("\nenvarying:");
		vsx = envary_with_all_of(sx, borrow(vas));
		puts("");
		print(vsx);
		puts("\ndefining vars:");
		dvas = define_all(vas, vsx);
		puts("");
		print(dvas);
		puts("");
		print_bytes(dvas);
		puts("\nseng'd:");
		sx = seng(vsx, dvas);
		puts("");
		print(sx);
		puts("");
		print_bytes(sx);
		puts("");
	}
	unref(sx);
	global_scope_unref();
	root_node_scope_unref();
	destroy(pa);
	puts("");
}
