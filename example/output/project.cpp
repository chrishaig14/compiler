#include "runtime/core/core.h"
#include "project.h"
Function0 f_main_f = Function0 (f_main);
Function0* function_main=&f_main_f;XObject* f_main(){
void* it = nullptr;
GC::enter_function();XObject* a = INT_TO_PTR(8);GC::declare("a",a);XObject* b = INT_TO_PTR(1);GC::declare("b",b);XObject* r = op_lt(a, b);GC::declare("r",r);(*(function_print))((*(function_Boolean_str))(r));return GC::function_return( INT_TO_PTR(0));}
int main(){
GC::enter_scope();auto x = PTR_TO_INT((*function_main)());GC::leave_scope();return x;}