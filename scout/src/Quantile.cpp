/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "Quantile.h"

using namespace std;
using namespace scout;


/*
 *---------------------------------------------------------------------------
 *
 * class Quantile
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Initializes certain attributes to approximate a density function.
 *
 * Creates a new Quantile instance and sets certain attributes.
 *
Quantile::Quantile() { 
  m_number_obs = 0; //Set observations counter to zero
  
  //Limits where the Legendre polynoms are shifted to
  m_limit_a = 0; //Lower limit for acceptable observations 
  m_limit_b = 1; //Upper limit for acceptable observations

  m_factor = 1; //Standard value for factor is one, 
                    //in that case factor has no influence on the incoming observations
  
  m_max_val = m_limit_a; //Set maximum for observed values to lower limit for acceptable observations 
  m_min_val = m_limit_b; //Set minimum for observed values to upper limit for acceptable observations 
  
  m_sum_obs = 0; //Set sum of all observations to zero
  m_squared_sum_obs = 0; //Set squared sum of all observations to zero

  m_number_coeff = STANDARD_NUMBER_COEFF;
  m_coeff = new double[m_number_coeff];

  for(int i=0; i<m_number_coeff; i++) {
    m_coeff[i] = 0; //Set all coefficients of approximated density function to zero
  }
}
*/

/**
 * @brief Initializes certain attributes to approximate a density function.
 *
 * Creates a new Quantile instance and sets certain attributes.
 */
Quantile::Quantile(int number_coeff) { 
  m_number_obs = 0; //Set observations counter to zero
  
  //Limits where the Legendre polynoms are shifted to
  m_limit_a = 0; //Lower limit for acceptable observations 
  m_limit_b = 1; //Upper limit for acceptable observations

  m_factor = 1; //Standard value for factor is one, 
                    //in that case factor has no influence on the incoming observations
  
  m_max_val = m_limit_a; //Set maximum for observed values to lower limit for acceptable observations 
  m_min_val = m_limit_b; //Set minimum for observed values to upper limit for acceptable observations 
  
  m_sum_obs = 0; //Set sum of all observations to zero
  m_squared_sum_obs = 0; //Set squared sum of all observations to zero

  m_number_coeff = number_coeff;
  m_coeff = new double[m_number_coeff];
#ifdef ADAPTIVE_APPROACH
  m_control_val = new double[m_number_coeff];
#endif
  for(int i=0; i<m_number_coeff; i++) {
    m_coeff[i] = 0; //Set all coefficients of approximated density function to zero
#ifdef ADAPTIVE_APPROACH
    m_control_val[i] = 0; //Set all control values for the adaptive appraoch to zero
#endif
  }
}


/**
 * @brief Destructor
 *
 * Free the memory.
 */
Quantile::~Quantile() { 
  delete[] m_coeff;
  m_coeff = NULL;

#ifdef ADAPTIVE_APPROACH
  delete[] m_control_val;
#endif
}


//--- Get and set class attributes ------------------------------------------

/**
 * @brief Get class attributes.
 *
 * This methods return certain class attributes.
 */
double Quantile::get_factor() {
  return m_factor;
}

double Quantile::get_n() {
  return m_number_obs;
}


double Quantile::get_coeff(int i) {
  return m_coeff[i];
}

#ifdef ADAPTIVE_APPROACH
 int Quantile::get_number_coeff() {
   return m_number_coeff;
 }

 double Quantile::get_control_val(int i) {
   return m_control_val[i];
 }
#endif

double Quantile::get_min_val() {
  return m_min_val;
}


double Quantile::get_max_val() {
  return m_max_val;
}

double Quantile::get_sum() {
  return m_sum_obs;
}

double Quantile::get_squared_sum() {
  return m_squared_sum_obs;
}

const std::string Quantile::get_metric() {
  return m_patternName;
}

double Quantile::get_lower_quant() {
  return m_lower_quant/m_factor;
}

double Quantile::get_median() {
  return m_median/m_factor;
}

double Quantile::get_upper_quant() {
  return m_upper_quant/m_factor;
}


/**
 * @brief Set class attributes.
 *
 * This methods set certain class attributes.
 */

//Set pattern name
void Quantile::set_metric(const string& pattern_name) {
  m_patternName = pattern_name;
}

