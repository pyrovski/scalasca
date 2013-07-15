/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <QPainter>
#include <QGridLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QMouseEvent>
#include <QDoubleSpinBox>
#include <QPushButton>

#include "Coloring.h"

#include <cmath>
#include <cassert>
#include <iostream>

using namespace std;


ColorWidget::ColorWidget() : QWidget(){

  setMinimumHeight(20);
  setMinimumWidth(100);
  setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
  mousePressed = false;

  lightenStart = 0.0;
  whiteStart = 0.0;

  colorPos[0] = 0.0;
  colorPos[1] = 0.1;
  colorPos[2] = 0.2;
  colorPos[3] = 0.3;
  colorPos[4] = 1.0;

  coloringMethod = EXPONENTIAL2;
  inDialog = false;

  setWhatsThis("By default, the colors are taken from a spectrum ranging from blue over cyan, green, and yellow to red, representing the whole range of possible values.  You can change the color settings in the menu, see menu Display -> Coloring -> General coloring. Exact zero values are represented by the color white (in topologies you can decide if you would like to use white or the minimal color, see menu Display -> Coloring -> Topology coloring).");
}

//copy constructor
//
ColorWidget::ColorWidget(ColorWidget* colorWidget) : QWidget() {

  setMinimumHeight(20);
  setMinimumWidth(100);
  setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
  mousePressed = false;

  lightenStart = colorWidget->lightenStart;
  whiteStart = colorWidget->whiteStart;
  for (unsigned i=0; i<5; i++)
    colorPos[i] = colorWidget->getColorPos(i);
  coloringMethod = colorWidget->coloringMethod;
  inDialog = colorWidget->inDialog;
}


//some set methods

void ColorWidget::setLightenStart(double value){lightenStart = value;}
void ColorWidget::setWhiteStart(double value){whiteStart = value;}
void ColorWidget::setColoringMethod(ColoringMethod coloringMethod){
  this->coloringMethod = coloringMethod;
}
void ColorWidget::setInDialog(bool inDialog){this->inDialog = inDialog;}



//this method is invoked by MainWidget when menu "Display/Coloring/General coloring"
//is activated;
//it connects ColorWidget with ColorDialog;
//starts a new dialog to input user color settings and sets
//the appropriete values
//
void ColorWidget::setColors(){

  //save the values at dialog start for the case of cancelling the
  //dialog (then they will be re-set)
  double _whiteStart = whiteStart;
  double _lightenStart = lightenStart;
  double _colorPos[5];
  for (unsigned i=0; i<5; i++)
    _colorPos[i] = colorPos[i];    
  ColoringMethod _coloringMethod = coloringMethod;

  //take a temporary ColorWidget for the case of cancelling the input
  ColorWidget* _colorWidget = new ColorWidget(this);
  //start a ColorDialog
  ColorDialog * colorDialog = new ColorDialog(this,_colorWidget);

  connect(colorDialog,SIGNAL(apply(ColorWidget*)),this,SLOT(onApply(ColorWidget*)));

  if (!colorDialog->exec()){
    //if the dialog ended with "cancel", undo all changes
    whiteStart = _whiteStart;
    lightenStart = _lightenStart;
    for (unsigned i=0; i<5; i++)
      colorPos[i] = _colorPos[i];    
    coloringMethod = _coloringMethod;

    //repaint the color legend with the current values
    update();
    repaint();

    //the tabs and value widgets should be also repainted
    emit apply();

  } else {
    //else take the values set
    onApply(_colorWidget);
  }


  disconnect(colorDialog,SIGNAL(apply(ColorWidget*)),this,SLOT(onApply(ColorWidget*)));

  delete _colorWidget;
  delete colorDialog;

}
//end of setColors()

