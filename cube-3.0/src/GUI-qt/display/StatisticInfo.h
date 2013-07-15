/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef _STATISTCINFO_H
#define _STATISTCINFO_H

#include <QWidget>
#include <QStringList>

class StatisticInfo : public QWidget
{
Q_OBJECT
 public:
  StatisticInfo(QStringList slist, QWidget* parent, bool tooltip = false);
 public slots:
  void toClipboard();

 protected:
  void paintEvent(QPaintEvent*);
 private:
  QString str1, str2 ,str3;
  QWidget* parent;
  bool first;
  bool isTooltip;
};

#endif

