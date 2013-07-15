/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "TypedWidget.h"

TypedWidget::TypedWidget() : QWidget(){}

TypedWidget::TypedWidget(QWidget* widget) : QWidget(widget){}

TypedWidget::~TypedWidget() {}


void TypedWidget::setType(WidgetType type){
  this->type = type;
}

WidgetType TypedWidget::getType(){
  return type;
}

