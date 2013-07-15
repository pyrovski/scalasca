/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef TOPOLOGYDIMENSIONBAR_H
#define TOPOLOGYDIMENSIONBAR_H

#include <QtGui>
#include "OrderWidget.h"
#include "DimensionSelectionWidget.h"

/**
   creates a button bar for topologies with more than tree dimensions
   view1: create ValuePopSlider widgets to select the dimensions to display
   view2: merge selected dimensions into one
 */
class TopologyDimensionBar : public QWidget
{
    Q_OBJECT
public:
    explicit
    TopologyDimensionBar( const std::vector<long> &       dims,
                          const std::vector<std::string> &dimnames,
                          QWidget*                        parent = 0 );
    std::vector<std::vector<int> >
    getFoldingVector();

    bool
    loadSettings( QSettings &settings,
                  int        topologyId );
    void
    saveSettings( QSettings &settings,
                  int        topologyId );
signals:
    void
    foldingDimensionsChanged( std::vector<std::vector<int> >);
    void
    selectedDimensionsChanged( std::vector<long>);

private slots:
    void
    selectedDimensionsChanged();
    void
    foldingDimensionsChanged();

private:
    void setAxisLabel(int dims);
    std::vector<long>         dims_;
    OrderWidget*              order;         // used for folding mode
    DimensionSelectionWidget* selection;
    QStackedLayout*           modeSelection; // select view: selectionWidget or foldingWidget
    QRadioButton*             foldBut;
    QRadioButton*             sliceBut;
    QLabel *asisLabel;
};

#endif // TOPOLOGYDIMENSIONBAR_H
