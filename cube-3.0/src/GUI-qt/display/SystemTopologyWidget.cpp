/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "SystemTopologyWidget.h"

#include "TreeWidget.h"
#include "ScrollArea.h"
#include "ValueWidget.h"

#include "Cartesian.h"

#include <QScrollBar>
#include <QSettings>
#include <qdebug.h>

#include "SystemTopologyView.h"
#include "SystemTopologyDrawing.h"
//#include "SystemTopologyGL.h"

/**
   controls several widgets:
   TopologyDimensionBar which allows to reorder the dimensions
   TopologyToolbar which allows to do transfomations (scaling ...) on view
   SystemTopologyView which draws the topology
 */

/*************** constructor / destructor *************************/

SystemTopologyWidget::SystemTopologyWidget( TreeWidget* systemTreeWidget,
                                            unsigned    topologyId )
{
    data      = new SystemTopologyData( systemTreeWidget, topologyId );
    transform = new SystemTopologyViewTransform( data );
    view      = new SystemTopologyDrawing( data, transform );
    //view = new SystemTopologyGL(data);

    connect( view, SIGNAL( selectItem( int ) ), this, SLOT( selectItem( int ) ) );

    connect( data, SIGNAL( dataChanged() ), view, SLOT( updateDrawing() ) );
    connect( data, SIGNAL( viewChanged() ), view, SLOT( updateDrawing() ) );
    connect( data, SIGNAL( rescaleRequest() ), transform, SLOT( rescale() ) );

    connect( transform, SIGNAL( rescaleRequest() ), view, SLOT( rescaleDrawing() ) );
    connect( transform, SIGNAL( viewChanged() ), view, SLOT( updateDrawing() ) );
    connect( transform, SIGNAL( zoomChanged( double ) ), view, SLOT( changeZoom( double ) ) );
    connect( transform, SIGNAL( xAngleChanged( int ) ), view, SLOT( setXAngle( int ) ) );
    connect( transform, SIGNAL( yAngleChanged( int ) ), view, SLOT( setYAngle( int ) ) );
    connect( transform, SIGNAL( positionChanged( int, int ) ), view, SLOT( move( int, int ) ) );

    dimensionBar           = 0;
    this->systemTreeWidget = systemTreeWidget;
    this->topologyId       = topologyId;
    this->setWidget( view );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
}

SystemTopologyWidget::~SystemTopologyWidget()
{
}

QWidget*
SystemTopologyWidget::getWidget()
{
    return this;
}

void
SystemTopologyWidget::resizeEvent( QResizeEvent* )
{
    int w = width() - verticalScrollBar()->width() - 5;
    int h = height() - horizontalScrollBar()->height() - 5;
    if ( w < 0 )
    {
        w = 0;
    }
    if ( h < 0 )
    {
        h = 0;
    }

    view->setSize( QSize( w, h ) );
}

int
SystemTopologyWidget::getOptimalWidth()
{
    return 0; // ??? todo
}

/* initializes the topology with data from the cube object
 */
void
SystemTopologyWidget::initialize( cube::Cube* cube )
{
    selectMode = FOLD;
    TopologyDimensionBar* bar = ( TopologyDimensionBar* )getDimensionSelectionBar( cube );
    data->setFoldingDimensions( bar->getFoldingVector() );
    data->initialize( cube );
}


/**************************** miscellaneous **********************/


/** create toolbar to select dimensions to display
 */
QWidget*
SystemTopologyWidget::getDimensionSelectionBar( cube::Cube* cube )
{
    if ( dimensionBar == 0 )
    {
        const std::vector<long>&       dims  = cube->get_cart( topologyId )->get_dimv();
        const std::vector<std::string> names = cube->get_cart( topologyId )->get_namedims();
        dimensionBar = new TopologyDimensionBar( dims, names );
        connect( dimensionBar, SIGNAL( selectedDimensionsChanged( std::vector<long>) ), data, SLOT( selectedDimensionsChanged( std::vector<long>) ) );
        connect( dimensionBar, SIGNAL( foldingDimensionsChanged( std::vector<std::vector<int> >) ), data, SLOT( foldingDimensionsChanged( std::vector<std::vector<int> >) ) );
    }
    return dimensionBar;
}

/**
   marks the given item as selected in System tree
 */
void
SystemTopologyWidget::selectItem( int systemID )
{
    TreeWidgetItem* newSelection;
    newSelection = systemTreeWidget->items[ systemID ];

    std::string            selection = newSelection->getText().toStdString();
    selection.erase( 0, selection.find( " " ) + 1 ); // remove prepended value
    emit systemTreeWidget->setMessage( QString( "Selected \"" ).append( QString::fromStdString( selection ) ).append( "\"" ) );

    if ( newSelection->isHidden() )
    {
        newSelection->setHidden( false );
    }
    TreeWidgetItem* item = newSelection->parent();
    while ( item != NULL )
    {
        if ( item->isHidden() )
        {
            item->setHidden( false );
        }
        if ( !item->isExpanded() )
        {
            item->setExpanded( true );
        }
        item = item->parent();
    }
    if ( systemTreeWidget->selectedItems().size() > 0 )
    {
        systemTreeWidget->deselect( systemTreeWidget->selectedItems().at( 0 ) );
    }
    systemTreeWidget->select( newSelection );
    systemTreeWidget->updateSizes();
}

