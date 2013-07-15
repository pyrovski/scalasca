/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "SystemBoxPlot.h"
#include <qdebug.h>
#include <algorithm>
#include <cmath>

using namespace std;


/**
   creates a bot plot which displays the values given by TreeWidget
 */
SystemBoxPlot::SystemBoxPlot( QWidget*    widget,
                              TreeWidget* t ) :
    BoxPlot( widget, t->getTabWidget()->getPrecisionWidget() )
{
    this->systemTreeWidget = t;
    this->prec             = t->getTabWidget()->getPrecisionWidget();
    this->valueWidget      = t->getTabWidget()->getValueWidget();

    setWhatsThis(
        "This Boxplot shows the distribution of values in the system tree."
        " It starts with the smallest value (minimum) and ends with the largest value (maximum)."
        " The bottom and top of the box represent the lower quartile (Q1) and the upper quartile (Q3)."
        " The bold line represents the median (Q2) and the dashed line represents the mean value."
        " To zoom into the boxplot, select an area with the mouse."
        " To zoom out, click with the middle mouse button in the drawing."
        );
}

void
SystemBoxPlot::updateValues()
{
    Item   item = calculateStatistics( false );
    Item   abs  = calculateStatistics( true );

    double start;
    if ( systemTreeWidget->tabWidget->getValueModus() == ABSOLUTE )
    {
        start = item.min;
    }
    else
    {
        start = 0.0;
    }

    // fill box plot with calculated data
    this->Set( item, abs, start );
}

// calculate statistical values
Item
SystemBoxPlot::calculateStatistics( bool absolute )
{
    int                     theCount;
    double                  theSum;
    double                  theMean;
    double                  theVariance;
    double                  theMin;
    double                  theMax;
    double                  theMedian;
    double                  theQ25;
    double                  theQ75;

    vector<TreeWidgetItem*> items = systemTreeWidget->getActiveSubset();
    vector<double>          values;

    theSum = 0.;
    double value;
    bool   ok;
    for ( unsigned i = 0; i < items.size(); i++ )
    {
        TreeWidgetItem* item = items.at( i );
        // single-threaded processes or thread
        if ( item->childCount() == 0 )
        {
            if ( absolute )
            {
                systemTreeWidget->getValue( item, item->isExpanded(), value, ok, ABSOLUTE );
            }
            else
            {
                value = item->getValue();
            }
            values.push_back( value );
            theSum += value;
        }
    }

    theCount = values.size();

    if ( theCount == 0 )
    {
        theCount = 0;
        this->Reset();
        return Item( "", 0, 0, 0, 0, 0, 0, 0, 0, 0 ); // not enough values to display
    }
    theMean = theSum / values.size();

    theVariance = 0.;
    for ( unsigned i = 0; i < values.size(); i++ )
    {
        theVariance += ( values.at( i ) - theMean ) * ( values.at( i ) - theMean );
    }
    if ( values.size() > 0 )
    {
        theVariance /= ( values.size() );
    }

    sort( values.begin(), values.end() );
    theMin    = *values.begin();
    theMax    = *( values.end() - 1 );
    theMedian = *( values.begin() + values.size() / 2 );
    theQ25    = *( values.begin() + values.size() / 4 );
    theQ75    = *( values.begin() + values.size() * 3 / 4 );

    return Item( "", theCount, theSum, theMean, theMin, theQ25, theMedian, theQ75, theMax, theVariance );
}

/**
   updates the value widget below the tab widget
 */
void
SystemBoxPlot::updateValueWidget()
{
    vector<double> values;
    Item           item;

    if ( systemTreeWidget->getTabWidget()->getValueModus() == ABSOLUTE )
    {
        item = calculateStatistics( true );
    }
    else
    {
        item = calculateStatistics( false );
    }

    /* from SystemTopologyWidget */
    //in case the topology is empty or if no items are selected, just
    //clear the value widget
    if ( item.count == 0 )
    {
        valueWidget->clear();
    }
    else
    {
        double sigma   = sqrt( item.variance );
        bool   intType = systemTreeWidget->hasIntegerType();
        double displayedValue;

        if ( systemTreeWidget->getTabWidget()->getValueModus() == ABSOLUTE )
        {
            displayedValue = item.mean;
            // in the absolute value mode display the mean value (with the min/max values)
            valueWidget->update( item.min, item.max, displayedValue, intType, false, item.mean, sigma );
        }
        else
        {
            displayedValue = 100; // show +/- sigma
            //in value modes other that the absolute value mode display the
            //current minvalue/value/maxvalue plus absolute
            //minvalue/value/maxvalue
            valueWidget->update( 0.0, 100.0, displayedValue,
                                 item.min, item.max, displayedValue, intType, false, item.mean, sigma );
        }
    }
}
