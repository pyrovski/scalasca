/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <QScrollBar>

#include "TabWidget.h"
#include "SystemTopologyWidget.h"
#include "SystemTopologyToolBar.h"
#include "PrecisionWidget.h"
#include "Statistics.h"
#include "SystemBoxPlot.h"

#include "Metric.h"
#include "Cartesian.h"

#include <cassert>
#include <qdebug.h>

TabWidget::TabWidget( TabWidgetType    type,
                      ColorWidget*     colorWidget,
                      ValueWidget*     valueWidget,
                      PrecisionWidget* precisionWidget ) : QTabWidget()
{
    //general initializations
    valueModus      = ABSOLUTE;
    cube            = NULL;
    metricTabWidget = NULL;
    callTabWidget   = NULL;
    systemTabWidget = NULL;
    splitterWidget  = NULL;
    subsetCombo     = NULL;

    this->type            = type;
    this->colorWidget     = colorWidget;
    this->valueWidget     = valueWidget;
    this->precisionWidget = precisionWidget;

    //tree-specific initializations
    treeFont = font();
    spacing  = 2;

    //topology-specific initializations
    currentSysTopWidget      = NULL;
    whiteForZero             = true;
    lineType                 = BLACK_LINES;
    antialiasing             = false;
    showUnusedTopologyPlanes = true;
    systemBoxWidget          = NULL;
    systemBoxPlotIndex       = -1;
#ifdef _AIX
    selectionSyntax = FRAME_SELECTION;
#else
    selectionSyntax = BACKGROUND_SELECTION;
#endif
}

TabWidget::~TabWidget()
{
}


/**************** get methods ******************/

//return the current value modus, see constants.h for the ValueModus type
//
ValueModus
TabWidget::getValueModus()
{
    return valueModus;
}

//get different widgets

ColorWidget*
TabWidget::getColorWidget()
{
    return colorWidget;
}
PrecisionWidget*
TabWidget::getPrecisionWidget()
{
    return precisionWidget;
}
ValueWidget*
TabWidget::getValueWidget()
{
    return valueWidget;
}
QSplitter*
TabWidget::getSplitterWidget()
{
    return splitterWidget;
}

//return the metric tree if any
//
TreeWidget*
TabWidget::getMetricTree()
{
    if ( metricTabWidget == NULL )
    {
        return NULL;
    }
    else if ( metricTabWidget->count() == 0 )
    {
        return NULL;
    }
    else
    {
        assert( metricTabWidget->widget( 0 ) != NULL );
        return ( TreeWidget* )( ( ( ScrollArea* )( metricTabWidget->widget( 0 ) ) )->component() );
    }
}

//return the call tree if any
//
TreeWidget*
TabWidget::getCallTree()
{
    if ( callTabWidget == NULL )
    {
        return NULL;
    }
    else if ( callTabWidget->count() == 0 )
    {
        return NULL;
    }
    else
    {
        assert( callTabWidget->widget( 0 ) != NULL );
        return ( TreeWidget* )( ( ( ScrollArea* )( callTabWidget->widget( 0 ) ) )->component() );
    }
}

//return the current tab element of the call tab
//(call tree of flat call profile)
//
TreeWidget*
TabWidget::getCurrentCallTree()
{
    if ( callTabWidget == NULL )
    {
        return NULL;
    }
    else if ( callTabWidget->count() == 0 )
    {
        return NULL;
    }
    else
    {
        QWidget* widget = callTabWidget->widget( callTabWidget->currentIndex() );
        assert( widget != NULL );
        return ( TreeWidget* )( ( ( ScrollArea* )widget )->component() );
    }
}


//return the system tree if any
//
TreeWidget*
TabWidget::getSystemTree()
{
    if ( systemTabWidget == NULL )
    {
        return NULL;
    }
    else if ( systemTabWidget->count() == 0 )
    {
        return NULL;
    }
    else
    {
        assert( systemTabWidget->widget( 0 ) != NULL );
        return ( TreeWidget* )( ( ( ScrollArea* )( systemTabWidget->widget( 0 ) ) )->component() );
    }
}

//return the ith external metric item
//

TreeWidgetItem*
TabWidget::getExternalMetricItem( int i )
{
    return externalMetricItems[ i ];
}

