/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "DimensionSelectionWidget.h"

#define DIMS 3
#define ALL -1

DimensionSelectionWidget::DimensionSelectionWidget( const std::vector<long> &dims,
                                                    const QStringList &      dimnames )
{
    this->dims     = dims;
    this->dimnames = dimnames;

    QVBoxLayout* vertical = new QVBoxLayout( this );
    vertical->setContentsMargins( 0, 0, 0, 0 );

    QWidget*     table = new QWidget();
    QGridLayout* grid  = new QGridLayout( table );
    grid->setContentsMargins( 3, 0, 3, 0 );
    grid->setVerticalSpacing( 0 );
    for ( uint i = 0; i < dims.size(); i++ )
    {
        int               value   = ( i >= 3 ) ? 0 : ALL;
        ValuePopupSlider* vslider = new ValuePopupSlider( value, (int) dims[ i ] - 1 );
        slider.push_back( vslider );
        connect( vslider, SIGNAL( valueChanged( int ) ), this, SLOT( selectionChanged() ) );
        sliders.push_back( vslider );
        grid->addWidget( vslider, 0, i + 1 );
        QLabel* label = new QLabel( dimnames.at( i ) );
        grid->addWidget( label, 1, i + 1, Qt::AlignHCenter );
    }

    order = new AxisOrderWidget( (int) dims.size() );
    connect( order, SIGNAL( orderChanged() ), this, SLOT( orderChanged() ) );

    std::vector<long> sel = getCurrentSelection();
    order->setSelectionVector( sel );

    vertical->addWidget( table );
    vertical->addWidget( order );
}

/**
   returns the user selections for all dimensions
   -1 to -3 = display all elements of current dimension in dimsion 1 to 3
   >=0 = display one element
 */
std::vector<long>
DimensionSelectionWidget::getSelectionVector()
{
    return order->getSelectionOrderVector();
}

void
DimensionSelectionWidget::setSelectionVector( std::vector<long> sel )
{
    for ( uint i = 0; i < sel.size(); i++ )
    {
        slider[ i ]->blockSignals( true );
        slider[ i ]->setValue( (int) sel[ i ] );
        slider[ i ]->blockSignals( false );
    }
    order->setSelectionVector( sel, true );
}

void
DimensionSelectionWidget::orderChanged()
{
    emit selectedDimensionsChanged();
}

void
DimensionSelectionWidget::selectionChanged()
{
    std::vector<long> sel = getCurrentSelection();
    order->setSelectionVector( sel );

    emit selectedDimensionsChanged();
}


/** retreives values from sliders (ALL or index) */
std::vector<long>
DimensionSelectionWidget::getCurrentSelection()
{
    std::vector<long> ret;

    int               count = 0;
    for ( uint i = 0; i < sliders.size(); i++ )
    {
        long val = sliders.at( i )->value();
        if ( val <= ALL )
        {
            count++;
        }
        ret.push_back( val );
    }
    if ( ( count != 3 ) && ( count != 2 ) )
    {
        ret.clear(); // not valid, show 2-3 dimensions
    }
    return ret;
}

//======================================================================================
//======================================================================================

AxisOrderWidget::AxisOrderWidget( uint n )
{
    ndims = n;

    cellheight = 20;
    labelwidth = cellheight + 10;
    move       = -1; // none selected to move
}

QSize
AxisOrderWidget::minimumSizeHint() const
{
    // width is controlled by slider panel
    return QSize( 1, cellheight + 1 );
}

const std::vector<long> &
AxisOrderWidget::getSelectionOrderVector()
{
    return dims;
}

/**
   @param seldims contains negative values, if all elements of a dimension will be shown
   @param hasAxisInfo if true, the negative values of seldims also contain the information,
         if the dimension is shown on x, y, or z axis
 */
void
AxisOrderWidget::setSelectionVector( const std::vector<long> &seldims, bool hasAxisInfo )
{
    std::vector<long> old( dims );
    this->dims = seldims;

    if ( dims.size() == 0 )   // selection is not valid
    {
        shownDimensions = 0;
        return;
    }

    /* sets x, y or z axis */
    if ( !hasAxisInfo )
    {
        int shown = 0; // number of displayed dimensions
        for ( int i = 0; i < ndims; i++ )
        {
            if ( dims[ i ] <= ALL )
            {
                shown++;
            }
        }

        if ( shown != shownDimensions )   // changed 2D <-> 3D => reset axis enumeration
        {
            shownDimensions = shown;
            shown           = 0;
            for ( int i = 0; i < ndims; i++ )
            {
                if ( dims[ i ] <= ALL )
                {
                    dims[ i ] = -( shown + 1 ); // enumerate the axis x,y,z with negative numbers
                    shown++;
                }
            }
        }
        else // keep axis enumeration, only value has changed
        {
            for ( int i = 0; i < ndims; i++ )
            {
                if ( dims[ i ] <= ALL )
                {
                    dims[ i ] = old[ i ];
                }
            }
        }
    }
    repaint();
}

