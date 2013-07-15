/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "MyTreeWidget.h"
#include "TreeWidgetItem.h"
#include "Coloring.h"
#include "PrecisionWidget.h"
#include "MessageWidget.h"

#include "Metric.h"


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
#include <QTimer>

MyTreeWidget::MyTreeWidget( ScrollArea* parent ) : QWidget( parent )
{
    assert( parent != NULL );
    this->parent = parent;

    setFocusPolicy( Qt::StrongFocus );
    setAutoFillBackground( false );

    top.clear();
    selected.clear();

    resize( 100, 200 );
    minIndex  = 0;
    maxIndex  = 0;
    tabWidget = NULL;
    spacing   = 2;

    findItem = NULL;

// to handle tooltip one has to track the mouseposition
    setMouseTracking( true );


    goToTop();
}


/******************* set methods *******************/

//set the tabWidget the tree is contained in
//
void
MyTreeWidget::setTabWidget( TabWidget* tabWidget )
{
    this->tabWidget = tabWidget;
}

//set the spacing between the line in the tree display
//
void
MyTreeWidget::setSpacing( int spacing )
{
    this->spacing = spacing;
}








/******************* get methods *******************/

//get list of selected items
//
std::vector<TreeWidgetItem*>
MyTreeWidget::selectedItems()
{
    return selected;
}

//get the tabWidget the tree is contained in
//
TabWidget*
MyTreeWidget::getTabWidget()
{
    return tabWidget;
}

//get number of root items
//
int
MyTreeWidget::topLevelItemCount()
{
    return top.size();
}

//get list of root items
//
std::vector<TreeWidgetItem*>
MyTreeWidget::topLevelItems()
{
    return top;
}

//get the index-th root item
//
TreeWidgetItem*
MyTreeWidget::topLevelItem( int index )
{
    if ( ( int )top.size() <= index )
    {
        return NULL;
    }
    else
    {
        return top[ index ];
    }
}





/*************** adding/removing items *****************/

//add a new root item; adding new non-root items happen just by
//creating a new item with a parent node already in one of the trees
//
bool
MyTreeWidget::addTopLevelItem( TreeWidgetItem* item )
{
    assert( item != NULL );
    assert( !item->isSelected() );
    assert( !item->isHidden() );

    //each item may belong to at most one tree
    if ( item->getTree() != NULL )
    {
        return false;
    }
    top.push_back( item );
    item->setTree( this );

    return true;
}

//remove an item from the tree
//
void
MyTreeWidget::removeItem( TreeWidgetItem* item )
{
    assert( item != NULL );

    if ( item->isSelected() )
    {
        deselect( item );
    }

    TreeWidgetItem* parent = item->parent();
    //if the item to be removed is a root item, remove from the
    //root item list
    if ( parent == NULL )
    {
        for ( int i = 0; i < ( int )top.size(); i++ )
        {
            if ( top[ i ] == item )
            {
                for (; i < ( int )top.size() - 1; i++ )
                {
                    top[ i ] = top[ i + 1 ];
                }
                top.pop_back();
                break;
            }
            assert( i < ( int )top.size() - 1 );
        }
    }
    else
    {
        //else just remove this child from the parent item
        parent->removeChild( item );
    }
    item->setTree( NULL );
}




/*********************** painting *********************/

//ensure that an item is in the visible rectangle of the scroll widget
//
void
MyTreeWidget::ensureVisible( TreeWidgetItem* item )
{
    QScrollBar* scrollBar = parent->verticalScrollBar();
    assert( scrollBar != NULL );
    if ( scrollBar->minimum() >= scrollBar->maximum() )
    {
        return;
    }

    //get the first visible root item
    TreeWidgetItem* next = NULL;
    for ( int i = 0; i < ( int )top.size(); i++ )
    {
        if ( !top[ i ]->isHidden() )
        {
            next = top[ i ];
            break;
        }
    }

    //search for the parameter item in the order of display, starting
    //from the root "next";
    //note: if the item is not in the tree of the first visible root
    //"next", then getNextItem will continue in the forthcoming trees,
    //if we have several roots
    int index = 0;
    while ( next != NULL && next != item )
    {
        ++index;
        next = getNextItem( next );
    }

    //if the item is not found, then return
    if ( next == NULL )
    {
        return;
    }

    //the item is displayed in the height y1 to y2
    int y1 = index * lineHeight;
    int y2 = y1 + lineHeight;
    //get the visible rectangle of the tree widget
    //(show in the window of the scroll widget)
    QRect rect = visibleRegion().boundingRect();
    //if not the whole item height is visible
    if ( rect.y() > y1 || rect.y() + rect.height() < y2 )
    {
        //compute number of displayed items
        int    numItems = height() / lineHeight;
        //compute number of visible items
        int    numVisibleItems = rect.height() / lineHeight;
        double percent         = 1.0;
        //if the item is not visible when we scroll to bottom, then
        //compute the necessary scrollbar position such that the item is
        //at the top of the visible rectangle;
        //otherwise, if the item is visible if we scroll to bottom, then
        //in general it is not possible to display the item in the first
        //line of the visible rectangle, for example the last item will be
        //always at the bottom, even if we scroll completely down; in this
        //case we just scroll to bottom
        if ( index < numItems - numVisibleItems )
        {
            percent = ( double )index / ( double )( numItems - numVisibleItems );
            if ( percent < 0.0 )
            {
                percent = 0.0;
            }
            else if ( percent > 1.0 )
            {
                percent = 1.0;
            }
        }
        int newPos = scrollBar->minimum() + ( int )( ( double )( scrollBar->maximum() - scrollBar->minimum() ) * percent );
        scrollBar->setValue( newPos );
    }
}


