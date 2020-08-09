//
// Created by chris on 5/8/20.
//

#include "UserObject.h"

bool UserObject::equal(const Object* other) const {
    const UserObject* other_ptr = dynamic_cast<const UserObject*>(other);
    if (other_ptr == nullptr) return false;
    if (this->type != other_ptr->type) return false;
    for (auto f: this->fields) {
        if (other_ptr->fields.count(f.first) == 0) return false;
        if (!f.second->equal(other_ptr->fields.at(f.first))) return false;
    }
    return true;
}
