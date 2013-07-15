/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _TYPEDWIDGET_H
#define _TYPEDWIDGET_H

#include <QWidget>

#include "Constants.h"

//TypedWidget allows to assign some types to widgets;
//WidgetType is defined in constants.h

class TypedWidget : public QWidget {

 public:

  TypedWidget();
  TypedWidget(QWidget* widget);
  ~TypedWidget();

  void setType(WidgetType type);
  WidgetType getType();

 private:

  WidgetType type;

};

#endif
