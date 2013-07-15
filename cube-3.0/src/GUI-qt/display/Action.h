/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _ACTION_H
#define _ACTION_H

#include <QAction>

//The Action class extends QAction with a few convenient functions

class Action : public QAction{

  Q_OBJECT

public:

  Action( const QString & text, QObject * parent );
  Action( const QIcon & icon, const QString & text, QObject * parent );
  ~Action();

public slots:

  //enabling and disabling the action leads to enabling/disabling
  //the corresponding menu elements, too
  void enable();
  void disable();

  //for check boxes
  void check();
  void uncheck();
  void setCheckedInvert(bool b);

};

#endif