//this slot is called when the user presses the "apply" button in the
//color dialog
//
void ColorWidget::onApply(ColorWidget* _colorWidget){
  whiteStart = _colorWidget->getWhiteStart();
  lightenStart = _colorWidget->getLightenStart();
  for (unsigned i=0; i<5; i++)
    colorPos[i] = _colorWidget->getColorPos(i);    
  coloringMethod = _colorWidget->getColoringMethod();
  //repaint the color legend with the current values
  update();
  repaint();
  //the tabs and value widgets should be also repainted
  emit apply();
}

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
//
void ColorWidget::setColorPos(unsigned index, double value){
  if (value<0.0) value = 0.0;
  if (value>1.0) value = 1.0;
  if (index==0){
    //colorPos[0] defines the left boundary of coloring
    colorPos[0] = value; 
    if (colorPos[4]<value) setColorPos(4,value);
  } else if (index==1){
    //colorPos[1] defines the position of cyan
    colorPos[1] = value; 
    if (colorPos[2]<value) setColorPos(2,value);
  } else if (index==2){
    //colorPos[2] defines the position of green
    colorPos[2] = value; 
    if (colorPos[1]>value) setColorPos(1,value);
    if (colorPos[3]<value) setColorPos(3,value);
  } else if (index==3){
    //colorPos[3] defines the position of yellow
    colorPos[3] = value; 
    if (colorPos[2]>value) setColorPos(2,value);
  } else if (index==4){
    //colorPos[4] defines the right boundary of coloring
    colorPos[4] = value; 
    if (colorPos[0]>value) setColorPos(0,value);
  }
}


//some get methods

double ColorWidget::getLightenStart(){return lightenStart;}
double ColorWidget::getWhiteStart(){return whiteStart;}
double ColorWidget::getColorPos(unsigned index){return colorPos[index];}
ColoringMethod ColorWidget::getColoringMethod(){return coloringMethod;}
bool ColorWidget::getInDialog(){ return inDialog; }



//this method is for painting the color scale;
//if inDialog is true, then we paint a dialog color scale with additional informations,
//otherwise a simple scale
//
void ColorWidget::paintEvent(QPaintEvent*){
  if (inDialog) paintInDialog();
  else paint();
}

//paint color scale for color dialog
//
void ColorWidget::paintInDialog(){

  QLinearGradient linearGrad;
  int h = height();
  int w = width();

  //generate the color gradient

  QVector<QGradientStop> gradStops;
  unsigned numGradStops = 1000;
  double diff = 1.0 / ((double)numGradStops);
  
  for (unsigned i=0; i<=numGradStops; i++){
    double x = i * diff;
    assert(x>=0.0);
    gradStops.push_back(QPair<qreal, QColor>(x,getColor(x,0.0,1.0)));
  }
  
  linearGrad.setStart(10,0);
  linearGrad.setFinalStop(w-10,0);
  linearGrad.setStops(gradStops);

  //draw the color gradient
  QPainter painter(this);
  painter.fillRect(10,0,w-20,h-40,linearGrad);
  
  //draw the value scale below the gradient from 0.0 to 1.0
  for (unsigned i=0; i<5; i++){
    int wPos = (int)(10.0+colorPos[i]*(w-20));
    painter.fillRect(wPos,0,(i==0||i==4 ? 3 : 1),h-40,QBrush(Qt::black));
  }
  
  painter.drawLine(10,h-30,w-10,h-30); 
  int d = (w-20)/10;
  for (unsigned i=0; i<=10; i++)
    painter.drawLine(10+i*d,h-33,10+i*d,h-27);
  
  painter.drawText(10,h-20,20,20,Qt::AlignLeft|Qt::AlignTop,"0.0");
  painter.drawText(w-30,h-20,20,20,Qt::AlignRight|Qt::AlignTop,"1.0");
}

