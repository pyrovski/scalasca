/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "StatisticInfo.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPainter>
#include <qdebug.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <assert.h>

StatisticInfo::StatisticInfo(QStringList slist,
                             QWidget* parent, bool tooltip) : QWidget(parent), isTooltip(tooltip)
{
    assert(slist.size()>=3);
    str1 = slist.at(0);
    str2 = slist.at(1);
    str3 = slist.at(2);
    this->parent = parent;
    first = true;
    resize(10,10);
    repaint();
    setMinimumSize(width(),height());
    setWhatsThis(
          "This window shows the statistical values from the boxplot:\n"
          "The lowest 25% of data have values less than Q1.\n"
          "The median separates the higher half of values from the lower half.\n"
          "The highest 25% of data have values more than Q3.");
}

void StatisticInfo::toClipboard() {
   QStringList l1 = str1.split("\n");
   QStringList l2 = str2.split("\n");
   QStringList l3 = str3.split("\n");

   QString txt;
   for (int i=1; i<l1.size() && i<l2.size(); i++) {
      txt += l1.at(i)+"\t"+l2.at(i)+"\t"+l3.at(i)+"\n";
   }
   QApplication::clipboard()->setText(txt);
}

void StatisticInfo::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    QRect rect1 = painter.boundingRect(0,0,0,0,Qt::AlignLeft,str1);
    QRect rect2 = painter.boundingRect(0,0,0,0,Qt::AlignRight,str2);
    QRect rect3 = painter.boundingRect(0,0,0,0,Qt::AlignRight,str3);

    if (isTooltip) {
       painter.fillRect(rect(), Qt::white);
       painter.drawRect(rect().x(), rect().y(), rect().width()-1, rect().height()-1);
    }

    int pad = 10;
    int middle = 20;
    if(first)
    {
        int w = rect1.width() + rect2.width() + rect3.width() + 2*middle;
        if (w<50) w = 50;
        int h = (rect1.height()>rect2.height()
                ? rect1.height() : rect2.height());
        if (h<50) h = 50;
        resize(w+2*pad, h+2*pad);
        setMinimumSize(rect().size());
        setMaximumSize(rect().size());
        first = false;
    }

    rect1.moveTo(pad, pad);
    painter.drawText(rect1, Qt::AlignLeft, str1);
    painter.drawText(rect1.width()+middle, pad,
                     rect2.width(), rect2.height(),
                     Qt::AlignRight, str2);
    painter.drawText(rect1.width()+rect2.width()+2*middle, pad,
                     rect3.width(), rect3.height(),
                     Qt::AlignRight, str3);
}

