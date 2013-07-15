/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _PRECISIONWIDGET_H
#define _PRECISIONWIDGET_H

#include <QDialog>

#include "Constants.h"

class QSpinBox;
class MainWidget;

//the PrecisionWidget class extends QDialog and is used for two purposes:
//1. to allow to open a dialog to get user-defined precision settings
//2. to convert numbers to strings, using the current precision settings
//
//the class supports two precision settings FORMAT0 and FORMAT1 (see constants.h):
//FORMAT0 is used for the tree display
//FORMAT1 is used in value widgets and in topologies

class PrecisionWidget : public QDialog {

  Q_OBJECT

 public:

  PrecisionWidget(QWidget * parent);
  ~PrecisionWidget();  

  //convert the number into string
  //parameters:
  //value:       the number to convert
  //integerType: the type of the number is integer if "integerType" is true,
  //             otherwise the type is double
  //format:      we support two precision formats (FORMAT0 and FORMAT1);
  //             this parameter allow to define one of them
  QString numberToQString(double value, bool integerType, PrecisionFormat format);

  //some get methods
  double getRoundThreshold(PrecisionFormat format);
  int getPrecision(PrecisionFormat format);
  int getRoundNr(PrecisionFormat format);
  int getUpperExpNr(PrecisionFormat format);

  //some set methods
  void setPrecision(int value, PrecisionFormat format);
  void setUpperExpNr(int value, PrecisionFormat format);
  void setRoundNr(int value, PrecisionFormat format); 


 signals:

  void apply();

 public slots:

   //executes the precision setting dialog
   int exec();

 private slots:

  //sets the values as defined in the dialog;
  //called when the "ok" button is clicked
  void setValues();
  //forget the last changes since the dialog was opened,
  //and re-set the values;
  //this slot is called when the used clicks on "cancel"
  void resetValues();
  //apply the current values as shown in the dialog;
  //called when the user clocks on the "apply" button
  void applyValues();

 private:

  //precision for the two supported formats
  int precision[2];
  //threshold for rounding to zero (in number of digits after the dot being zero)
  int roundNr[2];
  //threshold for rounding to zero (as double)
  double roundThreshold[2];
  //threshold for diplaying the number in scientific (exponent) format
  //(as number of leading digits before the dot)
  int upperExpNr[2];
  //threshold for diplaying the number in scientific (exponent) format
  //(as number)
  double upperExpThreshold[2];

  //spins for the dialog 
  QSpinBox *prec0Spin, *prec1Spin;
  QSpinBox *upper0Spin, *upper1Spin;
  QSpinBox *round0Spin, *round1Spin;

  //store values before starting a dialog for the case of "cancel"
  int v1, v2, v3, v4, v5, v6;
};

#endif 