void
AxisOrderWidget::drawElement( QPainter &painter, int x, int y, int elemnr )
{
    QString label[] = { "x", "y", "z" };
    int     w       = rect().height() / 7;
    int     height  = cellheight;
    QRect   rect( x + w, y + w, cellwidth - 2 * w, height - 2 * w );

    painter.setPen( Qt::black );

    painter.fillRect( x + w, y + w, cellwidth - 2 * w, height - 2 * w, Qt::gray );
    painter.drawRect( x + w, y + w, cellwidth - 2 * w, height - 2 * w );
    painter.drawText( rect, Qt::AlignCenter, label[ elemnr ] );
}

void
AxisOrderWidget::paintEvent( QPaintEvent* )
{
    cellwidth = ( rect().width() - 1 ) / ( ndims );
    QPainter painter( this );

    if ( ( int )dims.size() != ndims )
    {
        return;
    }

    int x = 0;
    int y = 0;
    for ( int i = 0; i < ndims; i++ )
    {
        x = i * cellwidth;

        if ( dims[ i ] <= ALL )   // draw existing elements
        {
            drawElement( painter, x, y, (int) -dims[ i ] - 1 );
            painter.setPen( Qt::black );
            painter.drawRect( QRect( x, y, cellwidth, cellheight ) );
        }
    }

    // draw element which is beeing moved on top
    if ( move >= 0 )
    {
        int x = currentPos.x() - cellwidth / 2;
        drawElement( painter, x, 0, (int) -dims[ move ] - 1 );
    }
}


void
AxisOrderWidget::mousePressEvent( QMouseEvent* event )
{
    int x       = event->x();
    int clicked = ( x ) / cellwidth;

    move = -1;
    bool elementClicked = true;
    if ( ( x - labelwidth < 0 ) || ( clicked >= ndims ) )
    {
        elementClicked = false;
    }
    else if ( dims[ clicked ] >= 0 )
    {
        elementClicked = false;
    }

    if ( elementClicked )
    {
        if ( event->button() == Qt::LeftButton )   // select element
        {
            move       = clicked;
            currentPos = event->pos();
        }
    }
}

void
AxisOrderWidget::mouseMoveEvent( QMouseEvent* event )
{
    currentPos = event->pos();
    repaint();
}

/**
   elements are swapped, if mouse is pressed on one element and released on onother
 */
void
AxisOrderWidget::mouseReleaseEvent( QMouseEvent* )
{
    if ( move < 0 )
    {
        return;            // no element selected
    }
    int dest = ( currentPos.x() ) / cellwidth;
    if ( ( dest >= 0 ) && ( dest < ndims ) && ( dims[ dest ] <= ALL ) )  // new position valid?
    {
        long old = dims[ dest ];
        dims[ dest ] = dims[ move ];
        dims[ move ] = old;
    }
    move = -1;

    repaint();
    emit orderChanged();
}

//======================================================================================
//======================================================================================

ValuePopupSlider::ValuePopupSlider( int value,
                                    int max ) : value_( value ), max_( max )
{
    win  = 0;
    push = new QPushButton();
    push->setMinimumWidth( push->fontMetrics().width( "__999__" ) );
    setValue( value );

    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    setLayout( layout );
    layout->addWidget( push );

    connect( push, SIGNAL( clicked() ), this, SLOT( showSlider() ) );
}

void
ValuePopupSlider::setValue( int val )
{
    value_ = val;
    if ( val >= 0 )
    {
        push->setText( QString::number( val ) );
    }
    else
    {
        push->setText( "all" );
    }
    emit valueChanged( val );
}

int
ValuePopupSlider::value() const
{
    return value_;
}

void
ValuePopupSlider::showSlider()
{
    if ( win && win->isVisible() )
    {
        win->close();
        delete win;
        win = 0;
    }
    else
    {
        win = new SliderPopup( -1, max_, value_ );
        connect( win->slider_, SIGNAL( valueChanged( int ) ), this, SLOT( setValue( int ) ) ); // set label
        win->show();
        QPoint pos = push->mapToGlobal( QPoint( 0, -win->size().height() ) );
        win->move( pos ); // show slider widget above the button
    }
}

//======================================================================================
//======================================================================================

/** label used in Slider popup */
class SliderPopupLabel : public QWidget
{
public:
    explicit
    SliderPopupLabel( int max ) : max_( max )
    {
    }
    virtual QSize
    minimumSizeHint() const
    {
        return QSize( this->fontMetrics().width( "all" ), 0 );
    }
protected:
    virtual void
    paintEvent( QPaintEvent* )
    {
        QPainter p( this );
        p.drawText( rect(), Qt::AlignTop, QString::number( max_ ) );
        p.drawText( 0, size().height(), "all" );
    }
private:
    int max_;
};

SliderPopup::SliderPopup( int min,
                          int max,
                          int value )
{
    setWindowFlags( Qt::Popup );

    slider_ = new QSlider( this );
    slider_->setOrientation( Qt::Vertical );
    QHBoxLayout* layout = new QHBoxLayout( this );
    setLayout( layout );

    slider_->setMinimum( min );
    slider_->setMaximum( max );
    slider_->setValue( value );

    layout->addWidget( slider_ );
    layout->addWidget( new SliderPopupLabel( max ) );
    connect( slider_, SIGNAL( sliderReleased() ), this, SLOT( close() ) );
}
