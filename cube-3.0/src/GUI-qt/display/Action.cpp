/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "Action.h"

Action::Action( const QString & text, QObject * parent ) : QAction(text, parent) {}

Action::Action( const QIcon & icon, const QString & text, QObject * parent ) : QAction(icon, text, parent) {}

Action::~Action(){}

//enabling the action leads to enabling
//the corresponding menu elements, too
void Action::enable(){
  setEnabled(true);
}

//disabling the action leads to disabling
//the corresponding menu elements, too
void Action::disable(){
  setEnabled(false);
}

//if the action has a check box: set checked
void Action::check(){
  setChecked(true);
}

//if the action has a check box: set unchecked
void Action::uncheck(){
  setChecked(false);
}

//if the action has a check box: set to !b
void Action::setCheckedInvert(bool b){
  setChecked(!b);
}

