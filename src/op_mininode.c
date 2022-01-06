#include "lee.h"
#include "shared_shortcuts.h"

mininode node_mini_sub(node* this, node* that) {
	if (ikt_ze(this)) return
		mini_nop(lyse(this,
			that
		));
	if (ikt_ze(that)) return
		mini_neg(lyse(that,
			this
		));
	if (ikt_be_finite(this) && ikt_eq(this, that)) {
		unref(lyse(this, that));
		return mininode_ZERO;
	}
	return minimize(sub(this, that), NULL);
}
