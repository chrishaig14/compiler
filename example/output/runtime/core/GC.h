//
// Created by chris on 17/12/20.
//

#ifndef UNTITLED1_GC_H
#define UNTITLED1_GC_H

#include <iostream>
#include "xobject.h"

class Scope {
public:
    Scope(Scope* parent) {
        this->parent = parent;
    }

    std::unordered_map<std::string, XObject*> vars;
    // std::vector<XObject*> reachable;

    void declare(const std::string& n, XObject* v) {
        this->vars[n] = v;
    }

    void set(const std::string& n, XObject* v) {
        if (this->vars.count(n) == 0) {
            this->parent->set(n, v);
        } else {
            this->vars[n] = v;
        }
    }

    void get_reachable() {
        // this->reachable.resize(this->vars.size(), nullptr);
        // int c = 0;
        for (auto v: this->vars) {
            XObject* obj = v.second;
            obj->set_reachable();
            // this->reachable[c] = obj;
            // c++;
            // if (obj->is_reachable()) {
            //     already in reachable list, don't add again
            // } else {
            //     obj->set_reachable();
            //     this->reachable[c] = obj;
            //     c++;
            // }
        }
        if (this->parent != nullptr) {
            this->parent->get_reachable();
            // this->reachable.insert(this->reachable.end(), parent_reachable.begin(), parent_reachable.end());
        }
        // return this->reachable;
        // int c = 0;
        // std::vector<XObject*> extra(this->vars.size(), nullptr);
        // for (auto v: this->vars) {
        //     XObject* obj = v.second;
        //     if (obj->is_reachable()) {
        //         // already in reachable list, don't add again
        //     } else {
        //         obj->set_reachable();
        //         extra[c] = obj;
        //         c++;
        //     }
        // }
        // this->reachable = std::vector<XObject*>(extra.begin(), extra.begin() + c);
        // if (this->parent != nullptr) {
        //     const std::vector<XObject*>& reachable_parent = this->parent->get_reachable();
        //     std::vector<XObject*> extra_p(this->vars.size(), nullptr);
        //     int k = 0;
        //     for (auto obj: reachable_parent) {
        //         if (obj->is_reachable()) {
        //             // already in reachable list, don't add again
        //         } else {
        //             obj->set_reachable();
        //             extra_p[k] = obj;
        //             k++;
        //         }
        //     }
        //     this->reachable.insert(this->reachable.end(), extra_p.begin(), extra_p.begin() + k);
        // }

        // return this->
        //         reachable;
    }

    Scope* parent;
// std::vector<XObject*> objects;
};

class GC {
    static std::vector<XObject*> all_objects;
    static Scope* current_scope;
    static Scope* current_function_scope;
    static std::vector<Scope*> function_scopes;
public:
    static void enter_scope() {
        // std::cout << "Now entering scope" << std::endl;
        current_scope = new Scope(current_scope);
    }

    static void enter_function() {
        current_function_scope = new Scope(current_scope);
        current_scope = current_function_scope;
        function_scopes.push_back(current_function_scope);
    }

    static XObject* function_return(XObject* f) {
        current_scope = current_function_scope;
        if (f != nullptr) {
            // mark return value as reachable/root
            if (has_tag(f, OBJECT_TAG)) {
                f->set_reachable();
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

    static void declare(const std::string& n, XObject* obj) {
        if (has_tag(obj, OBJECT_TAG)) {
            // std::cout<< "DECLARE OBJECT WITH TAG!" << std::endl;
            current_scope->declare(n, UNTAG(obj));
        }
    }

    static void set(const std::string& n, XObject* obj) {
        if (has_tag(obj, OBJECT_TAG)) {
            // std::cout<< "SET OBJECT WITH TAG!" << std::endl;

            current_scope->set(n, UNTAG(obj));
        }
    }

    static void leave_scope() {
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


    static XObject* register_object(XObject* u) {
        XObject* o = UNTAG(u);
        // std::cout << "Registered object " << o << " (" << o->class_name << ")" << std::endl;
        // current_scope->objects.push_back(o);
        all_objects.push_back(o);
        return u;
    }

    static void mark(const std::vector<XObject*>& root) {
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

    static void sweep() {
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

};

Scope* GC::current_scope = nullptr;
Scope* GC::current_function_scope = nullptr;
std::vector<Scope*> GC::function_scopes;
std::vector<XObject*> GC::all_objects;
// std::vector<XObject*>* GC::survivors = new std::vector<XObject*>();

#endif //UNTITLED1_GC_H