//return the name if the ith external metric item
//
std::string
TabWidget::getExternalMetricName( int i )
{
    return externalMetricNames[ i ];
}

//get the different tabs

TabWidget*
TabWidget::getMetricTabWidget()
{
    return metricTabWidget;
}
TabWidget*
TabWidget::getCallTabWidget()
{
    return callTabWidget;
}
TabWidget*
TabWidget::getSystemTabWidget()
{
    return systemTabWidget;
}


//return if unused topology planes should be displayed
//
bool
TabWidget::getShowUnusedTopologyPlanes()
{
    return showUnusedTopologyPlanes;
}

//return if lines in topology display should be antialiased
//
bool
TabWidget::getAntialiasing()
{
    return antialiasing;
}

//return the line style for topologies
//(black, gray, white or no lines, see constants.h for the LineType type)
//
LineType
TabWidget::getLineType()
{
    return lineType;
}

//tell how zero values in topologies are colored
//(white or minimal color)
//
bool
TabWidget::getWhiteForZero()
{
    return whiteForZero;
}

//return the current method for marking selected tree items
//(by blue background or by framing, see constants.h for the SelectionSyntax type)
//
SelectionSyntax
TabWidget::getSelectionSyntax()
{
    return selectionSyntax;
}


/******************* set methods ***************/

//set the value modus, see constants.h for the ValueModus type
//
void
TabWidget::setValueModus( ValueModus valueModus )
{
    this->valueModus = valueModus;
    if ( count() == 0 )
    {
        return;
    }
    ( ( TreeWidget* )( ( ( ScrollArea* )( widget( 0 ) ) )->component() ) )->computeReferenceValues();
    if ( type == CALLTAB )
    {
        ( ( TreeWidget* )( ( ( ScrollArea* )( widget( 1 ) ) )->component() ) )->computeReferenceValues();
    }
    updateValueWidget();
}

//set the different tab widgets;
//they are needed to determine the different trees

void
TabWidget::setMetricTabWidget( TabWidget* widget )
{
    metricTabWidget = widget;
}
void
TabWidget::setCallTabWidget( TabWidget* widget )
{
    callTabWidget = widget;
}
void
TabWidget::setSystemTabWidget( TabWidget* widget )
{
    systemTabWidget = widget;
}

//set the splitter widget the tab is in
//
void
TabWidget::setSplitterWidget( QSplitter* splitter )
{
    splitterWidget = splitter;
}

//set font and line spacing for the tree displays
//in this tab
//
void
TabWidget::setTreeDisplay( QFont font, int spacing )
{
    treeFont      = font;
    this->spacing = spacing;
    if ( count() > 0 )
    {
        TreeWidget* treeWidget = ( ( TreeWidget* )( ( ( ScrollArea* )widget( 0 ) )->component() ) );
        treeWidget->setFont( treeFont );
        treeWidget->setSpacing( spacing );
        treeWidget->updateSizes();
        if ( type == CALLTAB )
        {
            treeWidget = ( ( TreeWidget* )( ( ( ScrollArea* )widget( 1 ) )->component() ) );
            treeWidget->setFont( treeFont );
            treeWidget->setSpacing( spacing );
            treeWidget->updateSizes();
        }
    }
}

//set how zero values in topologies should be colored
//(white or minimal color)
//
void
TabWidget::setWhiteForZero( bool whiteForZero )
{
    this->whiteForZero = whiteForZero;
}

//set how selected tree items should be marked
//(by blue background or by framing, see constants.h for the SelectionSyntax type)
//
void
TabWidget::setSelectionSyntax( SelectionSyntax selectionSyntax )
{
    this->selectionSyntax = selectionSyntax;
    displayItems();
}


/**************** initialization and clean-up ***************/

