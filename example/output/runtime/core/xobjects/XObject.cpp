//
// Created by chris on 3/3/21.
//

#include "XObject.h"
#include "../xobject.h"

XObject::XObject(std::string class_name) {
    this->class_name = class_name;
    this->is_list = false;
    this->is_string = false;
    this->count = 0;
}

XObject::~XObject() {
    std::cout << "Deleted object " << this << " (" << this->class_name << ")" << std::endl;
}

TaggedObject* XObject::__eq__(TaggedObject* pObject) {
    return MAKE_BOOL(false);
}

