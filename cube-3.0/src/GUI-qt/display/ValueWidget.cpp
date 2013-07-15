/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <QString>
#include <QPainter>

#include "ValueWidget.h"
#include "PrecisionWidget.h"
#include "Coloring.h"

#include <cassert>
#include <iostream>
#include <cmath>

ValueWidget::ValueWidget(ColorWidget* colorWidget, PrecisionWidget* precisionWidget, bool systemVWidget) : QWidget() {

  this->colorWidget = colorWidget;
  this->precisionWidget = precisionWidget;
  
  systemValueWidget = systemVWidget;
  mean = 0.0;
  variance = 0.;

  QFontMetrics fm(font());

  setMinimumHeight(2 * (fm.lineSpacing() + 3));
  setMinimumWidth(100);
  setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

  minValue = 0.0;
  maxValue = 0.0;
  selectedValue = 0.0;

  minAbsValue = 0.0;
  maxAbsValue = 0.0;
  absValue = 0.0;

  absValueExists = false;
  selectedValueExists = false;
  minMaxValuesExist = false;

  intType = false;
  userDefinedMinMaxValues = false;

  setWhatsThis("Below each pane there is a selected value information widget. "
               "If no data is loaded, the widget is empty. Otherwise, the "
               "widget displays more extensive and precise information about "
               "the selected values in the tree above. This information widget "
               "and the topologies may have different precision settings than "
               "the trees, such that there is the possibility to display more "
               "precise information here than in the trees (see menu Display "
               "-> Precision).\nThe widget has a two-line display. The first "
               "line displays at most 4 numbers. The leftmost number shows "
               "the smallest value in the tree (or 0.0 in any percentage value "
               "mode for trees, or the user-defined minimal value for coloring "
               "if activated), and the rightmost number shows the largest "
               "value in the tree (or 100.0 in any percentage value mode in "
               "trees, or the user-defined maximal value for coloring if "
               "activated). Between these two numbers the current value of the "
               "selected node is displayed, if it is defined. Additionally, in "
               "the absolute value mode it follows in brackets the percentage "
               "of the selected value on the scale between the minimal and "
               "maximal values. Note that the values of expanded non-leaf "
               "system nodes and of nodes of trees on the left-hand-side of "
               "the metric tree are not defined.\nIf the value mode is not the "
               "absolute value mode, then in the second line similar "
               "information is displayed for the absolute values in a light "
               "gray color.\nIn case of multiple selection, the information "
               "refers to the sum of all selected values. In case of multiple "
               "selection in system trees in the peer distribution and in the "
               "peer percent modes this sum does not state any valuable "
               "information, but it is displayed for consistency reasons.\n"
               "When a system topology is shown with a percentage value mode, "
               "the mean \"+/-\" standard deviation of all values is presented "
               "when no process or thread is selected.\n"
               "If the widget width is not large enough to display all numbers "
               "in the given precision, then a part of the number displays get "
               "truncated and \"...\" ellipsis indicates that not all digits "
               "could be displayed.\nBelow these numbers, a small "
               "color bar shows the position of the color of the selected node "
               "in the color legend. In case of undefined values, the legend "
               "is filled with a gray grid.");

}










ValueWidget::~ValueWidget(){}