//return the item painted below the parameter item
//
TreeWidgetItem*
MyTreeWidget::getNextItem( TreeWidgetItem* item )
{
    assert( item != NULL );
    TreeWidgetItem* next = NULL;

    //search first for a visible child
    if ( !item->isHidden() && item->isExpanded() )
    {
        for ( int i = 0; i < item->childCount(); i++ )
        {
            if ( !item->child( i )->isHidden() )
            {
                next = item->child( i );
                break;
            }
        }
    }
    //if not found then going up the hierarchy
    if ( next == NULL )
    {
        TreeWidgetItem* childItem  = item;
        TreeWidgetItem* parentItem = childItem->parent();

        while ( next == NULL && parentItem != NULL )
        {
            for ( int i = 0; i < parentItem->childCount(); i++ )
            {
                if ( parentItem->child( i ) == childItem )
                {
                    ++i;
                    for (; i < parentItem->childCount(); i++ )
                    {
                        if ( !parentItem->child( i )->isHidden() )
                        {
                            next = parentItem->child( i );
                            break;
                        }
                    }
                    break;
                }
            }
            childItem  = parentItem;
            parentItem = childItem->parent();
        }

        //if not found then searching for a next root
        if ( next == NULL )
        {
            for ( int i = 0; i < ( int )top.size(); i++ )
            {
                if ( top[ i ] == childItem )
                {
                    ++i;
                    for (; i < ( int )top.size(); i++ )
                    {
                        if ( !top[ i ]->isHidden() )
                        {
                            next = top[ i ];
                            break;
                        }
                    }
                }
            }
        }
    }

    return next;
}

//return the item painted above the parameter item
//
TreeWidgetItem*
MyTreeWidget::getPreviousItem( TreeWidgetItem* item )
{
    assert( item != NULL );
    TreeWidgetItem* previous = NULL;

    //if the parameter item is a root item, then determine the
    //previous visible root item
    if ( item->parent() == NULL )
    {
        for ( int i = top.size() - 1; previous == NULL && i >= 0; i-- )
        {
            if ( top[ i ] == item )
            {
                --i;
                for (; i >= 0; i-- )
                {
                    if ( !top[ i ]->isHidden() )
                    {
                        previous = top[ i ];
                        break;
                    }
                }
            }
        }
        if ( previous == NULL )
        {
            return NULL;
        }
    }
    else
    {
        //else determine the previous child of the parent item, or return
        //parent item itself if the item is the first child
        previous = item->parent();
        int i = previous->childCount() - 1;
        for (; i >= 0; i-- )
        {
            if ( previous->child( i ) == item )
            {
                --i;
                break;
            }
        }
        if ( i >= 0 )
        {
            previous = previous->child( i );
        }
        else
        {
            return previous;
        }
    }

    assert( previous != NULL );

    //now if the determined item has a subree shown, then determine the last
    //visible item of that subtree
    while ( previous->isExpanded() && previous->childCount() > 0 )
    {
        int i = previous->childCount() - 1;
        for (; i >= 0; i-- )
        {
            if ( !previous->child( i )->isHidden() )
            {
                previous = previous->child( i );
                break;
            }
        }
        if ( i < 0 )
        {
            break;
        }
    }

    return previous;
}

//gives back the index-th visible item (from the top) in the tree display
//
TreeWidgetItem*
MyTreeWidget::getItemAtIndex( int index )
{
    //collect visible top items in reverse order in v
    std::vector<TreeWidgetItem*> v;
    for ( int i = ( int )top.size() - 1; i >= 0; i-- )
    {
        if ( !top[ i ]->isHidden() )
        {
            v.push_back( top[ i ] );
        }
    }

    int current = 0;
    while ( v.size() > 0 )
    {
        //take v's last element, that is the current-th visible element
        //(starting at 0)
        TreeWidgetItem* item = v[ v.size() - 1 ];
        v.pop_back();
        //if we've searched the current index, then return the item at
        //that index
        if ( current == index )
        {
            return item;
        }
        //if the item is expanded, then its children are displayed below
        //it, thus continue with them
        if ( item->isExpanded() )
        {
            //again, reverse order such that we can wourk up data from the
            //end of the vector
            for ( int i = item->childCount() - 1; i >= 0; i-- )
            {
                if ( !item->child( i )->isHidden() )
                {
                    v.push_back( item->child( i ) );
                }
            }
        }
        current++;
    }
    return NULL;
}


//scroll to the top of the tree
//
void
MyTreeWidget::goToTop()
{
    parent->verticalScrollBar()->setValue( 0 );
    parent->horizontalScrollBar()->setValue( 0 );
}

//compute the width and height of the painted tree
//
void
MyTreeWidget::computeSizes( int&w, int&h )
{
    //define the line height as the height necessary to write with the
    //current font
    QFontMetrics fm( font() );
    lineHeight = fm.ascent();

    iconSize = ( int )( 1.0 * ( double )lineHeight );
    if ( iconSize > 5 )
    {
        iconSize = iconSize - 1;
    }
    //left margin
    indent0 = ( int )( 0.0 * ( double )lineHeight );
    //indentation for different item depth
    indent1 = ( int )( 1.5 * ( double )lineHeight );
    //indentation left and right from icon
    indent2 = ( int )( 0.5 * ( double )lineHeight );
    //2*indent3 is the width of the expansion rectangle
    indent3 = ( int )( 0.37 * ( double )lineHeight );

    //increase line height by the current line spacing
    lineHeight += spacing;

    //collect all non-hidden roots in v;
    //the painted items are these non-hidden roots and all non-hidden
    //children of expanded painted items
    std::vector<TreeWidgetItem*> v;
    for ( int i = 0; i < ( int )top.size(); i++ )
    {
        if ( !top[ i ]->isHidden() )
        {
            v.push_back( top[ i ] );
        }
    }

    //basic line width;
    //each lines' with is this width plus indentation according to its depth
    //plus its text's width
    int basicW = indent0 + indent2 + iconSize + indent2 + 5;

    //in w we store the maximal line width
    w = 0;

    for ( int i = 0; i < ( int )v.size(); i++ )
    {
        TreeWidgetItem* item = v[ i ];
        QString         text = item->getText();
        //compute this line's width
        int             tmp = basicW + ( item->getDepth() + 1 ) * indent1 + fm.width( text );
        //check if this is larger then the maximal width up to now
        if ( tmp > w )
        {
            w = tmp;
        }
        //if the item is expanded, then consider also its non-hidden
        //children, since they are also displayed
        if ( item->isExpanded() )
        {
            for ( int i = 0; i < item->childCount(); i++ )
            {
                if ( !item->child( i )->isHidden() )
                {
                    v.push_back( item->child( i ) );
                }
            }
        }
    }

    //compute height as number of painted items times the line height

    h = v.size() * lineHeight;
}

