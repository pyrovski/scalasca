/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
// Controller

#ifndef _SYSTEMTOPOLOGYWIDGET_H
#define _SYSTEMTOPOLOGYWIDGET_H

#include <QScrollArea>
#include <vector>

#include "TreeWidget.h"
#include "Cube.h"
#include "SystemTopologyData.h"
#include "SystemTopologyView.h"
#include "SystemTopologyViewTransform.h"

class ScrollArea;
class QSettings;

class SystemTopologyWidget : public QScrollArea
{
    Q_OBJECT

signals:
    void
    xAngleChanged( int angle );
    void
    yAngleChanged( int angle );

public slots:
    void
    selectItem( int systemID );

public:

    SystemTopologyWidget( TreeWidget* systemTreeWidget,
                          unsigned    topologyId );
    ~SystemTopologyWidget();

    //initialize topology from the cube object
    void
    initialize( cube::Cube* cube );

    //re-compute the colors
    void
    updateColors();

    //update the colors and repaint
    void
    displayItems();

    //remove all data
    void
    cleanUp();

    //rescale the topology sizes such that the topology fits into the scroll widget's window
    void
    rescale();
    int
    getOptimalWidth();      // ????

    //save and load topology settings
    void
    saveSettings( QSettings& settings );
    bool
    loadSettings( QSettings& settings );

    //update the value widget
    void
    updateValueWidget();

    //set the color for the lines framing processes/threads in the topology
    void
    setLineType( LineType lineType );

    //toggle the flag if unused planes should be painted or not
    void
    toggleUnused();

    QWidget*
    getDimensionSelectionBar( cube::Cube* cube );
    QWidget*
    getWidget();

    SystemTopologyViewTransform*
    getTransform()
    {
        return transform;
    }
    SystemTopologyData*
    getData()
    {
        return data;
    }

protected:
    void
    resizeEvent( QResizeEvent* event );

private:
    typedef enum { SELECT, FOLD } Mode;
    Mode                         selectMode; // if > 2 dimensions: call multiDimFold or multiDimSelect

    TreeWidget*                  systemTreeWidget;
    TopologyDimensionBar*        dimensionBar;

    SystemTopologyData*          data;
    SystemTopologyView*          view;
    SystemTopologyViewTransform* transform;
    unsigned                     topologyId;
};

#endif
