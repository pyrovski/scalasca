/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
#include <iostream>

#include <QScrollBar>

#include "ScrollArea.h"
#include "TreeWidget.h"
#include "SystemTopologyWidget.h"
#include "SystemBoxPlot.h"

using namespace std;


// ------------------- SystemTopologyContainer -----------

int
SplitterContainer::getOptimalWidth()
{
    return ( ( ScrollArea* )( widget( 0 ) ) )->getOptimalWidth();
}



// ------------------- ScrollArea ------------------




ScrollArea::ScrollArea( QWidget*             parent,
                        ScrollAreaWidgetType _widgetType ) : QScrollArea( parent )
{
    setMinimumSize( 100, 200 );
    this->widgetType = _widgetType;
    //bars should be always displayed
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
}

ScrollArea::~ScrollArea()
{
}


//if the scroll area got smaller, then we must resize the contained tree, because
//the tree's width can get smaller, too
//(if the tree's width would be smaller then the scroll area width then
//it is defined to be the scroll area width, to fill out the whole visible area)
//
void
ScrollArea::resizeEvent( QResizeEvent* event )
{
    QScrollArea::resizeEvent( event );
    if ( widgetType == ScrollAreaTreeWidget )
    {
        ( ( TreeWidget* )widget() )->updateSizes();
        ( ( TreeWidget* )widget() )->repaint();
    }
    if ( widgetType == ScrollAreaTopologyWidget )
    {
        ( ( SystemTopologyWidget* )widget() )->repaint();
    }
    if ( widgetType == ScrollAreaBoxPlot )
    {
        ( ( SystemBoxPlot* )widget() )->repaint();
    }
}

//compute the minimal width that is necessary to display all informations
//
int
ScrollArea::getOptimalWidth()
{
    //store in w the width necessary for displaying the contained widget
    int w;

    switch ( widgetType )
    {
        case ScrollAreaTreeWidget:
            w = ( ( TreeWidget* )widget() )->getOptimalWidth();
            break;
        case ScrollAreaTopologyWidget:
            w = ( ( SystemTopologyWidget* )widget() )->getOptimalWidth();
            break;
        case ScrollAreaBoxPlot:
            w = ( ( SystemBoxPlot* )widget() )->getOptimalWidth();
            break;
        default:
            cerr << "Unknown type of the container widget help by ScrollArea. Return widgth as 150" << endl;
            w = 150;
            break;
    }
    //add to w the additional width for displaying the contained widget (frame, scroll bar, etc.)
    w += width() - maximumViewportSize().width();
    //however, don't get smaller than the minimal width
    if ( w < minimumWidth() )
    {
        w = minimumWidth();
    }
    return w;
}