//computes the width of the tree
//
int
MyTreeWidget::getOptimalWidth()
{
    int w, h;
    //compute the with of the painted tree
    computeSizes( w, h );
    //add some additional space such that the tree is not so squeezed
    w += 5;
    //take at least the minimum window size
    if ( w < minimumWidth() )
    {
        w = minimumWidth();
    }
    return w;
}


//rescales the display for the current tree size
//
void
MyTreeWidget::updateSizes()
{
    int vBarPos = parent->verticalScrollBar()->value();

    //compute optimal width
    int w = 0;
    int h = 0;
    computeSizes( w, h );

    //fill out at least the size of the scroll widget
    QSize minSize = parent->maximumViewportSize();
    if ( w < minSize.width() )
    {
        w = minSize.width();
    }
    if ( h < minSize.height() )
    {
        h = minSize.height();
    }

    resize( w, h );

    //restore the previous vertical scroll bar position
    //(it may change by resizing)
    parent->verticalScrollBar()->setValue( vBarPos );
}



//the main tree painting method
//
void
MyTreeWidget::paintEvent( QPaintEvent* )
{
    QPainter painter( this );

    //paint only the visible part of the widget,
    //i.e., the part visible in the scroll widget's window
    QRect rect = visibleRegion().boundingRect();
    painter.fillRect( rect, Qt::white );
    //determine which items should be painted
    minIndex = rect.y() / lineHeight;
    maxIndex = ( rect.y() + rect.height() ) / lineHeight + 1;

    //index stores the index of the row to paint
    int index = 0;

    //lastIndex stores the index of the previous sibling of the
    //currently painted item; we need this info to draw the vertical
    //lines from one child to the next one
    int lastIndex = -1;

    //in tmp we temporarily store the index of the last root for which
    //paintItem was called; note that if the root item is expanded, its
    //subtree will be recursively painted by paintItem
    int tmp = 0;

    //store if the previous sibling is expandable, i.e., if it has the
    //expand/collapse icon
    bool lastExpandable = false;

    //recursively paint the non-hidden root items
    //and the non-hidden children of painted items
    for ( int i = 0; i < ( int )top.size(); i++ )
    {
        if ( !top[ i ]->isHidden() )
        {
            tmp = index;
            paintItem( painter, top[ i ], lastIndex, lastExpandable, index );
            lastIndex = tmp;
            tmp       = index;
        }
    }
}