//Set factor to enlarge or shrink the observation values to the acceptable interval
void Quantile::set_factor_by_upperbound(double upperbound) { 
  m_factor = (m_limit_b-m_limit_a) / upperbound;
  m_min_val = upperbound;
}


#ifdef ADAPTIVE_APPROACH
void Quantile::set_global_values(int number_obs, double sum_obs, double squared_sum_obs,
                                 double min_val, double max_val, double *coeff,
                                 double *control_val)
#else
void Quantile::set_global_values(int number_obs, double sum_obs, double squared_sum_obs,
                                 double min_val, double max_val, double *coeff)
#endif
{
  m_number_obs = number_obs; //Set observations counter
  
  m_max_val = max_val; //Set maximum for observed values
  m_min_val = min_val; //Set minimum for observed values
  
  m_sum_obs = sum_obs; //Set sum of all observations
  m_squared_sum_obs = squared_sum_obs; //Set squared sum of all observations

  for(int i=0; i<m_number_coeff; i++) {
    m_coeff[i] = coeff[i]; //Set all coefficients of approximated density function
#ifdef ADAPTIVE_APPROACH
    m_control_val[i] = control_val[i]; //Set all control values for the adaptive appraoch to zero
#endif
  }
}


//--- Modify and calculate the density --------------------------------------

/**
 * @brief Adds one observation.
 *
 * Recalculates the coefficients of the approximated density function. Additionally,
 * the maximum and minimum value of all observations are updated.
 */
void Quantile::add_value(double val) {
  double *norm_leg_res = new double[m_number_coeff];
  
  m_number_obs ++; //count observations
    
  m_max_val = (m_max_val<val) ? val:m_max_val; //determine maximum
  m_min_val = (m_min_val>val) ? val:m_min_val; //determine minimum

  m_sum_obs += val; //calculate sum of all observations
  m_squared_sum_obs += (val*val); //calculate squared sum of all observations

  val *= m_factor; //modify value that it fits in the interval where the legendre polynomials are orthogonal
  calc_norm_leg(norm_leg_res, val); //calculate the legendre polynomials at point val

  for(int i=0; i<m_number_coeff; i++) { //re-calculate coefficients
    m_coeff[i] = ((m_number_obs-1)/m_number_obs)*m_coeff[i] + (norm_leg_res[i] / m_number_obs);
  }

#ifdef ADAPTIVE_APPROACH

  for(int i=0; i<m_number_coeff; i++) { //re-calculate coefficients
    m_control_val[i] = ((m_number_obs-1)/m_number_obs)*m_control_val[i] + (norm_leg_res[i]*norm_leg_res[i] / m_number_obs);
  }

#endif  

  delete[] norm_leg_res;
}

#ifdef ADAPTIVE_APPROACH

/**
 * @brief Calculates an optimal number of coefficients.
 *
 * This function calculates whether a smaller number of coefficients exists
 * which delivers a better accuracy.
 */
void Quantile::calc_opt_number_coeff() {
  double min_error = (1/(m_number_obs-1)) * (2*m_control_val[0] - (m_number_obs+1)*m_coeff[0]*m_coeff[0]);
  double func_val = min_error;
  int opt_number_coeff = 1;
  
  for(int i=1; i<m_number_coeff; i++) {
    
    func_val += (1/(m_number_obs-1)) * (2*m_control_val[i] - (m_number_obs+1)*m_coeff[i]*m_coeff[i]);

    if(func_val < min_error) {
      
      min_error = func_val;
      opt_number_coeff = i;

    }
  
  }

  m_number_coeff = opt_number_coeff + 1;

}

#endif


/**
 * @brief Represents the current density function.
 *
 * Calculates the value of the current density function at point x.
 */
double Quantile::calc_density(double x) { //estimated probability density function
  double res = 0.0;
  double *leg_res = new double[m_number_coeff];

  calc_legendre(leg_res, x);
  
  for(int i=0; i<m_number_coeff; i++) {
    res += m_coeff[i]*leg_res[i];
  }

  delete[] leg_res;
  return res;
}

  


/**
 * @brief Calculates the normalized Legendre polynomials.
 *
 * Calculates the value of the normalized Legendre polynomials at point x.
 */
