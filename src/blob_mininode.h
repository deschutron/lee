#ifndef blob_mininode_INCLUDED
#define blob_mininode_INCLUDED

#include "numtypes.h"
#include "mininode.h"

#define static_blob_mininode(x_as, x_s, x_len) (mininode){\
	.s = (x_s),\
	.as = (x_as),\
	.type = 's',\
	.len = (x_len),\
	mininode_SET_TAG_TO_ZERO\
};
mininode mininode_TRUE;
mininode mininode_FALSE;
mininode mininode_ZERO;
mininode mininode_ONE;
mininode mininode_TWO;
mininode mininode_X_STRING;
mininode mininode_Y_STRING;
mininode mininode_DOT_STRING;
mininode mininode_SLASH_STRING;

mininode blob_mininode(u32, u8*, fast_u8);
mininode leaf_mininode(u32, const char*);
mininode err_mininode(char*);

#endif