//initialize according to the type and the cube database
//
void
TabWidget::initialize( cube::Cube* cube, QString fileName,
                       cubeparser::Driver* driver, Statistics* statistics )
{
    this->cube = cube;

    TreeWidget* treeWidget;

    if ( type == METRICTAB )
    {
        //metric tabs have a metric tree
        ScrollArea* scrollArea = new ScrollArea( this, ScrollAreaTreeWidget );
        treeWidget = new TreeWidget( scrollArea, METRICTREE, fileName, driver, statistics );
        connect( treeWidget, SIGNAL( setMessage( QString ) ), this, SIGNAL( setMessage( QString ) ) );
        treeWidget->setFont( treeFont );
        treeWidget->setSpacing( spacing );
        treeWidget->setTabWidget( this );
        treeWidget->initialize( cube );
        scrollArea->setMainWidget( treeWidget );
        addTab( scrollArea, "Metric tree" );
    }
    else if ( type == CALLTAB )
    {
        //call tabs have a call tree and a flat call profile
        ScrollArea* scrollArea = new ScrollArea( this, ScrollAreaTreeWidget );
        treeWidget = new TreeWidget( scrollArea, CALLTREE, fileName, driver, statistics );
        connect( treeWidget, SIGNAL( setMessage( QString ) ), this, SIGNAL( setMessage( QString ) ) );
        treeWidget->setFont( treeFont );
        treeWidget->setSpacing( spacing );
        treeWidget->setTabWidget( this );
        treeWidget->initialize( cube );
        scrollArea->setMainWidget( treeWidget );
        addTab( scrollArea, "Call tree" );

        scrollArea = new ScrollArea( this, ScrollAreaTreeWidget );
        treeWidget = new TreeWidget( scrollArea, CALLFLAT );
        connect( treeWidget, SIGNAL( setMessage( QString ) ), this, SIGNAL( setMessage( QString ) ) );
        treeWidget->setFont( treeFont );
        treeWidget->setSpacing( spacing );
        treeWidget->setTabWidget( this );
        treeWidget->initialize( cube );
        scrollArea->setMainWidget( treeWidget );
        addTab( scrollArea, "Flat view" );
    }
    else if ( type == SYSTEMTAB )
    {
        subsetCombo = new QComboBox();
        subsetCombo->setModel( &subsetModel );
        subsetCombo->setWhatsThis(
            tr( "The Boxplot uses the currently selected subset of threads when determining its statistics."
                " Other defined subsets can be chosen from the combobox menu, such as \"All\" threads or"
                " \"Visited\" threads for only threads that visited the currently selected callpath."
                " Additional subsets can be defined from the System Tree with the \"Define subset\" context menu"
                " using the currently selected threads via multiple selection (control + left mouseclick)"
                " or with the \"Find items\" context menu selection option." ) );
        fillSubsetCombo();
        connect( subsetCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( displayItems() ) );
        connect( subsetCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( updateSubsetCombo() ) );

        //system tabs have a system tree, topologies and box plot
        {
            ScrollArea* scrollArea = new ScrollArea( this, ScrollAreaTreeWidget );
            treeWidget = new TreeWidget( scrollArea, SYSTEMTREE );
            connect( treeWidget, SIGNAL( setMessage( QString ) ), this, SIGNAL( setMessage( QString ) ) );
            connect( treeWidget, SIGNAL( selectionChanged() ), this, SLOT( resetSubsetCombo() ) );
            treeWidget->setFont( treeFont );
            treeWidget->setSpacing( spacing );
            treeWidget->setTabWidget( this );
            treeWidget->initialize( cube );
            scrollArea->setMainWidget( treeWidget );

            SplitterContainer* container = new SplitterContainer();
            container->setComponent( treeWidget ); // main component, used in TabWidget
            container->addWidget( scrollArea );

            connect( treeWidget, SIGNAL( definedSubsetsChanged( const QString & ) ),
                     this, SLOT( fillSubsetCombo( const QString & ) ) );
            container->addWidget( subsetCombo );

            QList<int> sizeList;
            sizeList << container->size().height() << 1;
            container->setSizes( sizeList );
            addTab( container, "System tree" );
        }
        {   // box plot tab
            SplitterContainer* container  = new SplitterContainer();
            ScrollArea*        scrollArea = new ScrollArea( this, ScrollAreaBoxPlot );
            systemBoxWidget = new SystemBoxPlot( scrollArea, treeWidget );
            scrollArea->setMainWidget( systemBoxWidget );
            scrollArea->setWidgetResizable( true );
            scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
            scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

            container->setComponent( systemBoxWidget ); // main component, used in TabWidget
            container->addWidget( scrollArea );

            // set lower splitter element to minimum size (1 pixel => replaced by minimumSize())
            QList<int> sizeList;
            sizeList << container->size().height() << 1;
            container->setSizes( sizeList );
            systemBoxPlotIndex = addTab( container, "Box Plot" );
        }
        {
            SystemTopologyWidget* systemTopologyWidget;
            unsigned              numTopologies = cube->get_cartv().size();

            for ( unsigned i = 0; i < numTopologies; i++ )
            {
                QString name = ( cube->get_cartv() ).at( i )->get_name().c_str();
                if ( name == "" )
                {
                    name.append( "Topology " );
                    name.append( QString::number( i ) );
                }
                SplitterContainer* container = new SplitterContainer();
                systemTopologyWidget = new SystemTopologyWidget( treeWidget, i );

                systemTopologyWidget->setLineType( lineType );
                systemTopologyWidget->initialize( cube );

                container->setComponent( systemTopologyWidget );
                container->addWidget( systemTopologyWidget );

                /** add topology dimension toolbar with scrollPane */
                QWidget* dimBar = systemTopologyWidget->getDimensionSelectionBar( cube );
                if ( dimBar != 0 )
                {
                    QScrollArea* scroll = new QScrollArea();
                    container->addWidget( scroll );
                    scroll->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
                    scroll->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
                    scroll->setFrameStyle( QFrame::NoFrame );
                    scroll->setMinimumHeight( dimBar->minimumSizeHint().height() );
                    scroll->setMaximumHeight( dimBar->minimumSizeHint().height() );
                    scroll->setWidget( dimBar );
                    long ndims = ( cube->get_cartv() ).at( i )->get_ndims();
                    if ( ndims <= 3 )   // minimize dimension selection bar
                    {
                        QList<int> sizeList;
                        sizeList << 1 << 0;
                        container->setSizes( sizeList );
                    }
                }

                addTab( container, name );
            }
        } // SYSTEMTAB
    }
    connect( this, SIGNAL( currentChanged( int ) ), this, SLOT( onCurrentChanged( int ) ) );
}

