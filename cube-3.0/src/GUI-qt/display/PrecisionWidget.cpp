/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cassert>
#include <iostream>
#include <sstream>
#include <cmath>

#include "PrecisionWidget.h"
#include "MainWidget.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QSpinBox>
#include <QAbstractButton>

using namespace std;


PrecisionWidget::PrecisionWidget(QWidget* parent) : QDialog(parent){

  setWindowTitle("Precision settings");

  //define layout of the dialog

  QVBoxLayout * layout = new QVBoxLayout();
  setLayout(layout);

  QGroupBox *groupBox0 = new QGroupBox(tr("Display in trees:"),this);
  QGridLayout* groupBox0Layout = new QGridLayout();
  groupBox0->setLayout(groupBox0Layout);
  layout->addWidget(groupBox0);

  QGroupBox *groupBox1 = new QGroupBox(tr("Display in the value widget under the tree widgets and in topologies:"),this);
  QGridLayout* groupBox1Layout = new QGridLayout();
  groupBox1->setLayout(groupBox1Layout);
  layout->addWidget(groupBox1);


  QLabel * prec0Label = new QLabel(this);
  prec0Label->setText("Number of digits after decimal point: ");
  groupBox0Layout->addWidget(prec0Label,1,1);

  prec0Spin = new QSpinBox(this);
  prec0Spin->setRange(1,20);
  prec0Spin->setSingleStep(1);
  prec0Spin->setValue(1);
  groupBox0Layout->addWidget(prec0Spin,1,2);

  QLabel * upper0Label = new QLabel(this);
  upper0Label->setText("Exponent representation above 10^x with x: ");
  groupBox0Layout->addWidget(upper0Label,2,1);

  upper0Spin = new QSpinBox(this);
  upper0Spin->setRange(0,20);
  upper0Spin->setSingleStep(1);
  upper0Spin->setValue(0);
  groupBox0Layout->addWidget(upper0Spin,2,2);

  QLabel * round0Label = new QLabel(this);
  round0Label->setText("Display zero for values below 10^(-x) with x: ");
  groupBox0Layout->addWidget(round0Label,3,1);

  round0Spin = new QSpinBox(this);
  round0Spin->setRange(1,20);
  round0Spin->setSingleStep(1);
  round0Spin->setValue(1);
  groupBox0Layout->addWidget(round0Spin,3,2);

  QLabel * prec1Label = new QLabel(this);
  prec1Label->setText("Number of digits after decimal point: ");
  groupBox1Layout->addWidget(prec1Label,1,1);

  prec1Spin = new QSpinBox(this);
  prec1Spin->setRange(1,20);
  prec1Spin->setSingleStep(1);
  prec1Spin->setValue(1);
  groupBox1Layout->addWidget(prec1Spin,1,2);

  QLabel * upper1Label = new QLabel(this);
  upper1Label->setText("Exponent representation above 10^x with x: ");
  groupBox1Layout->addWidget(upper1Label,2,1);

  upper1Spin = new QSpinBox(this);
  upper1Spin->setRange(0,20);
  upper1Spin->setSingleStep(1);
  upper1Spin->setValue(0);
  groupBox1Layout->addWidget(upper1Spin,2,2);

  QLabel * round1Label = new QLabel(this);
  round1Label->setText("Display zero for values below 10^(-x) with x: ");
  groupBox1Layout->addWidget(round1Label,3,1);

  round1Spin = new QSpinBox(this);
  round1Spin->setRange(1,20);
  round1Spin->setSingleStep(1);
  round1Spin->setValue(1);
  groupBox1Layout->addWidget(round1Spin,3,2);

  QDialogButtonBox * buttonBox = new QDialogButtonBox();
  QPushButton* applyButton = buttonBox->addButton(QDialogButtonBox::Apply);
  QPushButton* okButton = buttonBox->addButton(QDialogButtonBox::Ok);
  QPushButton* cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);
  connect(applyButton, SIGNAL(clicked()), this, SLOT(applyValues()));
  connect(okButton, SIGNAL(clicked()), this, SLOT(setValues()));  
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(resetValues()));
  layout->addWidget(buttonBox);

  //initialize values

  setPrecision(2,FORMAT0);
  setRoundNr(7,FORMAT0);
  setUpperExpNr(4,FORMAT0);

  setPrecision(2,FORMAT1);
  setRoundNr(7,FORMAT1);
  setUpperExpNr(4,FORMAT1);

  v1 = prec0Spin->value();
  v2 = prec1Spin->value();
  v3 = upper0Spin->value();
  v4 = upper1Spin->value();
  v5 = round0Spin->value();
  v6 = round1Spin->value();

  setWhatsThis("This is a dialog for precision settings. Besides \"Ok\" and \"Cancel\", the dialog offers an \"Apply\" button, that applies the current dialog settings to the display. Pressing \"Cancel\" undoes all changes due to the dialog, even if you already pressed \"Apply\" previously.\n\nThe dialog consists of two parts: precision settings for the tree displays, and precision settings for the selected value info widgets and the topology displays. For both formats, three values can be defined:\n\n - Number of digits after the decimal point: As the name suggests, you can specify the precision for the fraction part of the values. E.g., the number 1.234 is displayed as 1.2 if you set this precision to 1, as 1.234 if you set it to 3, and as 1.2340 if you set it to 4.\n\n - Exponent representation above 10^x with x: Here you can define above which threshold we should use scientific notation. E.g., the value 1000 is displayed as 1000 if this value is larger then 3 and as 1e3 otherwise.\n\n - Display zero values below 10^(-x) with x: Due to inexact floating point representation it often happens that the users wish to round down values near by zero to zero. Here you can define the threshold below which this rounding should take place. E.g., the value 0.0001 is displayed as 0.0001 if this value is larger than 3 and as zero otherwise.");

}

