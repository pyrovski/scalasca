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

#include "P2Statistic.h"

using namespace stats;

P2Statistic::P2Statistic()
{
  reset();  
} 

void
P2Statistic::reset() {

  counter = 0;
  
  q[0] = 0.0;
  q[1] = 0.0;
  q[2] = 0.0;
  q[3] = 0.0;
  q[4] = 0.0;

  n[0] = 1;
  n[1] = 2;
  n[2] = 3;
  n[3] = 4;
  n[4] = 5;

  nx[0] = 1.0;
  nx[1] = 2.0;
  nx[2] = 3.0;
  nx[3] = 4.0;
  nx[4] = 5.0;

  s = 0;
  s2 = 0;
}

void
P2Statistic::add(double val) 
{
  /*
   *---------------------------------------------------------------------------- 
   *
   * Note that all indices range from 0 to 4 here. The referred paper uses 
   * 1 to 5 instead.
   *
   * Name changes:
   *
   * q'   -> qx
   * d'   -> dx 
   * x_j  -> val (subsequent observation)
   *
   *----------------------------------------------------------------------------
   */

  // increments for desired marker positions
  static const double dnx[5]= {0.0, 0.25, 0.5, 0.75, 1.0};  

  long i, k, l, d;       
  double qx, dx;      

  counter++;

  s += val;
  s2 += val*val;

  if ( counter < 6 ) 
    {

    /* sort the first five obeservations in q[0-4] */ 

    for (i = 1; i < counter; i++) 
      {
	if (val < q[i-1]) 
	  {
	    for (l = counter; l > i; l--)
	      {
		q[l-1] = q[l-2];
	      }
	    break;
	  }
      }
    q[i-1] = val;
    
    } 
  else 
    {

    /*  find cell k such that q[k-1] <= val < q[k] and adjust extreme  */ 
    /*  values (q[0] and q[4]) if necessary.                           */ 

      for (k = 0; k < 5; k++)
	{
	  if (val < q[k]) 
	    break;
	}

    if (k == 0) 
      {
	q[0] = val;
	k = 1;
      }
    else if (k == 5) 
      {
	q[4] = val;
	k = 4;
      }

    /*  increment positions of markers k through 4 */
    for (i = k; i < 5; i++)
      {
	n[i]++;
      }

    /*  update desired positions for all markers */  
    for (i = 0; i < 5; i++)
      {
	nx[i] += dnx[i];
      }

    /*  adjust heighth of markers 1-3 if necessary */  
    for (i = 1; i < 4; i++) 
      {
	dx = nx[i] - n[i];
	
	if ((dx >=  1.0 && (n[i+1] - n[i]) >  1 ) ||
	    (dx <= -1.0 && (n[i-1] - n[i]) < -1)) {
	  
	  /*  adjust necessary ? */ 
	  dx = dx >= 0.0 ? 1.0 : -1.0;  // dx = sign(dx)  
	  d = static_cast<long>(dx);           // conversion long <- double
	  
	  /* try adjusting q[i] using P^2 formula */  
	  qx = q[i] + dx / (n[i+1] - n[i-1]) *
	    (   (n[i] - n[i-1] + dx) * (q[i+1] - q[i])
		/ (n[i+1] - n[i])
		+   (n[i+1] - n[i] - dx) * (q[i] - q[i-1])
		/ (n[i] - n[i-1])
		);
	  
	  /*  if value of P^2 formula is bad, use linear formula */  
	  if (q[i-1] < qx && qx < q[i+1])
	    {
	      q[i] = qx;   // o.k.
	    }
	  else
	    {
          q[i] = q[i] + dx * (q[i+d] - q[i]) / (n[i+d] - n[i]);  
	    }
	  n[i] += d;
	}
      }
    }
}


long   
P2Statistic::count() const 
{  
  return counter;
}

double 
P2Statistic::mean() const 
{
  if (counter == 0)
    {
      throw(StatsError("No values to perform calculation."));
    }
  else
    {
      return (s / counter); 
    }
}

double 
P2Statistic::med() const {
  
  if (counter == 0)
    {
    throw(StatsError("No values to perform calculation."));
    }  
  else if (counter < 5)
    {
      if (counter % 2)
        {
        return q[(counter - 1)/2];
        }
      else
        {
        return (q[counter/2 - 1] + q[counter/2])/2;
        }
    }
  else 
    {
    return q[2];
    }
}

double 
P2Statistic::min() const {
  
  if (counter == 0)
    {
    throw(StatsError("No values to perform calculation."));
    }
  else
    {
      return q[0];
    }
}

double 
P2Statistic::max() const {

  if (counter == 0)
    {
    throw(StatsError("No values to perform calculation."));
    }
  if (counter < 5)
    {
    return q[counter-1];
    }
  else
    {
    return q[4];
    }
}

double 
P2Statistic::q25() const {

  if (counter == 0)
    {
    throw(StatsError("No values to perform calculation."));
    }
  else if (counter < 5)
    {
    throw(StatsError("Not enough values to calculate q25."));
    }
  else 
    {
    return q[1];
    }
}

double 
P2Statistic::q75() const {
  
  if (counter == 0)
    {
     throw(StatsError("No values to perform calculation."));
    } 
  else if (counter < 5)
    {
    throw(StatsError("Not enough values to calculate q75."));
    }
  else 
    {
    return q[3];
    }
}

double 
P2Statistic::sum() const {
  
  if (counter == 0)
    {
      throw(StatsError("No values to perform calculation."));
    } 
  else
    {
      return s;
    }
}

double 
P2Statistic::var() const {
  
  if (counter == 0)
    {
    throw(StatsError("No values to perform calculation."));
    }
  else if (counter < 2)
    {
    throw(StatsError("Not enough values to calculate var."));
    }
  else
    {
    return (s2 - (s * s / counter)) / (counter - 1);
    }
}

