/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef _FONTWIDGET_H
#define _FONTWIDGET_H

#include <QDialog>
#include <QFontComboBox>
#include <QSpinBox>

//FontWidget is used to allow the user to define a font, font size, and
//line spacing for tree widgets

class FontWidget : public QDialog {
  
  Q_OBJECT
    
 signals:

  void apply(FontWidget* fontWidget);

 private slots:

  void onOk();
  void onApply();
  void onCancel();

 public:
  
  FontWidget(QWidget* parent, QFont font, int spacing);

  QFont getFont();
  int getSpacing();

 private:
  
  QFontComboBox* fontCombo;
  QSpinBox * sizeSpin;
  QSpinBox * spaceSpin;
   
  //to store the values upon creation,
  //that can be reset if the user presses "Cancel"
  QFont font;
  int spacing;
};

#endif