//paint normal color scale (for the main widget)
//
void ColorWidget::paint(){

  //generate the color gradient
  QLinearGradient linearGrad(QPointF(0,0),QPointF(width(),0));
  QVector<QGradientStop> gradStops;

  unsigned numGradStops = 500;
  for (unsigned i=0; i<=numGradStops; i++){
    assert((double)i>=0.0);
    gradStops.push_back(QPair<qreal, QColor>((double)i/(double)numGradStops, 
                         ColorWidget::getColor((double)i,0.0,(double)numGradStops)));
  }
  linearGrad.setStops(gradStops);
  
  //draw the color gradient
  assert(width()>0);
  assert(height()>0);

  QPainter painter(this);
  painter.fillRect(rect(),linearGrad);
  
}


//the user can use the mouse to specify the color positions
//if the mouse is pressed, kept pressed and moved,
//then the nearest color position will be changed accordingly


void ColorWidget::mousePressEvent(QMouseEvent *event){

  //the event should not be handled elsewhere
  event->accept();

  //applicable only in color dialogs
  if (!inDialog) return;

  //determine which color position will be changed by mouse movement
  setMouseColorPos(event);
}

void ColorWidget::mouseReleaseEvent(QMouseEvent* event){
  event->accept();
  mousePressed = false;
}

void ColorWidget::mouseMoveEvent(QMouseEvent *event)
{
  event->accept();
  if (mousePressed){
    updateColorPos(event);
    repaint();
  }
}

//if the mouse is clicked, determine which color position
//will be cganged by mouse movement
//
void ColorWidget::setMouseColorPos(QMouseEvent *event){

  int x = event->pos().x();
  int y = event->pos().y();
  int w = width();
  int h = height();

  //check if clicked inside the color scale
  if (y<0 || y>h-40 || x<10 || x>w-10) return;

  int distance = w;

  //find nearest colorPos
  for (int i=0; i<5; i++){
    int xPos = (int)(10.0+colorPos[i]*(w-20));
    int d = xPos-x;
    if (d<0) d = -d;
    if (d<=distance) {
      distance = d;
      mouseColorPos = i; 
    }
  }
  mousePressed = true;
}

//set new colorPos value after mouse movement
//
void ColorWidget::updateColorPos(QMouseEvent *event){
  int x = event->pos().x();
  int w = width();
  assert(mouseColorPos>=0 && mouseColorPos<=4);
  double newPos = (double)(x-10)/(double)(w-20);
  if (newPos<0) newPos = 0.0;
  if (newPos>1) newPos = 1.0;
  setColorPos(mouseColorPos,newPos);
  emit updateValueSpins();
  repaint();
}

//make colors lighter if its position is below the lightening threshold
//
QColor ColorWidget::lighten(QColor color, double position){

  if (lightenStart <= position) return color;

  color = color.toHsv();
  int saturation = color.saturation();
  assert(saturation>=0 && saturation<=255);
  saturation -= (int)((double)saturation * (lightenStart-position) / lightenStart);
  assert(saturation>=0 && saturation<=255);
  color.setHsv(color.hue(),saturation,color.value());
  return  color.toRgb();
}