//paints a certain item
//
void
MyTreeWidget::paintItem( QPainter& painter, TreeWidgetItem* item, int lastIndex, bool& lastExpandable, int& index )
{
    bool visible = index >= minIndex && ( index <= maxIndex || lastIndex <= maxIndex );

    assert( !item->isHidden() );

    /*

       Koordinate system: ------> x
     |
     |
     |
     |
                     y v


       We draw first something like this (dots are not painted, just connect descriptions to points/length):




                  (xA,yA)
     |
     |   (xD,yD)
     |      .
     |      .____                   .....(?,yTop)
     |      |    |  .
               (xB,yB) ----  |    |  .iconSize
                          .  |____|  .
                          .
                          .
                      (xC,yC)
     */

    int xB = 0;
    int yB = 0;

    //paint only if the item is visible
    if ( visible )
    {
        //y-position at the top of the line
        int yTop = 0;

        int xA = 0;
        int yA = 0;
        int xC = 0;
        int yC = 0;
        int xD = 0;
        int yD = 0;

        //compute the y-position at the top of the line
        yTop = index * lineHeight;

        //compute top-position of the vertical line
        {
            xA = indent0 + item->getDepth() * indent1 + indent2 + iconSize / 2;

            yA = lastIndex * lineHeight + lineHeight / 2;

            //if this is the first non-hidden child of the parent, then
            //the vertical line starts a bit deeper because of the parent's icon
            if ( item->parent() != NULL )
            {
                bool firstVisibleChild = false;
                for ( int i = 0; i < item->parent()->childCount(); i++ )
                {
                    if ( item->parent()->child( i ) == item )
                    {
                        firstVisibleChild = true;
                        break;
                    }
                    else if ( !item->parent()->child( i )->isHidden() )
                    {
                        break;
                    }
                }
                if ( firstVisibleChild )
                {
                    yA += ( iconSize + indent2 ) / 2;
                }
            }

            //if the previous sibling is expandable, then the vertical line starts
            //a bit deeper because of the sibling's icon
            if ( lastExpandable )
            {
                yA += indent3;
            }

            //the vertical line for the first root should have a non-negative
            //y-start-position
            if ( yA < 0 )
            {
                yA = 0;
            }
        }

        //compute the bottom position of the vertical line
        {
            xB = xA;
            yB = yTop + lineHeight / 2;
        }

        //compute the position of the right end of the horizontal line
        //leading to the color icon
        {
            xC = indent0 + ( item->getDepth() + 1 ) * indent1;
            yC = yB;
        }

        //compute top left position of the color icon
        {
            xD = xC + indent2;
            yD = yTop + ( lineHeight - iconSize ) / 2 - 1;
            if ( yD < 0 )
            {
                yD = 0;
            }
        }




        //paint special backgrounds
        {
            //if the item is the current find item, paint special background
            if ( findItem == item )
            {
                painter.fillRect( 0, yTop, width() - 1, lineHeight - 1, QBrush( QColor( 255, 200, 0, ( item->mark1 ? 200 : 100 ) ) ) );
            }
            //else if the item was found by the last search, paint special
            //background
            else if ( item->mark1 )
            {
                painter.fillRect( 0, yTop, width() - 1, lineHeight - 1, QBrush( QColor( 255, 255, 0, 200 ) ) );
            }
            //else if the item is the parent of an item found by the last
            //search, and it is collapsed, such that the item found by the
            //last search is not visible, then use also a special background
            //to denote that in
            else if ( !item->isExpanded() && item->mark2 )
            {
                painter.fillRect( 0, yTop, width() - 1, lineHeight - 1, QBrush( QColor( 255, 255, 0, 100 ) ) );
            }
            //if the item is selected and selections are marked by a special
            //backgound, then paint this background
            if ( item->isSelected() && tabWidget->getSelectionSyntax() == BACKGROUND_SELECTION )
            {
                painter.fillRect( 0, yTop, width() - 1, lineHeight - 1, QBrush( QColor( 0, 127, 255, 125 ) ) );
            }
        }

        //draw vertical line from the last sibling to this item
        painter.drawLine( xA, yA, xB, yB );
        //draw horizontal line from the end of that vertical line in the
        //direction of the color icon
        painter.drawLine( xB, yB, xC, yC );

        //draw the color icon
        {
            //draw the color icon itself
            assert( xD >= 0 );
            assert( yD >= 0 );
            painter.fillRect( xD, yD, iconSize, iconSize, QBrush( item->getColor() ) );
            painter.drawRect( xD, yD, iconSize, iconSize );

            double prec = ( tabWidget == NULL ? 0.0 : tabWidget->getPrecisionWidget()->getRoundThreshold( FORMAT0 ) );
            assert( prec >= 0.0 );
            assert( prec < 1.0 );
            //for positive values draw a raised color icon
            if ( item->getValue() > prec )
            {
                painter.setPen( Qt::black );

                painter.drawLine( xD + iconSize - 1, yD, xD + iconSize - 1, yD + iconSize - 1 );
                painter.drawLine( xD + iconSize - 2, yD + 1, xD + iconSize - 2, yD + iconSize - 2 );
                painter.drawLine( xD, yD + iconSize - 1, xD + iconSize - 1, yD + iconSize - 1 );
                painter.drawLine( xD + 1, yD + iconSize - 2, xD + iconSize - 2, yD + iconSize - 2 );

                painter.setPen( Qt::white );

                painter.drawLine( xD + 1, yD + 1, xD + 1, yD + iconSize - 2 );
                painter.drawLine( xD + 1, yD + 1, xD + iconSize - 2, yD + 1 );

                painter.setPen( Qt::black );
            }
            else if ( item->getValue() < -prec )
            {
                //for negative values draw a deepened color icon

                painter.setPen( Qt::black );

                painter.drawLine( xD + iconSize - 1, yD, xD + iconSize - 1, yD + iconSize - 1 );
                painter.drawLine( xD, yD + iconSize - 1, xD + iconSize - 1, yD + iconSize - 1 );
                painter.drawLine( xD + 1, yD + 1, xD + 1, yD + iconSize - 2 );
                painter.drawLine( xD + 1, yD + 1, xD + iconSize - 2, yD + 1 );

                painter.setPen( Qt::white );

                painter.drawLine( xD + 1, yD + iconSize - 2, xD + iconSize - 2, yD + iconSize - 2 );
                painter.drawLine( xD + iconSize - 2, yD + 1, xD + iconSize - 2, yD + iconSize - 2 );

                painter.setPen( Qt::black );
            }
        }

        //draw the text and eventual frame for selection framing or frame for clicked item for context menu
        {
            QString text = item->getText();
            painter.drawText( xC + iconSize + 2 * indent2, yTop, width() - xC, lineHeight, Qt::AlignLeft | Qt::AlignVCenter, text );
            if ( item->framed || ( item->isSelected() && tabWidget->getSelectionSyntax() == FRAME_SELECTION ) )
            {
                QRect rect = painter.boundingRect( xC + iconSize + 2 * indent2, yTop, width() - xC, lineHeight, Qt::AlignLeft | Qt::AlignVCenter, text );
                rect.setX( rect.x() - 2 );
                rect.setWidth( rect.width() + 2 );
                QPen pen = painter.pen();
                if ( item->framed )
                {
                    pen.setStyle( Qt::DashLine );
                }
                else
                {
                    pen.setWidth( 2 );
                }
                painter.setPen( pen );
                painter.drawRect( rect );
                pen.setWidth( 1 );
                pen.setStyle( Qt::SolidLine );
                painter.setPen( pen );
            }

            if ( item->statistic_available_frame )
            {
                QRect rect = painter.boundingRect( xC + iconSize + 2 * indent2, yTop, width() - xC, lineHeight, Qt::AlignLeft | Qt::AlignVCenter, text );
                rect.setX( rect.x() - 3 );
                rect.setWidth( rect.width() + 3 );
                QPen backup_pen = painter.pen();
                QPen pen;
                pen.setStyle( Qt::DashLine );
                pen.setColor( Qt::red );
                painter.setPen( pen );
                painter.drawRect( rect );
                pen.setWidth( 1 );
                pen.setStyle( Qt::SolidLine );
                painter.setPen( backup_pen );
            }
        }
    }

    lastIndex = index;
    index++;
    int tmp = 0;
    lastExpandable = false;
    //if the item is expanded, paint all non-hidden children
    if ( item->isExpanded() )
    {
        for ( int i = 0; i < item->childCount(); i++ )
        {
            if ( !item->child( i )->isHidden() )
            {
                tmp = index;
                paintItem( painter, item->child( i ), lastIndex, lastExpandable, index );
                if ( lastIndex > maxIndex )
                {
                    break;
                }
                lastIndex = tmp;
                tmp       = index;
            }
        }
    }

    //check if this item is expandable
    lastExpandable = false;
    for ( int i = 0; i < item->childCount(); i++ )
    {
        if ( !item->child( i )->isHidden() )
        {
            lastExpandable = true;
            break;
        }
    }

    //if yes, paint the expand/collapse icon
    if ( visible )
    {
        if ( lastExpandable )
        {
            painter.fillRect( xB - indent3, yB - indent3, 2 * indent3, 2 * indent3, QBrush( Qt::white ) );
            painter.drawRect( xB - indent3, yB - indent3, 2 * indent3, 2 * indent3 );
            int d = indent3 / 2;
            painter.drawLine( xB - d, yB, xB + d, yB );
            if ( !item->isExpanded() )
            {
                painter.drawLine( xB, yB - d, xB, yB + d );
            }
        }
    }
}
//end of paintItem()




/******************* selection / deselection ***********************/

//item selection is recorded at two places:
//1. the item's selected flag is set
//2. the item is inserted into the "selected" vector of the TreeWidget instance