PrecisionWidget::~PrecisionWidget(){}

int PrecisionWidget::exec(){

  //store values for the case that the user clicks on "cancel", in which case
  //the values at dialog start get re-set
  v1 = prec0Spin->value();
  v2 = prec1Spin->value();
  v3 = upper0Spin->value();
  v4 = upper1Spin->value();
  v5 = round0Spin->value();
  v6 = round1Spin->value();

  return QDialog::exec();
}

//some get methods

double PrecisionWidget::getRoundThreshold(PrecisionFormat format){
  assert(format==0 || format==1);
  return roundThreshold[format];
}
int PrecisionWidget::getPrecision(PrecisionFormat format){
  return precision[format];
}
int PrecisionWidget::getRoundNr(PrecisionFormat format){
  return roundNr[format];
}
int PrecisionWidget::getUpperExpNr(PrecisionFormat format){
  return upperExpNr[format];
}

//some set methods

void PrecisionWidget::setPrecision(int value, PrecisionFormat format){ 
  if (value<1 || value>20){
    std::cout << "Precision out of range\n";
    return;
  }
  precision[format] = value;
  if (format==FORMAT0) prec0Spin->setValue(value);
  else prec1Spin->setValue(value);
}

void PrecisionWidget::setUpperExpNr(int value, PrecisionFormat format){ 
  if (value<0 || value>20){
    std::cout << "Upper exponent threshold out of range\n";
    return;
  }
  upperExpNr[format] = value;
  upperExpThreshold[format] = pow(10.0,value);
  if (format==FORMAT0) upper0Spin->setValue(value);
  else upper1Spin->setValue(value);
}

void PrecisionWidget::setRoundNr(int value, PrecisionFormat format){ 
  if (value<1 || value>20){
    std::cout << "Rounding threshold out of range\n";
    return;
  }
  roundNr[format] = value;
  roundThreshold[format] = pow(10.0,-value);
  if (format==FORMAT0) round0Spin->setValue(value);
  else round1Spin->setValue(value);
}

//save the dialog values as current values
//(user pressed "ok")
void PrecisionWidget::setValues(){

  setPrecision(prec0Spin->value(),FORMAT0);
  setPrecision(prec1Spin->value(),FORMAT1);
  setUpperExpNr(upper0Spin->value(),FORMAT0);
  setUpperExpNr(upper1Spin->value(),FORMAT1);
  setRoundNr(round0Spin->value(),FORMAT0);
  setRoundNr(round1Spin->value(),FORMAT1);

  emit apply();

  accept();
}

