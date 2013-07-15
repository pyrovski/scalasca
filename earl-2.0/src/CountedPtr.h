/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
/*
 * CountedPtr - simple reference counted pointer.
 *
 * The is a non-intrusive implementation that allocates an additional
 * int and pointer for every counted object.
 */

#ifndef CountedPtr_H
#define CountedPtr_H

/* For ANSI-challenged compilers, you may want to #define
 * NO_MEMBER_TEMPLATES or explicit */

// AndrejK
#define NO_MEMBER_TEMPLATES

template <class X> class CountedPtr
{
public:
    typedef X element_type;

    explicit CountedPtr(X* p = 0) // allocate a new counter
        : itsCounter(0) {if (p) itsCounter = new counter(p);}
    ~CountedPtr()
        {release();}
    CountedPtr(const CountedPtr& r) throw()
        {acquire(r.itsCounter);}
    CountedPtr& operator=(const CountedPtr& r)
    {
        if (this != &r) {
            release();
            acquire(r.itsCounter);
        }
        return *this;
    }

#ifndef NO_MEMBER_TEMPLATES
    template <class Y> friend class CountedPtr<Y>;
    template <class Y> CountedPtr(const CountedPtr<Y>& r) throw()
        {acquire(r.itsCounter);}
    template <class Y> CountedPtr& operator=(const CountedPtr<Y>& r)
    {
        if (this != &r) {
            release();
            acquire(r.itsCounter);
        }
        return *this;
    }
#endif // NO_MEMBER_TEMPLATES

    X& operator*()  const throw()   {return *itsCounter->ptr;}
    X* operator->() const throw()   {return itsCounter->ptr;}
    bool unique()   const throw()
        {return (itsCounter ? itsCounter->count == 1 : true);}

    // AndrejK
    //X* get()        const throw()   {return itsCounter ? itsCounter->ptr : 0;}
    operator bool() const throw() {return itsCounter != NULL;}
    //bool operator! ()                         const throw() {return itsCounter == NULL;}
    bool operator< (const CountedPtr<X>& rhs) const throw() {return itsCounter <  rhs.itsCounter;}
    bool operator<=(const CountedPtr<X>& rhs) const throw() {return itsCounter <= rhs.itsCounter;}
    bool operator> (const CountedPtr<X>& rhs) const throw() {return itsCounter >  rhs.itsCounter;}
    bool operator>=(const CountedPtr<X>& rhs) const throw() {return itsCounter >= rhs.itsCounter;}
    bool operator==(const CountedPtr<X>& rhs) const throw() {return itsCounter == rhs.itsCounter;}
    bool operator!=(const CountedPtr<X>& rhs) const throw() {return itsCounter != rhs.itsCounter;}

private:

    struct counter {
        counter(X* p = 0, unsigned c = 1) : ptr(p), count(c) {}
        X*          ptr;
        unsigned    count;
    }* itsCounter;

    void acquire(counter* c) throw()
    { // increment the count
        itsCounter = c;
        if (c) ++c->count;
    }

    void release()
    { // decrement the count, delete if it is 0
        if (itsCounter) {
            if (--itsCounter->count == 0) {
                delete itsCounter->ptr;
                delete itsCounter;
            }
            itsCounter = 0;
        }
    }
};


#endif // CountedPtr_H