//-----------------------------------------------------------------------

//updates the value widget below the tab widget ; todo: move to controller class
void
SystemTopologyWidget::updateValueWidget()
{
    ValueWidget* valueWidget = systemTreeWidget->getTabWidget()->getValueWidget();

    //in case the topology is empty or if no items are selected, just
    //clear the vlaue widget
    if ( data->getDim( 0 ) == 0 || data->getDim( 1 ) == 0 || data->getDim( 2 ) == 0 || systemTreeWidget->selectedItems().size() == 0 )
    {
        valueWidget->clear();
    }
    else
    {
        double   _sigma    = 0.0;
        double   _variance = 0.0;
        double   _mean     = 0.0;

        double   tmp = 0.;
        bool     _ok = true;

        unsigned N = 0;
        for ( int i = 0; i < ( int )systemTreeWidget->items.size(); i++ )
        {
            TreeWidgetItem* item = systemTreeWidget->items.at( i );
            if ( item->childCount() != 0 )
            {
                continue;
            }

            systemTreeWidget->getValue( item, item->isExpanded(), tmp, _ok, ABSOLUTE );

            _mean += tmp;
            N++;
        }

        _mean = ( N != 0 ) ? _mean / N : 0.;

        for ( int i = 0; i < ( int )systemTreeWidget->items.size(); i++ )
        {
            TreeWidgetItem* item = systemTreeWidget->items.at( i );
            if ( item->childCount() != 0 )
            {
                continue;
            }

            systemTreeWidget->getValue( item, item->isExpanded(), tmp, _ok, ABSOLUTE );

            _variance += ( _mean - tmp ) * ( _mean - tmp );
        }
        _sigma = ( N != 0 ) ? sqrt( _variance / N ) : 0.;


        double value         = 0.0;
        double absoluteValue = 0.0;
        bool   ok            = false;
        bool   intType       = systemTreeWidget->hasIntegerType();

        //in the topology we handle single-selection only: we consider the
        //value of the first selected item (first not in the index but in
        //the time order)

        if ( systemTreeWidget->selectedItems().size() > 0 )
        {
            TreeWidgetItem* item = systemTreeWidget->selectedItems().at( 0 );
            intType = systemTreeWidget->hasAbsoluteIntegerType( item );

            systemTreeWidget->getValue( item, false, absoluteValue, ok, ABSOLUTE );

            if ( ok && systemTreeWidget->getTabWidget()->getValueModus() != ABSOLUTE )
            {
                systemTreeWidget->getValue( item, false, value, ok );
            }
        }

        //compute value for minimal color
        double min;
        if ( data->getUserDefinedMinMaxValues() )
        {
            min = data->getUserMinValue();
        }
        else if ( systemTreeWidget->tabWidget->getValueModus() == ABSOLUTE )
        {
            min = data->getMinValue();
        }
        else
        {
            min = 0.0;
        }

        //compute value for minimal color
        double max;
        if ( data->getUserDefinedMinMaxValues() )
        {
            max = data->getUserMaxValue();
        }
        else if ( systemTreeWidget->tabWidget->getValueModus() == ABSOLUTE )
        {
            max = data->getMaxValue();
        }
        else
        {
            max = 100.0;
        }

        if ( !ok )
        {
            //if one of the values was undefined, display only the min and
            //max values
            valueWidget->update( min, max, intType, false, _mean, _sigma );
        }
        else if ( systemTreeWidget->getTabWidget()->getValueModus() == ABSOLUTE )
        {
            //in the absolute value mode display the absolute value (with
            //the min/max values)
            valueWidget->update( min, max, absoluteValue, intType, false, _mean, _sigma );
        }
        else
        {
            //in value modes other that the absolute value mode display the
            //current minvalue/value/maxvalue plus absolute
            //minvalue/value/maxvalue
            valueWidget->update( 0.0, 100.0, value, data->getMinAbsValue(), data->getMaxAbsValue(), absoluteValue, intType, false, _mean, _sigma );
        }
    }
}


/****************** slots for menuitems / TabWidget ***************************/

void
SystemTopologyWidget::rescale()
{
    transform->rescale( false );
}

//set the line type for topologies: black, gray, white, or no lines
void
SystemTopologyWidget::setLineType( LineType lineType )
{
    data->setLineType( lineType );
}

//toggle the state if unused planes should be displayed or not
void
SystemTopologyWidget::toggleUnused()
{
    data->toggleUnused();
}

void
SystemTopologyWidget::displayItems()
{
    data->updateColors();
    view->updateDrawing();
}

void
SystemTopologyWidget::updateColors()
{
    data->updateColors();
    view->updateDrawing();
    updateValueWidget();
}

/************************ settings ***************************/

//save topology settings
//
void
SystemTopologyWidget::saveSettings( QSettings& settings )
{
    transform->saveSettings( settings, topologyId );
    dimensionBar->saveSettings( settings, topologyId );
}

bool
SystemTopologyWidget::loadSettings( QSettings& settings )
{
    dimensionBar->loadSettings( settings, topologyId );
    transform->loadSettings( settings, topologyId );
    return true;
}
