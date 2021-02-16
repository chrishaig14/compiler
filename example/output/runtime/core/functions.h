//
// Created by chris on 17/12/20.
//

#ifndef UNTITLED1_FUNCTIONS_H
#define UNTITLED1_FUNCTIONS_H

#include <vector>
#include "xobject.h"

class Function0 : public XObject {
    XObject* (* f)();
public:
    Function0() : XObject("function0") { this->f = nullptr; }

    Function0(XObject* (* f)()) : XObject("function0") { this->f = f; }

    virtual XObject* operator()() {
        return this->f();
    }

    void mark(std::vector<XObject*>& new_root) override {

    }
};

class Function1 : public XObject {
    XObject* (* f)(XObject*);
public:
    Function1() : XObject("function1") { this->f = nullptr; }

    Function1(XObject* (* f)(XObject*)) : XObject("function1") { this->f = f; }

    virtual XObject* operator()(XObject* a0) {
        return this->f(a0);
    }

    void mark(std::vector<XObject*>& new_root) override {

    }
};

class Function2 : public XObject {
    XObject* (* f)(XObject*, XObject*);
public:
    Function2() : XObject("function2") { this->f = nullptr; }

    Function2(XObject* (* f)(XObject*, XObject*)) : XObject("function2") { this->f = f; }

    virtual XObject* operator()(XObject* a0, XObject* a1) {
        return this->f(a0, a1);
    }

    void mark(std::vector<XObject*>& new_root) override {

    }
};

class Function3 : public XObject {
    XObject* (* f)(XObject*, XObject*, XObject*);
public:
    Function3() : XObject("function3") { this->f = nullptr; }

    Function3(XObject* (* f)(XObject*, XObject*, XObject*)) : XObject("function3") { this->f = f; }

    virtual XObject* operator()(XObject* a0, XObject* a1, XObject* a2) {
        return this->f(a0, a1, a2);
    }

    void mark(std::vector<XObject*>& new_root) override {

    }
};

class Partial0 : public Function0 {
    std::vector<XObject*> p;
    union {
        Function1* f1;
        Function2* f2;
        Function3* f3;
    } f;
public:
    Partial0(Function1
             * f,
             XObject* p0
    ) : Function0() {
        this->p = {p0};
        this->f.f1 = f;
    }

    Partial0(Function2
             * f,
             XObject* p0,
             XObject* p1
    ) : Function0() {
        this->p = {p0, p1};
        this->f.f2 = f;
    }

    Partial0(Function3
             * f,
             XObject* p0,
             XObject* p1,
             XObject* p2
    ) : Function0() {
        this->p = {p0, p1, p2};
        this->f.f3 = f;
    }

    XObject* operator()() override {
        std::vector<XObject*> args = this->p;
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
    std::vector<XObject*> p;
    union {
        Function2* f2;
        Function3* f3;
    } f;
public:
    Partial1(Function2* f, XObject* p0, XObject* p1) {
        this->p = {p0, p1};
        this->f.f2 = f;
    }

    Partial1(Function3* f, XObject* p0, XObject* p1, XObject* p2) {
        this->p = {p0, p1, p2};
        this->f.f3 = f;
    }

    XObject* operator()(XObject* a0) override {
        std::vector<XObject*> cur_args = {a0};
        std::vector<XObject*> args(this->p.size(), nullptr);
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
    std::vector<XObject*> p;
    union {
        Function3* f3;
    } f;
public:
    Partial2(Function3* f, XObject* p0, XObject* p1, XObject* p2) {
        this->p = {p0, p1, p2};
        this->f.f3 = f;
    }

    XObject* operator()(XObject* a0, XObject* a1) override {
        std::vector<XObject*> cur_args = {a0, a1};
        std::vector<XObject*> args(this->p.size(), nullptr);
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
