/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _VALUEWIDGET_H
#define _VALUEWIDGET_H

#include <QWidget>

class PrecisionWidget;
class ColorWidget;

//the ValueWidget class is used to display more precise values for the
//selected items; 
//currently three instances are created and displayed
//below the metric, call, and system tabs
//
//the value widget displays in a first row the minimal and maximal
//values for coloring, and in between the current selected value; 
//if several items are selected, the value is the sum, i.e, the value
//that is used for value computations on the right hand side
//
//the second row contains the same informations but with absolute values;
//displayed if the current value modus is not the absolute value modus
//
//the last row contains a color gradient fill upto the color of the
//selected value; undefined values are represented by a gray filling
//(no data loaded or the selected value is undefined)

class ValueWidget : public QWidget {

Q_OBJECT

 public:

//   ValueWidget(ColorWidget* colorWidget, PrecisionWidget* precisionWidget);
  ValueWidget(ColorWidget* colorWidget, PrecisionWidget* precisionWidget, bool systemVWidget = false);
  ~ValueWidget();

  //update current values, where all three lines exist
  void update(double minValue, double maxValue, double selectedValue, double minAbsValue, double maxAbsValue, double absValue, bool intType, bool userDefinedMinMaxValues, double _mean, double _variance);
  //update current values, no absolute value line (current modus is absolute value)
  void update(double minValue, double maxValue, double selectedValue, bool intType, bool userDefinedMinMaxValues, double _mean, double _variance);
  //no current selected value exists, or is undefined
  void update(double minValue, double maxValue, bool intType, bool userDefinedMinMaxValues, double _mean, double _variance);
  void clear();

 protected:

  //paints the widget
  void paintEvent(QPaintEvent*);

 private:

  // Should calculate median and variance for system tree?
   bool systemValueWidget;
   double mean;
   double variance;

  //the currently displayed minimal/maximal values and the selected value
  double minValue;
  double maxValue;
  double selectedValue;

  //the currently displayed absolute minimal/maximal values and the selected absolute value
  double minAbsValue;
  double maxAbsValue;
  double absValue;

  //is the value type integer? 
  bool intType;

  //which lines must be displayed? exist selected, absolute, and min/max values?
  bool selectedValueExists;
  bool absValueExists;
  bool minMaxValuesExist;

  //the colorWidget is used to get the color gradient displayed in the value widget
  ColorWidget* colorWidget;
  //the precisionWidget is used to convert numbers into strings
  PrecisionWidget* precisionWidget;

  //stores if the user defined minimal and maximal values for coloring
  bool userDefinedMinMaxValues;
};

#endif
