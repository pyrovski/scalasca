/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>

using namespace std;

namespace
{
    double const vint[] = {1.0, 2.0, 5.0, 10.0};
    double const sqr[] = {sqrt(2.0), sqrt(10.0), sqrt(50.0)};
    double ddel = 0.0002;

    double fround(double x, double y)
    {
        if(x <= 0.0)
        {
            return ceil(x/y - 0.5) * y;
        }
        else
        {
            return floor(x/y + 0.5) * y;
        }
    }

    double zdistz(double umina, double umaxa, int na)
    {
        int n = na > 1 ? na : 1;
        //int n = max(na, 1);
        double umin = umina < umaxa ? umina : umaxa;
        //double umin = min(umina, umaxa);
        double umax = umina > umaxa ? umina : umaxa;
        //double umax = max(umina, umaxa);

        if(umin == umax)
        {
            double dd = 0.25 * abs(umin) > 1.0e-2 ? 0.25 * abs(umin) : 1.0e-2;
            //double dd = max(0.25 * abs(umin), 1.0e-2);
            umin = umin - dd;
            umax = umax + dd;
            n = 1;
        }

        double a = (umax - umin)/n;
        double al = log10(a);
        if(a < 1.0)
        {
            al = static_cast<int>(al) - 1;
        }
        else
        {
            al = static_cast<int>(al);
        }
        int b = static_cast<int>(a/pow(10.0, al));
        int i = 0;
        for(i = 0; i < 3; ++i)
        {
            if(b < sqr[i])
            {
                break;
            }
        }
        return vint[i] * pow(10.0, al);
    }

    struct RangeHelper
    {
        double minVal, maxVal;
        int n;
        double dist;
    };

    RangeHelper zulimz(double umin, double umax, double dist)
    {
        double fm1 = umin/dist;
        int m1 = static_cast<int>(fm1);
        if(fm1 < 0.0)
        {
            m1 -= 1;
        }
        if(abs(m1 + 1.0 - fm1) < ddel)
        {
            m1 += 1;
        }
        RangeHelper returnRange;
        returnRange.minVal = dist*m1;

        double fm2 = umax/dist;
        int m2 = static_cast<int>(fm2 + 1.0);
        if(fm2 < -1.0)
        {
            m2 -= 1;
        }
        if(abs(fm2 + 1.0 - m2) < ddel)
        {
            m2 -= 1;
        }
        returnRange.maxVal = dist * m2;
        returnRange.minVal = returnRange.minVal < umin
                           ? returnRange.minVal : umin;
        returnRange.maxVal = returnRange.maxVal > umax
                           ? returnRange.maxVal : umax;
        returnRange.n = abs(m2 - m1);
        return returnRange;
    }

    RangeHelper zuintz(double umina, double umaxa, int npa)
    {
        double dist = zdistz(umina, umaxa, npa);
        RangeHelper returnRange = zulimz(umina, umaxa, dist);
        returnRange.dist = dist;
        return returnRange;
    }
}

void Pretty(double pmin, double pmax, int pno, vector<double> &vec)
{
    double nmin = fround(pmin, 0.0000000001);
    double nmax = fround(pmax, 0.0000000001);
    int nno = pno;
    if(pno <= 0)
    {
        nno = 5;
    }
    RangeHelper tempRange = zuintz(nmin, nmax, nno);
    for(int i = 0; i < tempRange.n; ++i)
    {
        vec.push_back(fround(tempRange.minVal + i*tempRange.dist, 0.00000001));
    }
    vec.push_back(tempRange.maxVal);
}

