/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_SMALLOBJALLOCATOR_H
#define PEARL_SMALLOBJALLOCATOR_H


#include <cassert>
#include <map>
#include <new>


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class BlockAllocator;


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   SmallObjAllocator
 * @ingroup PEARL_base
 * @brief   Custom memory management class for small objects in
 *          single-threaded applications.
 */
/*-------------------------------------------------------------------------*/

class SmallObjAllocator
{
  public:
    /// @name Constructors & destructor
    /// @{

    virtual ~SmallObjAllocator();

    /// @}
    /// @name Memory allocation routines
    /// @{

    virtual void* alloc  (std::size_t objectSize) const throw(std::bad_alloc);
    virtual void  dealloc(void* deadObject, std::size_t objectSize) const;

    /// @}
    /// @name Singleton interface
    /// @{

    static const SmallObjAllocator* instance();
    static void registerAllocator(const SmallObjAllocator* allocator);

    /// @}


  protected:
    /// Container type for storing the different allocators
    typedef std::map<std::size_t,BlockAllocator*> alloc_container;


    /// @name Internal memory allocation routines
    /// @{

    void* alloc  (alloc_container& container,
                  std::size_t      objectSize) const throw(std::bad_alloc);
    void  dealloc(alloc_container& container,
                  void*            deadObject,
                  std::size_t      objectSize) const;

    /// @}


  private:
    /// Single allocator instance
    static const SmallObjAllocator* m_instance;

    /// Allocators for different object sizes
    static alloc_container m_allocators;
};


//--- Inline methods --------------------------------------------------------

inline const SmallObjAllocator* SmallObjAllocator::instance()
{
  assert(m_instance);
  return m_instance;
}


}   /* namespace pearl */


#endif   /* !PEARL_SMALLOBJALLOCATOR_H */
