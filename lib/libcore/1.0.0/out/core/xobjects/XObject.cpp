//
// Created by chris on 3/3/21.
//

#include "XObject.h"
#include "../xobject.h"

XObject::XObject(std::string class_name) {
    this->class_name = class_name;
    this->is_list = false;
    this->is_string = false;
    this->gc_info = {false, false, false, false,0};
}

XObject::~XObject() {
    // std::cout << "Deleted object " << this << " (" << this->class_name << ")" << std::endl;
}

TaggedObject* XObject::__eq__(TaggedObject* pObject) {
    return MAKE_BOOL(false);
}

void XObject::traverse_count() {
    for (auto* obj: this->get_all_members()) {
        obj->gc_info.count++;
    }
    // if (has_tag(this->next, OBJECT_TAG)) {
    // XObject* obj = UNTAG(this->next);
    // obj->gc_info.count++;
    // }
}

void XObject::traverse(std::vector<XObject*>& roots) {
    for (auto* obj: this->get_all_members()) {
        if (!obj->gc_info.in_roots) {
            std::cout << "adding root " << obj << std::endl;
            obj->gc_info.in_roots = true;
            roots.push_back(obj);
            obj->traverse(roots);
        } else {
            std::cout << "skipping " << obj << " because it's already a root " << std::endl;
        }
    }
}
