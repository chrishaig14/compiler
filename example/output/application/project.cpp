#include "project.h"

Function1 _D_project_D_Person_D___init___f_o = Function1(_D_project_D_Person_D___init___f);
TaggedObject* _D_project_D_Person_D___init__ = FTAG(&_D_project_D_Person_D___init___f_o);

TaggedObject* _D_project_D_Person_D___init___f(TaggedObject* name) {
    TaggedObject* __return__ = GC::set_return(NEW(_D_project_D_Person, name));
    return __return__;
}

Function0 _D_project_D_main_f_o = Function0(_D_project_D_main_f);
TaggedObject* _D_project_D_main = FTAG(&_D_project_D_main_f_o);

TaggedObject* _D_project_D_main_f() {
    TaggedObject* x = GC::declare(CALL1(_D_project_D_Person_D___init__, MAKE_STRING("Chris")));
    TaggedObject* __return__ = GC::set_return(MAKE_INT(0));
    return __return__;
}

int main() {
    auto x = GET_INT(CALL0(_D_project_D_main));
    return x;
}