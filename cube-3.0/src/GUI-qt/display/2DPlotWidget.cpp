/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "2DPlotWidget.h"


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

My2DPlotWidget::My2DPlotWidget(QWidget* parent) : QWidget(parent)
{
    data = NULL;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(sizeHint());
    
    // default values 
    minX = 0.; maxX = 1.; 
    minY = 0.; maxY = 1.;
    setMouseTracking ( true );
}


//  slots
void My2DPlotWidget::resizeEvent(QResizeEvent * event) 
{
    xlabelheight=(event->size().height()*0.1 < 110)? 
        110:(unsigned int)(event->size().height()*0.1);
    ylabelwidth=(event->size().width()*0.1 < 60)? 
        60:(unsigned int)(event->size().width()*0.1);
}


/******************* set methods *******************/

void  My2DPlotWidget::setData(DataFor2DPlot * newdata){

  if (newdata== NULL)  return;
  if (data != NULL) 
    delete data; 
  data = newdata;

    if (data->size()>0)
    { 
        minX = 9.9999e+99;
        minY = 9.9999e+99;
        maxX = -9.9999e+99;
        maxY = -9.9999e+99;
        // for scaling of the coloring get the min and max values 
        LabeledTuple pair;
        foreach ( pair, *data)
        {
            minX = (minX >= pair.second.first)? pair.second.first : minX; 
            minY = (minY >= pair.second.second)? pair.second.second : minY; 
            maxX = (maxX <= pair.second.first)? pair.second.first : maxX; 
            maxY = (maxY <= pair.second.second)? pair.second.second : maxY; 
        }
        minY = (maxY == minY ) ? -1. : minY - (maxY - minY)*0.1;
        maxY = (maxY == minY ) ? +1. : maxY + (maxY - minY)*0.1;


    }

}

