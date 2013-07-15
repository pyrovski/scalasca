/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _MESSAGEWIDGET_H
#define _MESSAGEWIDGET_H

#include <QWidget>
//MessageWidget shows a warning message for N seconds and dissapears. 
// It dissapears,if user clicks on ot.


// Different kind of hints are possible: hint, warning, Error.
enum HintKind { Hint= 0, Warning=1, Error=2, Message=3} ;


class MessageWidget : public QWidget {

// Q_OBJECT

//  protected slots:

 public:

  MessageWidget(HintKind, QString);

 protected:



  /************** protected fields *****************/
  QString warning_message;

  HintKind mykind;

  //mouse event handling
  void mousePressEvent(QMouseEvent *);
     

};

#endif
