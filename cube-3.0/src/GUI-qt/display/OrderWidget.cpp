/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "OrderWidget.h"

#define DIMS 3
OrderWidget::OrderWidget( const std::vector<long> &dims,
                          const QStringList &      dimnames )
{
    this->dims     = dims;
    this->dimnames = dimnames;
    cellheight     = 20;
    labelwidth     = cellheight + 10;

    moveX = -1; // none selected to move
    //this->setFrameStyle(QFrame::Box);

    // initialize positions
    ndim = dimnames.size();
    place.assign( DIMS, std::vector<int>( std::max( ndim - 1, 1 ) ) );
    for ( uint i = 0; i < place.size(); i++ )
    {
        for ( uint j = 0; j < place[ i ].size(); j++ )
        {
            place[ i ][ j ] = -1;
        }
    }
    for ( int i = 0; i < ndim; i++ )
    {
        place[ i % DIMS ][ i / DIMS ] = i;
    }

    // check if labels can be displayes
    int count = 0;
    for ( int i = 0; i < dimnames.size(); ++i )
    {
        int len = dimnames.at( i ).length();
        if ( ( len > 0 ) && ( len <= 2 ) )
        {
            count++;
        }
    }
    if ( count == dimnames.size() )
    {
        for ( int i = 0; i < dimnames.size(); ++i )
        {
            this->dimnames.append( dimnames.at( i ) );
        }
    }
    else
    {
        for ( int i = 0; i < dimnames.size(); ++i )
        {
            this->dimnames.append( QString::number( i + 1 ) );
        }
    }
}

QSize
OrderWidget::minimumSizeHint() const
{
    //int h = this->fontMetrics().height();
    return QSize( ndim * 2 * cellheight, DIMS * cellheight + 1 );
}

/**
   returns the selected order: vector with 3 elements
   each element contains the dimension numbers, which will be merged
 */
std::vector<std::vector<int> >
OrderWidget::getFoldingVector() const
{
    std::vector<std::vector<int> > val;

    int                            count = 0;
    for ( uint i = 0; i < place.size(); i++ )
    {
        std::vector<int> line;
        for ( uint j = 0; j < place[ i ].size(); j++ )
        {
            int dim = place[ i ][ j ];
            if ( dim >= 0 )
            {
                count++;
                line.push_back( dim );
            }
        }
        //if ( line.size() > 0 ) // remove dimensions without elements
        {
            val.push_back( line );
        }
    }

    return val;
}

void
OrderWidget::setFoldingVector( std::vector<std::vector<int> > fold )
{
    for ( uint i = 0; i < place.size(); i++ )
    {
        for ( uint j = 0; j < place[ i ].size(); j++ )
        {
            place[ i ][ j ] = -1;
        }
    }

    for ( uint i = 0; i < fold.size(); i++ )
    {
        for ( uint j = 0; j < fold[ i ].size(); j++ )
        {
            place[ i ][ j ] = fold[ i ][ j ];
        }
    }

    repaint();
}

void
OrderWidget::mousePressEvent( QMouseEvent* event )
{
    int x    = event->x();
    int y    = event->y();
    int xpos = ( x - labelwidth ) / cellwidth;
    int ypos = y / cellheight;
    moveX = -1;

    if ( ( ypos < 0 ) || ( ypos >= DIMS ) )
    {
        return;
    }
    bool elementClicked = true;
    if ( ( x - labelwidth < 0 ) || ( xpos >= ndim - 1 ) )
    {
        elementClicked = false;
    }
    else if ( place[ ypos ][ xpos ] < 0 )
    {
        elementClicked = false;
    }

    if ( elementClicked )
    {
        if ( event->button() == Qt::LeftButton )   // select element
        {
            moveX      = xpos;
            moveY      = ypos;
            currentPos = event->pos();
        }
        else     // show tooltip
        {
            int     dim = place[ ypos ][ xpos ];
            QString txt = dimnames[ dim ] + ": size " + QString::number( dims[ dim ] );
            QPoint  localMouse( x, y );
            QPoint  mousePos = this->mapToGlobal( localMouse );
            QToolTip::showText( mousePos, txt );
        }
    }
}

void
OrderWidget::mouseMoveEvent( QMouseEvent* event )
{
    currentPos = event->pos();
    repaint();
}

/**
   elements are swapped, if mouse is pressed on one element and released on onother
 */
void
OrderWidget::mouseReleaseEvent( QMouseEvent* )
{
    if ( moveX < 0 )
    {
        return;             // no element selected
    }
    int xnew = ( currentPos.x() - labelwidth ) / cellwidth;
    int ynew = currentPos.y() / cellheight;

    if ( ( xnew >= 0 ) && ( xnew < ndim - 1 )  && // new position valid?
         ( ynew >= 0 ) && ( ynew < DIMS ) )
    {
        int oldval = place[ ynew ][ xnew ];
        place[ ynew ][ xnew ]   = place[ moveY ][ moveX ];
        place[ moveY ][ moveX ] = oldval;
        emit foldingDimensionsChanged();
    }
    //qDebug() << "from " << moveX << moveY << "to " << xnew << ynew;
    moveX = -1;

    repaint();
}

void
OrderWidget::drawElement( QPainter &painter, int x, int y, int elemnr )
{
    int   w      = rect().height() / 20;
    int   height = cellheight;
    QRect rect( x + w, y + w, cellwidth - 2 * w, height - 2 * w );

    painter.setPen( Qt::black );

    painter.fillRect( x + w, y + w, cellwidth - 2 * w, height - 2 * w, Qt::gray );
    painter.drawRect( x + w, y + w, cellwidth - 2 * w, height - 2 * w );
    painter.drawText( rect, Qt::AlignCenter, dimnames.at( elemnr ) );
}

void
OrderWidget::paintEvent( QPaintEvent* )
{
    QString  label[] = { "x", "y", "z" };
    cellwidth = ( rect().width() - 1 - labelwidth ) / ( ndim - 1 );
    QPainter painter( this );

    int      x = 0;
    int      y = 0;
    for ( int i = 0; i < DIMS; i++ )
    {
        for ( int j = 0; j < ndim - 1; j++ )
        {
            x = labelwidth + j * cellwidth;
            int   w = cellwidth;
            QRect r( x, y, w, cellheight );

            painter.setPen( Qt::gray );
            painter.drawRect( r );
            painter.drawLine( r.x(), r.y(), r.x() + w, r.y() + cellheight );
            painter.drawLine( r.x() + w, r.y(), r.x(), r.y() + cellheight );
            if ( place[ i ][ j ] >= 0 )   // draw existing elements
            {
                drawElement( painter, x, y, place[ i ][ j ] );
            }
        }
        painter.setPen( Qt::black );
        painter.drawRect( QRect( 0, y, labelwidth, cellheight ) );
        painter.drawRect( QRect( 0, y, labelwidth + ( ndim - 1 ) * cellwidth, cellheight ) );
        painter.drawText( QRect( 0, y, labelwidth, cellheight ), Qt::AlignCenter, label[ i ] );
        y += cellheight;
    }

    // draw element which is beeing moved on top
    if ( moveX >= 0 )
    {
        int x = currentPos.x() - cellwidth / 2;
        int y = currentPos.y() - cellheight / 2;
        drawElement( painter, x, y, place[ moveY ][ moveX ] );
    }
}
