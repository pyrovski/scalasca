/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _SYSTEMTOPOLOGYDATA_H
#define _SYSTEMTOPOLOGYDATA_H

#include <vector>
#include <math.h>
#include <iostream>
#include <map>

#include "TreeWidget.h"
#include "Constants.h"
#include "Cube.h"

class SystemTopologyData : public QObject
{
    Q_OBJECT
signals:
    void
    dataChanged();
    void
    viewChanged();
    void
    rescaleRequest();

public slots:
    void
    selectedDimensionsChanged( const std::vector<long> &dims );
    void
    foldingDimensionsChanged( std::vector<std::vector<int> >);

public:
    SystemTopologyData( TreeWidget* systemTreeWidget,
                        unsigned    topologyId );
    ~SystemTopologyData();

    // --------------------
    void
    initialize( cube::Cube* cube ); //initialize topology from the cube object
    void
    updateColors();                 //re-compute the colors
    bool
    updateSelection();              //re-mark selected items of topology

    // ------------------
    bool
    hasInvalidDimensions() const
    {
        return invalidDimensions;
    }
    const unsigned*
    getFoldingSeparators() const
    {
        if ( selectMode == FOLD )
        {
            return foldingDiv;
        }
        else
        {
            return 0;
        }
    }
    unsigned
    getDim( int i ) const;
    bool
    isSelected( int x,
                int y,
                int z ) const;
    unsigned
    getSystemId( int x,
                 int y,
                 int z ) const;
    QColor*
    getColor( int x,
              int y,
              int z ) const;
    QStringList
    getTooltipText( int x,
                    int y,
                    int z );

    //set user-defined minimal and maximal values for the minimal and maximal colors
    void
    setMinMaxValues( double minValue,
                     double maxValue );

    //set the color for the lines framing processes/threads in the topology
    LineType
    getLineType()
    {
        return lineType;
    }
    void
    setLineType( LineType lineType );

    //toggle the flag if unused planes should be painted or not
    void
    toggleUnused();

    void
    setFoldingDimensions( std::vector<std::vector<int> > fdims )
    {
        foldingDimensions = fdims;
    }
    void
    setUserDefinedMinMaxValues( bool val )
    {
        userDefinedMinMaxValues = val;
    }

    bool
    getUserDefinedMinMaxValues() const
    {
        return userDefinedMinMaxValues;
    }
    double
    getMinValue() const
    {
        return minValue;
    }
    double
    getMaxValue() const
    {
        return maxValue;
    }
    double
    getUserMinValue() const
    {
        return userMinValue;
    }
    double
    getUserMaxValue() const
    {
        return userMaxValue;
    }
    double
    getMinAbsValue() const
    {
        return minAbsValue;
    }
    double
    getMaxAbsValue() const
    {
        return maxAbsValue;
    }
    // view
    bool
    getAntialiasing() const
    {
        return systemTreeWidget->getTabWidget()->getAntialiasing();
    }

    int
    getNeighbors( int x,
                  int y,
                  int z );

    bool
    hasUniqueIds()
    {
        return uniqueIds;
    }

private:
    LineType lineType;

    // ------ data
    // the dimensions to draw (1-3)
    unsigned dim[ 3 ];
    // mark merged dimensions (draw separator), if two dimensions are merged
    unsigned foldingDiv[ 3 ];

    //the cube object for the data
    cube::Cube* cube;

    //the index of this topology in the cube object
    unsigned topologyId;

    //the topology itself: assignment of items to coordinates (without changed dim order)
    std::vector<std::vector<std::vector<int> > >    coordToSystemId;
    // assignment of displayed coordinates to items
    std::map<int, std::vector<std::vector<long> > > systemToCoord;
    // if more than 3 dimensions: assignment of complete coordinates to items
    std::vector<QPair<QString, QString> >           systemToCoordStr;
    // used if one system id maps to several coordinates: contains neighbors with different id
    QMap<QString, int>                              neighbors;
    bool                                            uniqueIds; // system contains neighbors with same system id
    bool                                            isInitialised;

    // for topologies with more than 3 dimensions:
    // value -1: show all elements, other: show only the given index
    std::vector<long> selectedDimensions;
    // 1. vector: displayed dimensions (2-3)
    // 2. vector: original dimension which are merged into current dimension
    std::vector<std::vector<int> > foldingDimensions;

    // colors for the topology items
    std::vector<std::vector<std::vector<QColor*> > > colors;
    // flags whether an item is selected or not.
    std::vector<std::vector<std::vector<bool> > >    selected_rects;

    //minimal and maximal current and abolute values
    double minValue, maxValue, minAbsValue, maxAbsValue;
    //for user-defined values for the minimal and maximal colors
    bool   userDefinedMinMaxValues;
    double userMinValue, userMaxValue;

    typedef enum { SELECT, FOLD } Mode;
    Mode        selectMode;        // if > 2 dimensions: call multiDimFold or multiDimSelect
    bool        invalidDimensions; // selection in SELECT-mode is not valid

    TreeWidget* systemTreeWidget;

    //-----------------------------------------------------------------

    QColor*
    getColorForId( unsigned id );
    void
    rotateTo( const QPoint &endPoint );
    void
    setDims( unsigned dim1,
             unsigned dim2,
             unsigned dim3 );
    void
    multiDimSelect( const std::vector<long>& coord,
                    unsigned                 id );
    void
    multiDimFold( const std::vector<long>& coord,
                  unsigned                 id );
    void
    reinit();
    void
    determineNeighbors();

    QPair<QString, QString>
    coordToString( const std::vector<long> &coord );

    QPair<QString, QString>
    getOriginalCoordString( const std::vector<long> &coord3D );
};

#endif
