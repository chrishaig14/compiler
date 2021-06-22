//
// Created by chris on 17/12/20.
//

#include "GC.h"

bool GC::collecting = false;

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

// void traverse(std::vector<XObject*>& roots) override {
// if (has_tag(this->next, OBJECT_TAG)) {
// XObject* obj = UNTAG(this->next);
// if (!obj->gc_info.in_roots) {
// obj->gc_info.in_roots = true;
// roots.push_back(obj);
// obj->traverse(roots);
// }
// }
// }
//
// void traverse_count() override {
// if (has_tag(this->next, OBJECT_TAG)) {
// XObject* obj = UNTAG(this->next);
// obj->gc_info.count++;
// }
// }

void GC::out_of_scope(TaggedObject* old_value_t, std::string c) {
    if (old_value_t != nullptr) {
        if (has_tag(old_value_t, OBJECT_TAG)) {
            XObject* old = dynamic_cast<XObject*>(UNTAG(old_value_t));
            if (GC::collecting and old->gc_info.count == 0) {
                delete old;
                return;
            }
            // std::cout << "object " << c << " : " << old << " out of scope " << std::endl;
            // std::cout << "(" << old << ") count before decreasing: " << old->gc_info.count << std::endl;
            old->dec_count();
            // std::cout << "(" << old << ") count after decreasing: " << old->gc_info.count << std::endl;
            if (old->gc_info.count == 0) { // doesnt have tag
                // std::cout << old << " deleting because cound == 0" << std::endl;
                delete old;
            } else {
                // std::cout << old << " out of scope but count != 0 " << std::endl;
                std::vector<XObject*> roots;
                std::vector<unsigned long> old_counts;
                old->gc_info.in_roots = true;
                roots.push_back(old);
                old->traverse(roots);
                // std::cout << "there are " << roots.size() << " roots " << std::endl;
                for (auto o: roots) {
                    old_counts.push_back(o->gc_info.count);
                    o->gc_info.count = 0;
                }
                for (auto o: roots) {
                    o->traverse_count();
                }

                bool found_isolated_cycle = true;
                for (size_t i = 0; i < roots.size(); i++) {
                    // std::cout << "total count for (" << roots[i] << ") = " << old_counts[i] << std::endl;
                    // std::cout << "inner count for (" << roots[i] << ") = " << roots[i]->gc_info.count << std::endl;
                    if (roots[i]->gc_info.count != old_counts[i]) {
                        found_isolated_cycle = false;
                        break;
                    }
                }
                if (found_isolated_cycle) {
                    GC::collecting = true;
                    // std::cout << "Found a cycle" << std::endl;
                    for (auto o: roots) {
                        std::cout << "Deleted " << o << std::endl;
                        delete o;
                    }
                    GC::collecting = false;
                } else {
                    for (size_t i = 0; i < roots.size(); i++) {
                        roots[i]->gc_info.in_roots = false;
                        roots[i]->gc_info.count = old_counts[i];
                    }
                    old->gc_info.RETURN = false;
                }

                // std::cout << "finished for object " << c << std::endl;
                std::cout << std::endl;
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
