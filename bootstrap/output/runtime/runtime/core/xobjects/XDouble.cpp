//
// Created by chris on 23/3/21.
//

#include <sstream>
#include "XDouble.h"
#include "../core.h"

XDouble::~XDouble() {

}

TaggedObject* XDouble::__eq__(TaggedObject* pObject) {
    return MAKE_BOOL(this->d == CAST(pObject, XDouble)->d);
}

XDouble::XDouble(double d) : XObject("Double") {
    this->d = d;
}

TaggedObject* core_D_Double_D_str_f(TaggedObject* _i) {
    std::ostringstream out;
    out.precision(50);
    out << CAST(_i, XDouble)->d;
    std::string s;
    s = out.str();
    TaggedObject* x = NEW(XString, s);
    return x;
}
DEFINE_FUNCTION(1, core_D_Double_D_str)
