//
// Created by chris on 5/6/21.
//

#ifndef XLANG_THREADS_H
#define XLANG_THREADS_H

#include <fstream>
#include "xobject.h"
#include "Function.h"
#include "xobjects/XString.h"
#include "macros.h"

class XThread : public XObject {
public:
    pthread_t thread;
    TaggedObject* func;

    XThread(TaggedObject* func) : XObject("Thread") {
        this->func = func;
    }
};

class XLock : public XObject {
public:
    pthread_mutex_t mutex;

    XLock() : XObject("Lock") {
    }
};

extern Function0 core_D_threads_D_Lock_D_new_o;
extern TaggedObject* core_D_threads_D_Lock_D_new;
TaggedObject* core_D_threads_D_Lock_D_new_f();

extern Function1 core_D_threads_D_Lock_D_lock_o;
extern TaggedObject* core_D_threads_D_Lock_D_lock;
TaggedObject* core_D_threads_D_Lock_D_lock_f(TaggedObject* lock);

extern Function1 core_D_threads_D_Lock_D_unlock_o;
extern TaggedObject* core_D_threads_D_Lock_D_unlock;
TaggedObject* core_D_threads_D_Lock_D_unlock_f(TaggedObject* lock);



extern Function1 core_D_threads_D_Thread_D_new_o;
extern TaggedObject* core_D_threads_D_Thread_D_new;
TaggedObject* core_D_threads_D_Thread_D_new_f(TaggedObject* func);

extern Function1 core_D_threads_D_Thread_D_start_o;
extern TaggedObject* core_D_threads_D_Thread_D_start;
TaggedObject* core_D_threads_D_Thread_D_start_f(TaggedObject* thread);

extern Function1 core_D_threads_D_Thread_D_join_o;
extern TaggedObject* core_D_threads_D_Thread_D_join;
TaggedObject* core_D_threads_D_Thread_D_join_f(TaggedObject* thread);

#endif //XLANG_THREADS_H
