#include "runtime/core/core.h"
#include "project.h"
Function0 f_main_f = Function0 (f_main);
Function0* function_main=&f_main_f;XObject* f_main(){
void* it = nullptr;
GC::enter_function();XObject* x = GC::register_object((new XDict(std::unordered_map<XObject*,XObject*>({{INT_TO_PTR(5), GC::register_object((TAG(new XString("Hello"))))}, {INT_TO_PTR(8), GC::register_object((TAG(new XString("Chris"))))}, {INT_TO_PTR(3), GC::register_object((TAG(new XString("Alex"))))}}))));GC::declare("x",x);(*(function_print))((*(function_String_add))(subscript(x, INT_TO_PTR(8)), subscript(x, INT_TO_PTR(3))));return GC::function_return( INT_TO_PTR(0));}
int main(){
GC::enter_scope();auto x = PTR_TO_INT((*function_main)());GC::leave_scope();return x;}