/**
 * @brief removes additional description from subset name
 * @param subsetName if empty, the currently selected subset is used
 */
QString
TabWidget::getSubsetName( const QString &subsetName )
{
    if ( !subsetCombo )
    {
        return QString();
    }

    QString name = subsetName;
    if ( name.isEmpty() )   // get name of selected Element
    {
        name = subsetCombo->currentText();
    }
    name.remove( QRegExp( "\\s*\\([0-9]* elements\\)$" ) );
    return name;
}

void
TabWidget::resetSubsetCombo()
{
    subsetCombo->setCurrentIndex( 0 );
}

/**
 * @brief TabWidget::fillSubsetCombo
 * @param name if not empty, the entry with the given name selected
 */
void
TabWidget::fillSubsetCombo( const QString &name )
{
    if ( !subsetCombo )
    {
        return;
    }

    QStringList list;
    list << "All";
    list << "Visited";

    int idx = -1;
    if ( getSystemTree() != 0 )
    {
        const QHash<QString, std::vector<TreeWidgetItem*> > &definedSubsets = getSystemTree()->getDefinedSubsets();
        QStringList                                          keys           = definedSubsets.keys();
        keys.sort();
        for ( int i = 0; i < keys.size(); ++i )
        {
            uint size = definedSubsets.value( keys.at( i ) ).size();
            list << keys.at( i ) + " (" + QString::number( size ) + " elements)";
            if ( !name.isEmpty() && keys.at( i ) == name )   // select new item
            {
                idx = list.size() - 1;
            }
        }
    }
    subsetModel.setStringList( list );
    if ( idx >= 0 )
    {
        subsetCombo->setCurrentIndex( idx );
    }
}

/**
   updates the number of processes/threads
   @oaram updateAll update even if currently not selected
 */
void
TabWidget::updateSubsetCombo()
{
    if ( subsetCombo && getSystemTree() )
    {
        // update "All" (only initalisation necessary)
        QString name = "All";
        int     idx  = subsetCombo->findText( name, Qt::MatchExactly );
        if ( idx >= 0 )
        {
            int     count = getSystemTree()->getSubsetItemCount( name );
            QString txt   = name + " (" + QString::number( count ) + " elements)";
            if ( idx >= 0 )
            {
                subsetModel.setData( subsetModel.index( idx ), txt );
            }
        }

        // update "Visited"
        name = "Visited";
        idx  = subsetCombo->findText( name, Qt::MatchContains );
        if ( idx == subsetCombo->currentIndex() )
        {   //visible
            int     visited = getSystemTree()->getSubsetItemCount( name );
            QString txt     = name + " (" + QString::number( visited ) + " elements)";
            if ( idx >= 0 )
            {
                subsetModel.setData( subsetModel.index( idx ), txt );
            }
        }
        else
        {
            subsetModel.setData( subsetModel.index( idx ), name );
        }
    }
}