//return a color corresponding to the value parameter 
//having minValue at color position 0.0 
//and maxValue at color position 1.0 on the color scale;
//whiteForZero specifies for topology widgets
//if the value zero should become white as color 
//
QColor ColorWidget::getColor(double value, double minValue, double maxValue, bool whiteForZero){

  //Names and Values of the borders are not consistent.
  assert(minValue<=maxValue);

  //the method is called with absolute values only
  assert(value>=0.0);

  //rounding threshold to compensate float rounding errors
  double threshold = 0.000001;

  //if the value is out of range, return gray
  if (value<minValue-threshold || value>maxValue+threshold) 
    return Qt::gray;

  //normally we return white for zero values, but
  //for topology widgets optionally only if all thread values are zero
  if (value==0.0 && (whiteForZero || (minValue==0.0 && maxValue==0.0))) return Qt::white;

  //compute the color position x of the value on the range between 0.0 for minValue and and 1.0 for maxValue
  double x;
  if (minValue==maxValue)
    if (value==0.0) x = 0.0;
    else x = 1.0;
  else x = (value-minValue)/(maxValue-minValue);
  if (x < 0.0) x = 0.0;
  else if (x > 1.0) x = 1.0;

  //return white if the position x is below the whiteStart threshold
  if (whiteForZero && x<whiteStart) return Qt::white;

  double r, g, b;

  //check to which of the 4 color blocks the value belongs 
  //(1: blue to cyan, 2: cyan to green, 3: green to yellow, 4: yellow to red)
  //and compute the RGB values

  if (inDialog){
    //for the color setting dialog, if the value is out of color range, return gray
    if (x < colorPos[0] || x > colorPos[4]) return Qt::gray;
  } else { 
    //else use only the range between the start and end points
    x = colorPos[0] + x * (colorPos[4] - colorPos[0]);
  }

  //we must take attention to the case that colorPos[i]==colorPos[i+1]
  //for some i!
  //that's why we account for non-empty color intervalls only

  if (x <= colorPos[1] && colorPos[0]<colorPos[1]) {
    //value is between blue and cyan
    r = 0.0;
    g = incr(0.0,colorPos[1],x);
    b = 255.0;
  } else if (colorPos[1] <= x && x <= colorPos[2] && colorPos[1] < colorPos[2]){
    //value is between cyan and green
    r = 0.0;
    g = 255.0;
    b = decr(colorPos[1],colorPos[2],x);
  } else if (colorPos[2] <= x && x <= colorPos[3] && colorPos[2] < colorPos[3]){
    //value between green and yellow
    r = incr(colorPos[2],colorPos[3],x);
    g = 255.0;
    b = 0.0;
  } else {
    //value between yellow and red
    r = 255.0;
    g = decr(colorPos[3],1.0,x);
    b = 0.0;
  } 
  
  QColor color =  QColor((int)r, (int)g, (int)b);
  color = lighten(color,x);

  return color;
}

//below are methods for approximating a function f: [min,max]->[0,255] at value;
//xxxIncr assigns 0 to min, 255 to max, and interpolates inbetween according to xxx;
//xxxDecr assigns 255 to min, 0 to max, and interpolates inbetween according to xxx

double ColorWidget::linIncr(double min, double max, double value){
  if (min==max) return 255.0;
  else return 255.0 * (value-min)/(max-min);
}
double ColorWidget::linDecr(double min, double max, double value){
  return linIncr(min,max,max-(value-min));
}

double ColorWidget::quadratic1Incr(double min, double max, double value){
  if (min==max) return 255.0;
  else {
    double x = (value-min)/(max-min);
    return 255.0 * x * x;
  }
}
double ColorWidget::quadratic1Decr(double min, double max, double value){
  return quadratic1Incr(min,max,max-(value-min));
}

double ColorWidget::quadratic2Incr(double min, double max, double value){
  if (min==max) return 255.0;
  else {
    double x = 1 - (value-min)/(max-min);
    return 255.0 * (1.0 - x * x);
  }
}
double ColorWidget::quadratic2Decr(double min, double max, double value){
  return quadratic2Incr(min,max,max-(value-min));
}

double ColorWidget::exp1Incr(double min, double max, double value){
  if (min==max) return 255.0;
  else {
    double x = (value-min)/(max-min);
    return 255.0 * (exp(x)-1.0)/(exp(1.0)-1.0);
  }
}
double ColorWidget::exp1Decr(double min, double max, double value){
  return exp1Incr(min,max,max-(value-min));
}

double ColorWidget::exp2Incr(double min, double max, double value){
  if (min==max) return 255.0;
  else {
    double x = (value-min)/(max-min);
    x = 1.0 + x * (exp(1.0)-1.0);
    return 255.0 * log(x);
  }
}
double ColorWidget::exp2Decr(double min, double max, double value){
  return exp2Incr(min,max,max-(value-min));
}

