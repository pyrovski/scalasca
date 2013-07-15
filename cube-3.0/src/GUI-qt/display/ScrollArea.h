/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _SCROLLAREA_H
#define _SCROLLAREA_H

#include <QWidget>

class Container
{
public:
    void*
    component()
    {
        return component_;
    }
    void
    setComponent( void* w )
    {
        component_ = w;
    }
protected:
    void* component_;
};

#include <QSplitter>
 #include <QMotifStyle>
class SplitterContainer : public QSplitter, public Container
{
public:
    SplitterContainer() : QSplitter( Qt::Vertical )
    {
        setStyle( new QMotifStyle );
        //setStyle(new QWindowsStyle);
        //setStyle(new QPlastiqueStyle);
    }
    int
    getOptimalWidth();
};

#include <QScrollArea>

/** the ScrollArea class extends the QScrollArea class by two items
   - QScrollArea::resizeEvent gets extended:
   if the scroll area got smaller, then we must resize the contained widget, because
   it's width can get smaller, too
   (if e.g. the contained tree's width would be smaller then the scroll area width then
   the tree's width is defined to be the scroll area width, to fill out the whole visible area)
   - getOptimalWidth() is a new function, returning the minimal width necessary to display all information
   - ScrollArea holds different kinds of widgets, TreeWidget, SystemTopologyWidget, boxPlot.
 */


enum ScrollAreaWidgetType { ScrollAreaTreeWidget, ScrollAreaTopologyWidget, ScrollAreaBoxPlot };

class ScrollArea : public QScrollArea, public Container
{
    Q_OBJECT

public:

    ScrollArea( QWidget* widget,
                ScrollAreaWidgetType );
    ~ScrollArea();

    int
    getOptimalWidth();

    void
    setMainWidget( QWidget* w )
    {
        QScrollArea::setWidget( w );
        Container::setComponent( w );
    }

protected:

    void
    resizeEvent( QResizeEvent* event );

private:

    //stores if the scroll area contains a tree widget or a topology widget
    ScrollAreaWidgetType widgetType;
};

#endif
