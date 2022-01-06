#include "lee.h"
#include "shared_shortcuts.h"

node* node_VOID =         static_empty_node('v'+'o'*256+'i'*65536);
node* node_ZERO =         static_empty_node('\'');
node* node_EMPTY_STRING = static_empty_node('"');
node* node_EMPTY_LIST =   static_empty_node('"');
node* node_EMPTY_SET =    static_empty_node('"');

node* empty_node_new(u32 as) {
	return node_new(
		as, 0,
		NULL, NULL, NULL, NULL, NULL, NULL,
		0, NULL, NULL,
		NULL, NULL
	);
}
