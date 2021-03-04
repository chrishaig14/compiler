//
// Created by chris on 4/3/21.
//

#include "Tuple.h"

Tuple::Tuple(const std::string& n, int num) : XObject(n), members(num, nullptr) {}

void Tuple::mark(std::vector<XObject*>& new_root) {
    for (auto& m: this->members) {
        if (has_tag(m, OBJECT_TAG)) {
            XObject* element = UNTAG(m);
            if (!element->is_reachable() && !element->inserted) {
                new_root.push_back(element);
                element->inserted = true;
            }
        }
    }
}


Tuple2::Tuple2(XObject* mem_1, XObject* mem_2) : Tuple("Tuple2", 2) {
    this->members[0] = mem_1;
    this->members[1] = mem_2;
}

XObject* Tuple2::get_member(int i) {
    assert(i >= 1 && i <= 2);
    return this->members[i - 1];
}

Tuple3::Tuple3(XObject* mem_1, XObject* mem_2, XObject* mem_3) : Tuple("Tuple3", 3) {
    this->members[0] = mem_1;
    this->members[1] = mem_2;
    this->members[2] = mem_3;
}

XObject* Tuple3::get_member(int i) {
    assert(i >= 1 && i <= 3);
    return this->members[i - 1];
}
