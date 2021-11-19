//
// Created by chris on 14/4/21.
//
#include "Function.h"
#include "GC.h"
TaggedObject* Partial2::operator()(TaggedObject* a0, TaggedObject* a1) {
    std::vector<TaggedObject*> cur_args = {a0, a1};
    std::vector<TaggedObject*> args(this->p.size(), nullptr);
    int k = 0;
    for (size_t i = 0; i < this->p.size(); i++) {
        if (this->p[i] == nullptr) {
            args[i] = cur_args[k];
            k++;
        } else {
            args[i] = this->p[i];
        }
    }
    if (args.size() == 3) {
        return CALL3(this->f.f3, args[0], args[1], args[2]);
    }
    throw std::runtime_error("Bad args");
}

Partial2::Partial2(TaggedObject* f, TaggedObject* p0, TaggedObject* p1, TaggedObject* p2) {
    this->p = {p0, p1, p2};
    this->f.f3 = f;
}

TaggedObject* Partial1::operator()(TaggedObject* a0) {
    std::vector<TaggedObject*> cur_args = {a0};
    std::vector<TaggedObject*> args(this->p.size(), nullptr);
    int k = 0;
    for (size_t i = 0; i < this->p.size(); i++) {
        if (this->p[i] == nullptr) {
            args[i] = cur_args[k];
            k++;
        } else {
            args[i] = this->p[i];
        }
    }
    if (args.size() == 2) {
        return CALL2(this->f.f2, args[0], args[1]);
    }
    if (args.size() == 3) {
        return CALL3(this->f.f3, args[0], args[1], args[2]);
    }
    throw std::runtime_error("bad args!");
}

Partial1::Partial1(TaggedObject* f, TaggedObject* p0, TaggedObject* p1, TaggedObject* p2) {
    this->p = {GC::declare(p0), GC::declare(p1), GC::declare(p2)};
    this->f.f3 = f;
}

Partial1::Partial1(TaggedObject* f, TaggedObject* p0, TaggedObject* p1) {
    this->p = {GC::declare(p0), GC::declare(p1)};
    this->f.f2 = f;
}

TaggedObject* Partial0::operator()() {
    std::vector<TaggedObject*> args = this->p;
    if (args.size() == 1) {
        return CALL1(this->f.f1, args[0]);
    } else if (args.size() == 2) {
        return CALL2(this->f.f2, args[0], args[1]);
    } else if (args.size() == 3) {
        return CALL3(this->f.f3, args[0], args[1], args[2]);
    } else {
        throw std::runtime_error("bad args!");
    }
}

Partial0::Partial0(TaggedObject* f, TaggedObject* p0, TaggedObject* p1, TaggedObject* p2) : Function0() {
    this->p = {p0, p1, p2};
    this->f.f3 = f;
}

Partial0::Partial0(TaggedObject* f, TaggedObject* p0, TaggedObject* p1) : Function0() {
    this->p = {p0, p1};
    this->f.f2 = f;
}

Partial0::Partial0(TaggedObject* f, TaggedObject* p0) : Function0() {
    this->p = {p0};
    this->f.f1 = f;
}


TaggedObject* Function4::operator()(TaggedObject* a0, TaggedObject* a1, TaggedObject* a2, TaggedObject* a3) {
    return this->f(a0, a1, a2, a3);
}

Function4::Function4(TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*, TaggedObject*)) : XObject(
        "function4") {
    this->f = f;
}

Function4::Function4() : XObject("function4") {
    this->f = nullptr;
}

TaggedObject*
Function5::operator()(TaggedObject* a0, TaggedObject* a1, TaggedObject* a2, TaggedObject* a3, TaggedObject* a4) {
    return this->f(a0, a1, a2, a3, a4);
}

Function5::Function5(TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*, TaggedObject*, TaggedObject*))
        : XObject("function5") {
    this->f = f;
}

Function5::Function5() : XObject("function5") {
    this->f = nullptr;
}

TaggedObject* Function3::operator()(TaggedObject* a0, TaggedObject* a1, TaggedObject* a2) {
    return this->f(a0, a1, a2);
}

Function3::Function3(TaggedObject* (* f)(TaggedObject*, TaggedObject*, TaggedObject*)) : XObject("function3") {
    this->f = f;
}

Function3::Function3() : XObject("function3") {
    this->f = nullptr;
}

Function2::~Function2() {

}

TaggedObject* Function2::operator()(TaggedObject* a0, TaggedObject* a1) {
    return this->f(a0, a1);
}

Function2::Function2(TaggedObject* (* f)(TaggedObject*, TaggedObject*)) : XObject("function2") {
    this->f = f;
}

Function2::Function2() : XObject("function2") {
    this->f = nullptr;
}

TaggedObject* Function1::operator()(TaggedObject* a0) {
    return this->f(a0);
}

Function1::Function1(TaggedObject* (* f)(TaggedObject*)) : XObject("function1") {
    this->f = f;
}

Function1::Function1() : XObject("function1") {
    this->f = nullptr;
}

TaggedObject* Function0::operator()() {
    return this->f();
}

Function0::Function0(TaggedObject* (* f)()) : XObject("function0") {
    this->f = f;
}

Function0::Function0() : XObject("function0") {
    this->f = nullptr;
}
