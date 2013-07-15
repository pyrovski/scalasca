/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "ColorMapPlotWidget.h"


#include <iostream>
#include <cassert>

#include <QMouseEvent>
#include <QPainter>
#include <QRect>
#include <QScrollBar>
#include <QFontMetrics>
#include <QMessageBox>
#include <QRegExp>
#include <QToolTip>
#include <QVector>
MyColorMapPlotWidget::MyColorMapPlotWidget(QWidget* parent) : QWidget(parent)
{
    data = NULL;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(sizeHint());

    // dummy default values.
    minX = 0.; maxX = 1.; 
    minY = 0.; maxY = 1.;
    minZ = 0.; maxZ = 1.;
    setMouseTracking ( true );
}


//  slots
void MyColorMapPlotWidget::resizeEvent(QResizeEvent * event) 
{
    xlabelheight=(event->size().height()*0.1 < 130)? 
        130:(unsigned int)(event->size().height()*0.1);
    ylabelwidth=(event->size().width()*0.1 < 80)? 
        80:(unsigned int)(event->size().width()*0.1);
}


/******************* set methods *******************/

//set the tabWidget the tree is contained in
//
void  MyColorMapPlotWidget::setData(LabeledMatrix * newdata){

  if (newdata== NULL)  return;
  if (data != NULL) 
    delete data; 
  data = newdata;

    if (data->second.size()>0)
    { 
        minX = 9.9999e+99;
        minY = 9.9999e+99;
        minZ = 9.9999e+99;
        maxX = -9.9999e+99;
        maxY = -9.9999e+99;
        maxZ = -9.9999e+99;
        LabeledRow row;
        // for scaling of the coloring get the min and max values 
        foreach ( row, (*data).second)
        {
            Triple triple;
            foreach ( triple, row.second)
            {
                minX = (minX >= triple.first.first)? triple.first.first : minX; 
                minY = (minY >= triple.first.second)? triple.first.second : minY; 
                minZ = (minZ >= triple.second)? triple.second : minZ;
    
                maxX = (maxX <= triple.first.first)? triple.first.first : maxX; 
                maxY = (maxY <= triple.first.second)? triple.first.second : maxY; 
                maxZ = (maxZ <= triple.second)? triple.second : maxZ;
            }
        }
        minZ = (maxZ == minZ ) ? -1. : minZ;
        maxZ = (maxZ == minZ ) ? +1. : maxZ;
    }

}

//the painting method
//
void MyColorMapPlotWidget::paintEvent(QPaintEvent*){

  QPainter painter(this);

 // paint the background
  QRect rectangle = rect();
  painter.fillRect(rectangle,Qt::white);

 QBrush mybrush(Qt::SolidPattern);
 QPen mypen(mybrush,2);
 mypen.setColor(Qt::lightGray);
 mypen.setWidth(1);
 painter.setBackgroundMode(Qt::OpaqueMode);
 double marginpercent = 0.93;

// calculation of margins, steps for the drawing of the content
 unsigned xaxeslenght = (unsigned)(width()  *marginpercent) - ylabelwidth;
 unsigned yaxeslenght = (unsigned)(height() *marginpercent) - xlabelheight;
 double stepX = xaxeslenght/(maxX-minX+1);
 double stepY = yaxeslenght/(maxY-minY+1);


 QMatrix ymirror(1,0,0,-1,0,height());
 painter.setWorldMatrix(ymirror);


// axes

 mypen.setColor(Qt::lightGray);
 mypen.setWidth(1);
 painter.setPen(mypen);

 painter.drawLine(
        ylabelwidth, xlabelheight,
        ylabelwidth, xlabelheight + yaxeslenght
        );
painter.drawLine(
        ylabelwidth              , xlabelheight,
        ylabelwidth + xaxeslenght, xlabelheight
        );
 
// paint the data 
    if (data != NULL && data->second.size() >=1)
    {
         if (minX < maxX && minY < maxY && minZ < maxZ)
        {
            // first the color map

            LabeledRow row;
            foreach ( row, (*data).second)
            {
                Triple triple;
                foreach ( triple, row.second)
                {
                    signed x = (signed)(ylabelwidth + (triple.first.first - minX )*stepX  );
                    signed y = (signed)(xlabelheight + (triple.first.second - minY )*stepY);
                    short z = (short)(((maxZ- triple.second )/((maxZ - minZ)*1.05)) * 255);
                    mypen.setColor( QColor(z, z, z, 255) );
                    painter.setBackground( QBrush( QColor(z, z, z, 255),Qt::SolidPattern));
                    painter.setBrush( QBrush( QColor(z, z, z, 255),Qt::SolidPattern));
                    painter.setPen(mypen);
                    painter.drawRect(
                        x+2,y+4,
                        (signed)stepX-1,(signed)stepY-1
                        );
                }
             }
            mypen.setColor( QColor(100, 100, 100, 255) );
            painter.setBackground( QBrush( Qt::NoBrush));
            painter.setBrush( QBrush(Qt::NoBrush));
            painter.setPen(mypen);

            // Labels for x-axis are a slightly rotated
            QString ylabel;
            unsigned i = 0;
            painter.setWorldMatrix(QMatrix(1,0,0,1,0,0), false);
            foreach ( ylabel, (*data).first)
            {
                unsigned x = (unsigned)(ylabelwidth + (i)*stepX );
                painter.translate(x+5, height()- xlabelheight + 10 );
                painter.rotate(60);
                painter.drawText(0, 0,  ylabel);
                 painter.setWorldMatrix(QMatrix(1,0,0,1,0,0), false);
                i++;
            }
            i = 0;
            // Labels for y-axis are a horizontal
            foreach ( row, (*data).second)
            {
                unsigned y = (unsigned)(height() - xlabelheight - (i)*stepY );
                painter.translate(1 , y-(unsigned)(stepY/2.));
                painter.drawText(0, 0,  row.first);
                 painter.setWorldMatrix(QMatrix(1,0,0,1,0,0), false);
                i++;
            }

        }
    }
 
}

