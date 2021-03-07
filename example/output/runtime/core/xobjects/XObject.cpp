//
// Created by chris on 3/3/21.
//

#include "XObject.h"
#include "../xobject.h"

XObject::XObject(std::string class_name) {
    this->class_name = class_name;
    this->is_list = false;
    this->inserted = false;
    this->reachable = false;
    this->is_string = false;
    this->is_user = false;
}

void XObject::set_reachable() {
    // std::cout << "Set object " << this << " (" << this->class_name << ") reachable" << std::endl;
    this->reachable = true;
}

void XObject::reset_reachable() {
    // std::cout << "Reset object " << this << " (" << this->class_name << ") reachable" << std::endl;
    this->reachable = false;
}

bool XObject::is_reachable() {
    return this->reachable;
}

XObject::~XObject() {
    // std::cout << "Deleted object " << this << " (" << this->class_name << ")" << std::endl;
}

TaggedObject* XObject::__eq__(TaggedObject* pObject) {
    return MAKE_BOOL(false);
}

