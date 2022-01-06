#ifndef empty_mininode_INCLUDED
#define empty_mininode_INCLUDED

#include "numtypes.h"
#include "mininode.h"

#define static_empty_mininode(x_as) (mininode){\
	.p = NULL,\
	.as = (x_as),\
	.type = '_',\
	.len = 0,\
	mininode_SET_TAG_TO_ZERO\
}

mininode mininode_NULL;
mininode mininode_VOID;
mininode mininode_EMPTY_STRING;
mininode mininode_EMPTY_LIST;
mininode mininode_EMPTY_SET;

mininode empty_mininode(u32);

#endif