//the  painting method
//
void My2DPlotWidget::paintEvent(QPaintEvent*){

 QPainter painter(this);
 // paint the background 
 QRect rectangle = rect();
 painter.fillRect(rectangle,Qt::white);

 QBrush mybrush(Qt::SolidPattern);
 QPen mypen(mybrush,2);
 mypen.setColor(Qt::lightGray);
 mypen.setWidth(1);

 double marginpercent = 0.93;


 // calculate margins, steps for the painting
 signed xaxeslenght = (signed)(width()  *marginpercent) - ylabelwidth;
 signed yaxeslenght = (signed)(height() *marginpercent) - xlabelheight;
 signed xgridstep = (xaxeslenght < 100)?20:50;
 signed ygridstep = (yaxeslenght < 100)?20:50;
 double stepX = xaxeslenght/(maxX-minX);
 double stepY = yaxeslenght/(maxY-minY);


 QMatrix ymirror(1,0,0,-1,0,height());
 painter.setWorldMatrix(ymirror);

// precise position of mouse 
// draw the cross-lines to easy read of the values
if (MouseOver.x() > ylabelwidth && MouseOver.x()< (width()  *marginpercent) )
{
 painter.drawLine(
        MouseOver.x(), xlabelheight ,
        MouseOver.x(),  (signed int)(height() *marginpercent)
        );
}

if (MouseOver.y() > (height()  *(1.-marginpercent))  &&  
        MouseOver.y() < height() -  xlabelheight)
{
painter.drawLine(
        ylabelwidth , height() -  MouseOver.y(), 
        (signed int)(width()  *marginpercent)  , height() - MouseOver.y()
        );

// paint the actual y-value close to the mouse cursor
 mypen.setColor(Qt::green);
 mypen.setWidth(3);
 painter.setPen(mypen);
painter.setWorldMatrix(ymirror, true);

  painter.drawText(
            MouseOver.x() +5 , MouseOver.y() - 5,  
        QString::number( 
            ( (maxY-minY)/ yaxeslenght * (height() - MouseOver.y()- xlabelheight ) +  minY ),
                 'E', 9)
    );

painter.setWorldMatrix(ymirror, true);
}


// axes

 mypen.setColor(QColor(10,10,10,20));
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
 

// grid definion
 mypen.setColor(QColor(10,10,10,20));
 mypen.setStyle(Qt::CustomDashLine);
 QVector<qreal> dashes;
 qreal space = 10;

 dashes << 10 << space << 5 << space << 10 << space
            << 1 << space;

 mypen.setDashPattern(dashes);
 mypen.setWidth(1);
 painter.setPen(mypen);

 // create grid
 int i = 0;
 int xgridlineposition = i*xgridstep;
 while (xgridlineposition <= xaxeslenght)
 {

  painter.drawLine(
        xgridlineposition + ylabelwidth ,xlabelheight,
        xgridlineposition + ylabelwidth, xlabelheight+ yaxeslenght
        );
  i++; 
  xgridlineposition = i*xgridstep;
 }
 i =0;
 int ygridlineposition = i*ygridstep;
 while (ygridlineposition <= yaxeslenght)
 {

  painter.drawLine(
        ylabelwidth,            xlabelheight + ygridlineposition,
        ylabelwidth + xaxeslenght,    xlabelheight + ygridlineposition
        );
  i++;
 ygridlineposition = i*ygridstep;
 }




 mypen.setColor(QColor(255,0,0,20));
 mypen.setStyle(Qt::SolidLine);
 mypen.setWidth(2);
 painter.setPen(mypen);

    if (data != NULL && data->size() >=1)
    {

        if (minX < maxX && minY < maxY)
        {

            // paint the data as linie
            for (int i=1; i< data->size(); i++)
            {
            signed x1 = (signed)(ylabelwidth + ((data->at(i-1)).second.first - minX )*stepX);
            signed y1 = (signed)(xlabelheight + ((data->at(i-1)).second.second - minY )*stepY  );

            signed x2 = (signed)(ylabelwidth + ((data->at(i)).second.first - minX )*stepX  );
            signed y2 = (signed)(xlabelheight + ((data->at(i)).second.second - minY )*stepY  );

            // coloring depends on mouse position
            // if the mouse over the actual value - it will be painted opaque, otherwise - with a little alpha-blending
            if (x1 < MouseOver.x()  && x2 > MouseOver.x() )
            {
             mypen.setColor(QColor(255,0,0,255));
            }
            else
            {
             mypen.setColor(QColor(255,0,0,50));
            }
             painter.setPen(mypen);    

            painter.drawLine(
                x1 , y1 , x2, y2
                );
            }
            mypen.setColor(Qt::blue);
             painter.setPen(mypen);
            // paint the vaues abouve the line
            LabeledTuple pair;
            foreach ( pair, *data)
            {

            unsigned x = (unsigned)(ylabelwidth + (pair.second.first - minX )*stepX  - 3);
            unsigned y = (unsigned)(xlabelheight + (pair.second.second - minY )*stepY  - 3);

            if (x-stepX < MouseOver.x()  && x+stepX > MouseOver.x() )
            {
             mypen.setColor(QColor(0,0,255,255));
            }
            else
            {
             mypen.setColor(QColor(0,0,255,50));
            }
             painter.setPen(mypen);    

            painter.drawEllipse(
                x,y,
                6,6
                );
            painter.setWorldMatrix(ymirror, true);
            painter.translate(x+10, height() - y - 5  );
            painter.rotate(-45);
            
            painter.drawText(
                0,0, QString::number(pair.second.second, 'g', 4)
                );
            painter.setWorldMatrix(QMatrix(1,0,0,1,0,0), false);
            painter.setWorldMatrix(ymirror, true);


            }

        }
    }

 mypen.setColor(QColor(150,150,150,255));
 painter.setPen(mypen);
 mypen.setWidth(1);
 painter.setWorldMatrix(ymirror*ymirror);
 painter.setWorldMatrix(QMatrix(1,0,0,1,-55,5), true);



 // vaues on y-axis
 i =0;
 ygridlineposition = i*ygridstep;
 while (ygridlineposition <= yaxeslenght)
 {

  painter.drawText(
        ylabelwidth,    height() - xlabelheight - ygridlineposition,
        QString::number( minY + i*(maxY - minY)*ygridstep/yaxeslenght, 'E', 2)
        );
  i++;
 ygridlineposition = i*ygridstep;
 }

 painter.setWorldMatrix(QMatrix(1,0,0,1,0,0), false);


if (data != NULL && data->size() >=1)
    {

        if (minX < maxX && minY < maxY)
        {
            // labels un x-axis
            LabeledTuple pair;
            foreach ( pair, *data)
            {

            unsigned x = (unsigned)(ylabelwidth + (pair.second.first - minX )*stepX );

            if (x-stepX < MouseOver.x()  && x+stepX > MouseOver.x() )
            {
            mypen.setColor(QColor(0,0,0,255));
            }
            else
            {
            mypen.setColor(QColor(150,150,150,150));
            }
             painter.setPen(mypen);    

             painter.translate(x, height() - xlabelheight  );
             painter.rotate(60);
            painter.drawEllipse(-2,-2, 4,4);
            painter.drawText(0,10,    pair.first);
             painter.setWorldMatrix(QMatrix(1,0,0,1,0,0), false);
            }
        }
    }
 painter.setWorldMatrix(QMatrix(1,0,0,1,0,0), false);
}