/**
   returns the index of the selected system topology
   @param tabIndex index of the tab
 */
int
TabWidget::topologyIndex( int tabIndex )
{
    int idx = tabIndex - 1; // don't count system tree (always 1st tab)
    if ( tabIndex > systemBoxPlotIndex )
    {
        idx--;                                // don't count SystemBoxPlot tab
    }
    return idx;
}



//called when files are closed;
//clear the tab, no data is shown
//
void
TabWidget::cleanUp()
{
    currentSysTopWidget = NULL;
    disconnect( this, SIGNAL( currentChanged( int ) ), this, SLOT( onCurrentChanged( int ) ) );
    //remove tab elements
    while ( count() > 0 )
    {
        QWidget* child = widget( 0 );
        removeTab( 0 );
        delete child;
    }
}


//clean up external values;
//called when external files are closed
//
void
TabWidget::clearExternalValues()
{
    for ( int i = 0; i < ( int )( externalMetricItems.size() ); i++ )
    {
        delete externalMetricItems[ i ];
    }
    externalMetricItems.clear();
    externalMetricNames.clear();
}


/************** value computations ******************/


//(re-)compute the values for the current tab element;
//in case of system tabs, compute the values for the system tree,
//since the topologies use the tree values
//
void
TabWidget::computeValues()
{
    if ( count() > 0 )
    {
        int index = ( type == SYSTEMTAB ? 0 : currentIndex() );
        ( ( TreeWidget* )( ( ( ScrollArea* )( widget( index ) ) )->component() ) )->computeValues();
        displayItems();
        updateSubsetCombo();
    }
}

//compute the needed external values from the "cubeExternal"
//database for the external percentage value modus
//
void
TabWidget::computeExternalValues( cube::Cube* cubeExternal )
{
    assert( type == METRICTAB );

    clearExternalValues();

    const std::vector<cube::Metric*>& metricVec = cubeExternal->get_metv();
    const std::vector<cube::Cnode*>&  callVec   = cubeExternal->get_cnodev();
    const std::vector<cube::Thread*>& threadVec = cubeExternal->get_thrdv();

    //build the tree of the external metric items
    for ( int i = 0; i < ( int )metricVec.size(); i++ )
    {
        //take the next metric item
        cube::Metric*   metric       = metricVec[ i ];
        cube::Metric*   metricParent = metric->get_parent();
        TreeWidgetItem* itemParent   = NULL;
        //search for the (already created) parent item
        if ( metricParent != NULL )
        {
            for ( int j = 0; j < ( int )externalMetricItems.size(); j++ )
            {
                if ( ( ( cube::Metric* )( ( TreeWidgetItem* )( externalMetricItems[ j ]->cubeObject ) ) ) == metricParent )
                {
                    itemParent = externalMetricItems[ j ];
                    break;
                }
            }
            assert( itemParent != NULL );
        }
        //create a new item for the current metric with the found parent
        TreeWidgetItem* item = new TreeWidgetItem( itemParent );
        item->cubeObject = metric;
        externalMetricItems.push_back( item );
        //we store the metric names because we delete the cube object
        //after computing the external values
        externalMetricNames.push_back( metric->get_uniq_name() );

        //compute external metric values
        double totalValue = 0.0;
        for ( int j = 0; j < ( int )callVec.size(); j++ )
        {
            for ( int k = 0; k < ( int )threadVec.size(); k++ )
            {
                totalValue += cube->get_sev( metric,
                                             callVec[ j ],
                                             threadVec[ k ] );
            }
        }
        item->totalValue = totalValue;
        item->ownValue   = totalValue;
        if ( itemParent != NULL )
        {
            itemParent->ownValue -= totalValue;
        }
    }
    //clean up the cube object references such that the cube
    //object can be deleted
    for ( int i = 0; i < ( int )( externalMetricItems.size() ); i++ )
    {
        externalMetricItems[ i ]->cubeObject = NULL;
    }
}


/***************** display **********************/

