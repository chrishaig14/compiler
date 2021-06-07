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
            GC::out_of_scope(old_value_t);
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
            if (old->gc_info.count == 0) { // doesnt have tag
                delete old;
            } else {
                old->gc_info.RETURN = false;
            }
        }
    }
}

TaggedObject* GC::set_return(TaggedObject* obj) {
    if (obj != nullptr) {
        if (has_tag(obj, OBJECT_TAG)) {
            if (UNTAG(obj)->gc_info.count != 0) {
                UNTAG(obj)->gc_info.RETURN = true;
            }
        }
    }
    return obj;
}
