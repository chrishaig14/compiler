//
// Created by chris on 17/12/20.
//

#include "GC.h"

void GC::enter_scope() {
    // std::cout << "Now entering scope" << std::endl;
    current_scope = new Scope(current_scope);
}

void GC::enter_function() {
    current_function_scope = new Scope(current_scope);
    current_scope = current_function_scope;
    function_scopes.push_back(current_function_scope);
}

XObject* GC::function_return(XObject* f) {
    current_scope = current_function_scope;
    if (f != nullptr) {
        // mark return value as reachable/root
        if (has_tag(f, OBJECT_TAG)) {
            UNTAG(f)->set_reachable();
        }
    }
    leave_scope();
    function_scopes.pop_back();
    if (function_scopes.empty()) {
        // this is main, probably
        return f;
    }
    current_function_scope = function_scopes.back();
    return f;
}

void GC::declare(const std::string& n, XObject* obj) {
    if (has_tag(obj, OBJECT_TAG)) {
        // std::cout<< "DECLARE OBJECT WITH TAG!" << std::endl;
        current_scope->declare(n, UNTAG(obj));
    }
}

void GC::set(const std::string& n, XObject* obj) {
    if (has_tag(obj, OBJECT_TAG)) {
        // std::cout<< "SET OBJECT WITH TAG!" << std::endl;

        current_scope->set(n, UNTAG(obj));
    }
}

void GC::leave_scope() {
    // std::cout << "Now leaving scope" << std::endl;
    Scope* old_scope = current_scope;
    current_scope = current_scope->parent;
    delete old_scope;
    if (current_scope != nullptr) {
        current_scope->get_reachable();
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
    sweep();
    // std::cout << "Done leaving scope" << std::endl;
}

XObject* GC::register_object(XObject* u) {
    XObject* o = UNTAG(u);
    // std::cout << "Registered object " << o << " (" << o->class_name << ")" << std::endl;
    // current_scope->objects.push_back(o);
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

Scope* GC::current_scope = nullptr;
Scope* GC::current_function_scope = nullptr;


std::vector<Scope*> GC::function_scopes;
std::vector<XObject*> GC::all_objects;