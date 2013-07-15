/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef SYSTEMBOXPLOT_H
#define SYSTEMBOXPLOT_H

#include <QWidget>

#include "BoxPlot.h"
#include "TreeWidget.h"
#include "ValueWidget.h"

class SystemBoxPlot : public BoxPlot
{
public:
    SystemBoxPlot( QWidget*,
                   TreeWidget* );
    void
    updateValues();
    void
    updateValueWidget();

    int
    getOptimalWidth()
    {
        return 400;
    };



private:
    QString
    getSelectedLabels( TreeWidget* tree );
    Item
    calculateStatistics( bool absolute );

    TreeWidget*      systemTreeWidget; // contains data to display
    PrecisionWidget* prec;
    ValueWidget*     valueWidget;
};

#endif // SYSTEMBOXPLOT_H
