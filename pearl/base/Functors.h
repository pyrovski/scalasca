/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_FUNCTORS_H
#define PEARL_FUNCTORS_H


#include <functional>


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    Functors.h
 * @ingroup PEARL_base
 * @brief   Declaration of various functor classes.
 *
 * This header file provides the declaration of various functor classes.
 */
/*-------------------------------------------------------------------------*/


namespace pearl {

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @struct  less_ptr
 * @ingroup PEARL_base
 * @brief   Dereferencing comparison functor class for operator<().
 *
 * less_ptr<> is a comparison functor class similar to the predefined STL
 * function object std::less<>, but instead of comparing the two operands
 * directly using operator <(), the arguments are dereferenced beforehand.
 * This is useful for applying STL algorithms to containers of pointer
 * types.
 *
 * @b Example:
 * @code
 *   #include <algorithm>
 *   #include <cstdlib>
 *   #include <iostream>
 *   #include <vector>
 *
 *   #include <pearl/Functors.h>
 *
 *   using namespace std;
 *
 *   vector<int*> data;
 *
 *   int main()
 *   {
 *     for (int i = 0; i < 10; ++i)
 *       data.push_back(new int(rand()));
 *
 *     sort(data.begin(), data.end(), pearl::less_ptr<int*>());
 *
 *     for (int i = 0; i < 10; ++i)
 *       cout << *data[i] << endl;
 *   }
 * @endcode
 *
 * @param ptrT Pointer type
 */
/*-------------------------------------------------------------------------*/

template<typename ptrT>
struct less_ptr : public std::binary_function<const ptrT, const ptrT, bool>
{
  bool operator ()(const ptrT lhs, const ptrT rhs) const
  {
    return *lhs < *rhs;
  }
};


}   /* namespace pearl */


#endif   /* !PEARL_FUNCTORS_H */
