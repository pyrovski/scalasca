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

/*****************************************************************************/
/*       This program tests the earl/topology c++ interface                  */
/*****************************************************************************/

#include <earl.h>
#include <iostream>

using namespace earl;
using namespace std;

int main(int argc, char **argv)
{
  try
    {
      // open trace file
      EventTrace trace(argv[1]);

      // iterate through the trace
      for ( int i = 0 ; i < trace.get_ncarts() ; i++ ) 
	{
	  // retrive cart topology i
	  const Cartesian* cart = trace.get_cart(i);
	  // print the cart topology id
	  cout << "id  " << cart->get_id() << endl;

	  // print the number of dimension
	  int ndims =  cart->get_ndims();
	  cout << "ndims   " << ndims << endl;

	  // print the vector dimv
	  std::vector<long> dimv;
	  cart->get_dimv(dimv);

	  for( size_t i = 0 ; i < dimv.size() ; i++ )
	    {
	      cout << "dimv[" << i << "] = " << dimv[i] << endl;
	    }

	  // print the vector periodv
	  std::vector<bool> periodv;
	  cart->get_periodv(periodv);

	  for( size_t i = 0 ; i < periodv.size() ; i++ )
	    {
	      cout << "periodv[" << i << "] = " << periodv[i] << endl;
	    }

	  cout << endl << endl;

	  // print the coordinates for each location
	  for ( int i = 0 ; i < trace.get_nlocs() ; i++ ) 
	    {
	      // retrieve location i
	      Location* loc = trace.get_loc(i);

	      std::vector<long> coordv;
	      cart->get_coords(coordv, loc);
	      
	      cout << "---------------------" << endl;
	      cout << "coordv for Location " << i << endl;
	      cout << "---------------------" << endl;
	      
	      // print the vector coordv
	       for( size_t i = 0 ; i < coordv.size() ; i++ )
		 {		  
		   cout << "coordv[" << i << "] = " << coordv[i] << endl;
		 }

	       // test the get_loc(coordv) function
	       Location* temp = cart->get_loc(coordv);
	       cout << "loc_id = " << temp->get_id() << endl;
	    }
	}
    }
  catch ( Error error )
    {
      // print error message and exit
      cerr << error.get_msg() << endl;
      exit(EXIT_FAILURE);
    }
}
