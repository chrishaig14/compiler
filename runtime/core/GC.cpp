//
// Created by chris on 17/12/20.
//

#include "GC.h"

TaggedObject* GC::declare(TaggedObject* obj) {
    if (has_tag(obj, OBJECT_TAG)) {
        XObject* o = UNTAG(obj);
        o->inc_count();
    }
    return obj;
}

TaggedObject* GC::assign(TaggedObject* old_value_t, TaggedObject* new_value_t) {
    if (old_value_t != nullptr) {
        if (has_tag(old_value_t, OBJECT_TAG)) {
            if (new_value_t != nullptr) {
                XObject* new_ = UNTAG(new_value_t);
                new_->inc_count();
            }
            XObject* old = UNTAG(old_value_t);
            old->dec_count();
            if (old->count == 0) {
                delete old;
            }
        }
    } else if (has_tag(new_value_t, OBJECT_TAG)) {
        XObject* new_ = UNTAG(new_value_t);
        new_->inc_count();
    }
    return new_value_t;
}

void GC::out_of_scope(TaggedObject* old_value_t) {
    if (old_value_t != nullptr) {
        if (has_tag(old_value_t, OBJECT_TAG)) {
            XObject* old = UNTAG(old_value_t);
            old->dec_count();
            if (old->count == 0) { // doesnt have tag
                delete old;
            } else if (old->count & (unsigned long) 1 << 63) {
                old->count = old->count & ~((unsigned long) 1 << 63);
            }
        }
    }
}

TaggedObject* GC::set_return(TaggedObject* obj) {
    if (obj != nullptr) {
        if (has_tag(obj, OBJECT_TAG)) {
            if (UNTAG(obj)->count != 0) {
                UNTAG(obj)->count = UNTAG(obj)->count | ((unsigned long) 1 << 63);
            }
        }
    }
    return obj;
}
