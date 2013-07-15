/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_COUNTEDPTR_H
#define PEARL_COUNTEDPTR_H


/*-------------------------------------------------------------------------*/
/**
 * @file    CountedPtr.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class CountedPtr.
 *
 * This header file provides the declaration of the class CountedPtr.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @class   CountedPtr
 * @ingroup PEARL_base
 * @brief   Non-intrusive reference-counted smart pointer.
 */
/*-------------------------------------------------------------------------*/

template<typename T>
class CountedPtr
{
  public:
    /// Type name for the counted value
    typedef T element_type;


    /// @name Constructors & destructor
    /// @{

    explicit CountedPtr(T* ptr = 0);
    CountedPtr(const CountedPtr& rhs) throw();
    ~CountedPtr() throw();

    /// @}
    /// @name Initialization
    /// @{

    void initialize(T* ptr);

    /// @}
    /// @name Assignment operator
    /// @{

    CountedPtr& operator=(const CountedPtr& rhs) throw();

    /// @}
    /// @name Value access
    /// @{

    T* get() const throw();

    T& operator*() const throw();
    T* operator->() const throw();

    /// @}


  private:
    /// Reference counting data structure
    struct Counter {
      Counter(T* ptr) : m_ptr(ptr), m_ref(1) {}

      T*            m_ptr;
      unsigned int  m_ref;
    };

    /// Reference counting member data
    Counter* m_counter;


    /* Private methods */
    void acquire(Counter* counter) throw();
    void release() throw();
};


//--- Constructors & destructor ---------------------------------------------

template<typename T>
inline CountedPtr<T>::CountedPtr(T* ptr)
  : m_counter(new Counter(ptr))
{
}


template<typename T>
inline CountedPtr<T>::CountedPtr(const CountedPtr& rhs) throw()
{
  acquire(rhs.m_counter);
}


template<typename T>
inline CountedPtr<T>::~CountedPtr() throw()
{
  release();
}


//--- Initialization --------------------------------------------------------

template<typename T>
inline void CountedPtr<T>::initialize(T* ptr)
{
  if (m_counter && m_counter->m_ptr == 0)
    m_counter->m_ptr = ptr;
}


//--- Assignment operator ---------------------------------------------------

template<typename T>
inline CountedPtr<T>& CountedPtr<T>::operator=(const CountedPtr& rhs) throw()
{
  if (this != &rhs) {
    release();
    acquire(rhs.m_counter);
  }

  return *this;
}


//--- Value access ----------------------------------------------------------

template<typename T>
inline T* CountedPtr<T>::get() const throw()
{
  return (m_counter ? m_counter->m_ptr : 0);
}


template<typename T>
inline T& CountedPtr<T>::operator*() const throw()
{
  return *m_counter->m_ptr;
}


template<typename T>
inline T* CountedPtr<T>::operator->() const throw()
{
  return m_counter->m_ptr;
}


//--- Private methods -------------------------------------------------------

template<typename T>
inline void CountedPtr<T>::acquire(Counter* counter) throw()
{
  m_counter = counter;
  if (m_counter)
    ++m_counter->m_ref;
}


template<typename T>
inline void CountedPtr<T>::release() throw()
{
  if (!m_counter)
    return;

  --m_counter->m_ref;
  if (m_counter->m_ref == 0) {
    delete m_counter->m_ptr;
    delete m_counter;
  }
  m_counter = 0;
}


}   /* namespace pearl */


#endif   /* !PEARL_COUNTEDPTR_H */
