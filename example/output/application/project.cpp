#include "project.h"
Function0 _D_project_D_xl_D_main_f_o = Function0 (_D_project_D_xl_D_main_f);
TaggedObject* _D_project_D_xl_D_main=FTAG(&_D_project_D_xl_D_main_f_o);
TaggedObject* _D_project_D_xl_D_main_f(){
TaggedObject* x = GC::declare(FALSE);
CALL1(core_D_print, CALL1(core_D_Boolean_D_str, x));
TaggedObject* __return__ = GC::set_return( MAKE_INT(0));
return __return__;
}

int main(){
auto x = GET_INT(CALL0(_D_project_D_xl_D_main));
return x;
}