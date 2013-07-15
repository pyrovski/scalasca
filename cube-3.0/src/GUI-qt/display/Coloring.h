/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _COLORING_H
#define _COLORING_H

#include <QDialog>

#include "Constants.h"

class QDoubleSpinBox;

//the ColorWidget class stores the color settings and serves with
//converting values into colors;
//it also invokes ColorDialog instances to allow the user to make color settings


class ColorWidget : public QWidget {

Q_OBJECT

 public:

  ColorWidget();
  //copy constructor
  ColorWidget(ColorWidget* colorWidget);

  //some set methods
  void setLightenStart(double value);
  void setWhiteStart(double value);
  void setColoringMethod(ColoringMethod coloringMethod);
  void setInDialog(bool inDialog);

  //this method is invoked by MainWidget when menu "Display/Coloring/General coloring"
  //is activated;
  //it connects ColorWidget with ColorDialog;
  //starts a new dialog to input user color settings and sets
  //the appropriete values
  void setColors();


  //return a color corresponding to the value parameter 
  //having minValue at color position 0.0 
  //and maxValue at color position 1.0 on the color scale;
  //whiteForZero specifies for topology widgets
  //if the value zero should become white as color 
  QColor getColor(double value, double minValue, double maxValue, bool whiteForZero = true);


  //in the color dialog the user may define the position of the colors 
  //blue, cyan, green, yellow, and red in the
  //color scale;
  //this method is invoked when in the color dialog one of those
  //5 color positions are changed;
  //it sets the corresponding colorPos values, and assures
  //that colorPos[i]<=colorPos[j] for all i<=j,  
  //parameters:
  //index: index of the color position
  //value: new position between 0 and 1
  void setColorPos(unsigned index, double value);

  //some get methods
  double getLightenStart();
  double getWhiteStart();
  double getColorPos(unsigned index);
  ColoringMethod getColoringMethod();
  bool getInDialog();

 signals:

  void updateValueSpins();

  //this signal is emitted when the color settings have been changed
  void apply();

 public slots:

  void onApply(ColorWidget* _colorWidget);

 protected:

  //the main painting method
  void paintEvent(QPaintEvent*);

  //mouse event handling methods;
  //the user can use the mouse to specify the color positions
  //if the mouse is pressed, kept pressed and moved,
  //then the nearest color position will be changed accordingly
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent *event);

 private:

  //defines till which percentage of the color scale 
  //colors should be lightened
  double lightenStart;

  //defines till which percentage of the color scale 
  //colors should be white
  double whiteStart;

  //defines some color positions on a color range from 0.0 to 1.0:
  //blue has the position 0.0
  //red has the position 1.0
  //color[0]: colors below this position are gray
  //color[4]: colors above this position are gray
  //color[1]: position of cyan
  //color[2]: position of green
  //color[3]: position of yellow
  //colors corresponding to points 
  //between the blue-cyan-green-yellow-red data points are approximated
  //according to different possible functions (e.g. linearly)
  double colorPos[5];

  //specifies which function should be used for that apporimation;
  //Coloringmethod is defines in constants.h
  ColoringMethod coloringMethod;

  //remembers if we are in a color dialog
  bool inDialog;

  //painting color scales in dialogs
  void paintInDialog();

  //painting color scale for the main widget
  void paint();

  //if the mouse is clicked, determine which color position
  //(i.e. which colorPos element)
  //will be changed by mouse movement
  void setMouseColorPos(QMouseEvent *event);

  //set new colorPos value after mouse movement
  void updateColorPos(QMouseEvent *event);

  //functions for the different ColoringMethods
  double linIncr(double min, double max, double value);
  double linDecr(double min, double max, double value);
  double quadratic1Incr(double min, double max, double value);
  double quadratic1Decr(double min, double max, double value);
  double quadratic2Incr(double min, double max, double value);
  double quadratic2Decr(double min, double max, double value);
  double exp1Incr(double min, double max, double value);
  double exp1Decr(double min, double max, double value);
  double exp2Incr(double min, double max, double value);
  double exp2Decr(double min, double max, double value);
  double incr(double min, double max, double value);
  double decr(double min, double max, double value);

  //stores if the mouse is currently pressed
  bool mousePressed;

  //stores which color position (i.e. which index in colorPos)
  //will be changed by mouse movement
  int mouseColorPos;

  //method for lightening colors
  QColor lighten(QColor color, double position);

};

//the ColorDialog class defines a color dialog to allow the user
//color settings;
//used only by ColorWidget

class ColorDialog : public QDialog {

  Q_OBJECT
  
 public:

  ColorDialog(QWidget * parent, ColorWidget* colorWidget);

 signals:


  void apply(ColorWidget*);

 public slots:

  void onUpdateValueSpins();

 private slots:

  void setLightenStart(double value);
  void setWhiteStart(double value);
  void setColorPos0(double value);
  void setColorPos1(double value);
  void setColorPos2(double value);
  void setColorPos3(double value);
  void setColorPos4(double value);

  void onLinear(bool activ);
  void onQuadratic1(bool activ);
  void onQuadratic2(bool activ);
  void onExponential1(bool activ);
  void onExponential2(bool activ);

  void onApply();

 private:

  QDoubleSpinBox* spin[5];
  ColorWidget* colorWidget;

  void setColorPos(unsigned index, double value);

};


#endif
