#include "lee.h"
#include "shared_shortcuts.h"

mininode box_mininode(u32 as, node* l) {
	return mininode_of(as, l, NULL, NULL, 1);
}
node* box_mininode_seng(mininode this, scope* sc, node* vads) {
	node*(* f)(node*);
	f = node_box_f(this.as);
	if (!this.l) return
		err_node_new(
			"No arg to evalue box node",
			restore(this, sc)
		);
	node* sl;
	sl = seng(this.l, vads);
	if (sl->as == stru32z("ERR")) return
		sl;
	if (f) return
		(*f)(sl);
	return with_scope(box_node_new(this.as, sl), sc);
}
mininode box_mininode_mini_seng(mininode this, scope* sc, node* vads) {
	node*(* f)(node*);
	mininode(* mini_f)(node*);
	f = node_box_f(this.as);
	mini_f = mininode_box_f(this.as);
	if (!this.l) return
		err_mininode("nup");
	node* sl;
	sl = seng(this.l, vads);
	if (sl->as == stru32z("ERR")) return
		minimize(sl, sc);
	if (mini_f && !sc ?: !sl->sc ?: sc == sl->sc) return
		(*mini_f)(sl);
	if (f) return
		minimize((*f)(sl), sc);
	return box_mininode(this.as, sl);
}
mininode(* mininode_box_f(u32 as))(node*) {
	return
		as == stru32z("+_") ?
			&node_mini_nop :
		as == stru32z("-_") ?
			&node_mini_neg :
		as == stru32z("*_") ?
			&node_mini_first :
			NULL;
}
mininode node_mini_nop(node* this) {
	return minimize(this, this->sc);
}
mininode node_mini_neg(node* this) {
	return
		this->as == stru32z("-_") ?
			minimize(this->l, this->sc) :
		ikt_eq(this, node_ZERO) ?
			mininode_ZERO :
		!this->as && this->op == '-' ?
			mini_sub(lyse(this, borrow(this->r)), borrow(this->l)) :
		minimize(neg(this), this->sc);
}
mininode node_mini_first(node* this) {
	return mini_lyse(this,
		this->as == '"' ?
			blob_mininode('\'', this->s, 1)
		:
		this->as == stru32z("&_") ?
			this->l_type == 'm' ?
				borrow(this->ml) :
				minimize(borrow(this->l), borrow(this->sc)) :
		this->as == stru32z("(,") ?
			this->l_type == 'm' ?
				this->ml.as == stru32z("..._") ?
					mini_first(borrow(this->ml.l)) :
					borrow(this->ml) :
				minimize(comma_first(borrow(this->l)), borrow(this->sc)) :
				minimize(borrow(this), borrow(this->sc))
	);
}
