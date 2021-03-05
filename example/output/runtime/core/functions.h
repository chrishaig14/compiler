//
// Created by chris on 17/12/20.
//

#ifndef UNTITLED1_FUNCTIONS_H
#define UNTITLED1_FUNCTIONS_H

#include <vector>
#include "xobject.h"

class Function0 : public XObject {
    TaggedObject* (* f)();
public:
    Function0() : XObject("function0") { this->f = nullptr; }

    Function0(TaggedObject* (* f)()) : XObject("function0") { this->f = f; }

    virtual TaggedObject* operator()() {
        return this->f();
    }

    void mark(std::vector<XObject*>& new_root) override {

    }
};

class Function1 : public XObject {
    TaggedObject* (* f)(TaggedObject*);
public:
    Function1() : XObject("function1") { this->f = nullptr; }

    Function1(TaggedObject* (* f)(TaggedObject*)) : XObject("function1") { this->f = f; }

    virtual TaggedObject* operator()(TaggedObject* a0) {
        return this->f(a0);
    }

    void mark(std::vector<XObject*>& new_root) override {

    }
};

class Function2 : public XObject {
    TaggedObject* (* f)(TaggedObject*, TaggedObject*);
public:
    Function2() : XObject("function2") { this->f = nullptr; }

    Function2(TaggedObject* (* f)(TaggedObject*, TaggedObject*)) : XObject("function2") { this->f = f; }

    virtual TaggedObject* operator()(TaggedObject* a0, TaggedObject* a1) {
        return this->f(a0, a1);
    }

    void mark(std::vector<XObject*>& new_root) override {

    }
};

class Function3 : public XObject {
    TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*);
public:
    Function3() : XObject("function3") { this->f = nullptr; }

    Function3(TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*)) : XObject("function3") { this->f = f; }

    virtual TaggedObject* operator()(TaggedObject* a0, TaggedObject* a1, TaggedObject* a2) {
        return this->f(a0, a1, a2);
    }

    void mark(std::vector<XObject*>& new_root) override {

    }
};

class Partial0 : public Function0 {
    std::vector<TaggedObject*> p;
    union {
        Function1* f1;
        Function2* f2;
        Function3* f3;
    } f;
public:
    Partial0(Function1* f, TaggedObject* p0) : Function0() {
        this->p = {p0};
        this->f.f1 = f;
    }

    Partial0(Function2* f, TaggedObject* p0, TaggedObject* p1) : Function0() {
        this->p = {p0, p1};
        this->f.f2 = f;
    }

    Partial0(Function3* f, TaggedObject* p0, TaggedObject* p1, TaggedObject* p2) : Function0() {
        this->p = {p0, p1, p2};
        this->f.f3 = f;
    }

    TaggedObject* operator()() override {
        std::vector<TaggedObject*> args = this->p;
        if (args.size() == 1) {
            return (*this->f.f1)(args[0]);
        } else if (args.size() == 2) {
            return (*this->f.f2)(args[0], args[1]);
        } else if (args.size() == 3) {
            return (*this->f.f3)(args[0], args[1], args[2]);
        } else {
            throw std::runtime_error("bad args!");
        }
    }
};

class Partial1 : public Function1 {
    std::vector<TaggedObject*> p;
    union {
        Function2* f2;
        Function3* f3;
    } f;
public:
    Partial1(Function2* f, TaggedObject* p0, TaggedObject* p1) {
        this->p = {p0, p1};
        this->f.f2 = f;
    }

    Partial1(Function3* f, TaggedObject* p0, TaggedObject* p1, TaggedObject* p2) {
        this->p = {p0, p1, p2};
        this->f.f3 = f;
    }

    TaggedObject* operator()(TaggedObject* a0) override {
        std::vector<TaggedObject*> cur_args = {a0};
        std::vector<TaggedObject*> args(this->p.size(), nullptr);
        int k = 0;
        for (int i = 0; i < this->p.size(); i++) {
            if (this->p[i] == nullptr) {
                args[i] = cur_args[k];
                k++;
            } else {
                args[i] = this->p[i];
            }
        }
        if (args.size() == 2) {
            return (*this->f.f2)(args[0], args[1]);
        }
        if (args.size() == 3) {
            return (*this->f.f3)(args[0], args[1], args[2]);
        }
        throw std::runtime_error("bad args!");
    }
};

class Partial2 : public Function2 {
    std::vector<TaggedObject*> p;
    union {
        Function3* f3;
    } f;
public:
    Partial2(Function3* f, TaggedObject* p0, TaggedObject* p1, TaggedObject* p2) {
        this->p = {p0, p1, p2};
        this->f.f3 = f;
    }

    TaggedObject* operator()(TaggedObject* a0, TaggedObject* a1) override {
        std::vector<TaggedObject*> cur_args = {a0, a1};
        std::vector<TaggedObject*> args(this->p.size(), nullptr);
        int k = 0;
        for (int i = 0; i < this->p.size(); i++) {
            if (this->p[i] == nullptr) {
                args[i] = cur_args[k];
                k++;
            } else {
                args[i] = this->p[i];
            }
        }
        if (args.size() == 3) {
            return (*this->f.f3)(args[0], args[1], args[2]);
        }
        throw std::runtime_error("Bad args");
    }
};

#endif //UNTITLED1_FUNCTIONS_H
