/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_REGION_H
#define PEARL_REGION_H


#include "NamedObject.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    Region.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Region.
 *
 * This header file provides the declaration of the class Region and all
 * related definitions.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @class   Region
 * @ingroup PEARL_base
 * @brief   Stores information related to a source-code region.
 *
 * Instances of the Region class provide information about source-code
 * regions of the target application. Such a region is characterized by
 * a name, a file in which it is defined, begin and end line numbers,
 * and a region type. In addition, a short textual description can be
 * stored for each region.
 *
 * The numerical identifiers of the individual regions are globally defined
 * and continuously enumerated, i.e., the ID is element of [0,@#regions-1].
 */
/*-------------------------------------------------------------------------*/

class Region : public NamedObject
{
  public:
    /// Symbolic names for region classification
    enum {
      // Region classes
      CLASS_INTERNAL       = (  0 << 24),
      CLASS_USER           = (  1 << 24),
      CLASS_MPI            = (  2 << 24),
      CLASS_OMP            = (  4 << 24),

      // Categories for class MPI
      CAT_MPI_SETUP        = (  1 << 16),
      CAT_MPI_COLLECTIVE   = (  2 << 16),
      CAT_MPI_P2P          = (  4 << 16),
      CAT_MPI_RMA          = (  8 << 16),
      CAT_MPI_IO           = ( 16 << 16),

      // Categories for class OMP
      CAT_OMP_PARALLEL     = (  1 << 16),
      CAT_OMP_SYNC         = (  2 << 16),

      // Call types for category OMP_SYNC
      TYPE_OMP_BARRIER     = (  1 <<   8),

      // Modes for call types OMP_BARRIER
      MODE_OMP_IMPLICIT    = (  1 <<   0),
      MODE_OMP_EXPLICIT    = (  2 <<   0),

      // Call types for category MPI_SETUP
      TYPE_MPI_INIT        = (  1 <<  8),
      TYPE_MPI_FINALIZE    = (  2 <<  8),

      // Call types for category MPI_COLLECTIVE
      TYPE_MPI_BARRIER     = (  1 <<  8),
      TYPE_MPI_ONE_TO_N    = (  2 <<  8),
      TYPE_MPI_N_TO_ONE    = (  4 <<  8),
      TYPE_MPI_N_TO_N      = (  8 <<  8),
      TYPE_MPI_PARTIAL     = ( 16 <<  8),

      // Call types for category MPI_P2P
      TYPE_MPI_SEND        = (  1 <<  8),
      TYPE_MPI_RECV        = (  2 <<  8),
      TYPE_MPI_SENDRECV    = (  3 <<  8),
      TYPE_MPI_TEST        = (  4 <<  8),
      TYPE_MPI_WAIT        = (  8 <<  8),

      // Call types for category MPI_RMA
      TYPE_MPI_RMA_COMM    = (  1 <<  8),
      TYPE_MPI_RMA_COLL    = (  2 <<  8),
      TYPE_MPI_RMA_GATS    = (  4 <<  8),
      TYPE_MPI_RMA_PASSIVE = (  8 <<  8),

      // Transfer modes for call types MPI_SEND/MPI_RECV
      MODE_MPI_STANDARD    = (  1 <<  0),
      MODE_MPI_SYNCHRONOUS = (  2 <<  0),
      MODE_MPI_BUFFERED    = (  4 <<  0),
      MODE_MPI_READY       = (  8 <<  0),
      MODE_MPI_NONBLOCKING = ( 16 <<  0),
      MODE_MPI_PERSISTENT  = ( 32 <<  0),

      // Testing/waiting modes for call types MPI_TEST/MPI_WAIT/MPI_START
      MODE_MPI_SINGLE      = (  1 <<  0),
      MODE_MPI_MANY        = (  2 <<  0),

      // RMA communication modes
      MODE_RMA_PUT         = (  1 <<  0),
      MODE_RMA_GET         = (  2 <<  0),

      MODE_RMA_START       = (  1 <<  0),
      MODE_RMA_COMPLETE    = (  2 <<  0),
      MODE_RMA_POST        = (  4 <<  0),
      MODE_RMA_WAIT        = (  8 <<  0),
      MODE_RMA_TEST        = ( 16 <<  0),

      MODE_RMA_FENCE       = (  1 <<  0),
      MODE_RMA_WIN_CREATE  = (  2 <<  0),
      MODE_RMA_WIN_FREE    = (  4 <<  0),
      
      MODE_RMA_LOCK        = (  1 <<  0),
      MODE_RMA_UNLOCK      = (  2 <<  0)
    };


    /// @name Constructors & destructor
    /// @{

    Region(ident_t            id,
           const std::string& name,
           const std::string& file,
           line_t             begin,
           line_t             end,
           const std::string& description,
           uint8_t            type);

    /// @}
    /// @name Get region information
    /// @{

    std::string get_file() const;
    line_t      get_begin() const;
    line_t      get_end() const;
    std::string get_description() const;
    std::string get_type() const;
    uint8_t     get_type_id() const;

    /// @}
    /// @name Get region classification
    /// @{

    uint32_t get_class() const;

    /// @}


  private:
    /// Name of the file in which the region is defined
    std::string m_file;

    /// Begin line number
    line_t m_begin;

    /// End line number
    line_t m_end;

    /// Description of the source-code region
    std::string m_description;

    /// %Region type identifier
    uint8_t m_type;

    /// %Region classification
    uint32_t m_class;
};


//--- Inline methods --------------------------------------------------------

/**
 * Returns the region classification encoded in a bit string.
 *
 * @return %Region classification
 */
inline uint32_t Region::get_class() const
{
  return m_class;
}


/**
 * Returns the region type identifier.
 *
 * @return %Region type identifier
 */
inline uint8_t Region::get_type_id() const
{
  return m_type;
}


}   /* namespace pearl */


#endif   /* !PEARL_REGION_H */