void ValueWidget::paintEvent(QPaintEvent*){

  //drawGrad is true, if there is a selected value, and the normal
  //color gradient can be painted; else we use gray filling
  bool drawGrad = true;

  //percent will be 100 * (selectedValue-minValue)/(maxValue-minValue) if defined
  double percent = 0.0;
  //absPercent will be 100 * (absValue-minAbsValue)/(maxAbsValue-minAbsValue) if defined
  double absPercent = 0.0;

  //type of the selected value: if an absolute value exists, then we
  //are in some percentage modus, and the type is not integer; 
  //otherwise we are in the absolute value modus, whose type is stored in the field intType
  bool intType1 = (absValueExists ? false : intType);
  //type of the absolute value
  bool intType2 = intType;

  if (selectedValueExists){

    percent = maxValue - minValue;
    if (percent != 0.0) percent = 100.0 * (selectedValue - minValue) / percent;
    else percent = 100.0;

    if (absValueExists){
      absPercent = maxAbsValue - minAbsValue;
      if (absPercent != 0.0) absPercent = 100.0 * (absValue - minAbsValue) / absPercent;
      else absPercent = 100.0;
    } 

  } else drawGrad = false;

  //compute the value strings for display
  QString minValueString;
  QString maxValueString;
  QString selectedValueString;
  QString minAbsValueString;
  QString maxAbsValueString;
  QString absValueString;

  minValueString.append(precisionWidget->numberToQString(minValue,intType1,FORMAT1));
  if (userDefinedMinMaxValues) minValueString.append("(u)");
  maxValueString.append(precisionWidget->numberToQString(maxValue,intType1,FORMAT1));
  if (userDefinedMinMaxValues) maxValueString.append("(u)");
  if (selectedValueExists){
    selectedValueString.append(precisionWidget->numberToQString(selectedValue,intType1,FORMAT1));
    if (minValue!=0.0 || maxValue!=100.0){ 
      selectedValueString.append(" (");
      selectedValueString.append(precisionWidget->numberToQString(percent,false,FORMAT1));
      selectedValueString.append("%)");
    }
  }

  //if the strings to display are too wide, we shorten them and add "..." at the end
  QFontMetrics fm(font());

  if (fm.width(minValueString) + fm.width(selectedValueString) + fm.width(maxValueString) > width()-10){
    minValueString.append("...");
    maxValueString.append("...");
    selectedValueString.append("...");
    while ( (minValueString.length()>4 || selectedValueString.length()>4 || maxValueString.length()>4) && 
	    fm.width(minValueString) + fm.width(selectedValueString) + fm.width(maxValueString) > width()-10){
      if (minValueString.length()>4 && minValueString.length()>=selectedValueString.length() && minValueString.length()>=maxValueString.length()) {
	minValueString.chop(4);
	minValueString.append("...");
      } else if (selectedValueString.length()>4 && selectedValueString.length()>=maxValueString.length()) {
	selectedValueString.chop(4);
	selectedValueString.append("...");      
      } else {
	assert(maxValueString.length()>4);
	maxValueString.chop(4);
	maxValueString.append("...");      
      } 
    }
  }

  if (absValueExists){

    //compute the absolute value strings for display
    minAbsValueString.append(precisionWidget->numberToQString(minAbsValue,intType2,FORMAT1));
    maxAbsValueString.append(precisionWidget->numberToQString(maxAbsValue,intType2,FORMAT1));

    if (absPercent <= 100) {
        absValueString.append(precisionWidget->numberToQString(absValue,intType2,FORMAT1));
        absValueString.append(" (");
        absValueString.append(precisionWidget->numberToQString(absPercent,false,FORMAT1));
        absValueString.append("%)");
    } else {
        absValueString.append(precisionWidget->numberToQString(mean,intType1,FORMAT1));
        absValueString.append(" +/- ");
        absValueString.append(precisionWidget->numberToQString(variance,intType1,FORMAT1));
    }

    //shorten them, too, if necessary
    if (fm.width(minAbsValueString) + fm.width(absValueString) + fm.width(maxAbsValueString) > width()-10){
      minAbsValueString.append("...");
      maxAbsValueString.append("...");
      absValueString.append("...");
      while ( (minAbsValueString.length()>4 || absValueString.length()>4 || maxAbsValueString.length()>4) && 
	      fm.width(minAbsValueString) + fm.width(absValueString) + fm.width(maxAbsValueString) > width()-10){
	if (minAbsValueString.length()>4 && minAbsValueString.length()>=absValueString.length() && minAbsValueString.length()>=maxAbsValueString.length()) {
	  minAbsValueString.chop(4);
	  minAbsValueString.append("...");
	} else if (absValueString.length()>4 && absValueString.length()>=maxAbsValueString.length()) {
	  absValueString.chop(4);
	  absValueString.append("...");      
	} else {
	  assert(maxAbsValueString.length()>4);
	  maxAbsValueString.chop(4);
	  maxAbsValueString.append("...");      
	} 
      }
    }
    
  }

  //start painting  
  QPainter painter;
  painter.begin(this);

  //draw a rectangle
  painter.fillRect(0,0,width()-1,height()-1,Qt::white);
  painter.drawRect(0,0,width()-1,height()-1);
  //divide the rectangle into two parts for the numbers and the color gradient
  painter.drawLine(0,height()-6,width()-1,height()-6);

  //if color gradient should be painted
  if (drawGrad) {
    //define color gradient
    QLinearGradient linearGrad(QPointF(0,0),QPointF(width(),0));
    QVector<QGradientStop> gradStops;
    unsigned grades = 100;
    for (unsigned i=0; i<=grades; i++)
      gradStops.push_back(QPair<qreal, QColor>((double)i/(double)grades, colorWidget->getColor((double)i,0.0,(double)grades)));
    linearGrad.setStops(gradStops);
    //fill out the available scale until the value percentage with the gradient
    int xPos = (int)(fabs(percent) * (double)width() / 100.0);
    painter.fillRect(2,height()-5,xPos,3,linearGrad);
  } else if (minMaxValuesExist){
    //no color gradient, but gray filled scale (no values defined)
    QBrush brush(Qt::black,Qt::Dense5Pattern);
    painter.fillRect(2,height()-5,width()-2,3,brush);
  }

  //now paint the numbers
  if (minMaxValuesExist){
    painter.drawText(2, 2, width()-4, height()-10, Qt::AlignLeft  | Qt::AlignTop, minValueString);
    painter.drawText(2, 2, width()-4, height()-10, Qt::AlignRight | Qt::AlignTop, maxValueString);

    if (selectedValueExists) {
      painter.drawText(fm.width(minValueString) + 4, 2,
                       width()-fm.width(minValueString) - fm.width(maxValueString) - 8,
                       height()-10,Qt::AlignHCenter|Qt::AlignTop, selectedValueString);

      if (absValueExists) {
	painter.setPen(Qt::gray);
	painter.drawText(2, 2, width()-4, height()-10, Qt::AlignLeft  | Qt::AlignBottom, minAbsValueString);
	painter.drawText(2, 2, width()-4, height()-10, Qt::AlignRight | Qt::AlignBottom, maxAbsValueString);
	painter.drawText(fm.width(minAbsValueString) + 4, 2, 
			 width() - fm.width(minAbsValueString) - fm.width(maxAbsValueString) - 8, height()-10, 
			 Qt::AlignHCenter | Qt::AlignBottom,
			 absValueString);
      }
    }
  }

  painter.end();


}