//display the current tab element
//
void
TabWidget::displayItems()
{
    if ( count() > 0 )
    {
        if ( type == METRICTAB || type == CALLTAB )
        {
            ( ( TreeWidget* )( ( ( ScrollArea* )widget( currentIndex() ) )->component() ) )->displayItems();
        }
        else
        {
            assert( type == SYSTEMTAB );
            ( ( TreeWidget* )( ( ( ScrollArea* )( widget( 0 ) ) )->component() ) )->displayItems();
            if ( currentSysTopWidget )
            {
                currentSysTopWidget->displayItems();
            }
            if ( systemBoxWidget && currentIndex() == systemBoxPlotIndex )
            {
                systemBoxWidget->updateValues();
            }
#if 0
            if ( currentIndex() == 0 )
            {
                ( ( TreeWidget* )( ( ( ScrollArea* )( widget( 0 ) ) )->component() ) )->displayItems();
            }
            else
            {
                currentSysTopWidget->displayItems();
            }
#endif
        }
    }
}

/** signal currentChanged() is emitted when the visible tab element has changed;
   that signal gets connected to this slot;
   parameter: index is the new index of the current tab element
 */
void
TabWidget::onCurrentChanged( int index )
{
    if ( type == CALLTAB )
    {
        //for call tabs:
        //(re-)compute values of the current tab element

        ( ( TreeWidget* )( ( ( ScrollArea* )( widget( index ) ) )->component() ) )->computeValues();
        ( ( TreeWidget* )( ( ( ScrollArea* )( widget( index ) ) )->component() ) )->computeMaxValues();
    }
    else if ( type == SYSTEMTAB )
    {
        currentSysTopWidget = NULL;

        // handle subset Combobox
        if ( index <= systemBoxPlotIndex )
        {
            QSplitter* parent   = ( QSplitter* )subsetCombo->parentWidget();
            QList<int> sizeList = parent->sizes();
            // show the same instance of QCombobox on the first two tabs -> change the parent widget
            subsetCombo->setParent( widget( index ) );

            // save collapsed state
            parent = ( QSplitter* )subsetCombo->parentWidget();
            parent->setSizes( sizeList );
        }

        // if current tab is SystemBoxPlot
        if ( index == systemBoxPlotIndex )
        {
            displayItems();
        }
        else if ( index > systemBoxPlotIndex )   // SystemTopologyWidgets
        {
            currentSysTopWidget = ( ( SystemTopologyWidget* )( ( ( ScrollArea* )widget( index ) )->component() ) );

            //update the colors, because since the last display the values
            //of the system tree could have changed
            currentSysTopWidget->updateColors(); // todo check
        } // topology
        SystemTopologyToolBar::getInstance()->setParent( currentSysTopWidget );
    } // SYSTEMTAB

    //if current tab element is a tree:
    //ensure that the selected item is visible if the current
    //tab item is a tree
    if ( index == 0 || type == CALLTAB )
    {
        TreeWidget* treeWidget = ( ( TreeWidget* )( ( ( ScrollArea* )( widget( index ) ) )->component() ) );
        if ( treeWidget->selectedItems().size() > 0 )
        {
            treeWidget->ensureVisible( treeWidget->selectedItems().at( 0 ) );
        }
        //todo : better, more generic handling of contents
        //      ((ScrollArea*)(widget(index)))->horizontalScrollBar()->setValue(0);
        // redraw system tree and call tab trees
        displayItems();
    }

    //repaint value widget
    updateValueWidget();
}


//update the value widgets below the tab
//
void
TabWidget::updateValueWidget()
{
    if ( count() > 0 )
    {
        int index = currentIndex();
        //if current tab element is a tree
        if ( index == 0 || type == CALLTAB )
        {
            ( ( TreeWidget* )( ( ( ScrollArea* )( widget( index ) ) )->component() ) )->updateValueWidget();
        }
        else if ( type == SYSTEMTAB )
        {
            //else it is a topology
            if ( index != systemBoxPlotIndex )
            {
                assert( currentSysTopWidget != NULL );
                currentSysTopWidget->updateValueWidget();
            }
            else if ( index == this->systemBoxPlotIndex )   // box plot
            {
                systemBoxWidget->updateValueWidget();
            }
        }
    }
}