double ColorWidget::incr(double min, double max, double value){
  if (coloringMethod == LINEAR) return linIncr(min,max,value);
  else if (coloringMethod == QUADRATIC1) return quadratic1Incr(min,max,value);
  else if (coloringMethod == QUADRATIC2) return quadratic2Incr(min,max,value);
  else if (coloringMethod == EXPONENTIAL1) return exp1Incr(min,max,value);
  else return exp2Incr(min,max,value);
}

double ColorWidget::decr(double min, double max, double value){
  if (coloringMethod == LINEAR) return linDecr(min,max,value);
  else if (coloringMethod == QUADRATIC1) return quadratic1Decr(min,max,value);
  else if (coloringMethod == QUADRATIC2) return quadratic2Decr(min,max,value);
  else if (coloringMethod == EXPONENTIAL1) return exp1Decr(min,max,value);
  return exp2Decr(min,max,value);
}







//below there follow method definitions for the ColorDialog class;
//they are used by ColorWidget only to allow the user color settings

ColorDialog::ColorDialog(QWidget * parent, ColorWidget* _colorWidget) : QDialog(parent) {

  setWindowTitle("Color settings");
  colorWidget = _colorWidget;
  colorWidget->setInDialog(true);
  connect(colorWidget,SIGNAL(updateValueSpins()),this,SLOT(onUpdateValueSpins()));

  QVBoxLayout* layout = new QVBoxLayout();
  setLayout(layout);

  colorWidget->setMinimumSize(400,100);
  layout->addWidget(colorWidget);

  QHBoxLayout* spinLayout = new QHBoxLayout();

  //spin for colorPos[0]
  spin[0] = new QDoubleSpinBox();
  spin[0]->setRange(0.0,1.0);
  spin[0]->setSingleStep(0.01);
  assert(colorWidget->getColorPos(0)<=1.0);
  spin[0]->setValue(colorWidget->getColorPos(0));
  connect(spin[0],SIGNAL(valueChanged(double)),this,SLOT(setColorPos0(double)));

  QLabel * spin0Label = new QLabel(this);
  spin0Label->setText(tr("Start at"));

  QVBoxLayout * spin0Layout = new QVBoxLayout;
  spin0Layout->addWidget(spin0Label);
  spin0Layout->addWidget(spin[0]);
  spinLayout->addLayout(spin0Layout);

  //spin for colorPos[1]
  spin[1] = new QDoubleSpinBox(this);
  spin[1]->setRange(0.0,1.0);
  spin[1]->setSingleStep(0.01);
  spin[1]->setValue(colorWidget->getColorPos(1));
  connect(spin[1],SIGNAL(valueChanged(double)),this,SLOT(setColorPos1(double)));

  QLabel * spin1Label = new QLabel(this);
  spin1Label->setText(tr("Cyan at"));

  QVBoxLayout * spin1Layout = new QVBoxLayout;
  spin1Layout->addWidget(spin1Label);
  spin1Layout->addWidget(spin[1]);
  spinLayout->addLayout(spin1Layout);

  //spin for colorPos[2]
  spin[2] = new QDoubleSpinBox(this);
  spin[2]->setRange(0.0,1.0);
  spin[2]->setSingleStep(0.01);
  spin[2]->setValue(colorWidget->getColorPos(2));
  connect(spin[2],SIGNAL(valueChanged(double)),this,SLOT(setColorPos2(double)));

  QLabel * spin2Label = new QLabel(this);
  spin2Label->setText(tr("Green at"));

  QVBoxLayout * spin2Layout = new QVBoxLayout;
  spin2Layout->addWidget(spin2Label);
  spin2Layout->addWidget(spin[2]);
  spinLayout->addLayout(spin2Layout);

  //spin for colorPos[3]
  spin[3] = new QDoubleSpinBox(this);
  spin[3]->setRange(0.0,1.0);
  spin[3]->setSingleStep(0.01);
  spin[3]->setValue(colorWidget->getColorPos(3));
  connect(spin[3],SIGNAL(valueChanged(double)),this,SLOT(setColorPos3(double)));

  QLabel * spin3Label = new QLabel(this);
  spin3Label->setText(tr("Yellow at"));

  QVBoxLayout * spin3Layout = new QVBoxLayout;
  spin3Layout->addWidget(spin3Label);
  spin3Layout->addWidget(spin[3]);
  spinLayout->addLayout(spin3Layout);

  //spin for colorPos[5]
  spin[4] = new QDoubleSpinBox(this);
  spin[4]->setRange(0.0,1.0);
  spin[4]->setSingleStep(0.01);
  assert(colorWidget->getColorPos(4)>=0.0);
  spin[4]->setValue(colorWidget->getColorPos(4));
  connect(spin[4],SIGNAL(valueChanged(double)),this,SLOT(setColorPos4(double)));

  QLabel * spin4Label = new QLabel(this);
  spin4Label->setText("End at");

  QVBoxLayout * spin4Layout = new QVBoxLayout;
  spin4Layout->addWidget(spin4Label);
  spin4Layout->addWidget(spin[4]);
  spinLayout->addLayout(spin4Layout);


  layout->addLayout(spinLayout);

  //scaling possibilities
  QGroupBox *groupBox = new QGroupBox(tr("Coloring method"),this);

  QRadioButton* button1 = new QRadioButton(tr("Linear"));
  QRadioButton* button2 = new QRadioButton(tr("Quadratic 1"));
  QRadioButton* button3 = new QRadioButton(tr("Quadratic 2"));
  QRadioButton* button4 = new QRadioButton(tr("Exponential 1"));
  QRadioButton* button5 = new QRadioButton(tr("Exponential 2"));

  if (colorWidget->getColoringMethod()==LINEAR) button1->setChecked(true);
  else if (colorWidget->getColoringMethod()==QUADRATIC1) button2->setChecked(true);
  else if (colorWidget->getColoringMethod()==QUADRATIC2) button3->setChecked(true);
  else if (colorWidget->getColoringMethod()==EXPONENTIAL1) button4->setChecked(true);
  else button5->setChecked(true);

  QVBoxLayout *vbox = new QVBoxLayout();
  vbox->addWidget(button1);
  vbox->addWidget(button2);
  vbox->addWidget(button3);
  vbox->addWidget(button4);
  vbox->addWidget(button5);
  vbox->addStretch(1);
  groupBox->setLayout(vbox);
  
  layout->addWidget(groupBox);

  connect(button1,SIGNAL(toggled(bool)),this,SLOT(onLinear(bool)));
  connect(button2,SIGNAL(toggled(bool)),this,SLOT(onQuadratic1(bool)));
  connect(button3,SIGNAL(toggled(bool)),this,SLOT(onQuadratic2(bool)));
  connect(button4,SIGNAL(toggled(bool)),this,SLOT(onExponential1(bool)));
  connect(button5,SIGNAL(toggled(bool)),this,SLOT(onExponential2(bool)));


  QHBoxLayout* lightenLayout = new QHBoxLayout();
  QLabel * lightenStartLabel = new QLabel(this);
  lightenStartLabel->setText("Lighten colors for values under\nthis percentage of the maximal value:");
  lightenLayout->addWidget(lightenStartLabel);

  QDoubleSpinBox * lightenStartSpin = new QDoubleSpinBox(this);
  lightenStartSpin->setRange(0.0,100.0);
  lightenStartSpin->setSingleStep(0.1);
  lightenStartSpin->setValue(colorWidget->getLightenStart()*100.0);
  connect(lightenStartSpin,SIGNAL(valueChanged(double)),this,SLOT(setLightenStart(double)));
  lightenLayout->addWidget(lightenStartSpin);

  layout->addLayout(lightenLayout);

  QHBoxLayout* whiteLayout = new QHBoxLayout();
  QLabel * whiteStartLabel = new QLabel(this);
  whiteStartLabel->setText("Use white to color values under\nthis percentage in the value range:");
  whiteLayout->addWidget(whiteStartLabel);

  QDoubleSpinBox * whiteStartSpin = new QDoubleSpinBox(this);
  whiteStartSpin->setRange(0.0,100.0);
  whiteStartSpin->setSingleStep(0.1);
  whiteStartSpin->setValue(colorWidget->getWhiteStart()*100.0);
  connect(whiteStartSpin,SIGNAL(valueChanged(double)),this,SLOT(setWhiteStart(double)));
  whiteLayout->addWidget(whiteStartSpin);

  layout->addLayout(whiteLayout);

  QDialogButtonBox * buttonBox = new QDialogButtonBox();
  buttonBox->addButton(QDialogButtonBox::Ok);
  QPushButton* applyButton = buttonBox->addButton(QDialogButtonBox::Apply);
  buttonBox->addButton(QDialogButtonBox::Cancel);

  connect(buttonBox,  SIGNAL(accepted()), this, SLOT(accept()));
  connect(applyButton,SIGNAL(clicked()),  this, SLOT(onApply()));
  connect(buttonBox,  SIGNAL(rejected()), this, SLOT(reject()));
  layout->addWidget(buttonBox);

  setWhatsThis("This is a dialog where diverse color settings can be changed. \"Ok\" applies the settings to the display and closes the dialog, \"Apply\" applies the settings to the display, and \"Cancel\" cancels all changes since the dialog was opened and restores the state before (even if \"Apply\" was pressed in between).\n\nAt the top of the dialog you see a color legend with some vertical black lines, showing the position of the color scale start, the colors cyan, green, and yellow, and the color scale end. These lines can be dragged with the left mouse button, or their position can also be changed by typing in some values between 0.0 (left end) and 1.0 (right end) below the color legend in the corresponding spins.\n\nThe different coloring methods offer different functions to interpolate the colors at positions between the above 5 data points.\n\nWith the upper spin below the coloring methods you can define a threshold percentage value between 0.0 and 100.0, below which colors are lightened. The nearer to the left end of the color scale the stronger the lightening (with linear increase).\n\nWith the spin at the bottom of the dialog you can define a threshold percentage value between 0.0 and 100.0, below which values should be colored white.");
}

