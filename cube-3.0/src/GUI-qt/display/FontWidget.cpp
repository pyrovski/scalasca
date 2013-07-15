/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "FontWidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

FontWidget::FontWidget(QWidget* parent, QFont font, int spacing) : QDialog(parent){

  setWindowTitle("Font settings");

  this->font = font;
  this->spacing = spacing;

  QVBoxLayout * layout = new QVBoxLayout();
  setLayout(layout);

  QLabel * fontLabel = new QLabel(this);
  fontLabel->setText("Font: ");
  layout->addWidget(fontLabel);

  fontCombo = new QFontComboBox(this);
  layout->addWidget(fontCombo);
  fontCombo->setCurrentFont(font);

  QLabel * sizeLabel = new QLabel(this);
  sizeLabel->setText("Size [pt]: ");
  layout->addWidget(sizeLabel);

  sizeSpin = new QSpinBox(this);
  sizeSpin->setRange(1,30);
  sizeSpin->setSingleStep(1);
  sizeSpin->setValue(font.pointSize());
  layout->addWidget(sizeSpin);

  QLabel * spaceLabel = new QLabel(this);
  spaceLabel->setText("Line spacing [pixel]: ");
  layout->addWidget(spaceLabel);

  spaceSpin = new QSpinBox(this);
  spaceSpin->setRange(0,30);
  spaceSpin->setSingleStep(1);
  spaceSpin->setValue(spacing);
  layout->addWidget(spaceSpin);

  QDialogButtonBox * buttonBox = new QDialogButtonBox();
  buttonBox->addButton(QDialogButtonBox::Ok);
  QPushButton* applyButton = buttonBox->addButton(QDialogButtonBox::Apply);
  buttonBox->addButton(QDialogButtonBox::Cancel);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(onOk()));
  connect(applyButton,SIGNAL(clicked()),  this, SLOT(onApply()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(onCancel()));

  layout->addWidget(buttonBox);

  setWhatsThis("Opens a dialog to specify the font, the font size (in pt), and the line spacing for the tree displays.  The \"Ok\" button applies the settings to the display and closes the dialog, the \"Apply\" button applies the settings to the display, and \"Cancel\" cancels all changes since the dialog was opened (even if \"Apply\" was pressed in between) and closes the dialog.");
}

QFont FontWidget::getFont(){
  QFont font = fontCombo->currentFont();
  font.setPointSize(sizeSpin->value());
  return font;
}

int FontWidget::getSpacing(){
  return spaceSpin->value();
}

void FontWidget::onOk(){
  onApply();
  accept();
}

void FontWidget::onApply(){
  emit apply(this);
}

void FontWidget::onCancel(){
  fontCombo->setCurrentFont(font);
  sizeSpin->setValue(font.pointSize());
  spaceSpin->setValue(spacing);
  onApply();
  reject();
}