//defines if zero values in topologies should be represented by the
//color white or by the minimal color;
//this slot is connected to the actions in the menu
//Display/Coloring/Topology coloring

void
TabWidget::whiteOff()
{
    assert( type == SYSTEMTAB );
    whiteForZero = false;
    if ( currentSysTopWidget != NULL )
    {
        currentSysTopWidget->updateColors();
        displayItems();
    }
}
void
TabWidget::whiteOn()
{
    assert( type == SYSTEMTAB );
    whiteForZero = true;
    if ( currentSysTopWidget != NULL )
    {
        currentSysTopWidget->updateColors();
        displayItems();
    }
}

//set line coloring in topology widgets;
//connected to the actions in the menu Display/Coloring/Topology line coloring

void
TabWidget::blackLines()
{
    assert( type == SYSTEMTAB );
    lineType = BLACK_LINES;
    for ( int i = 1; i < count(); i++ )
    {
        if ( i != systemBoxPlotIndex )
        {
            ( ( SystemTopologyWidget* )( ( ( ScrollArea* )widget( i ) )->component() ) )->setLineType( BLACK_LINES );
        }
    }
}
void
TabWidget::grayLines()
{
    assert( type == SYSTEMTAB );
    lineType = GRAY_LINES;
    for ( int i = 1; i < count(); i++ )
    {
        if ( i != systemBoxPlotIndex )
        {
            ( ( SystemTopologyWidget* )( ( ( ScrollArea* )widget( i ) )->component() ) )->setLineType( GRAY_LINES );
        }
    }
}
void
TabWidget::whiteLines()
{
    assert( type == SYSTEMTAB );
    lineType = WHITE_LINES;
    for ( int i = 1; i < count(); i++ )
    {
        if ( i != systemBoxPlotIndex )
        {
            ( ( SystemTopologyWidget* )( ( ( ScrollArea* )widget( i ) )->component() ) )->setLineType( WHITE_LINES );
        }
    }
}
void
TabWidget::noLines()
{
    assert( type == SYSTEMTAB );
    lineType = NO_LINES;
    for ( int i = 1; i < count(); i++ )
    {
        if ( i != systemBoxPlotIndex )
        {
            ( ( SystemTopologyWidget* )( ( ( ScrollArea* )widget( i ) )->component() ) )->setLineType( NO_LINES );
        }
    }
}



//define if lines in topologies should be drawn with antialiasing
//
void
TabWidget::setAntialiasing( bool value )
{
    antialiasing = value;
    //   if (currentIndex()>0)
    //todo      ((SystemTopologyWidget*)(((ScrollArea*)widget(currentIndex()))->component()))->repaint();
}

//toggle modus if lines in topologies should be drawn with antialiasing
void
TabWidget::toggleAntialiasing()
{
    assert( type == SYSTEMTAB );
    antialiasing = !antialiasing;
    //   if (currentIndex()>0)
    //todo      ((SystemTopologyWidget*)(((ScrollArea*)widget(currentIndex()))->component()))->repaint();
}

//change the modus if unused topology planes should be displayed
void
TabWidget::toggleUnused()
{
    assert( type == SYSTEMTAB );
    showUnusedTopologyPlanes = !showUnusedTopologyPlanes;

    for ( int i = 1; i < count(); i++ )
    {
        if ( i != systemBoxPlotIndex )
        {
            ( ( SystemTopologyWidget* )( ( ( ScrollArea* )widget( i ) )->component() ) )->toggleUnused();
        }
    }
}



/***************** settings ***********************/


//save tab settings
//
void
TabWidget::saveSettings( QSettings& settings )
{
    assert( count() > 0 );

    QString groupName;
    if ( type == METRICTAB )
    {
        groupName = "metrictab";
    }
    else if ( type == CALLTAB )
    {
        groupName = "calltab";
    }
    else if ( type == SYSTEMTAB )
    {
        groupName = "systemtab";
    }
    else
    {
        assert( false );
    }
    settings.beginGroup( groupName );

    //recursively save settings for the tab elements;
    //first the tree at index 0
    ( ( TreeWidget* )( ( ( ScrollArea* )( widget( 0 ) ) )->component() ) )->saveSettings( settings );
    //for call tabs also the flat call profile
    if ( type == CALLTAB )
    {
        ( ( TreeWidget* )( ( ( ScrollArea* )( widget( 1 ) ) )->component() ) )->saveSettings( settings );
    }
    else if ( type == SYSTEMTAB && count() > 1 )
    {
        //and for system tabs all topologies
        for ( int i = 1; i < count(); i++ )
        {
            if ( i != systemBoxPlotIndex )
            {
                ( ( SystemTopologyWidget* )( ( ( ScrollArea* )widget( i ) )->component() ) )->saveSettings( settings );
            }
        }
    }

    //miscellaneous
    settings.setValue( "currentIndex", currentIndex() );
    settings.setValue( "selectionSyntax", selectionSyntax );
    settings.setValue( "whiteForZero", whiteForZero );
    if ( type == SYSTEMTAB )
    {
        settings.setValue( "lineType", lineType );
    }

    settings.endGroup();
}

