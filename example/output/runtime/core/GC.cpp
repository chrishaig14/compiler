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

TaggedObject* GC::function_return(TaggedObject* f) {
    if (f != nullptr) {
        // mark return value as reachable/root
        if (has_tag(f, OBJECT_TAG)) {
            UNTAG(f)->set_reachable();
        }
    }
    frames.pop_back();
    if (frames.empty()) {
        // this is main, probably
        leave_scope(nullptr);
        return f;
    }
    delete frame;
    frame = frames.back();
    leave_scope(frame->scope);
    return f;
}

TaggedObject* GC::temp(TaggedObject* obj) {
    if (has_tag(obj, OBJECT_TAG)) {
        // std::cout<< "DECLARE OBJECT WITH TAG!" << std::endl;
        frame->scope->declare("temp" + std::to_string(frame->scope->temp), UNTAG(obj));
        frame->scope->temp++;
    }
    return obj;
}

void GC::declare(const std::string& n, TaggedObject* obj) {
    if (has_tag(obj, OBJECT_TAG)) {
        // std::cout<< "DECLARE OBJECT WITH TAG!" << std::endl;
        frame->scope->declare(n, UNTAG(obj));
    }
}

void GC::set(const std::string& n, TaggedObject* obj) {
    if (has_tag(obj, OBJECT_TAG)) {
        // std::cout<< "SET OBJECT WITH TAG!" << std::endl;

        frame->scope->set(n, UNTAG(obj));
    }
}

void GC::leave_scope(Scope* parent) {
    if (parent != nullptr) {
        frame->scope = parent;
        if (frame->scope != nullptr) {
            frame->scope->get_reachable();
        }
        for (int i = 0; i < frames.size() - 1; i++) {
            frames[i]->scope->get_reachable();
        }
    }
    std::vector<XObject*> root(all_objects.size(), nullptr);
    int k = 0;
    for (int i = 0; i < all_objects.size(); i++) {
        // all_objects[i]->reset_reachable();
        if (all_objects[i]->is_reachable()) {
            root[k] = all_objects[i];
            all_objects[i]->reset_reachable();
            k++;
        }
    }
    root.resize(k);
    mark(root);
    for (int i = 0; i < all_objects.size(); i++) {
        // all_objects[i]->reset_reachable();
            all_objects[i]->inserted= false;
            k++;
    }
    sweep();
    // std::cout << "Done leaving scope" << std::endl;
}

void GC::leave_local_scope() {
    // std::cout << "Now leaving scope" << std::endl;
    Scope* old = frame->scope;
    GC::leave_scope(old->parent);
    delete old;
}

TaggedObject* GC::register_object(TaggedObject* u) {
    XObject* o = UNTAG(u);
    // std::cout << "Registered object " << o << " (" << o->class_name << ")" << std::endl;
    // frame->scope->objects.push_back(o);
    all_objects.push_back(o);
    return u;
}

void GC::mark(const std::vector<XObject*>& root) {
    int root_size = root.size();
    // std::cout << "The root size is " << root_size << std::endl;
    std::vector<XObject*> new_root;
    for (int i = 0; i < root_size; i++) {
        XObject* obj = root[i];
        // std::cout << "Checking object of class: " << obj->class_name << std::endl;
        obj->set_reachable();
        obj->mark(new_root);
    }
    if (new_root.size() != 0) {
        mark(new_root);
    }
}

void GC::sweep() {
    // survivors->resize(all_objects->size(), nullptr);
    int c = 0;
    int total = all_objects.size();
    for (int i = 0; i < total; i++) {
        XObject* obj = all_objects[i];
        if (!obj->is_reachable()) {
            delete obj;
        } else {
            all_objects[c] = obj;
            // survivors.push_back(obj);
            obj->reset_reachable();
            c++;
        }
    }
    all_objects.resize(c);
    // all_objects = std::vector<XObject*>(survivors.begin(), survivors.begin() + c);
}

Frame* GC::frame = nullptr;


std::vector<Frame*> GC::frames;
std::vector<XObject*> GC::all_objects;