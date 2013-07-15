/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _2DPLOTWIDGET_H
#define _2DPLOTWIDGET_H

#include <QWidget>
#include <QPair>
#include <QPoint>
#include <QMouseEvent>
#include <QVector>


// definition of the data type plotted by this widget.
typedef QPair<double, double> Tuple;
typedef QPair<QString, Tuple> LabeledTuple;
typedef QVector<LabeledTuple> DataFor2DPlot;



class My2DPlotWidget : public QWidget {

Q_OBJECT

 protected slots:

  
 // Refreshing parameters by resizing
  void resizeEvent(QResizeEvent *);

 public:

  My2DPlotWidget(QWidget* parent);
  ~My2DPlotWidget() { if (data == NULL) delete data;}

  /********************** set methods ************************/


  //set the data, to be plotted
  void setData(DataFor2DPlot *);

  /********** resize policy *******************/
   virtual QSize sizeHint() const { return QSize(850, 650); }
   QSize minimumSize() { return sizeHint(); }
   virtual QSize minimumSizeHint() const  { return sizeHint(); }


  QSizePolicy sizePolicy() { return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); }
 protected:

  /************** protected fields *****************/

  // Data to be plotted.
  DataFor2DPlot * data;
  double minX, maxX, minY, maxY;

   // spacing for labels 
  int xlabelheight, ylabelwidth;

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


//  //mouse event handling
  virtual void mouseMoveEvent(QMouseEvent *event) { MouseOver.rx() = event->x(); MouseOver.ry() = event->y(); update(); }

// general event handler
// used for tooltip handling
//   bool event(QEvent * event);

//For handling tooltips position of the mouse pointer will be tracked and saved.
//Used in  ::event(...) to define treeitem under mouse pointer
  QPoint MouseOver;

};

#endif
