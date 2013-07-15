/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "SplitterDialog.h"
#include "TypedWidget.h"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QVBoxLayout>

SplitterDialog::SplitterDialog(QWidget* parent, QSplitter* splitter) : QDialog(parent){

  this->splitter = splitter;

  setWindowTitle("Dimension order");
 
  //store the current order
  if (((TypedWidget*)(splitter->widget(0)))->getType()==METRICWIDGET){
    metricIndex = 0;
    metricWidget = splitter->widget(0);
  } else if (((TypedWidget*)(splitter->widget(1)))->getType()==METRICWIDGET){
    metricIndex = 1;
    metricWidget = splitter->widget(1);
  } else {
    metricIndex = 2;
    metricWidget = splitter->widget(2);
  }

  if (((TypedWidget*)(splitter->widget(0)))->getType()==CALLWIDGET){
    callIndex = 0;
    callWidget = splitter->widget(0);
  }else if (((TypedWidget*)(splitter->widget(1)))->getType()==CALLWIDGET){
    callIndex = 1;
    callWidget = splitter->widget(1);
  } else {
    callIndex = 2;
    callWidget = splitter->widget(2);
  }

  if (((TypedWidget*)(splitter->widget(0)))->getType()==SYSTEMWIDGET){
    systemIndex = 0;
    systemWidget = splitter->widget(0);
  } else if (((TypedWidget*)(splitter->widget(1)))->getType()==SYSTEMWIDGET){
    systemIndex = 1;
    systemWidget = splitter->widget(1);
  } else {
    systemIndex = 2;
    systemWidget = splitter->widget(2);
  }

  //add ok and cancel buttons to the dialog
  QDialogButtonBox * buttonBox = new QDialogButtonBox(this);
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(setWidgets()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  //define the buttons for the possible orders
  QGroupBox *groupBox = new QGroupBox(tr("Possible dimension orders"),this);

  button1 = new QRadioButton(tr("Metric \tCall \tSystem"));
  button2 = new QRadioButton(tr("Metric \tSystem \tCall"));
  button3 = new QRadioButton(tr("Call \tMetric \tSystem"));
  button4 = new QRadioButton(tr("Call \tSystem \tMetric"));
  button5 = new QRadioButton(tr("System \tMetric \tCall"));
  button6 = new QRadioButton(tr("System \tCall \tMetric"));

  //check the current order's button
  if (metricIndex==0 && callIndex==1 && systemIndex==2) button1->setChecked(true);
  else if (metricIndex==0 && callIndex==2 && systemIndex==1) button2->setChecked(true);
  else if (metricIndex==1 && callIndex==0 && systemIndex==2) button3->setChecked(true);
  else if (metricIndex==2 && callIndex==0 && systemIndex==1) button4->setChecked(true);
  else if (metricIndex==1 && callIndex==2 && systemIndex==0) button5->setChecked(true);
  else if (metricIndex==2 && callIndex==1 && systemIndex==0) button6->setChecked(true);

  //define the dialog layout
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addWidget(button1);
  vbox->addWidget(button2);
  vbox->addWidget(button3);
  vbox->addWidget(button4);
  vbox->addWidget(button5);
  vbox->addWidget(button6);

  vbox->addStretch(1);
  groupBox->setLayout(vbox);

  //connect the buttons' signals to the corresponding slots
  connect(button1,SIGNAL(toggled(bool)),(SplitterDialog*)this,SLOT(onButton1(bool)));
  connect(button2,SIGNAL(toggled(bool)),this,SLOT(onButton2(bool)));
  connect(button3,SIGNAL(toggled(bool)),this,SLOT(onButton3(bool)));
  connect(button4,SIGNAL(toggled(bool)),this,SLOT(onButton4(bool)));
  connect(button5,SIGNAL(toggled(bool)),this,SLOT(onButton5(bool)));
  connect(button6,SIGNAL(toggled(bool)),this,SLOT(onButton6(bool)));

  QVBoxLayout* layout = new QVBoxLayout();
  layout->addWidget(groupBox);
  layout->addWidget(buttonBox);
  setLayout(layout);
}

//this slot is called when the uses clicks the ok button;
//it sets the chosen order
//
void SplitterDialog::setWidgets(){
  if (metricIndex==0) splitter->insertWidget(0,metricWidget);
  else if (callIndex==0) splitter->insertWidget(0,callWidget);
  else splitter->insertWidget(0,systemWidget);

  if (metricIndex==1) splitter->insertWidget(1,metricWidget);
  else if (callIndex==1) splitter->insertWidget(1,callWidget);
  else splitter->insertWidget(1,systemWidget);

  if (metricIndex==2) splitter->insertWidget(2,metricWidget);
  else if (callIndex==2) splitter->insertWidget(2,callWidget);
  else splitter->insertWidget(2,systemWidget);

  accept();
}

//slots connected to the buttons;
//thus locally store the chosen order;
//do not apply yet

void SplitterDialog::onButton1(bool checked){
  if (checked){
    metricIndex = 0;
    callIndex = 1;
    systemIndex = 2;
  }
}

void SplitterDialog::onButton2(bool checked){
  if (checked){
    metricIndex = 0;
    callIndex = 2;
    systemIndex = 1;
  }
}

void SplitterDialog::onButton3(bool checked){
  if (checked){
    metricIndex = 1;
    callIndex = 0;
    systemIndex = 2;
  }
}

void SplitterDialog::onButton4(bool checked){
  if (checked){
    metricIndex = 2;
    callIndex = 0;
    systemIndex = 1;
  }
}

void SplitterDialog::onButton5(bool checked){  
  if (checked){
    metricIndex = 1;
    callIndex = 2;
    systemIndex = 0;
  }
}

void SplitterDialog::onButton6(bool checked){  
  if (checked){
    metricIndex = 2;
    callIndex = 1;
    systemIndex = 0;
  }
}

