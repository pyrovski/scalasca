/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef DIMENSIONSELECTIONWIDGET_H
#define DIMENSIONSELECTIONWIDGET_H

#include <QtGui>
#include <vector>

class SliderPopup;
class ValuePopupSlider;
class AxisOrderWidget;

/**
   Widget to select the dimensions and to assign an axis to each selected dimension
 */
class DimensionSelectionWidget : public QFrame
{
    Q_OBJECT
public:
    explicit
    DimensionSelectionWidget( const std::vector<long> &ndim,
                              const QStringList &      dimnames );
    std::vector<long>
    getSelectionVector();
    void
    setSelectionVector( std::vector<long> );

signals:
    void
    selectedDimensionsChanged();

private slots:
    void
    orderChanged();
    void
    selectionChanged();

private:
    std::vector<long>
    getCurrentSelection();

    std::vector<ValuePopupSlider*> sliders;  // used for selection view
    void
    drawElement( QPainter &painter,
                 int       x,
                 int       y,
                 int       elemnr );

    int                            ndim; // number of total dims
    std::vector<long>              dims;
    std::vector<ValuePopupSlider*> slider;
    QStringList                    dimnames;
    AxisOrderWidget*               order;
};

/**
 * @brief The AxisOrderWidget class
 * allow the user to change the axis order of the displayed dimensions (ALL selected)
 */
class AxisOrderWidget : public QWidget
{
    Q_OBJECT
public:
    AxisOrderWidget( uint ndims );
    void
    paintEvent( QPaintEvent* );
    virtual QSize
    minimumSizeHint() const;
    void
    setSelectionVector( const std::vector<long> &dims,
                        bool                     hasAxisInfo = false );

    const std::vector<long> &
    getSelectionOrderVector();

protected:
    void
    mousePressEvent( QMouseEvent* event );
    void
    mouseMoveEvent( QMouseEvent* event );
    void
    mouseReleaseEvent( QMouseEvent* event );

signals:
    void
    orderChanged();

private:
    int               ndims;           // number of total dims
    int               shownDimensions; // number of dimensions which are shown
    std::vector<long> dims;            // vector which contains the index of the selected elements or -1 for complete dimension

    QPoint            currentPos;      // current position of mouse while dragging
    int               cellwidth;       // width of each element
    int               cellheight;      // height of each dimension cell
    int               labelwidth;      // width of the legend
    int               move;            // index of element to move
    int               moveY;

    void
    drawElement( QPainter &painter,
                 int       x,
                 int       y,
                 int       elemnr );
};


/**
   creates a widget which displays an integer value or the text "all" if value < 0
   if the widget is clicked, a slider widget appears to change the value
 */
class ValuePopupSlider : public QWidget
{
    Q_OBJECT
public:
    /**
       @param value the displayed value
       @param maximum the maximum value of the widget/slider
     */
    ValuePopupSlider( int value,
                      int max );
    int
    value() const;

public slots:
    void
    setValue( int );

private slots:
    void
    showSlider();

signals:
    void
    valueChanged( int );

private:
    int          value_;
    int          max_;
    QPushButton* push;
    SliderPopup* win;
    friend class TopologyDimensionBar;
};

/**
   part of ValuePopupSlider: a pop widget which contains a slider
 */
class SliderPopup : public QWidget
{
    Q_OBJECT
public:
    explicit
    SliderPopup( int min,
                 int max,
                 int value );
private:
    QSlider* slider_;
    friend class ValuePopupSlider;
};

#endif // DIMENSIONSELECTIONWIDGET_H
