#include "lee.h"
#include "shared_shortcuts.h"

#define boo 'b'+('o'+ 'o'*256)*256
#define num 'n'+('u'+ 'm'*256)*256
mininode mininode_TRUE = static_blob_mininode(boo, "true" "\0\0\0", 4);
mininode mininode_FALSE = static_blob_mininode(boo, "false" "\0\0", 5);
mininode mininode_ZERO = static_blob_mininode(num, "0" "\0\0\0\0\0\0", 1);
mininode mininode_ONE  = static_blob_mininode(num, "1" "\0\0\0\0\0\0", 1);
mininode mininode_TWO  = static_blob_mininode(num, "2" "\0\0\0\0\0\0", 1);
mininode mininode_X_STRING = static_blob_mininode('"', "x" "\0\0\0\0\0\0", 1);
mininode mininode_Y_STRING = static_blob_mininode('"', "y" "\0\0\0\0\0\0", 1);
mininode mininode_DOT_STRING =
	static_blob_mininode('"', "." "\0\0\0\0\0\0\0", 1);
mininode mininode_SLASH_STRING =
	static_blob_mininode('"', "/" "\0\0\0\0\0\0\0", 1);
#undef num
#undef boo

mininode blob_mininode(u32 as, u8* s, fast_u8 len) {
	return mininode_of(as, NULL, s, NULL, len);
}
mininode leaf_mininode(u32 as, const char* s) {
	fast_u8 len;
	len = strnlen(s, mininode_s_BUFLEN + 1);
	if (len >= mininode_s_BUFLEN + 1) return
		box_mininode(stru32z("+_"), leaf_node_new(as, s));
	return mininode_of(as, NULL, (u8*)s, NULL, len);
}
mininode err_mininode(char* msg) {
	return leaf_mininode(stru32z("ERR"), msg);
}