//load tab settings
//
bool
TabWidget::loadSettings( QSettings& settings )
{
    assert( count() > 0 );

    bool result = true;


    QString groupName;
    if ( type == METRICTAB )
    {
        groupName = "metrictab";
    }
    else if ( type == CALLTAB )
    {
        groupName = "calltab";
    }
    else if ( type == SYSTEMTAB )
    {
        groupName = "systemtab";
    }
    else
    {
        assert( false );
    }
    settings.beginGroup( groupName );

    //recursively load settings for all tab elements:
    //first the tree at index 0

    result = ( ( TreeWidget* )( ( ( ScrollArea* )( widget( 0 ) ) )->component() ) )->loadSettings( settings );
    //for call tabs also the flat call profile

    if ( result && type == CALLTAB )
    {
        result = ( ( TreeWidget* )( ( ( ScrollArea* )( widget( 1 ) ) )->component() ) )->loadSettings( settings );
    }
    else if ( result && type == SYSTEMTAB && count() > 1 )
    {
        //and for system tabs for all topologies
        for ( int i = 1; result && i < count(); i++ )
        {
            if ( i != systemBoxPlotIndex )
            {
                setCurrentIndex( i );
                QString name = "topologyDimension";
                name.append( QString::number( i ) );
                result =        ( ( SystemTopologyWidget* )( ( ( ScrollArea* )widget( i ) )->component() ) )->loadSettings( settings );
                //restore also topology dimension order
            }
        }
    }

    //miscellaneous

    setCurrentIndex( settings.value( "currentIndex", 0 ).toInt() );

    selectionSyntax = ( SelectionSyntax )( settings.value( "selectionSyntax", 0 ).toInt() );
    whiteForZero    = settings.value( "whiteForZero", true ).toBool();
    if ( currentSysTopWidget != NULL )
    {
        currentSysTopWidget->updateColors();
    }

    if ( type == SYSTEMTAB )
    {
        LineType lineType;
        lineType = ( LineType )( settings.value( "lineType", 0 ).toInt() );
        if ( lineType == BLACK_LINES )
        {
            blackLines();
        }
        else if ( lineType == GRAY_LINES )
        {
            grayLines();
        }
        else if ( lineType == WHITE_LINES )
        {
            whiteLines();
        }
        else
        {
            noLines();
        }
    }

    settings.endGroup();

    updateValueWidget();
    displayItems();

    return result;
}



/************ miscellaneous **************/

//return the list of external metric items
int
TabWidget::numExternalMetricItems()
{
    return externalMetricItems.size();
}

//return the minimal width necessary to display all informations
int
TabWidget::getOptimalWidth()
{
    ScrollArea* as_scroll_area = dynamic_cast<ScrollArea*>( widget( currentIndex() ) );
    int         w              = 0;
    if ( as_scroll_area != NULL )
    {
        w = ( ( ScrollArea* )( widget( currentIndex() ) ) )->getOptimalWidth();
    }
    else
    {
        w = ( ( SplitterContainer* )( widget( currentIndex() ) ) )->getOptimalWidth();
    }

    w += width() - ( ( ScrollArea* )( widget( currentIndex() ) ) )->width();
    if ( w < minimumWidth() )
    {
        w = minimumWidth();
    }
    return w;
}

/**
   enables box plot tab, if system tab is not first tab
 */
void
TabWidget::enableSystemBoxPlot( bool first )
{
    if ( ( systemBoxWidget ) && ( type == SYSTEMTAB ) )
    {
        setTabEnabled( systemBoxPlotIndex, !first );
    }
}
