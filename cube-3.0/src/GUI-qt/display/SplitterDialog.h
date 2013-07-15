/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _SPLITTERDIALOG_H
#define _SPLITTERDIALOG_H

#include <QDialog>
#include <QSplitter>
#include <QRadioButton>

//a class to get user-input for the order of the metric/call/system tabs

class SplitterDialog : public QDialog {

Q_OBJECT

 public:

  SplitterDialog(QWidget* parent, QSplitter* splitter);

  //slots to activate the clicked orders
  private slots:

    void onButton1(bool checked);
    void onButton2(bool checked);
    void onButton3(bool checked);
    void onButton4(bool checked);
    void onButton5(bool checked);
    void onButton6(bool checked);
    void setWidgets();

 private:

  // buttons representing the different orders
  QRadioButton * button1;
  QRadioButton * button2;
  QRadioButton * button3;
  QRadioButton * button4;
  QRadioButton * button5;
  QRadioButton * button6;

  //the splitter widget for that the order gets defined
  QSplitter* splitter;

  //the three widgets being in the splitter
  QWidget* metricWidget;
  QWidget* callWidget;
  QWidget* systemWidget;

  //current indices of the three widgets above
  unsigned metricIndex,callIndex,systemIndex;

};

#endif
