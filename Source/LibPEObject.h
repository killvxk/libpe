#pragma once

#include "LibPE.h"

LIBPE_NAMESPACE_BEGIN

template <class T>
class ScopedPtr
{
public:
    ScopedPtr() : m_ptr(NULL) {}
    ScopedPtr(T *p) : m_ptr(p) {}
    ~ScopedPtr() { Reset(); }

    void Attach(T *p) {
        if(p == m_ptr) { return; }
        if(NULL != m_ptr) {
            delete m_ptr;
        }
        m_ptr = p;
    }

    T * Detach() {
        T *p = m_ptr;
        m_ptr = NULL;
        return p;
    }

    void Reset() {
        if(NULL != m_ptr) { delete m_ptr; }
        m_ptr = NULL;
    }

    operator T * () { return m_ptr; }
    T & operator *() { return *m_ptr; }
    T ** operator &() { return &m_ptr; }
    T * operator ->() { return m_ptr; }
    bool operator !() const { return (NULL == m_ptr); }
    bool operator <(T *ptr) const { return m_ptr < ptr; }
    bool operator ==(T *ptr) const { return m_ptr == ptr; }
    bool operator !=(T *ptr) const { return m_ptr != ptr; }

    T * operator =(T *ptr) {
        Attach(ptr);
        return m_ptr;
    }

    T *m_ptr;
};

class RefCountPersistent
{
public:
    RefCountPersistent() {}
    ~RefCountPersistent() {}

    uint32_t AddRef(void) { return 1; }
    uint32_t Release(void) { return 1; }
};

class RefCount
{
public:
    RefCount() : m_nRefCount(0) {}
    ~RefCount() {}

    uint32_t AddRef(void) { return ++m_nRefCount; }
    uint32_t Release(void) { return --m_nRefCount; }

private:
    uint32_t m_nRefCount;
};

class RefCountThreadSafe
{
public:
    RefCountThreadSafe() : m_nRefCount(0) {}
    ~RefCountThreadSafe() {}

    uint32_t AddRef(void) { return (uint32_t)::InterlockedIncrement((volatile LONG *)&m_nRefCount); }
    uint32_t Release(void) { return (uint32_t)::InterlockedDecrement((volatile LONG *)&m_nRefCount); }

private:
    uint32_t m_nRefCount;
};

#define LIBPE_INTERFACE_IMPL()                                              \
    public:                                                                 \
        LIBPE_METHOD_(uint32_t, AddRef)()                                   \
        {                                                                   \
            return m_oRefCount.AddRef();                                    \
        }                                                                   \
                                                                            \
        LIBPE_METHOD_(uint32_t, Release)()                                  \
        {                                                                   \
            uint32_t nRefCount = m_oRefCount.Release();                     \
                                                                            \
            if (nRefCount == 0) {                                           \
                delete this;                                                \
                return 0;                                                   \
            }                                                               \
                                                                            \
            return nRefCount;                                               \
        }

#define LIBPE_PERSISTENT_OBJECT()                                           \
    private:                                                                \
        RefCountPersistent m_oRefCount;                                     \
                                                                            \
    LIBPE_INTERFACE_IMPL()

#define LIBPE_SINGLE_THREAD_OBJECT()                                        \
    private:                                                                \
        RefCount m_oRefCount;                                               \
                                                                            \
    LIBPE_INTERFACE_IMPL()

#define LIBPE_MULTI_THREAD_OBJECT()                                         \
    private:                                                                \
        RefCountThreadSafe m_oRefCount;                                     \
                                                                            \
    LIBPE_INTERFACE_IMPL()

LIBPE_NAMESPACE_END