//select an item
//
void
MyTreeWidget::select( TreeWidgetItem* item )
{
    assert( item != NULL );
    //if already selected, do nothing
    for ( int i = 0; i < ( int )selected.size(); i++ )
    {
        if ( selected[ i ] == item )
        {
            return;
        }
    }
    //if selection is caused by calling this method, then
    //re-route it to the tree widget item;
    //this will set its flag and call this method again
    if ( !item->isSelected() )
    {
        item->setSelected( true );
    }
    else
    {
        selected.push_back( item );
        if ( selected.size() == 1 && tabWidget->currentIndex() == 0 )
        {
            ensureVisible( item );
        }

        repaint();
        onSelection();
    }
    emit selectionChanged();
}

//deselect an item
//
void
MyTreeWidget::deselect( TreeWidgetItem* item, bool toUpdate )
{
    assert( item != NULL );

    //if deselection is caused by calling this method, then
    //re-route it to the tree widget item;
    //this will set its flag and call this method again

    if ( item->isSelected() )
    {
        item->setSelected( false );
    }
    else
    {
        for ( int i = 0; i < ( int )selected.size(); i++ )
        {
            if ( selected[ i ] == item )
            {
                for (; i < ( int )selected.size() - 1; i++ )
                {
                    TreeWidgetItem* tmp = selected[ i ];
                    selected[ i ]     = selected[ i + 1 ];
                    selected[ i + 1 ] = tmp;
                }
                selected.pop_back();

                if ( toUpdate )
                {
                    repaint();
                    onDeselection();
                }
                break;
            }
        }
    }
}




/************ item expansion/ collapse *******************/

//expand a tree item
//
void
MyTreeWidget::expandItem( TreeWidgetItem* item )
{
    if ( item->isExpanded() )
    {
        return;
    }

    //expand the item
    item->setExpanded( true );
    //if the expanded item was the current found item, and it was not
    //found by the last search itself, but is a parent of such a found
    //element, then we recursively change the current found item to the
    //first child that was either found by the search or is a parent of
    //such a found item
    if ( item == findItem && !item->mark1 )
    {
        while ( true )
        {
            for ( int i = 0; i < findItem->childCount(); i++ )
            {
                if ( findItem->child( i )->mark1 || findItem->child( i )->mark2 )
                {
                    findItem = findItem->child( i );
                    break;
                }
            }
            if ( findItem->mark1 || !item->isExpanded() )
            {
                break;
            }
            bool hasVisibleChild = false;
            for ( int i = 0; i < findItem->childCount(); i++ )
            {
                if ( !findItem->child( i )->isHidden() )
                {
                    hasVisibleChild = true;
                }
            }
            if ( !hasVisibleChild )
            {
                break;
            }
        }
    }
    setVisibleFlagToSelfAndChildren( item );
    if ( item->isSelected() )
    {
        onSelection();
    }
    emit collapsedChanged();
}

//collapse a tree item
//
void
MyTreeWidget::collapseItem( TreeWidgetItem* item )
{
    if ( !item->isExpanded() )
    {
        return;
    }

    emit collapsedChanged();

    item->setExpanded( false );
    setInvisibleFlagToChildren( item );
    //due to collapse selected items can be hidden now;
    //in those cases select the first visible parent of the selected item
    checkSelection();
}

//recursively expand all tree items
//
void
MyTreeWidget::expandAll()
{
    std::vector<TreeWidgetItem*> v;

    //start with the roots
    for ( int i = 0; i < ( int )top.size(); i++ )
    {
        //expand only non-hidden items
        if ( !top[ i ]->isHidden() )
        {
            //expand only items with some non-hidden children
            bool expandable = false;
            for ( int j = 0; j < top[ i ]->childCount(); j++ )
            {
                if ( !top[ i ]->child( j )->isHidden() )
                {
                    expandable = true;
                    //collect in v the children to recursively expand later
                    v.push_back( top[ i ]->child( j ) );
                }
            }
            if ( expandable )
            {
                top[ i ]->setExpanded( true );
                top[ i ]->setVisible(); // top is visible
            }
        }
    }

    //now expand the items in v
    for ( int i = 0; i < ( int )v.size(); i++ )
    {
        bool expandable = false;
        v[ i ]->setVisible();
        //expand only items with some non-hiddent children
        for ( int j = 0; j < v[ i ]->childCount(); j++ )
        {
            if ( !v[ i ]->child( j )->isHidden() )
            {
                expandable = true;
                v.push_back( v[ i ]->child( j ) );
                v[ i ]->child( j )->setVisible(); // children should be also visible
            }
        }
        if ( expandable )
        {
            v[ i ]->setExpanded( true );
        }
    }

    //by expansion values can change;
    //just simulate as if the selection would have changed
    onSelection();
    //by expansion the tree size can change, thus adjust
    updateSizes();
}

//recursively collapse all tree items
//
void
MyTreeWidget::collapseAll()
{
    std::vector<TreeWidgetItem*> v;
    for ( int i = 0; i < ( int )top.size(); i++ )
    {
        top[ i ]->setExpanded( false );
        top[ i ]->setVisible(); // we can see the top one
        for ( int j = 0; j < top[ i ]->childCount(); j++ )
        {
            v.push_back( top[ i ]->child( j ) );
        }
    }
    for ( int i = 0; i < ( int )v.size(); i++ )
    {
        v[ i ]->setExpanded( false );
        v[ i ]->setInvisible(); // if it is not top->mark as invisible
        for ( int j = 0; j < v[ i ]->childCount(); j++ )
        {
            v.push_back( v[ i ]->child( j ) );
        }
    }

    //due to collapse selected items can be hidden now;
    //in those cases select the first visible parent of the selected item
    checkSelection();
}

//recursively expand all items in the subtree of the item clicked with
//the mouse
//
void
MyTreeWidget::expandClicked()
{
    assert( clickedItem != NULL );
    assert( !clickedItem->isHidden() );

    std::vector<TreeWidgetItem*> v;
    v.push_back( clickedItem );

    for ( int i = 0; i < ( int )v.size(); i++ )
    {
        //expand only items with some non-hidden children
        bool expandable = false;
        for ( int j = 0; j < v[ i ]->childCount(); j++ )
        {
            if ( !v[ i ]->child( j )->isHidden() )
            {
                expandable = true;
                v.push_back( v[ i ]->child( j ) );
            }
        }
        if ( expandable )
        {
            v[ i ]->setExpanded( true );
        }
    }
    setVisibleFlagToSelfAndChildren( clickedItem );
    onSelection();
    updateSizes();
}