//save the dialog values as current values
//and apply them to the display
//(user pressed "apply"
void PrecisionWidget::applyValues(){

  setPrecision(prec0Spin->value(),FORMAT0);
  setPrecision(prec1Spin->value(),FORMAT1);
  setUpperExpNr(upper0Spin->value(),FORMAT0);
  setUpperExpNr(upper1Spin->value(),FORMAT1);
  setRoundNr(round0Spin->value(),FORMAT0);
  setRoundNr(round1Spin->value(),FORMAT1);

  emit apply();
}

//re-set the values upon dialog start
//(user pressed "cancel")
void PrecisionWidget::resetValues(){

  setPrecision(v1,FORMAT0);
  setPrecision(v2,FORMAT1);
  setUpperExpNr(v3,FORMAT0);
  setUpperExpNr(v4,FORMAT1);
  setRoundNr(v5,FORMAT0);
  setRoundNr(v6,FORMAT1);

  emit apply();

  reject();
}


//convert the number into string
//parameters:
//value:       the number to convert
//integerType: the type of the number is integer if "integerType" is true,
//             otherwise the type is double
//format:      we support two precision formats (FORMAT0 and FORMAT1);
//             this parameter allow to define one of them
QString PrecisionWidget::numberToQString(double value, bool integerType, PrecisionFormat format){

  assert(upperExpThreshold[format]>0);
  assert(precision[format]>0);

  //the final result
  QString text;

  int decimalPlaces = 0;

  if (value==0.0){
    //if the value to display is zero, then display is straightforward
    if (integerType) text.append('0');
    else {
      text.append("0.");
      while (decimalPlaces++<precision[format])
	text.append('0');
    }
  } else {
    
    //if the value is not zero, then first handle sign
    if (value<0){
      text.append('-');
      value = -value;
    }
  
    std::ostringstream ostr;
    ostr.precision(precision[format]);
    ostr << std::showpoint;

    //check if scientific or fixed point format applies and use std to
    //convert the number into a string "str"; though we set the
    //precision, it can happen that we get too few characters
    //(skipped trailing zeros)
    bool scientific = false;
    if (value>=upperExpThreshold[format]) {
      scientific = true;
      ostr <<  std::scientific << value;
    } else {
      ostr <<  std::fixed << value;
    }
    std::string str = ostr.str();
    
    //search for the dot's position within the string "str"
    //and append all numeric characters before the dot to the final QString "text"
    int pos = 0;
    char c;
    while (true){
      assert(pos<(int)str.length());
      c = str.at(pos++);
      if (c=='.') break;
      text.append(c);
    }
    //if the value is of type integer and fixed point format applies,
    //we are ready; 
    //otherwise, we append further numeric characters after the dot
    //according to the precision settings
    if (scientific || !integerType) {
      text.append('.');

      //search for the position of the last non-zero numerical
      //character after the dot (and before the "e" of the
      //scientific format)
      int startPos = pos;
      int lastNonZero = pos - 1;
      while (pos<(int)str.length()){
	c = str.at(pos);
	if (c=='e' || c=='E') break;
	if (c!='0') lastNonZero = pos;
	pos++;
      }
      //append the non-zero characters to the final string
      for (int i=startPos; i<=lastNonZero; i++){
	text.append(str.at(i));
	decimalPlaces++;
      }
      //fill up with zeros until the required precision is reached
      while (decimalPlaces++<precision[format])
	text.append('0');

      //in scientific format we append the exponent part      
      if (pos<(int)str.length()){
	assert(str.at(pos)=='e' || str.at(pos)=='E');
	text.append('e');
	pos++;
	//remove leading zeros and the "+" sign
	//and handle the "-" sign
	while (pos<(int)str.length()){
	  if (str.at(pos)!='0' && str.at(pos)!='+' && str.at(pos)!='-')
	    break;
	  if (str.at(pos)=='-') text.append('-');
	  pos++;
	}
	//for empty exponent append a zero
	if (pos==(int)str.length()){
	  text.append('0');
	} else {
	  //else append the exponent
	  for (int i=pos; i<(int)str.length(); i++)
	    text.append(str.at(i));
	}
      }
    }
  }

  return text;
}




