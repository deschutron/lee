#include "lee.h"
#include "shared_shortcuts.h"

mininode mininode_NULL = (mininode){.p = NULL, .footer = 0};
mininode mininode_VOID =
	#ifdef ENV64
		static_empty_mininode('v'+('o'+ 'i'*256)*256)
	#else
		(mininode){
			.l = node_VOID,
			.as = '+'+ '_'*256,
			.type = 'n',
			.len = 1
		}
	#endif
;
mininode mininode_EMPTY_STRING = static_empty_mininode('"');
mininode mininode_EMPTY_LIST = static_empty_mininode('"');
mininode mininode_EMPTY_SET  = static_empty_mininode('"');

mininode empty_mininode(u32 as) {
	fprintf(stderr, "▵");
	return static_empty_mininode(as);
}