//recursively collapse all items in the subtree of the item clicked with the mouse
//
void
MyTreeWidget::collapseClicked()
{
    assert( clickedItem != NULL );
    assert( !clickedItem->isHidden() );

    std::vector<TreeWidgetItem*> v;
    v.push_back( clickedItem );

    for ( int i = 0; i < ( int )v.size(); i++ )
    {
        v[ i ]->setExpanded( false );
        for ( int j = 0; j < v[ i ]->childCount(); j++ )
        {
            v.push_back( v[ i ]->child( j ) );
        }
    }

    setInvisibleFlagToChildren( clickedItem );
    //due to collapse selected items can be hidden now;
    //in those cases select the first visible parent of the selected item
    checkSelection();
}


//this slot recursively expands all peers, i.e., nodes of the same
//type, in a system tree
//
void
MyTreeWidget::expandPeers()
{
    TreeWidgetItemType itemType = clickedItem->type;

    //expand all items of the type of the clicked item
    //which are collapsed and have at least one non-hidden child

    //collect first all non-hidden roots
    std::vector<TreeWidgetItem*> items;
    for ( int i = 0; i < ( int )top.size(); i++ )
    {
        if ( !top[ i ]->isHidden() )
        {
            items.push_back( top[ i ] );
        }
    }

    for ( unsigned i = 0; i < items.size(); i++ )
    {
        //if the item is of the given type, is not yet expanded, and has
        //some non-hidden children, then expand it
        if ( items[ i ]->type == itemType && !items[ i ]->isExpanded() )
        {
            bool expandable = false;
            for ( int j = 0; j < ( int )items[ i ]->childCount(); j++ )
            {
                if ( !items[ i ]->child( j )->isHidden() )
                {
                    expandable = true;
                    break;
                }
            }
            if ( expandable )
            {
                expandItem( items[ i ] );
            }
        }
        //collect the item's children
        for ( int j = 0; j < ( int )items[ i ]->childCount(); j++ )
        {
            if ( !items[ i ]->child( j )->isHidden() )
            {
                items.push_back( items[ i ]->child( j ) );
            }
        }
    }

    onSelection();
    updateSizes();
}


//this slot recursively collapses all peers, i.e., nodes of the same
//type, of the clicked node in a system tree
//
void
MyTreeWidget::collapsePeers()
{
    TreeWidgetItemType itemType = clickedItem->type;

    //collect first all non-hidden roots
    std::vector<TreeWidgetItem*> items;
    for ( int i = 0; i < ( int )top.size(); i++ )
    {
        if ( !top[ i ]->isHidden() )
        {
            items.push_back( top[ i ] );
        }
    }

    //now collapse all nodes of the type of the clicked node which are
    //expanded
    for ( unsigned i = 0; i < items.size(); i++ )
    {
        if ( items[ i ]->type == itemType && items[ i ]->isExpanded() )
        {
            collapseItem( items[ i ] );
        }
        //collect the item's children for recursion
        for ( int j = 0; j < ( int )items[ i ]->childCount(); j++ )
        {
            if ( !items[ i ]->child( j )->isHidden() )
            {
                items.push_back( items[ i ]->child( j ) );
            }
        }
    }

    //due to collapse selected items can be hidden now;
    //in those cases select the first visible parent of the selected item
    checkSelection();
}



//starting from the clicked node, this slot recursively expands the children with the
//highest total value till a leaf has been reached
//
void
MyTreeWidget::expandLargest()
{
    TreeWidgetItem* item = clickedItem;

    //starting at the clicked node, we search for the child with the largest value,
    //expand it, and recursively continue with that child
    unsigned numberChildren = item->childCount();
    unsigned largestChildIndex;
    double   largestChildValue, childValue;
    while ( numberChildren > 0 )
    {
        //check if the current node is expandable, i.e.,
        //if it has at least one non-hidden child
        bool expandable = false;
        for ( int j = 0; j < ( int )item->childCount(); j++ )
        {
            if ( !item->child( j )->isHidden() )
            {
                expandable = true;
                break;
            }
        }
        //if the current node is not expandable, than we are at a leaf and ready
        if ( !expandable )
        {
            break;
        }

        //expand the current item if not yet expanded
        if ( !item->isExpanded() )
        {
            expandItem( item );
        }

        //find the child with the largest total value
        largestChildIndex = 0;
        largestChildValue = ( ( TreeWidgetItem* )( item->child( 0 ) ) )->totalValue;
        for ( unsigned i = 1; i < numberChildren; i++ )
        {
            childValue = ( ( TreeWidgetItem* )( item->child( i ) ) )->totalValue;
            if ( childValue > largestChildValue )
            {
                largestChildIndex = i;
                largestChildValue = childValue;
            }
        }
        //recursion will continue with the child with the largest total value
        item           = ( TreeWidgetItem* )( item->child( largestChildIndex ) );
        numberChildren = item->childCount();
    }

    setVisibleFlagToSelfAndChildren( item );
    onSelection();
    updateSizes();
}
//end of expandLargest()



