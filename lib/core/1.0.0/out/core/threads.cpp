//
// Created by chris on 5/6/21.
//

#include "threads.h"
#include <pthread.h>

DEFINE_FUNCTION(1, core_D_threads_D_Thread_D_new)

TaggedObject* core_D_threads_D_Thread_D_new_f(TaggedObject* func) {
    return NEW(XThread, func);
}

DEFINE_FUNCTION(1, core_D_threads_D_Thread_D_start)

void* run_func(void* func) {
    return CALL0((TaggedObject*) func);
}

TaggedObject* core_D_threads_D_Thread_D_start_f(TaggedObject* th) {
    XThread* thread_obj = CAST(th, XThread);
    pthread_create(&thread_obj->thread, nullptr, run_func, thread_obj->func);
    return nullptr;
}

DEFINE_FUNCTION(1, core_D_threads_D_Thread_D_join)

TaggedObject* core_D_threads_D_Thread_D_join_f(TaggedObject* th) {
    void* ret;
    pthread_join(CAST(th, XThread)->thread, &ret);
    return nullptr;
}

DEFINE_FUNCTION(0, core_D_threads_D_Lock_D_new)

TaggedObject* core_D_threads_D_Lock_D_new_f() {
    XLock* l = CAST(NEW(XLock, {}), XLock);
    pthread_mutex_init(&l->mutex,NULL);
    return TAG(l);
}

DEFINE_FUNCTION(1, core_D_threads_D_Lock_D_lock)

TaggedObject* core_D_threads_D_Lock_D_lock_f(TaggedObject* lock) {
    pthread_mutex_lock(&CAST(lock, XLock)->mutex);
    return nullptr;
}

DEFINE_FUNCTION(1, core_D_threads_D_Lock_D_unlock)

TaggedObject* core_D_threads_D_Lock_D_unlock_f(TaggedObject* lock) {
    pthread_mutex_unlock(&CAST(lock, XLock)->mutex);
    return nullptr;
}