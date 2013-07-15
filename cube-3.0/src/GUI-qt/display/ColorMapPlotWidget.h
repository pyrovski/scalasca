/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _COLORMAPPLOTWIDGET_H
#define _COLORMAPPLOTWIDGET_H

#include <QWidget>
#include <QPair>
#include <QPoint>
#include <QMouseEvent>
#include <QVector>




// The definition of the data type accepted by this widget for plotting the data.
typedef QPair<double, double> Tuple;
typedef QPair< Tuple, double> Triple;  // first pair -> x,y coords, last -> z value

typedef QVector< Triple> Row; 
typedef QVector< QString> LabelsRow; 

typedef QPair<QString, Row>  LabeledRow;
typedef QPair<  LabelsRow,  QVector<LabeledRow> > LabeledMatrix; 


class MyColorMapPlotWidget : public QWidget {

Q_OBJECT

 protected slots:

  
 // Refreshing parameters by resizing
  void resizeEvent(QResizeEvent *);

 public:

  MyColorMapPlotWidget(QWidget* parent);
  ~MyColorMapPlotWidget() { if (data == NULL) delete data;}

  /********************** set methods ************************/


  //set the tab widget the tree is inside of
  void setData(LabeledMatrix *);

  /********** resize policy *******************/
   virtual QSize sizeHint() const { return QSize(850, 650); }
   QSize minimumSize() { return sizeHint(); }
   virtual QSize minimumSizeHint() const { return sizeHint(); }


  QSizePolicy sizePolicy() 
		{ return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); }
 protected:

  /************** protected fields *****************/

  // Data to be plotted.
  LabeledMatrix * data;
  double minX, maxX, minY, maxY, minZ, maxZ;

   // spacing for labels 
  unsigned int xlabelheight, ylabelwidth;

  // Axes title
  QString xtitle, ytitle;

  // Title 
  QString title;


  //the main painting method
  void paintEvent(QPaintEvent*);


  //mouse event handling
//  void mousePressEvent(QMouseEvent *event);

  //key event handling
//   void keyPressEvent(QKeyEvent *event);


  //mouse event handling
  virtual void mouseMoveEvent(QMouseEvent *event) 
	{ MouseOver.rx() = event->x(); MouseOver.ry() = event->y(); update(); }

// general event handler
// used for tooltip handling
//    bool event(QEvent * event);

//For handling tooltips position of the mouse pointer will be tracked and saved.
//Used in  ::event(...) to define treeitem under mouse pointer
  QPoint MouseOver;

};

#endif
