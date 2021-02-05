#include "runtime/core/core.h"
#include "project.h"
Function0 f_main_f = Function0 (f_main);
Function0* function_main=&f_main_f;XObject* f_main(){
void* it = nullptr;
GC::enter_function();XObject* x = GC::register_object((new XDict({{GC::register_object((TAG(new XString("Hello")))), INT_TO_PTR(5)}, {GC::register_object((TAG(new XString("Bye")))), INT_TO_PTR(19)}, {GC::register_object((TAG(new XString("Charlie")))), INT_TO_PTR(31)}})));GC::declare("x",x);(*(function_print))((*(function_String_add))((*(function_Integer_str))(subscript(x, GC::register_object((TAG(new XString("Byea")))))), GC::register_object((TAG(new XString("foo"))))));XObject* w = GC::register_object((new XList({INT_TO_PTR(6), INT_TO_PTR(9), INT_TO_PTR(2)})));GC::declare("w",w);(*(function_print))((*(function_String_add))(GC::register_object((TAG(new XString("The number is: ")))), (*(function_Integer_str))(subscript(w, INT_TO_PTR(1)))));return GC::function_return( INT_TO_PTR(0));}
int main(){
GC::enter_scope();auto x = PTR_TO_INT((*function_main)());GC::leave_scope();return x;}