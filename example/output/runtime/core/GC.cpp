//
// Created by chris on 17/12/20.
//

#include "GC.h"

void GC::enter_local_scope() {
    // std::cout << "Now entering scope" << std::endl;
    frame->scope = new Scope(frame->scope);
}

void GC::enter_function(std::string function_name) {
    frame = new Frame(function_name);
    frames.push_back(frame);
    GC::enter_local_scope();
}

TaggedObject* GC::temp(TaggedObject* obj) {
    if (has_tag(obj, OBJECT_TAG)) {
        // std::cout<< "DECLARE OBJECT WITH TAG!" << std::endl;
        frame->scope->declare("temp" + std::to_string(frame->scope->temp), UNTAG(obj));
        frame->scope->temp++;
    }
    return obj;
}

TaggedObject* GC::declare(TaggedObject* obj) {
    if (has_tag(obj, OBJECT_TAG)) {
        XObject* o = UNTAG(obj);
        o->inc_count();
    }
    return obj;
}

Frame* GC::frame = nullptr;

TaggedObject* GC::inc(TaggedObject* pObject) {
    XObject* o = UNTAG(pObject);
    o->count++;
    return pObject;
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
            if (old->count == 0) {
                delete old;
            }
        }
    }
}


std::vector<Frame*> GC::frames;
std::vector<XObject*> GC::all_objects;