//update selected and absolute values
void ValueWidget::update(double minValue, double maxValue, double selectedValue, double minAbsValue, double maxAbsValue, double absValue, bool intType, bool userDefinedMinMaxValues, double _mean, double _variance){
  this->minValue = minValue;
  this->maxValue = maxValue;
  minMaxValuesExist = true;
  this->selectedValue = selectedValue;
  this->selectedValueExists = true;
  this->absValue = absValue;
  this->minAbsValue = minAbsValue;
  this->maxAbsValue = maxAbsValue;
  this->absValueExists = true;
  this->intType = intType;
  this->userDefinedMinMaxValues = userDefinedMinMaxValues;


  this->mean = _mean;
  this->variance = _variance;

  repaint();
}

//update selected value, no absolute value defined
void ValueWidget::update(double minValue, double maxValue, double selectedValue, bool intType, bool userDefinedMinMaxValues, double _mean, double _variance){
  this->minValue = minValue;
  this->maxValue = maxValue;
  minMaxValuesExist = true;
  this->selectedValue = selectedValue;
  this->selectedValueExists = true;
  this->absValueExists = false;
  this->intType = intType;
  this->userDefinedMinMaxValues = userDefinedMinMaxValues;

  this->mean = _mean;
  this->variance = _variance;

  repaint();
}

//no selected value defined
void ValueWidget::update(double minValue, double maxValue, bool intType, bool userDefinedMinMaxValues, double _mean, double _variance){
  this->minValue = minValue;
  this->maxValue = maxValue;
  this->selectedValueExists = false;
  this->absValueExists = false;
  this->intType = intType;
  minMaxValuesExist = true;
  this->userDefinedMinMaxValues = userDefinedMinMaxValues;

  this->mean = _mean;
  this->variance = _variance;

  repaint();
}

//clean up, no data loaded
void ValueWidget::clear(){
  selectedValueExists = false;
  minMaxValuesExist = false;
  repaint();
}