void Quantile::calc_norm_leg(double *norm_leg_res, double x) {
  double *leg_res = new double[m_number_coeff];
  
  calc_legendre(leg_res, x);
  
  norm_leg_res[0] = 1.;
  norm_leg_res[1] = 6.*x-3.;
  
  for(int i=2; i<m_number_coeff; i++) {
    norm_leg_res[i] = (2.*i+1.)/(m_limit_b-m_limit_a) * leg_res[i];
  }

  delete[] leg_res;
}


/**
 * @brief Calculates the Legendre polynomials.
 *
 * Calculates the value of the Legendre polynomials at point x.
 */
void Quantile::calc_legendre(double *leg_res, double x) {

  leg_res[0] = 1.;
  leg_res[1] = (2.*x-m_limit_a-m_limit_b)/(m_limit_b-m_limit_a);
    
  for(int i=2; i<m_number_coeff; i++) {
    leg_res[i] = ((((2.*i-1.)*(2.*x-m_limit_a-m_limit_b)/(m_limit_b-m_limit_a))*leg_res[i-1] - (i-1.)*leg_res[i-2]) / i);
  }

}

//--- Calculate and print out Quantiles -------------------------------------

/**
 * @brief Quantile approximation.
 *
 * Approximates certain quantiles by calculating the integral of the density function.
 * The integration method is an approximation technique which guarantees that the 
 * integral function is a monotnous function.
 */
void Quantile::calc_quantiles() {
  bool lq_found=false, me_found=false, uq_found=false;
  double dx = 0.0001;
  int steps = (m_limit_b - m_limit_a) / dx;
  
  double inte = 0.0;
  double center = m_limit_a + 0.5*dx;
  double tmp, neg=0;

  
#ifdef ADAPTIVE_APPROACH

  if(m_number_obs > 5) {
    calc_opt_number_coeff();
  }

#endif

  
  for( int i=0; !uq_found && i<steps; i++, center+=dx )  {
    tmp = dx * calc_density(center);
    
    if(tmp<0) {
      neg += tmp;
      
    }else {
      
      if(neg>=0) {
	inte += tmp;
	  
      }else {
	  neg += tmp;
	  
      }
      
    }
      
    if(inte >= 0.25 && !lq_found) {
      m_lower_quant = center;
      lq_found = true;
    }
    if(inte >= 0.5 && !me_found) {
      m_median = center;
      me_found = true;
    }
    if(inte >= 0.75 && !uq_found) {
      m_upper_quant = center;
      uq_found = true;
    }
  }
  
}

/**
 * @brief Approximate arbitrary quantile
 *
 * Approximates certainan arbitrary quantile by calculating the integral of the density function.
 * The integration method is an approximation technique which guarantees that the 
 * integral function is a monotnous function.
 */ 
double Quantile::calc_quantile(double req_quant) {
  double dx = 0.0001;
  int steps = (m_limit_b - m_limit_a) / dx;
  
  double inte = 0.0, quant_val = 0.0;
  double center = m_limit_a + 0.5*dx;
  double tmp, neg=0;
  
  for( int i=0; i<steps; i++, center+=dx )  {
    tmp = dx * calc_density(center);
    
    if(tmp<0) {
      neg += tmp;
      
    }else {
      
      if(neg>=0) {
	inte += tmp;
	  
      }else {
	  neg += tmp;
	  
      }
      
    }
      
    if(inte >= req_quant) {
      quant_val = center;
      break;
    }
  }
  
  return quant_val/m_factor;
}


//--- Integration approximation of the density function ---------------------

  /**
 * @brief Integrates the density function.
 *
 * This method calculates the integral of the density function of an arbitrary region. 
 * The integration method based on an approximation method. The accuracy is determined by
 * the variable dx.
 */   
double Quantile::integrate(double a, double b, double dx) {
  // If a greater than b, then swap
  if( a>b ) {
    double tmp = a; a = b; b = tmp;
  }
  
  // Number of stripes in interval
  int    rec_ct = (int)( ( b - a )/dx );
  
  dx = ( b - a )/rec_ct;
  
  double res = 0.0;        // actual sum
  double center   = a + 0.5*dx; // x in the middle of current stripe
  
  // integrate
  for( int i=0; i<rec_ct ; i++, center+=dx )  {
    res += dx * calc_density(center);
  }
  
  return res;
}