void ColorDialog::onApply(){
  emit apply(colorWidget);
}

void ColorDialog::onLinear(bool activ){
  if (activ) colorWidget->setColoringMethod(LINEAR);
  repaint();
}
void ColorDialog::onQuadratic1(bool activ){
  if (activ) colorWidget->setColoringMethod(QUADRATIC1);
  repaint();
}
void ColorDialog::onQuadratic2(bool activ){
  if (activ) colorWidget->setColoringMethod(QUADRATIC2);
  repaint();
}
void ColorDialog::onExponential1(bool activ){
  if (activ) colorWidget->setColoringMethod(EXPONENTIAL1);
  repaint();
}
void ColorDialog::onExponential2(bool activ){
  if (activ) colorWidget->setColoringMethod(EXPONENTIAL2);
  repaint();
}

void ColorDialog::setLightenStart(double value){ colorWidget->setLightenStart(value/100.0); repaint();}
void ColorDialog::setWhiteStart(double value){ colorWidget->setWhiteStart(value/100.0); repaint();}

void ColorDialog::setColorPos0(double value){ setColorPos(0,value);}
void ColorDialog::setColorPos1(double value){ setColorPos(1,value);}
void ColorDialog::setColorPos2(double value){ setColorPos(2,value);}
void ColorDialog::setColorPos3(double value){ setColorPos(3,value);}
void ColorDialog::setColorPos4(double value){ setColorPos(4,value);}

void ColorDialog::setColorPos(unsigned index, double value){
  colorWidget->setColorPos(index,value);
  onUpdateValueSpins();
  repaint();
}

void ColorDialog::onUpdateValueSpins(){
  for (int i=0; i<5; i++)
    spin[i]->setValue(colorWidget->getColorPos(i));
}

