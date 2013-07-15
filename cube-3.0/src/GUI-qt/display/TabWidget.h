/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _TABWIDGET
#define _TABWIDGET

#include <QTabWidget>
#include <QSplitter>
#include <QSettings>
#include <QToolBar>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>

#include "Coloring.h"
#include "Constants.h"

#include "Driver.h"
#include "TopologyDimensionBar.h"

class Statistics;
class TreeWidget;
class TreeWidgetItem;
class SystemTopologyWidget;
class ValueWidget;
class PrecisionWidget;
class SystemBoxPlot;

/**
   the class TabWidget is used to display different widgets for one of the info types
   metric, call, or system;
   we create three instances of this class one for each of the three info types;
   - the metric tab contains only the metric tree
   - the call tab contains the call tree and a flat profile
   - the system tab contains the system tree and one widget for each topology

   the TabWidget instances are embedded, together with a value modus
   combo and a value widget, in a TypedWidget that is added to the
   splitter widget
 */

class TabWidget : public QTabWidget
{
    Q_OBJECT

signals:

    //this signal is connected to the main widget's setMessage slot
    //that displays text in the main widget's status menu
    void
    setMessage( QString message );

public slots:

    /************ public slots for value computations and display **********/

    //set the value modus, see constants.h for the ValueModus type
    void
    setValueModus( ValueModus valueModus );

    //(re-)compute the values for the current tab element;
    //in case of system tabs, compute the values for the system tree,
    //since the topologies use the tree values
    void
    computeValues();

    //display the current tab element
    void
    displayItems();

    /************ public slots for system tabs only **********/

    //defines if zero values in topologies should be represented by the
    //color white or by the minimal color;
    //this slot is connected to the actions in the menu
    //Display/Coloring/Topology coloring
    void
    whiteOff();
    void
    whiteOn();

    //set line coloring in topology widgets;
    //connected to the actions in the menu Display/Coloring/Topology line coloring
    void
    blackLines();
    void
    grayLines();
    void
    whiteLines();
    void
    noLines();

    //sets line rendering in topology;
    //connected to the menu Display/Topology/Topology antialiasing
    void
    toggleAntialiasing();

    //slot for handling requests if unused planes in topologies should be painted;
    //connected to menu Display/Topology/Show also unused...
    void
    toggleUnused();

private slots:

    //signal currentChanged() is emitted when the visible tab element has changed;
    //that signal gets connected to this slot
    void
    onCurrentChanged( int index );

    void
    resetSubsetCombo();
    void
    fillSubsetCombo( const QString &name = "" );
    void
    updateSubsetCombo();

public:

    TabWidget( TabWidgetType    type,
               ColorWidget*     colorWidget,
               ValueWidget*     valueWidget,
               PrecisionWidget* precisionWidget );
    ~TabWidget();

    /************ get methods **********/

    //return the metric tree
    TreeWidget*
    getMetricTree();

    //return the call tree
    TreeWidget*
    getCallTree();

    //return the call tree or the flat call profile, depending on which one is active
    TreeWidget*
    getCurrentCallTree();

    //return the system tree
    TreeWidget*
    getSystemTree();

    //return the different tab widgets
    TabWidget*
    getMetricTabWidget();
    TabWidget*
    getCallTabWidget();
    TabWidget*
    getSystemTabWidget();

    //return the splitter widget
    QSplitter*
    getSplitterWidget();

    //return the current value modus
    ValueModus
    getValueModus();

    //return the color widget
    ColorWidget*
    getColorWidget();

    //return the precision widget
    PrecisionWidget*
    getPrecisionWidget();

    //return the precision widget
    ValueWidget*
    getValueWidget();

    //return the ith external metric item
    TreeWidgetItem*
    getExternalMetricItem( int i );

    //return the name if the ith external metric item
    std::string
    getExternalMetricName( int i );

    //return if unused topology planes should be displayed
    bool
    getShowUnusedTopologyPlanes();

    //return if lines in topology display should be antialiased
    bool
    getAntialiasing();

    //return the current method for marking selected tree items
    //(by blue background or by framing, see constants.h for the SelectionSyntax type)
    SelectionSyntax
    getSelectionSyntax();

    //return the line style for topologies
    //(black, gray, white or no lines, see constants.h for the LineType type)
    LineType
    getLineType();

