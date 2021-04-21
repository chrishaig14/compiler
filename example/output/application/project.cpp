#include "project.h"
Function0 project_D_main_f_o = Function0 (project_D_main_f);
TaggedObject* project_D_main=FTAG(&project_D_main_f_o);
TaggedObject* project_D_main_f(){
TaggedObject* x = GC::declare(CALL0(pack_D_pepe_D_mod_D_get_john));
x = GC::assign(x, CALL1(pack_D_pepe_D_mod_D_Person_D___init__, MAKE_STRING("Hello")));
CALL1(core_D_print, CAST(x, pack_D_pepe_D_mod_D_Person)->name);
TaggedObject* __return__ = GC::set_return( MAKE_INT(0));
return __return__;
}

int main(){
auto x = GET_INT(CALL0(project_D_main));
return x;
}