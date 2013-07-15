/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef SCOUT_QUANTILE_H
#define SCOUT_QUANTILE_H


#include <string>

#define ADAPTIVE_APPROACH


/*-------------------------------------------------------------------------*/
/**
 * @file  Quantile.h
 * @brief Declaration of the Quantile class.
 *
 * This header file provides the declaration of the Quantile class which can
 * be used to generate a statistics for the Cube presenter.
 */
/*-------------------------------------------------------------------------*/

namespace scout
{

  /*-------------------------------------------------------------------------*/
  /**
   * @class Quantile
   * @brief Quantile class to generate a Cube statistics file.
   *
   * This class provides a complex technique to approximate certain quantiles
   * of an arbitrary number of observations. In addition, several instances 
   * of this class can be combined to one global statistical result.
   */
  /*-------------------------------------------------------------------------*/

  class Quantile {

  private:
    // pattern name
    std::string m_patternName;

    //attributes of Legendre polynomials
    double m_limit_a, m_limit_b; //interval limits
    double *m_coeff; //coefficients of the orthogonal series function (approximated density function)
    double m_factor; //standardisation factor
    int m_number_coeff; //maxium number of used coefficients

#ifdef ADAPTIVE_APPROACH

    double *m_control_val; //control values required for the adaptive approach

#endif
        
    //relevant values for cube file
    double m_number_obs, m_sum_obs, m_squared_sum_obs; //number of observations , sum and squared sum of observations
    double m_min_val, m_lower_quant, m_median, m_upper_quant, m_max_val; //relevant values to draw a boxplot
   
  public: 
    /// @name Constructors & destructor
    /// @{

    //Quantile();
    Quantile(int numberCoeff);
    ~Quantile();

    /// @}
    /// @name Get and set class attributes
    /// @{
        
    double get_factor();
    double get_n();
    double get_coeff(int i);
#ifdef ADAPTIVE_APPROACH
    int get_number_coeff();
    double get_control_val(int i);
#endif
    double get_min_val();
    double get_max_val();
    double get_sum();
    double get_squared_sum();
    const std::string get_metric();
    double get_lower_quant();
    double get_median();
    double get_upper_quant();

    void set_metric(const std::string& pattern_name);
    void set_factor_by_upperbound(double upperbound);  

#ifdef ADAPTIVE_APPROACH
    void set_global_values(int number_obs, double sum_obs, double squared_sum_obs, double min_val, double max_val, double *coeff, double *control_val);
#else
    void set_global_values(int number_obs, double sum_obs, double squared_sum_obs, double min_val, double max_val, double *coeff);
#endif

    /// @}
    /// @name Modify and calculate the density
    /// @{

    void add_value(double val);

#ifdef ADAPTIVE_APPROACH
   
    void calc_opt_number_coeff();
   
#endif

    double calc_density(double x);
    void calc_norm_leg(double *norm_leg_res, double x);      
    void calc_legendre(double *leg_res, double x);  

    /// @}
    /// @name Calculate and print out Quantiles
    /// @{
    
    void calc_quantiles();
    double calc_quantile(double req_quant);
    
    /// @}
    /// @name Integration approximation of the density function
    /// @{

    double integrate(double a, double b, double dx);

    /// @}

  }; //Quantile class

}  //namespace scout


#endif   // !SCOUT_QUANTILE_H