//check if the selected items are visible;
//if not (can happen if its parent got collapsed or
//if the item got hidden) then
//change selection to its first visible parent
//
void
MyTreeWidget::checkSelection()
{
    if ( items.size() == 0 )
    {
        return;
    }

    //for each selected item, if it is not visible, get a new selection
    for ( int i = 0; i < ( int )selected.size(); i++ )
    {
        TreeWidgetItem* item         = selected[ i ];
        TreeWidgetItem* newSelection = item;
        while ( item != NULL )
        {
            if ( item->isHidden() )
            {
                newSelection = item->parent();
            }
            else if ( !item->isExpanded() )
            {
                newSelection = item;
            }
            item = item->parent();
        }
        if ( newSelection != selected[ i ] )
        {
            deselect( selected[ i ] );
            if ( newSelection != NULL )
            {
                select( newSelection );
            }
            //to be independent of the implementation of deselect,
            //we start checking at the beginning of the selected list
            i = -1;
        }
    }

    //ensure that the current find item is visible
    TreeWidgetItem* item = findItem;
    while ( item != NULL )
    {
        if ( item->isHidden() )
        {
            findItem = item->parent();
        }
        else if ( !item->isExpanded() )
        {
            findItem = item;
        }
        item = item->parent();
    }

    //if no selection is left, that can happen if for example we hidden
    //all roots, then make the root with the largest total value visible
    //and select it
    if ( selected.size() == 0 )
    {
        assert( top.size() > 0 );
        TreeWidgetItem* maxItem = top[ 0 ];
        for ( int i = 1; i < ( int )top.size(); i++ )
        {
            if ( top[ i ]->totalValue > maxItem->totalValue )
            {
                maxItem = top[ i ];
            }
        }
        if ( maxItem->isHidden() )
        {
            maxItem->setHidden( false );
            for ( int i = 0; i < maxItem->childCount(); i++ )
            {
                maxItem->child( i )->setHidden( true );
            }
        }
        select( maxItem );
    }

    onSelection();
    updateSizes();
}
//end of checkSelection()






/***************** mouse/key event handling ********************/

//key event handling
//
void
MyTreeWidget::keyPressEvent( QKeyEvent* event )
{
    switch ( event->key() )
    {
        //move the selection one item down in the currently displayed tree;
        //only for single-selection
        case Qt::Key_Down:
        {
            if ( ( int )selected.size() == 1 )
            {
                TreeWidgetItem* item = selected[ 0 ];
                assert( !item->isHidden() );
                TreeWidgetItem* next = getNextItem( item );
                if ( next != NULL )
                {
                    deselect( item );
                    select( next );
                }
            }
            event->accept();
            repaint();
            break;
        }
        //move the selection one item up in the currently displayed tree;
        //only for single-selection
        case Qt::Key_Up:
        {
            if ( ( int )selected.size() == 1 )
            {
                TreeWidgetItem* item = selected[ 0 ];
                assert( !item->isHidden() );
                TreeWidgetItem* previous = getPreviousItem( item );
                if ( previous != NULL )
                {
                    deselect( item );
                    select( previous );
                }
            }
            event->accept();
            repaint();
            break;
        }
        case Qt::Key_Left:
        case Qt::Key_Minus:
        case Qt::Key_Backspace:
        {
            TreeWidgetItem* item = selected[ 0 ];
            collapseItem( item );
            event->accept();
            repaint();
            break;
        }
        case Qt::Key_Right:
        case Qt::Key_Plus:
        case Qt::Key_Return:
        {
            TreeWidgetItem* item = selected[ 0 ];
            expandItem( item );
            event->accept();
            repaint();
            break;
        }

        default:
        {
            event->ignore();
        }
    }
}


//method handling mouse events
//
void
MyTreeWidget::mousePressEvent( QMouseEvent* event )
{
    bool to_recompute = false;

    //the left mouse button opens a context menu for the item the user clicked on
    if ( event->button() == Qt::LeftButton )
    {
        //get the index of the click
        int index = event->y() / lineHeight;
        //get the item at that index;
        //if the click was outside of any item row, item gets NULL
        TreeWidgetItem* item = getItemAtIndex( index );

        //if the click was over an item
        if ( item != NULL )
        {
            //check if the clicked item is expandable
            bool expandable = false;
            for ( int i = 0; i < item->childCount(); i++ )
            {
                if ( !item->child( i )->isHidden() )
                {
                    expandable = true;
                    break;
                }
            }
            if ( expandable )
            {
                //check if the expand/collapse quad was clicked
                int x1 = indent0 + item->getDepth() * indent1 + indent2 + iconSize / 2;
                int y1 = index * lineHeight + lineHeight / 2;
                int x  = event->x();
                int y  = event->y();
                if ( !( x >= x1 - indent3 && x <= x1 + indent3 &&
                        y >= y1 - indent3 && y <= y1 + indent3 ) )
                {
                    expandable = false;
                }
                else
                {
                    //                              cout << " +/- was clicked" << endl;
                    if ( item->isSelected() )
                    {
                        //                              cout << " ITEM IS SELECTED , RECOMPUTE CURRENT AND RIGHT PANELS" << endl;
                        to_recompute = true;
                    }
                    else
                    {
                        to_recompute = false;
                    }
                }
            }
            //if the item is not expandable or not the item/collapse quad was clicked,
            //then the event causes a new selection
            if ( !expandable )
            {
                Qt::KeyboardModifiers mods = event->modifiers();
                //if the control key is pressed, then multiple selection is possible
                if ( mods.testFlag( Qt::ControlModifier ) )
                {
                    //if the clicked item is currently selected, it get
                    //deselected, if it is not the only selected item
                    if ( item->isSelected() )
                    {
                        if ( selected.size() > 1 )
                        {
                            deselect( item );
                        }
                    }
                    else
                    {
                        //otherwise, if the clicked item is not selected, it gets
                        //selected, additionally to the currently selected item
                        //list

                        //for metric trees multiselection is allowed only within the same subtree
                        if ( type == METRICTREE && selected.size() > 0 )
                        {
                            //get the root of the first selected item
                            TreeWidgetItem* root     = selected[ 0 ];
                            std::string     root_uom = ( ( cube::Metric* )( root->cubeObject ) )->get_uom();
                            while ( root->parent() != NULL )
                            {
                                root = root->parent();
                            }

                            //get the root of the clicked item
                            TreeWidgetItem* root2     = item;
                            std::string     root2_uom = ( ( cube::Metric* )( root2->cubeObject ) )->get_uom();
                            while ( root2->parent() != NULL )
                            {
                                root2 = root2->parent();
                            }

                            //select clicked item if both roots are the same
                            if ( root_uom == root2_uom )
                            {
                                select( item );      // select item
                                if ( root != root2 ) // if not same roots, metric might be incompatible.
                                {
                                    // So, warn the user about it.
                                    MessageWidget* wwidget = new MessageWidget( Warning, "Be careful. Metrics with different roots might be incompatible for operation \"+\"." );
                                    wwidget->show();
                                }
                            }
                            else
                            {
                                // popups the red error message for 3 seconds.
                                QString message( "Multiple metric selection is possible only if the unit of measurement is compatible. \nIn this case \"" );
                                message += QString( root2_uom.c_str() );
                                message += "\" is incompatible with \"";
                                message += QString( root_uom.c_str() );
                                message += "\"";
                                // new widget
                                MessageWidget* wwidget = new MessageWidget( Error, message );
                                wwidget->show();
/*          // old standard widget
                QMessageBox::warning(this,"Multiple metric selection", message.toLatin1().data()) ;*/
                            }
                        }
                        else
                        {
                            select( item );
                        }
                    }
                }
                else
                {
                    //if the control key is not pressed, then we are in single
                    //selection mode

                    //we deselect all selected items
                    for ( int i = 0; i < ( int )selected.size(); i++ )
                    {
                        selected[ i ]->setSelected( false, false );
                    }
                    selected.clear();
                    //and select the clicked item
                    select( item );
                }
            }
            else
            {
                // if the item is expandable and the item/collapse quad was
                //clicked, then expand/collapse the clicked item
                if ( item->isExpanded() )
                {
                    collapseItem( item );
                    setInvisibleFlagToChildren( item );
                }
                else
                {
                    expandItem( item );
                    to_recompute = true;
//				for (int i=0; i<item->childCount(); i++)
//				{
//					item->child(i)->setInvalid();
//				}
                    setVisibleFlagToSelfAndChildren( item );
                }
                //the tree width can change, thus recompute it
                updateSizes();
            }
        }

        if ( to_recompute )
        {
            tabWidget->computeValues();
        }
        event->accept();
        repaint();
    }
    else
    {
        event->ignore();
    }
}
//end of mousePressEvent