    //tell how zero values in topologies are colored
    //(white or minimal color)
    bool
    getWhiteForZero();

    /************** set methods ******************/

    //set the different tab widgets;
    //they are needed to determine the different trees
    void
    setMetricTabWidget( TabWidget* widget );
    void
    setCallTabWidget( TabWidget* widget );
    void
    setSystemTabWidget( TabWidget* widget );

    //set the splitter widget the tab is in
    void
    setSplitterWidget( QSplitter* splitter );

    //set font and line spacing for the tree displays
    //in this tab
    void
    setTreeDisplay( QFont font,
                    int   spacing );

    //set how selected tree items should be marked
    //(by blue background or by framing, see constants.h for the SelectionSyntax type)
    void
    setSelectionSyntax( SelectionSyntax selectionSyntax );

    //set how zero values in topologies should be colored
    //(white or minimal color)
    void
    setWhiteForZero( bool whiteForZero );

    /*********** initialization / clean up **************/

    //initialize the tab widget according to the data specified in the parameter
    void
    initialize( cube::Cube*         cube,
                QString             fileName = "",
                cubeparser::Driver* driver = NULL,
                Statistics*         statistics = 0 );

    //called when files are closed;
    //clear the tab, no data is shown
    void
    cleanUp();

    //initialize the topology tool bar
    void
    initializeTopologyToolBar( QToolBar* toolBar );


    /************* settings **********/

    //save tab settings
    void
    saveSettings( QSettings& settings );

    //load tab settings
    bool
    loadSettings( QSettings& settings );


    /************ value computation ****************/

    //compute the needed external values from the "cubeExternal"
    //database for the external percentage value modus
    void
    computeExternalValues( cube::Cube* cubeExternal );

    //clean up external values;
    //called when external files are closed
    void
    clearExternalValues();

    //return the number of external metric items
    int
    numExternalMetricItems();

    /************** miscellaneous ****************/

    //update the value widget below the tab
    void
    updateValueWidget();

    //return the minimal width necessary to display all informations
    int
    getOptimalWidth();

    // disable/enable the box plot tab, if system widget is 1st
    void
    enableSystemBoxPlot( bool first );

    QString
    getSubsetName( const QString &name = "" );

private:
    void
    resetTopology();

    /************ general private *********/

    //type: metric, call or system tab;
    //see constants.h for TabWidgetType
    TabWidgetType type;

    //the cube database
    cube::Cube* cube;

    //value modus;
    //see constants.h for ValueModus
    ValueModus valueModus;

    //different widgets needed to access

    TabWidget*       metricTabWidget;
    TabWidget*       callTabWidget;
    TabWidget*       systemTabWidget;

    QSplitter*       splitterWidget;
    ColorWidget*     colorWidget;
    ValueWidget*     valueWidget;
    PrecisionWidget* precisionWidget;

    //external metric items and their names for the external percentage value modus
    std::vector<TreeWidgetItem*> externalMetricItems;
    std::vector<std::string>     externalMetricNames;


    /****** tree-widget-specific private ************/

    //fond and line spacing for trees
    QFont treeFont;
    int   spacing;

    //how should selected items be marked: by a special background or by framing
    SelectionSyntax selectionSyntax;


    /*************** topology-specific private *****************/

    //the currently displayed topology widget;
    //if the tab is not a system tab or if the system tree is displayed,
    //this is NULL
    SystemTopologyWidget* currentSysTopWidget;

    //system box widget : if the tab is not a system tab or if the system tree is displayed,
    //this is NULL
    SystemBoxPlot* systemBoxWidget;

    int
    topologyIndex( int tabIndex );

    //the topology tool bar's actions are enabled only if in the tab widget
    //the current element is a topology display (SystemTabWidget)
    void
    enableTopologyButtons();
    void
    disableTopologyButtons();

    //line type and line antialiasing for line drawing in topologies
    LineType lineType;
    bool     antialiasing;
    void
    setAntialiasing( bool value );

    //should unsused planes be displayed in topologies?
    bool showUnusedTopologyPlanes;

    //in topologies: should zero values be colored by white or by the minimal color?
    bool             whiteForZero;

    int              systemBoxPlotIndex; // index of box plot widget

    QComboBox*       subsetCombo;
    QStringListModel subsetModel;
};

#endif
