//
// Created by chris on 23/3/21.
//

#include "XDouble.h"

XDouble::~XDouble() {

}

TaggedObject* XDouble::__eq__(TaggedObject* pObject) {
    return MAKE_BOOL(this->d == CAST(pObject, XDouble)->d);
}

XDouble::XDouble(double d) : XObject("Double") {
    this->d = d;
}