/************* miscellaneous *************/


//set all values of all items to 0.0
//
void
MyTreeWidget::initializeValues()
{
    for ( unsigned i = 0; i < items.size(); i++ )
    {
//	  if (items[i]->isInvalid())
//	  {
        items[ i ]->totalValue = 0.0;
        items[ i ]->ownValue   = 0.0;
        items[ i ]->rootValue  = 0.0;
        if ( type == SYSTEMTREE )
        {
            items[ i ]->minValue = 0.0;
            items[ i ]->maxValue = 0.0;
        }
    }
//  }

    for ( unsigned i = 0; i < top.size(); i++ )
    {
        top[ i ]->setVisible();
    }
}

//searches for item containing a user-defined string in their names
//and marks those items by a distinguished background color
//
std::vector<TreeWidgetItem*>
MyTreeWidget::findItems( QString text )
{
    QRegExp regExp( text );
    assert( regExp.isValid() );

    std::vector<TreeWidgetItem*> items;
    std::vector<TreeWidgetItem*> result;

    //collect non-hidden roots in items
    for ( int i = 0; i < ( int )top.size(); i++ )
    {
        if ( !top[ i ]->isHidden() )
        {
            items.push_back( top[ i ] );
        }
    }

    //recursively collect non-hidden children in "items" and collect in
    //"result" those items from "items" whose name contains the given
    //string
    for ( int i = 0; i < ( int )items.size(); i++ )
    {
        if ( regExp.indexIn( items[ i ]->name ) != -1 )
        {
            result.push_back( items[ i ] );
        }
        for ( int j = 0; j < ( int )items[ i ]->childCount(); j++ )
        {
            if ( !items[ i ]->child( j )->isHidden() )
            {
                items.push_back( items[ i ]->child( j ) );
            }
        }
    }
    return result;
}


//sort items by name or by value, ascending or descending, using
//bubble sort
//
void
MyTreeWidget::sortItems( bool sortByName, bool ascending )
{
    bool ready = false;
    while ( !ready )
    {
        ready = true;
        for ( int i = 0; i < ( int )top.size() - 1; i++ )
        {
            bool toChange;
            if ( sortByName )
            {
                toChange = ( ascending ?
                             top[ i ]->name.compare( top[ i + 1 ]->name ) > 0 :
                             top[ i ]->name.compare( top[ i + 1 ]->name ) < 0 );
            }
            else
            {
                toChange = ( ascending ?
                             top[ i ]->getValue() > top[ i + 1 ]->getValue() :
                             top[ i ]->getValue() < top[ i + 1 ]->getValue() );
            }
            if ( toChange )
            {
                TreeWidgetItem* tmp = top[ i ];
                top[ i ]     = top[ i + 1 ];
                top[ i + 1 ] = tmp;
                ready        = false;
            }
        }
    }
    repaint();
}



//Pavels addition
//Tooltip handler

bool
MyTreeWidget::event( QEvent* event )
{
    if ( event->type() == QEvent::MouseMove )
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>( event );

        MouseOver = QPoint( mouseEvent->x(), mouseEvent->y() );
    }

// if (event->type() == QEvent::ToolTip) {
//     QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
//
//     int index = MouseOver.y() / lineHeight;
//     //get the item at that index;
//     //if the click was outside of any item row, item gets NULL
//     TreeWidgetItem* itemover = getItemAtIndex(index);
//
//     if (itemover != 0)
//     {
// // get inheritance of item and build a string "name of name of name of"
//
//         QString text = "";
//         TreeWidgetItem* currentitem = itemover;
//         int deepness=0;
//         while (currentitem != 0)
//         {
//             if (deepness==0)
//                 text = text.append(currentitem->getText());
//             else
//                 text = text.append("\n            of\n ").append(currentitem->getText());
//             currentitem = currentitem->parent();
//             deepness++;
//         }
//
//
//
//
//         QToolTip::showText(helpEvent->globalPos(), text);
//     }
//     else
//     QToolTip::hideText();
// }
    return QWidget::event( event );
}

void
MyTreeWidget::setVisibleFlagToSelfAndChildren( TreeWidgetItem* item )
{
    if ( item->isExpanded() )
    {
        item->setVisible();
        for ( int i = 0; i < item->childCount(); i++ )
        {
            item->child( i )->setVisible();
            setVisibleFlagToSelfAndChildren( item->child( i ) );
        }
    }
}


void
MyTreeWidget::setInvisibleFlagToChildren( TreeWidgetItem* item )
{
    for ( int i = 0; i < item->childCount(); i++ )
    {
        item->child( i )->setInvisible();
        setInvisibleFlagToChildren( item->child( i ) );
    }
}
