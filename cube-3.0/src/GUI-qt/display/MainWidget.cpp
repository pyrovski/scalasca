/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <QString>
#include <QPixmap>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "ParseContext.h"
#include "cube_error.h"
#include "services.h"

#include "Constants.h"
#include "MainWidget.h"
#include "Action.h"
#include "TabWidget.h"
#include "TreeWidget.h"
#include "TypedWidget.h"
#include "SplitterDialog.h"
#include "ValueWidget.h"
#include "PrecisionWidget.h"
#include "MessageWidget.h"
#include "FontWidget.h"
#include "Statistics.h"

#include <cassert>
#include <iostream>
#ifndef CUBE_COMPRESSED
#  include <fstream>
#else
#  include "zfstream.h"
#endif

using namespace std;


MainWidget::MainWidget()
{
    resize( 800, 600 );
    setWindowIcon( QIcon( ":images/CubeIcon.xpm" ) );

    statusBar()->showMessage( tr( "Initializing display..." ) );

    //the common color widget for defining coloring and converting values into colors
    colorWidget = new ColorWidget();
    connect( colorWidget, SIGNAL( apply() ), this, SLOT( onApply() ) );

    //the precision widget for setting user-defined precision
    //also used to convert numbers into text
    precisionWidget = new PrecisionWidget( this );
    connect( precisionWidget, SIGNAL( apply() ), this, SLOT( onApply() ) );

    //the three value widgets placed below the tab widgets
    metricValueWidget = new ValueWidget( colorWidget, precisionWidget );
    callValueWidget   = new ValueWidget( colorWidget, precisionWidget );
    systemValueWidget = new ValueWidget( colorWidget, precisionWidget, true );

    //the three tab widgets containing the trees/topologies

    metricTabWidget = new TabWidget( METRICTAB, colorWidget, metricValueWidget, precisionWidget );
    callTabWidget   = new TabWidget( CALLTAB, colorWidget, callValueWidget, precisionWidget );
    systemTabWidget = new TabWidget( SYSTEMTAB, colorWidget, systemValueWidget, precisionWidget );

    metricTabWidget->setMetricTabWidget( metricTabWidget );
    callTabWidget->setMetricTabWidget( metricTabWidget );
    systemTabWidget->setMetricTabWidget( metricTabWidget );

    metricTabWidget->setCallTabWidget( callTabWidget );
    callTabWidget->setCallTabWidget( callTabWidget );
    systemTabWidget->setCallTabWidget( callTabWidget );

    metricTabWidget->setSystemTabWidget( systemTabWidget );
    callTabWidget->setSystemTabWidget( systemTabWidget );
    systemTabWidget->setSystemTabWidget( systemTabWidget );


    //the splitter widget

    splitterWidget = new QSplitter();

    metricTabWidget->setSplitterWidget( splitterWidget );
    callTabWidget->setSplitterWidget( splitterWidget );
    systemTabWidget->setSplitterWidget( splitterWidget );

    //metric splitter element

    TypedWidget* metricWidget = new TypedWidget();
    metricWidget->setType( METRICWIDGET );
    QVBoxLayout* metricLayout = new QVBoxLayout();
    metricLayout->setMargin( LAYOUT_MARGIN );
    metricLayout->setSpacing( LAYOUT_SPACING );

    metricCombo = new QComboBox();

    metricLayout->addWidget( metricCombo );
    metricLayout->addWidget( metricTabWidget );
    metricLayout->addWidget( metricValueWidget );

    ( ( QWidget* )metricWidget )->setLayout( metricLayout );

    //call splitter element

    TypedWidget* callWidget = new TypedWidget();
    callWidget->setType( CALLWIDGET );
    QVBoxLayout* callLayout = new QVBoxLayout();
    callLayout->setMargin( LAYOUT_MARGIN );
    callLayout->setSpacing( LAYOUT_SPACING );

    callCombo = new QComboBox();

    callLayout->addWidget( callCombo );
    callLayout->addWidget( callTabWidget );
    callLayout->addWidget( callValueWidget );

    ( ( QWidget* )callWidget )->setLayout( callLayout );

    //system splitter element

    TypedWidget* systemWidget = new TypedWidget();
    systemWidget->setType( SYSTEMWIDGET );
    QVBoxLayout* systemLayout = new QVBoxLayout();
    systemLayout->setMargin( LAYOUT_MARGIN );
    systemLayout->setSpacing( LAYOUT_SPACING );

    systemCombo = new QComboBox();

    systemLayout->addWidget( systemCombo );
    systemLayout->addWidget( systemTabWidget );
    systemLayout->addWidget( systemValueWidget );

    ( ( QWidget* )systemWidget )->setLayout( systemLayout );


    metricCombo->setWhatsThis( "Each tree view has its own value mode combo, a drop-down menu above the tree, where it is possible to change the way the severity values are displayed.\n\nThe default value mode is the Absolute value mode. In this mode, as explained below, the severity values from the Cube file are displayed. However, sometimes these values may be hard to interpret, and in such cases other value modes can be applied. Basically, there are three categories of additional value modes.\n\n- The first category presents all severities in the tree as percentage of a reference value. The reference value can be the absolute value of a selected or a root node from the same tree or in one of the trees on the left-hand-side. For example, in the Own root percent value mode the severity values are presented as percentage of the own root's (inclusive) severity value. This way you can see how the severities are distributed within the tree. The value modes 2-8 below fall into this category.\n\nAll nodes of trees on the left-hand-side of the metric tree have undefined values. (Basically, we could compute values for them, but it would sum up the severities over all metrics, that have different meanings and usually even different units, and thus those values would not have much expressiveness.) Since we cannot compute percentage values based on undefined reference values, such value modes are not supported. For example, if the call tree is on the left-hand-side, and the metric tree is in the middle, then the metric tree does not offer the Call root percent mode.\n\n- The second category is available for system trees only, and shows the distribution of the values within hierarchy levels. E.g., the Peer percent value mode displays the severities as percentage of the maximal value on the same hierarchy depth. The value modes 9-10 fall into this category.\n\n- Finally, the External percent value mode relates the severity values to severities from another external Cube file.\n\nDepending on the type and position of the tree, the following value modes may be available:\n\n1) Absolute (default)\n2) Metric root percent\n3) Metric selection percent\n4) Call root percent\n5) Call selection percent\n6) System root percent\n7) System selection percent\n8) Own root percent\n9) Peer percent\n10) Peer distribution\n11) External percent" );

    callCombo->setWhatsThis( metricCombo->whatsThis() );
    systemCombo->setWhatsThis( metricCombo->whatsThis() );

    //splitter widget

    splitterWidget->addWidget( ( QWidget* )metricWidget );
    splitterWidget->addWidget( ( QWidget* )callWidget );
    splitterWidget->addWidget( ( QWidget* )systemWidget );


    //the main widget

    QWidget* mainWidget = new QWidget();
    setCentralWidget( mainWidget );

    QVBoxLayout* mainLayout = new QVBoxLayout();
    //LAYOUT_MARGIN and LAYOUT_SPACING are defined in constants.h
    mainLayout->setMargin( LAYOUT_MARGIN );
    mainLayout->setSpacing( LAYOUT_SPACING );

    mainLayout->addWidget( splitterWidget );
    mainLayout->addWidget( colorWidget );

    mainWidget->setLayout( mainLayout );


    //event handling

    connect( metricCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( onSetMetricValueModus( int ) ) );
    connect( callCombo,   SIGNAL( currentIndexChanged( int ) ), this, SLOT( onSetCallValueModus( int ) ) );
    connect( systemCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( onSetSystemValueModus( int ) ) );

    connect( metricTabWidget, SIGNAL( setMessage( QString ) ), this, SLOT( setMessage( QString ) ) );
    connect( callTabWidget,  SIGNAL( setMessage( QString ) ), this, SLOT( setMessage( QString ) ) );
    connect( systemTabWidget, SIGNAL( setMessage( QString ) ), this, SLOT( setMessage( QString ) ) );


    //topology tool bar
    topologyToolBar = SystemTopologyToolBar::getInstance();
    addToolBar( topologyToolBar );
    topologyToolBar->hide();

    //menu creation
    //note: system tab widget must be created before createMenu() gets called
    //note: topologyToolBar must be created before createMenu() gets called
    createMenu();


    //initialization

    cube                 = NULL;
    lastExternalFileName = "";
    cubeExternal         = NULL;
    dynloadThreshold     = 1000000000;

    updateWidgetTitle();
    update();

    //initialize tree look (font, line spacing)
    treeFont = font();
    spacing  = 5;

    metricTabWidget->setTreeDisplay( treeFont, spacing );
    callTabWidget->setTreeDisplay( treeFont, spacing );
    systemTabWidget->setTreeDisplay( treeFont, spacing );

    updateCombos();

    statusBar()->showMessage( tr( "Ready" ) );
    statusBar()->setWhatsThis( "This is the status bar. It displays some status information, like state of execution for longer procedures, hints for menus the mouse pointing at etc." );

    setWhatsThis( CUBE_NAME " is a presentation component suitable for displaying performance data for parallel programs including MPI and OpenMP applications. Program performance is represented in a multi-dimensional space including various program and system resources. The tool allows the interactive exploration of this space in a scalable fashion and browsing the different kinds of performance behavior with ease. Cube also includes a library to read and write performance data as well as operators to compare, integrate, and summarize data from different experiments." );

    menuBar()->setWhatsThis( "The menu bar consists of three menus, a file menu, a display menu, and a help menu. Some menu functions have also a keyboard shortcut, which is written beside the menu item's name in the menu. E.g., you can open a file with Ctrl+O without going into the menu.  A short description of the menu items is visible in the status bar if you stay for a short while with the mouse above a menu item." );

    statistics = 0;
#if defined( WITH_VAMPIR ) || ( WITH_PARAVER )
    fileLine   = 0;
    configLine = 0;
#endif


    // load defualt settings
    loadSettings( QString( "<default>" ) );
}
//end of constructor


MainWidget::~MainWidget()
{
#if defined( WITH_VAMPIR ) || ( WITH_PARAVER )
    disableTraceBrowserMenu();
#endif
    delete statistics;
    statistics = 0;
    delete cube;
}


/*****************************************************/
/****** methods for the value modus           ********/
/*****************************************************/

//Besides being inclusive/exclusive values (for collapsed/expanded items),
//the values displayed for the different items are computed according to the value modus.
//Not all value modi are available in all tabs.
//
//- Absolute value: the corresponding values from the cube database.
//
//- Own root percent: the absolute value as percentage of the absolute inclusive value
//  of the root item of the given item.
//- Metric root percent: the absolute value as percentage of the absolute inclusive value
//  of the root of the selected metric item(s).
//- Call root percent: the absolute value as percentage of the absolute inclusive value
//  of the root item of the selected call item. If several call items are selected with different roots,
//  we take the sum up those root values for the reference value.
//- System root percent: the absolute value as percentage of the absolute inclusive value
//  of the root item of the selected system item. If several system items are selected with different roots,
//  we take the sum up those root values for the reference value.
//
//- Metric selection percent: the absolute value as percentage of the sum of absolute
//  (inclusive or exclusive according to collapsed/expanded) values
//  of the selected metric items.
//- Call selection percent: the absolute value as percentage of the sum of absolute
//  (inclusive or exclusive according to collapsed/expanded) values
//  of the selected call items.
//- System selection percent: the absolute value as percentage of the sum of absolute
//  (inclusive or exclusive according to collapsed/expanded) values
//  of the selected system items.
//
//- Peer percent (for the system component only) : the absolute inclusive value of the item as percentage
//  of the largest absolute inclusive peer value (peer = item at the same hierarchy depth)
//- Peer distribution: we assign the value 100.0 to the largest inclusive absolute peer value,
//  0.0 to the smallest one, and scale the values inbetween linearly
//
//- External percent: like metric root percent, but the metric root value is read from
//  some external data

//The displayed names (xxx_NAME) and the internal type for value modi are defined in constants.h.



//return the internal type for a value modus with the given displayed name
//parameter: displayed name of the value modus
//
ValueModus
MainWidget::getValueModus( QString text )
{
    if ( text == ABSOLUTE_NAME )
    {
        return ABSOLUTE;
    }
    else if ( text.compare( OWNROOT_NAME ) == 0 )
    {
        return OWNROOT;
    }
    else if ( text.compare( METRICSELECTED_NAME ) == 0 )
    {
        return METRICSELECTED;
    }
    else if ( text.compare( CALLSELECTED_NAME ) == 0 )
    {
        return CALLSELECTED;
    }
    else if ( text.compare( SYSTEMSELECTED_NAME ) == 0 )
    {
        return SYSTEMSELECTED;
    }
    else if ( text.compare( METRICROOT_NAME ) == 0 )
    {
        return METRICROOT;
    }
    else if ( text.compare( CALLROOT_NAME ) == 0 )
    {
        return CALLROOT;
    }
    else if ( text.compare( SYSTEMROOT_NAME ) == 0 )
    {
        return SYSTEMROOT;
    }
    else if ( text.compare( PEER_NAME ) == 0 )
    {
        return PEER;
    }
    else if ( text.compare( PEERDIST_NAME ) == 0 )
    {
        return PEERDIST;
    }
    else if ( text.compare( EXTERNAL_NAME ) == 0 )
    {
        return EXTERNAL;
    }
    else
    {
        assert( false );
        return ABSOLUTE;
    }
}

//return the displayed name for a given value modus
//parameter: the value modus
//
QString
MainWidget::getValueModusText( ValueModus modus )
{
    if ( modus == ABSOLUTE )
    {
        return ABSOLUTE_NAME;
    }
    else if ( modus == OWNROOT )
    {
        return OWNROOT_NAME;
    }
    else if ( modus == METRICSELECTED )
    {
        return METRICSELECTED_NAME;
    }
    else if ( modus == CALLSELECTED )
    {
        return CALLSELECTED_NAME;
    }
    else if ( modus == SYSTEMSELECTED )
    {
        return SYSTEMSELECTED_NAME;
    }
    else if ( modus == METRICROOT )
    {
        return METRICROOT_NAME;
    }
    else if ( modus == CALLROOT )
    {
        return CALLROOT_NAME;
    }
    else if ( modus == SYSTEMROOT )
    {
        return SYSTEMROOT_NAME;
    }
    else if ( modus == PEER )
    {
        return PEER_NAME;
    }
    else if ( modus == PEERDIST )
    {
        return PEERDIST_NAME;
    }
    else if ( modus == EXTERNAL )
    {
        return EXTERNAL_NAME;
    }
    else
    {
        assert( false );
        return ABSOLUTE_NAME;
    }
}


//sets the value modus for the metric component as defined by the metric combo
//
void
MainWidget::onSetMetricValueModus( int modus )
{
    assert( modus < metricCombo->count() );
    ValueModus vmodus = getValueModus( metricCombo->itemText( modus ) );
    bool       ok     = true;
    //load external file for external percentage if no external data is currently loaded
    if ( vmodus == EXTERNAL && lastExternalFileName == "" )
    {
        ok = openExternalFile();
    }
    if ( ok )
    {
        metricTabWidget->setValueModus( vmodus );
        metricTabWidget->displayItems();
    }
    else
    {
        //if loading external data failed or was cancelled, then re-set the old value modus
        QString name = getValueModusText( metricTabWidget->getValueModus() );
        metricCombo->setCurrentIndex( metricCombo->findText( name ) );
    }
}

//sets the value modus for the call component as defined by the call combo
//
void
MainWidget::onSetCallValueModus( int modus )
{
    assert( modus < callCombo->count() );
    ValueModus vmodus = getValueModus( callCombo->itemText( modus ) );
    bool       ok     = true;
    //load external file for external percentage if no external data is currently loaded
    if ( vmodus == EXTERNAL && lastExternalFileName == "" )
    {
        ok = openExternalFile();
    }
    if ( ok )
    {
        callTabWidget->setValueModus( vmodus );
        callTabWidget->displayItems();
    }
    else
    {
        //if loading external data failed or was cancelled, then re-set the old value modus
        QString name = getValueModusText( callTabWidget->getValueModus() );
        callCombo->setCurrentIndex( callCombo->findText( name ) );
    }
}

//sets the value modus for the system component as defined by the system combo
//
void
MainWidget::onSetSystemValueModus( int modus )
{
    assert( modus < systemCombo->count() );
    ValueModus vmodus = getValueModus( systemCombo->itemText( modus ) );
    bool       ok     = true;
    //load external file for external percentage if no external data is currently loaded
    if ( vmodus == EXTERNAL && lastExternalFileName == "" )
    {
        ok = openExternalFile();
    }
    if ( ok )
    {
        systemTabWidget->setValueModus( vmodus );
        systemTabWidget->displayItems();
    }
    else
    {
        //if loading external data failed or was cancelled, then re-set the old value modus
        QString name = getValueModusText( systemTabWidget->getValueModus() );
        systemCombo->setCurrentIndex( systemCombo->findText( name ) );
    }
}



/**********************************************************/
/****************** menu **********************************/
/**********************************************************/

//creates the pull-down menu
//
void
MainWidget::createMenu()
{
    menuBar()->setStatusTip( tr( "Ready" ) );

    //file menu

    fileMenu = menuBar()->addMenu( tr( "&File" ) );
    fileMenu->setStatusTip( tr( "Ready" ) );

    Action* openAct = new Action( tr( "&Open..." ), this );
    openAct->setShortcut( tr( "Ctrl+O" ) );
    openAct->setStatusTip( tr( "Opens a cube file" ) );
    connect( openAct, SIGNAL( triggered() ), this, SLOT( openFile() ) );
    fileMenu->addAction( openAct );
    openAct->setWhatsThis( "Offers a selection dialog to open a Cube file. In case of an already opened file, it will be closed before a new file gets opened. If a file got opened successfully, it gets added to the top of the recent files list (see below). If it was already in the list, it is moved to the top." );

    Action* closeAct = new Action( tr( "&Close" ), this );
    closeAct->setShortcut( tr( "Ctrl+W" ) );
    closeAct->setStatusTip( tr( "Closes the open file" ) );
    closeAct->setEnabled( false );
    connect( closeAct, SIGNAL( triggered() ), this, SLOT( closeFile() ) );
    connect( this, SIGNAL( fileClosed() ), closeAct, SLOT( disable() ) );
    connect( this, SIGNAL( fileOpened() ), closeAct, SLOT( enable() ) );
    fileMenu->addAction( closeAct );
    closeAct->setWhatsThis( "Closes the currently opened Cube file. Disabled if no file is opened." );

    Action* openExtAct = new Action( tr( "Open &external..." ), this );
    openExtAct->setStatusTip( tr( "Opens a second cube file for external percentage" ) );
    connect( openExtAct, SIGNAL( triggered() ), this, SLOT( openExternalFile() ) );
    fileMenu->addAction( openExtAct );
    openExtAct->setWhatsThis( "Opens a file for the external percentage value mode (see further help on the value modes)." );

    Action* closeExtAct = new Action( tr( "Close e&xternal" ), this );
    closeExtAct->setStatusTip( tr( "Closes the second cube file for external percentage" ) );
    closeExtAct->setEnabled( false );
    connect( closeExtAct, SIGNAL( triggered() ), this, SLOT( closeExternalFile() ) );
    connect( this, SIGNAL( externalFileClosed() ), closeExtAct, SLOT( disable() ) );
    connect( this, SIGNAL( externalFileOpened() ), closeExtAct, SLOT( enable() ) );
    fileMenu->addAction( closeExtAct );
    closeExtAct->setWhatsThis( "Closes the current external file and removes all corresponding data. Disabled if no external file is opened." );

#if defined( WITH_VAMPIR ) || ( WITH_PARAVER )
    createTraceBrowserMenu();
#endif

    QMenu* settingsMenu = fileMenu->addMenu( "Settings" );
    settingsMenu->setStatusTip( tr( "Ready" ) );
    settingsMenu->setWhatsThis( "This menu item offers the saving, loading, and the deletion of settings. On the one hand, settings store the outlook of the application like the widget sizes, color and precision settings, the order of panes, etc. On the other hand, settings can also store which data is loaded, which tree nodes are expanded, etc. When saving a setting, the outlook is always saved. While saving, you will be asked if you would also like to save the data-related settings.  Note that you can save several settings under different names." );

    Action* saveDefaultSettingsAct = new Action( tr( "&Save settings as <default>" ), this );
    saveDefaultSettingsAct->setStatusTip( tr( "Saves the current settings as default settings. They are loaded automatically after start." ) );
    connect( saveDefaultSettingsAct, SIGNAL( triggered() ), this, SLOT( saveDefaultSettings() ) );
    settingsMenu->addAction( saveDefaultSettingsAct );
    saveDefaultSettingsAct->setWhatsThis( "Save the current settings under a user-defined name as default.\nOn the one hand, settings store the outlook of the application like the widget sizes, color and precision settings, the order of panes, etc. On the other hand, settings can also store which data is loaded, which tree nodes are expanded, etc. When saving a setting, the outlook is always saved. \n\n Default settings are loaded automatically after start." );


    Action* saveSettingsAct = new Action( tr( "&Save settings as..." ), this );
    saveSettingsAct->setStatusTip( tr( "Saves the current settings" ) );
    connect( saveSettingsAct, SIGNAL( triggered() ), this, SLOT( saveSettings() ) );
    settingsMenu->addAction( saveSettingsAct );
    saveSettingsAct->setWhatsThis( "Save the current settings under a user-defined name.\nOn the one hand, settings store the outlook of the application like the widget sizes, color and precision settings, the order of panes, etc. On the other hand, settings can also store which data is loaded, which tree nodes are expanded, etc. When saving a setting, the outlook is always saved. While saving, you will be asked if you would also like to save the data-related settings. Note that you can save several settings under different names." );


    Action* loadSettingsAct = new Action( tr( "&Load settings..." ), this );
    loadSettingsAct->setStatusTip( tr( "Loads some settings" ) );
    connect( loadSettingsAct, SIGNAL( triggered() ), this, SLOT( loadSettings() ) );
    settingsMenu->addAction( loadSettingsAct );
    loadSettingsAct->setWhatsThis( "Load a previously stored setting with a user-defined name. If the setting stores also data settings, then the corresponding data is also loaded. Note that if the corresponding Cube file does not exist or is not consistent with the stored settings (e.g. the number of items has been changed), Cube cannot load the data.\nOn the one hand, settings store the outlook of the application like the widget sizes, color and precision settings, the order of panes, etc. On the other hand, settings can also store which data is loaded, which tree nodes are expanded, etc. When saving a setting, the outlook is always saved. While saving, you will be asked if you would also like to save the data-related settings. Note that you can save several settings under different names." );

    Action* deleteSettingsAct = new Action( tr( "&Delete settings..." ), this );
    deleteSettingsAct->setStatusTip( tr( "Deletes some settings" ) );
    connect( deleteSettingsAct, SIGNAL( triggered() ), this, SLOT( deleteSettings() ) );
    settingsMenu->addAction( deleteSettingsAct );
    deleteSettingsAct->setWhatsThis( "Delete a previously stored setting with a user-defined name.\nOn the one hand, settings store the outlook of the application like the widget sizes, color and precision settings, the order of panes, etc. On the other hand, settings can also store which data is loaded, which tree nodes are expanded, etc. When saving a setting, the outlook is always saved. While saving, you will be asked if you would also like to save the data-related settings. Note that you can save several settings under different names." );

    fileMenu->addSeparator();

#ifdef WITH_DYNLOAD
    Action* dynloadAct = new Action( tr( "D&ynamic loading threshold..." ), this );
    dynloadAct->setStatusTip( tr( "Sets the threshold for dynamic loading of data" ) );
    connect( dynloadAct, SIGNAL( triggered() ), this, SLOT( setDynloadThreshold() ) );
    fileMenu->addAction( dynloadAct );
    dynloadAct->setWhatsThis( "By default, Cube always loads the whole amount of data when you open a Cube file. However, Cube offers also a possibility to load only those data which is needed for the current display. To be more precise, the data for the selected metric(s) and, if a selected metric is expanded, the data for its children are loaded. If you change the metric selection, possibly some new data is needed for the display that is dynamically loaded on demand. Currently not needed data gets unloaded.\n\nThis functionality is useful most of all for large files. Under this menu item you can define a file size threshold (in bytes) above which Cube offers you dynamic data loading. If a file being opened is larger than this threshold, Cube will ask you if you wish dynamic loading." );
#endif

    Action* screenshotAct = new Action( tr( "Sc&reenshot..." ), this );
    screenshotAct->setStatusTip( tr( "Saves a screenshot into the file cube.png" ) );
    connect( screenshotAct, SIGNAL( triggered() ), this, SLOT( screenshot() ) );
    fileMenu->addAction( screenshotAct );
    screenshotAct->setWhatsThis( "The function offers you to save a screenshot in a png file. Unfortunately the outer frame of the main window is not saved, only the application itself." );

    fileMenu->addSeparator();

    Action* quitAct = new Action( tr( "&Quit" ), this );
    quitAct->setShortcut( tr( "Ctrl+Q" ) );
    quitAct->setStatusTip( tr( "Exits the application" ) );
    connect( quitAct, SIGNAL( triggered() ), this, SLOT( closeApplication() ) );
    fileMenu->addAction( quitAct );
    quitAct->setWhatsThis( "Closes the application." );

    fileMenu->addSeparator();

    //display menu

    QMenu* displayMenu = menuBar()->addMenu( tr( "&Display" ) );
    displayMenu->setStatusTip( tr( "Ready" ) );

    splitterOrderAct = new Action( tr( "D&imension order..." ), this );
    splitterOrderAct->setStatusTip( tr( "Sets the order of the dimensions metric, call chain, and system." ) );
    connect( splitterOrderAct, SIGNAL( triggered() ), this, SLOT( setDimensionOrder() ) );
    displayMenu->addAction( splitterOrderAct );
    splitterOrderAct->setWhatsThis( "As explained above, Cube has three resizable panes. Initially the metric pane is on the left, the call pane is in the middle, and the system pane is on the right-hand-side. However, sometimes you may be interested in other orders, and that is what this menu item is about. It offers all possible pane orderings.  For example, assume you would like to see the metric and call values for a certain thread.  In this case, you should place the system pane on the left, the metric pane in the middle, and the call pane on the right. Note that in panes left-hand-side of the metric pane we have no meaningful values, since they miss a reference metric; in this case we specify the values to be undefined, denoted by a \"-\" sign." );

    Action* colorsAct = new Action( tr( "&General coloring..." ), this );
    colorsAct->setStatusTip( tr( "Defines the colors for the display." ) );
    connect( colorsAct, SIGNAL( triggered() ), this, SLOT( setColors() ) );
    displayMenu->addAction( colorsAct );
    colorsAct->setWhatsThis( "Opens a dialog that allows the user to change the general color settings. See also help on the dialog itself." );

    Action* precisionAct = new Action( tr( "&Precision..." ), this );
    precisionAct->setStatusTip( tr( "Defines the precision of the display for numbers." ) );
    connect( precisionAct, SIGNAL( triggered() ), this, SLOT( setPrecision() ) );
    precisionAct->setWhatsThis( "Activating this menu item opens a dialog for precision settings. See also help on the dialog itself." );
    displayMenu->addAction( precisionAct );

    QMenu* treeMenu = displayMenu->addMenu( tr( "Trees" ) );
    treeMenu->setStatusTip( tr( "Ready" ) );

    Action* fontAct = new Action( tr( "Fo&nt..." ), this );
    fontAct->setStatusTip( tr( "Defines font type and size, and spacing for the tree widgets." ) );
    connect( fontAct, SIGNAL( triggered() ), this, SLOT( setFont() ) );
    fontAct->setWhatsThis( "Opens a dialog to specify the font, the font size (in pt), and the line spacing for the tree displays.  The \"Ok\" button applies the settings to the display and closes the dialog, the \"Apply\" button applies the settings to the display, and \"Cancel\" cancels all changes since the dialog was opened (even if \"Apply\" was pressed in between) and closes the dialog." );
    treeMenu->addAction( fontAct );

    QMenu* selectionMenu = treeMenu->addMenu( tr( "Selection marking" ) );
    selectionMenu->setWhatsThis( "Here you can specify if selected items in trees should be marked by a blue background or by a frame." );
    selectionMenu->setStatusTip( tr( "Ready" ) );

    selection1Act = new Action( tr( "Blue background" ), this );
    selection1Act->setStatusTip( tr( "Selected items in trees should be marked by a blue background." ) );
    selection1Act->setCheckable( true );
    selection1Act->setChecked( true );
    connect( selection1Act, SIGNAL( triggered() ), this, SLOT( setSelectionBackground() ) );
    selection1Act->setWhatsThis( "Here you can specify that selected items in trees should be marked by a blue background." );
    selectionMenu->addAction( selection1Act );

    selection2Act = new Action( tr( "Framing" ), this );
    selection2Act->setStatusTip( tr( "Selected items in trees should be marked by a frame." ) );
    selection2Act->setCheckable( true );
    selection2Act->setChecked( false );
    connect( selection2Act, SIGNAL( triggered() ), this, SLOT( setSelectionFrame() ) );
    selection2Act->setWhatsThis( "Here you can specify that selected items in trees should be marked by a frame." );
    selectionMenu->addAction( selection2Act );

    QActionGroup* selectionActionGroup = new QActionGroup( this );
    //only one element in this group can be checked
    selectionActionGroup->setExclusive( true );
    selectionActionGroup->addAction( selection1Act );
    selectionActionGroup->addAction( selection2Act );

    QMenu* widthMenu = displayMenu->addMenu( tr( "Optimize width" ) );
    widthMenu->setStatusTip( tr( "Ready" ) );
    widthMenu->setWhatsThis( "Under this menu item Cube offers widget rescaling such that the amount of information shown is maximized, i.e., Cube optimally distributes the available space between its components. You can chose if you would like to stick to the current main window size, or if you allow to resize it." );

    Action* width1Act = new Action( tr( "&Keep main window size" ), this );
    width1Act->setStatusTip( tr( "Optimize widths of tabs in order to show equally large percentages." ) );
    connect( width1Act, SIGNAL( triggered() ), this, SLOT( distributeWidth() ) );
    width1Act->setWhatsThis( "Under this menu item Cube offers widget rescaling with keeping the main window size, such that the amount of information shown is maximized, i.e., Cube optimally distributes the available space between its components." );
    widthMenu->addAction( width1Act );

    Action* width2Act = new Action( tr( "&Adapt main window size" ), this );
    width2Act->setStatusTip( tr( "Resizes the main window width and adapt tab widths in order to show all informations" ) );
    connect( width2Act, SIGNAL( triggered() ), this, SLOT( adaptWidth() ) );
    width2Act->setWhatsThis( "Under this menu item Cube offers widget rescaling possibly changing the main window size, such that the amount of information shown is maximized, i.e., Cube optimally distributes the available space between its components." );
    widthMenu->addAction( width2Act );

    //topology menu
    QMenu* topologyMenu = menuBar()->addMenu( tr( "&Topology" ) );
    topologyMenu->setStatusTip( tr( "Ready" ) );

    QMenu* topologyColoringMenu = topologyMenu->addMenu( tr( "Item coloring" ) );
    topologyColoringMenu->setStatusTip( tr( "Ready" ) );
    topologyColoringMenu->setWhatsThis( "This menu item offers a choice how zero-valued system nodes should be colored in the topology display. The two offered options are either to use white or to use white only if all system leaf values are zero and use the minimal color otherwise." );

    white1Act = new Action( tr( "color for zero: white" ), this );
    white1Act->setStatusTip( tr( "Use white as color for zero values in the topology display" ) );
    connect( white1Act, SIGNAL( triggered() ), systemTabWidget, SLOT( whiteOn() ) );
    white1Act->setCheckable( true );
    white1Act->setChecked( true );
    topologyColoringMenu->addAction( white1Act );
    white1Act->setWhatsThis( "Defines that always the white color should be used to color zero-valued items in topologies." );

    white2Act = new Action( tr( "color for zero: white only if all zero" ), this );
    white2Act->setStatusTip( tr( "Use white as color for zero values in the topology display only if all values are zero" ) );
    connect( white2Act, SIGNAL( triggered() ), systemTabWidget, SLOT( whiteOff() ) );
    white2Act->setCheckable( true );
    white2Act->setChecked( false );
    topologyColoringMenu->addAction( white2Act );
    white2Act->setWhatsThis( "Defines that the white color should be used to color zero-valued items in topologies only if all system leaf values are zero, and the minimal color should be used otherwise." );

    QActionGroup* whiteActionGroup = new QActionGroup( this );
    whiteActionGroup->setExclusive( true );
    whiteActionGroup->addAction( white1Act );
    whiteActionGroup->addAction( white2Act );

    QMenu* topologyLineStyleMenu = topologyMenu->addMenu( tr( "Line coloring" ) );
    topologyLineStyleMenu->setStatusTip( tr( "Ready" ) );
    topologyLineStyleMenu->setWhatsThis( "Allows to define the color of the lines in topology painting. Available colors are black, gray, white, or no lines." );

    blackLinesAct = new Action( tr( "Black" ), this );
    blackLinesAct->setStatusTip( tr( "Lines in topology should be painted black." ) );
    connect( blackLinesAct, SIGNAL( triggered() ), systemTabWidget, SLOT( blackLines() ) );
    blackLinesAct->setCheckable( true );
    blackLinesAct->setChecked( true );
    blackLinesAct->setWhatsThis( "Sets the color of the lines in topology painting to black." );
    topologyLineStyleMenu->addAction( blackLinesAct );

    grayLinesAct = new Action( tr( "Gray" ), this );
    grayLinesAct->setStatusTip( tr( "Lines in topology should be painted gray." ) );
    connect( grayLinesAct, SIGNAL( triggered() ), systemTabWidget, SLOT( grayLines() ) );
    grayLinesAct->setCheckable( true );
    grayLinesAct->setChecked( false );
    grayLinesAct->setWhatsThis( "Sets the color of the lines in topology painting to gray." );
    topologyLineStyleMenu->addAction( grayLinesAct );

    whiteLinesAct = new Action( tr( "White" ), this );
    whiteLinesAct->setStatusTip( tr( "Lines in topology should be painted white." ) );
    connect( whiteLinesAct, SIGNAL( triggered() ), systemTabWidget, SLOT( whiteLines() ) );
    whiteLinesAct->setCheckable( true );
    whiteLinesAct->setChecked( false );
    whiteLinesAct->setWhatsThis( "Sets the color of the lines in topology painting to white." );
    topologyLineStyleMenu->addAction( whiteLinesAct );

    noLinesAct = new Action( tr( "No lines" ), this );
    noLinesAct->setStatusTip( tr( "Lines in topology should not be painted." ) );
    connect( noLinesAct, SIGNAL( triggered() ), systemTabWidget, SLOT( noLines() ) );
    noLinesAct->setCheckable( true );
    noLinesAct->setChecked( false );
    noLinesAct->setWhatsThis( "Defines that there should be no framing lines for the items in topologies." );
    topologyLineStyleMenu->addAction( noLinesAct );

    QActionGroup* linesActionGroup = new QActionGroup( this );
    //only one element in this group can be checked
    linesActionGroup->setExclusive( true );
    linesActionGroup->addAction( blackLinesAct );
    linesActionGroup->addAction( grayLinesAct );
    linesActionGroup->addAction( whiteLinesAct );
    linesActionGroup->addAction( noLinesAct );

    QMenu* toolbarMenu = topologyMenu->addMenu( tr( "Toolbar" ) );
    toolbarMenu->setStatusTip( tr( "Ready" ) );
    connect( toolbarMenu, SIGNAL( aboutToShow() ), this, SLOT( updateToolbarMenu() ) );
    toolbarMenu->setWhatsThis( "This menu item allows to specify if the tool bar's buttons should be labeled by icons, by a text description, or if the tool bar should be hidden. See also help on the tool bar itself." );

    textAct = new Action( tr( "Text" ), this );
    textAct->setStatusTip( tr( "Toolbar icons should be labeled with text" ) );

    connect( textAct, SIGNAL( triggered() ), topologyToolBar, SLOT( setButtonsToText() ) );
    connect( textAct, SIGNAL( triggered() ), topologyToolBar, SLOT( show() ) );
    textAct->setCheckable( true );
    textAct->setChecked( false );
    textAct->setWhatsThis( "Here you can specify that the tool bar's buttons should be labeled by text descriptions. See also help on the tool bar itself." );
    toolbarMenu->addAction( textAct );

    iconAct = new Action( tr( "Icon" ), this );
    iconAct->setStatusTip( tr( "Toolbar icons should be labeled with icons" ) );
    connect( iconAct, SIGNAL( triggered() ), topologyToolBar, SLOT( setButtonsToIcon() ) );
    connect( iconAct, SIGNAL( triggered() ), topologyToolBar, SLOT( show() ) );
    iconAct->setCheckable( true );
    iconAct->setChecked( true );
    iconAct->setWhatsThis( "Here you can specify that the tool bar's buttons should be labeled by icons. See also help on the tool bar itself." );
    toolbarMenu->addAction( iconAct );

    hideToolbarAct = new Action( tr( "Hide" ), this );
    hideToolbarAct->setStatusTip( tr( "Hide toolbar" ) );
    connect( hideToolbarAct, SIGNAL( triggered() ), topologyToolBar, SLOT( hide() ) );
    hideToolbarAct->setCheckable( true );
    hideToolbarAct->setChecked( false );
    hideToolbarAct->setWhatsThis( "Here you can hide the tool bar. See also help on the tool bar itself." );
    toolbarMenu->addAction( hideToolbarAct );

    QActionGroup* toolbarActionGroup = new QActionGroup( this );
    toolbarActionGroup->setExclusive( true );
    toolbarActionGroup->addAction( textAct );
    toolbarActionGroup->addAction( iconAct );
    toolbarActionGroup->addAction( hideToolbarAct );

    topologyMenu->addSeparator();

    emptyAct = new Action( tr( "Show also unused hardware in topology" ), this );
    emptyAct->setCheckable( true );
    emptyAct->setChecked( true );
    emptyAct->setStatusTip( tr( "Show also planes of unused hardware in the topology display" ) );
    connect( emptyAct, SIGNAL( triggered() ), systemTabWidget, SLOT( toggleUnused() ) );
    emptyAct->setWhatsThis( "If this menu item is not checked, unused topology planes, i.e., planes whose grid elements don't have any processes/threads assigned to, are hidden. Unused plane elements, if not hidden, are colored gray. See also help on the topologies themselves." );
    topologyMenu->addAction( emptyAct );

    antialiasingAct = new Action( tr( "Topology antialiasing" ), this );
    antialiasingAct->setCheckable( true );
    antialiasingAct->setChecked( false );
    antialiasingAct->setStatusTip( tr( "Activate/deactivate antialiasing for rendering in topology" ) );
    antialiasingAct->setWhatsThis( "If this menu item is checked, antialiasing is used when painting lines in the topologies. See also help on the topologies themselves." );
    connect( antialiasingAct, SIGNAL( triggered() ), systemTabWidget, SLOT( toggleAntialiasing() ) );

    topologyMenu->addAction( antialiasingAct );

    //help menu

    QMenu* helpMenu = menuBar()->addMenu( tr( "&Help" ) );
    helpMenu->setStatusTip( tr( "Ready" ) );

    Action* introAct = new Action( tr( "Getting started" ), this );
    introAct->setStatusTip( tr( "Displayes a short introduction to Cube." ) );
    connect( introAct, SIGNAL( triggered() ), this, SLOT( introduction() ) );
    helpMenu->setWhatsThis( "Opens a dialog with some basic informations on the usage of Cube." );
    helpMenu->addAction( introAct );

    Action* keysAct = new Action( tr( "Mouse and keyboard control" ), this );
    keysAct->setStatusTip( tr( "Shows all supported mouse and keyboard controls" ) );
    connect( keysAct, SIGNAL( triggered() ), this, SLOT( keyHelp() ) );
    keysAct->setWhatsThis( "List all control possibilities for keyboard and mouse." );
    helpMenu->addAction( keysAct );

    QAction* whatsThisAct = QWhatsThis::createAction();
    whatsThisAct->setStatusTip( tr( "Change into help mode for display components" ) );
    whatsThisAct->setWhatsThis( tr( "Here you can get more specific information on parts of the Cube GUI. If you activate this menu item, you switch to the \"What's this?\" mode. If you now click on a widget an appropriate help text is shown. The mode is left when help is given or when you press Esc.\n\nAnother way to ask the question is to move the focus to the relevant widget and press Shift+F1." ) );
    helpMenu->addAction( whatsThisAct );

    Action* aboutQtAct = new Action( tr( "About Qt" ), this );
    aboutQtAct->setStatusTip( tr( "Shows Qt's About box" ) );
    aboutQtAct->setWhatsThis( tr( "Opens a dialog with Qt release information." ) );
    connect( aboutQtAct, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );
    helpMenu->addAction( aboutQtAct );

    Action* aboutAct = new Action( tr( "About CUBE" ), this );
    aboutAct->setStatusTip( tr( "Shows CUBE's About box" ) );
    aboutAct->setWhatsThis( tr( "Opens a dialog with some release information." ) );
    connect( aboutAct, SIGNAL( triggered() ), this, SLOT( about() ) );
    helpMenu->addAction( aboutAct );


    helpMenu->addSeparator();
    Action* performanceInfoAct = new Action( tr( "Selected metrics description" ), this );
    performanceInfoAct->setStatusTip( tr( "Shows the online description of the selected metrics." ) );
    connect( performanceInfoAct, SIGNAL( triggered() ), this, SLOT( onMetricsInfo() ) );
    performanceInfoAct->setWhatsThis( "Shows some (usually more extensive) online description for the selected metrics. For metrics it might point to an online documentation explaining their semantics." );
    helpMenu->addAction( performanceInfoAct );

    Action* regionInfoAct = new Action( tr( "Selected regions description" ), this );
    regionInfoAct->setStatusTip( tr( "Shows the online description of the selected regions in a  program." ) );
    connect( regionInfoAct, SIGNAL( triggered() ), this, SLOT( onRegionsInfo() ) );
    regionInfoAct->setWhatsThis( "Shows some (usually more extensive) online description for the clicked region in program. For regions representing library functions it might point to the corresponding library documentation." );
    helpMenu->addAction( regionInfoAct );
}


//since the topology tool bar can be hidden from the tool bar itself, without using the menu for that,
//and I did not find any signal signalling this event,
//if the menu is to be shown, I update the status of the topology tool bar menu
//
void
MainWidget::updateToolbarMenu()
{
    if ( topologyToolBar->isHidden() )
    {
        hideToolbarAct->setChecked( true );
    }
    else if ( topologyToolBar->toolButtonStyle() == Qt::ToolButtonTextOnly )
    {
        textAct->setChecked( true );
    }
    else
    {
        iconAct->setChecked( true );
    }
}


/*************************************************************/
/************ widget title, status bar ***********************/
/*************************************************************/

//update the title of the application to show infos to the loaded file and external file
//
void
MainWidget::updateWidgetTitle()
{
    QChar   pathSeparator = QDir::separator();
    //CUBE_NAME is defined in constants.h
    QString widgetTitle( CUBE_NAME );

    //append the name of the currently opened file
    if ( cube != NULL )
    {
        QString openFileName = openedFiles[ 0 ];

        if ( openFileName.at( openFileName.length() - 1 ) == pathSeparator )
        {
            openFileName.chop( 1 );
        }

        int     lastSeparator = openFileName.lastIndexOf( pathSeparator );
        QString fileName      = openFileName.mid( lastSeparator + 1 );

        QString lastDirectory = openFileName.left( lastSeparator );
        lastSeparator = lastDirectory.lastIndexOf( pathSeparator );
        if ( lastSeparator == -1 )
        {
            lastDirectory = "";
        }
        else
        {
            lastDirectory = lastDirectory.mid( lastSeparator + 1 );
        }

        widgetTitle.append( ": " );
        widgetTitle.append( lastDirectory );
        widgetTitle.append( pathSeparator );
        widgetTitle.append( fileName );
    }

    //append the name of the current external file
    QString externalName = lastExternalFileName;
    if ( externalName != "" )
    {
        if ( externalName.at( externalName.length() - 1 ) == pathSeparator )
        {
            externalName.chop( 1 );
        }

        int     lastSeparator = externalName.lastIndexOf( pathSeparator );
        QString fileName      = externalName.mid( lastSeparator + 1 );

        QString lastDirectory = externalName.left( lastSeparator );
        lastSeparator = lastDirectory.lastIndexOf( pathSeparator );
        if ( lastSeparator == -1 )
        {
            lastDirectory = "";
        }
        else
        {
            lastDirectory = lastDirectory.mid( lastSeparator + 1 );
        }

        widgetTitle.append( " external: " );
        widgetTitle.append( lastDirectory );
        widgetTitle.append( pathSeparator );
        widgetTitle.append( fileName );
    }

    setWindowTitle( widgetTitle );
}

//sets the status bar text
//
void
MainWidget::setMessage( const QString message )
{
    statusBar()->showMessage( tr( message.toStdString().c_str() ) );
}


/*************************************************************/
/******************** file handling **************************/
/*************************************************************/


//remember the names of the last 5 files that were opened;
//they can be re-opened via the display menu
//
void
MainWidget::rememberFileName( QString fileName )
{
    QChar pathSeparator = QDir::separator();

    //if the file name is widthout path then extend it with the current path
    if ( fileName.lastIndexOf( pathSeparator ) == -1 )
    {
        fileName = QDir::currentPath() + pathSeparator + fileName;
    }

    //clear the actions from the display menu to re-open files
    for ( int i = 0; i < ( int )lastFileAct.size(); i++ )
    {
        fileMenu->removeAction( lastFileAct.at( i ) );
    }
    lastFileAct.clear();

    //insert the new fileName at the beginning of the openedFiles vector
    //and remove eventual duplicates
    openedFiles.push_back( QString( "" ) );
    for ( int i = openedFiles.size() - 1; i > 0; i-- )
    {
        openedFiles[ i ] = openedFiles[ i - 1 ];
    }
    openedFiles[ 0 ] = fileName;
    for ( int i = 1; i < ( int )openedFiles.size(); i++ )
    {
        if ( openedFiles[ i ].compare( openedFiles[ 0 ] ) == 0 )
        {
            for ( int j = i; j < ( int )openedFiles.size() - 1; j++ )
            {
                openedFiles[ j ] = openedFiles[ j + 1 ];
            }
            openedFiles.pop_back();
            break;
        }
    }

    //we remember at most 5 file names
    if ( openedFiles.size() > 5 )
    {
        openedFiles.pop_back();
    }

    //create an action for opening the last file (fileName)
    if ( openedFiles.size() > 0 )
    {
        QString name = openedFiles[ 0 ];
        if ( name.at( name.length() - 1 ) == pathSeparator )
        {
            name.chop( 1 );
        }
        name = name.mid( name.lastIndexOf( pathSeparator ) + 1 );
        QAction* act = fileMenu->addAction( name );
        connect( act, SIGNAL( triggered() ), this, SLOT( openLast0() ) );
        act->setStatusTip( openedFiles[ 0 ] );
        lastFileAct.push_back( act );
    }
    //create an action for opening the 2nd last file
    if ( openedFiles.size() > 1 )
    {
        QString name = openedFiles[ 1 ];
        if ( name.at( name.length() - 1 ) == pathSeparator )
        {
            name.chop( 1 );
        }
        name = name.mid( name.lastIndexOf( pathSeparator ) + 1 );
        QAction* act = fileMenu->addAction( name );
        connect( act, SIGNAL( triggered() ), this, SLOT( openLast1() ) );
        act->setStatusTip( openedFiles[ 1 ] );
        lastFileAct.push_back( act );
    }
    //create an action for opening the 3rd last file
    if ( openedFiles.size() > 2 )
    {
        QString name = openedFiles[ 2 ];
        if ( name.at( name.length() - 1 ) == pathSeparator )
        {
            name.chop( 1 );
        }
        name = name.mid( name.lastIndexOf( pathSeparator ) + 1 );
        QAction* act = fileMenu->addAction( name );
        connect( act, SIGNAL( triggered() ), this, SLOT( openLast2() ) );
        act->setStatusTip( openedFiles[ 2 ] );
        lastFileAct.push_back( act );
    }
    //create an action for opening the 4th last file
    if ( openedFiles.size() > 3 )
    {
        QString name = openedFiles[ 3 ];
        if ( name.at( name.length() - 1 ) == pathSeparator )
        {
            name.chop( 1 );
        }
        name = name.mid( name.lastIndexOf( pathSeparator ) + 1 );
        QAction* act = fileMenu->addAction( name );
        connect( act, SIGNAL( triggered() ), this, SLOT( openLast3() ) );
        act->setStatusTip( openedFiles[ 3 ] );
        lastFileAct.push_back( act );
    }
    //create an action for opening the 5th last file
    if ( openedFiles.size() > 4 )
    {
        QString name = openedFiles[ 4 ];
        if ( name.at( name.length() - 1 ) == pathSeparator )
        {
            name.chop( 1 );
        }
        name = name.mid( name.lastIndexOf( pathSeparator ) + 1 );
        QAction* act = fileMenu->addAction( name );
        connect( act, SIGNAL( triggered() ), this, SLOT( openLast4() ) );
        act->setStatusTip( openedFiles[ 4 ] );
        lastFileAct.push_back( act );
    }

    for ( unsigned i = 0; i < lastFileAct.size(); i++ )
    {
        lastFileAct[ i ]->setWhatsThis( "At the bottom of the File menu the last 5 opened files are offered for re-opening, the top-most being the most recently opened one. A full path to the file is visible in the status bar if you move the mouse above one of the recent file items in the menu." );
    }
}
//end of rememberFileName(...)


//re-open the last file, the second last file, etc.
void
MainWidget::openLast0()
{
    readFile( openedFiles[ 0 ] );
}
void
MainWidget::openLast1()
{
    readFile( openedFiles[ 1 ] );
}
void
MainWidget::openLast2()
{
    readFile( openedFiles[ 2 ] );
}
void
MainWidget::openLast3()
{
    readFile( openedFiles[ 3 ] );
}
void
MainWidget::openLast4()
{
    readFile( openedFiles[ 4 ] );
}


//input a file name to open and open it via readFile(...)
//
void
MainWidget::openFile()
{
    QString lastName     = ( openedFiles.size() == 0 ? "." : openedFiles[ 0 ] );
    QString openFileName = QFileDialog::getOpenFileName( this,
                                                         tr( "Choose a file to open" ),
                                                         lastName,
                                                    #ifndef CUBE_COMPRESSED
                                                         tr( "Cube files (*.cube);;All files (*.*);;All files (*)" ) );
#else
                                                         tr( "Cube files (*.cube.gz *.cube);;All files (*.*);;All files (*)" ) );
#endif

    if ( openFileName.length() == 0 )
    {
        statusBar()->showMessage( tr( "Ready" ) );
        return;
    }

    readFile( openFileName );
}


//load the contents of a cube file into a cube object
//and display the contents
//
bool
MainWidget::readFile( const QString fn )
{
    QDir    fileDir( fn );
    QString fileName = fileDir.absolutePath();

    if ( services::check_file( fileName.toStdString().c_str() ) != 0 )
    {
        return false;
    }

#ifndef CUBE_COMPRESSED
    std::ifstream in( fileName.toStdString().c_str() );
#else
    gzifstream    in( fileName.toStdString().c_str(), ios_base::in | ios_base::binary );
#endif

    if ( !in )
    {
        std::cerr << "Error while opening " << fileName.toStdString() << std::endl;
        setCursor( Qt::ArrowCursor );
        statusBar()->showMessage( tr( "Ready" ) );
        return false;
    }

    statusBar()->showMessage( tr( "Parsing..." ) );
    setCursor( Qt::BusyCursor );

    //remember the file name for opening recent files
    rememberFileName( fileName );

    //check if metric data should be loaded dynamically
    bool    dynamicParsing = false;
#ifdef WITH_DYNLOAD
    QString infoFileName;
    {
        QFile file( fileName );
        //use dynamic loading only if metric is on the left-hand-side;
        //during dynamic loading is active, the Display->Dimension order
        //menu gets disabled
        if ( file.size() > dynloadThreshold && ( ( TypedWidget* )( splitterWidget->widget( 0 ) ) )->getType() == METRICWIDGET )
        {
            if ( QMessageBox::question( this, "Dynamic loading", "Do you want to use dynamic metric loading on demand?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes ) == QMessageBox::Yes )
            {
                infoFileName = fileName;
                infoFileName.append( ".info" );
                infoFileName = QFileDialog::getSaveFileName( this, "Choose info file name", infoFileName, "", 0, QFileDialog::DontConfirmOverwrite );
                if ( infoFileName != "" )
                {
                    dynamicParsing = true;
                }
            }
        }
    }
#endif

    statusBar()->showMessage( tr( "Closing previous data..." ) );
    closeFile();

    //read in the data into a new cube object
    assert( cube == NULL );
    cube = new cube::Cube();
    cubeparser::Driver* driver = new cubeparser::Driver();

    try {
        if ( dynamicParsing )
        {
#ifdef WITH_DYNLOAD
            driver->parse_stream_dynamic( in, *cube, infoFileName.toStdString() );
            splitterOrderAct->setEnabled( false );
#endif
        }
        else
        {
            driver->parse_stream( in, *cube );
        }
    }
    catch ( cube::RuntimeError e )
    {
        MessageWidget* wwidget = new MessageWidget( Error, QString( "Parse error. \n" ).append( QString::fromStdString( e.get_msg() ) ) );
        wwidget->show();
        //     QMessageBox::critical(this, "Parse error", QString::fromStdString(e.get_msg()));
        setCursor( Qt::ArrowCursor );
        statusBar()->showMessage( tr( "Ready" ) );
        closeFile();
        return false;
    }

    if ( driver->parseContext->cubeVersion != 3 || !dynamicParsing )
    {
        delete driver;
        driver = NULL;
    }

    //initialize statistics info
    assert( statistics == 0 );
    statistics = new Statistics( fileName.toStdString(), precisionWidget );
#if defined( WITH_VAMPIR ) || ( WITH_PARAVER )
    initializeTraceBrowserMenu( fileName );
#endif

    //initialize the displays in their display order

    int metricIndex, callIndex, systemIndex;
    getWidgetIndices( metricIndex, callIndex, systemIndex );

    statusBar()->showMessage( tr( "Initializing tabs 1/3..." ) );
    if ( metricIndex == 0 )
    {
        metricTabWidget->initialize( cube, fileName, driver, statistics );
    }
    else if ( callIndex == 0 )
    {
        callTabWidget->initialize( cube, "", NULL, statistics );
    }
    else
    {
        systemTabWidget->initialize( cube );
    }

    statusBar()->showMessage( tr( "Initializing tabs 2/3..." ) );
    if ( metricIndex == 1 )
    {
        metricTabWidget->initialize( cube, fileName, driver, statistics );
    }
    else if ( callIndex == 1 )
    {
        callTabWidget->initialize( cube, "", NULL, statistics );
    }
    else
    {
        systemTabWidget->initialize( cube );
    }

    statusBar()->showMessage( tr( "Initializing tabs 3/3..." ) );
    if ( metricIndex == 2 )
    {
        metricTabWidget->initialize( cube, fileName, driver, statistics );
    }
    else if ( callIndex == 2 )
    {
        callTabWidget->initialize( cube, "", NULL, statistics );
    }
    else
    {
        systemTabWidget->initialize( cube );
    }

    //call computeValues on the left-most tab, this will
    //cause to compute the values of the further tabs and
    //to actualize the displays
    statusBar()->showMessage( tr( "Computing values..." ) );
    if ( metricIndex == 0 )
    {
        metricTabWidget->computeValues();
    }
    else if ( callIndex == 0 )
    {
        callTabWidget->computeValues();
    }
    else
    {
        systemTabWidget->computeValues();
    }

    updateWidgetTitle();
    emit fileOpened();

    setCursor( Qt::ArrowCursor );
    statusBar()->showMessage( tr( "Ready" ) );

    //add local documentation paths from CUBE_DOCPATH environment variable
    //as additional "mirrors"
    char* ptr = getenv( "CUBE_DOCPATH" );
    if ( ptr )
    {
        QString                     docpath( ptr );
        QStringList                 paths = docpath.split( ':', QString::SkipEmptyParts );

        QStringList::const_iterator it = paths.begin();
        while ( it != paths.end() )
        {
            QString mirror( "file://" + *it );
            if ( !mirror.endsWith( '/' ) )
            {
                mirror.append( '/' );
            }

            cube->def_mirror( mirror.toStdString() );

            ++it;
        }
    }
    return true;
}
//end of readFile(...)


//remove loaded data
//
void
MainWidget::closeFile()
{
    metricTabWidget->cleanUp();
    callTabWidget->cleanUp();
    systemTabWidget->cleanUp();

    delete cube;
    cube = NULL;
    updateWidgetTitle();

    metricValueWidget->clear();
    callValueWidget->clear();
    systemValueWidget->clear();


#if defined( WITH_VAMPIR ) || ( WITH_PARAVER )
    disableTraceBrowserMenu();
#endif
    delete statistics;
    statistics = 0;

    emit fileClosed();
    statusBar()->showMessage( tr( "Ready" ) );

    //during dynamic loading, the metric tab must be kept on the
    //left-hand-side; after closing, the menu point for changing the
    //dimension order may be enabled again
    splitterOrderAct->setEnabled( true );
}


//input a file name to open as external file for external percentage
//and open it via readExternalFile(...)
//
bool
MainWidget::openExternalFile()
{
    QString fileName = QFileDialog::getOpenFileName( this,
                                                     tr( "Choose a file for external percentage" ),
                                                     lastExternalFileName,
                                                #ifndef CUBE_COMPRESSED
                                                     tr( "Cube files (*.cube);;All files (*.*);;All files (*)" ) );
#else
                                                     tr( "Cube files (*.cube.gz *.cube);;All files (*.*);;All files (*)" ) );
#endif

    if ( fileName.length() == 0 )
    {
        statusBar()->showMessage( tr( "Ready" ) );
        return false;
    }
    bool result = readExternalFile( fileName );
    if ( result )
    {
        updateWidgetTitle();
        emit externalFileOpened();
    }

    //update the item values
    int metricIndex, callIndex, systemIndex;
    getWidgetIndices( metricIndex, callIndex, systemIndex );
    if ( metricIndex == 0 )
    {
        metricTabWidget->computeValues();
    }
    else if ( callIndex == 0 )
    {
        callTabWidget->computeValues();
    }
    else
    {
        systemTabWidget->computeValues();
    }

    return result;
}


//read the contents of a cube file into a temporary cube object for
//the external percentage modus and compute the relevant values for the display
//
bool
MainWidget::readExternalFile( const QString fileName )
{
    statusBar()->showMessage( tr( "Parsing..." ) );
    setCursor( Qt::BusyCursor );

#ifndef CUBE_COMPRESSED
    std::ifstream in( fileName.toStdString().c_str() );
#else
    gzifstream    in( fileName.toStdString().c_str(), ios_base::in | ios_base::binary );
#endif

    if ( !in )
    {
        std::cerr << "Error while opening " << fileName.toStdString() << std::endl;
        setCursor( Qt::ArrowCursor );
        statusBar()->showMessage( tr( "Ready" ) );
        return false;
    }

    lastExternalFileName = fileName;

    //read in external data into the external cube object
    assert( cubeExternal == NULL );
    cubeExternal = new cube::Cube();
    in >> *cubeExternal;

    //compute relevant values and store them in the metric tab
    assert( metricTabWidget != NULL );
    metricTabWidget->computeExternalValues( cubeExternal );

    //the relevant values got stores in the metric tab widget
    //and we can delete the external cube object
    delete cubeExternal;
    cubeExternal = NULL;

    setCursor( Qt::ArrowCursor );
    statusBar()->showMessage( tr( "Ready" ) );

    return true;
}

//remove loaded external data for external percentage
//
void
MainWidget::closeExternalFile()
{
    //check if external modus is set in one of the tabs;
    //if yes, activate the the first non-external modus
    ValueModus metricModus = getValueModus( metricCombo->itemText( metricCombo->currentIndex() ) );
    if ( metricModus == EXTERNAL )
    {
        for ( int i = 0; i < metricCombo->count(); i++ )
        {
            if ( getValueModus( metricCombo->itemText( i ) ) != EXTERNAL )
            {
                metricCombo->setCurrentIndex( i );
                break;
            }
            assert( i + 1 < metricCombo->count() );
        }
    }

    ValueModus callModus = getValueModus( callCombo->itemText( callCombo->currentIndex() ) );
    if ( callModus == EXTERNAL )
    {
        for ( int i = 0; i < callCombo->count(); i++ )
        {
            if ( getValueModus( callCombo->itemText( i ) ) != EXTERNAL )
            {
                callCombo->setCurrentIndex( i );
                break;
            }
            assert( i + 1 < callCombo->count() );
        }
    }

    ValueModus systemModus = getValueModus( systemCombo->itemText( systemCombo->currentIndex() ) );
    if ( systemModus == EXTERNAL )
    {
        for ( int i = 0; i < systemCombo->count(); i++ )
        {
            if ( getValueModus( systemCombo->itemText( i ) ) != EXTERNAL )
            {
                systemCombo->setCurrentIndex( i );
                break;
            }
            assert( i + 1 < systemCombo->count() );
        }
    }

    //clear external values of the metric tab widget
    metricTabWidget->clearExternalValues();

    lastExternalFileName = "";
    updateWidgetTitle();
    emit externalFileClosed();
}



//returns if currently a file is loaded
bool
MainWidget::fileLoaded()
{
    return cube != NULL;
}


/*********************************************************/
/****************** miscellaneous ************************/
/*********************************************************/


//closes the application after eventually saving settings
//
void
MainWidget::closeApplication()
{
    //ask if settings should be saved
    if ( QMessageBox::question( this,
                                tr( "Settings" ),
                                tr( "Do you want to save the current settings?" ),
                                QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
    {
        saveSettings();
    }
    closeFile();

    close();
}

//clean-up before close
//
void
MainWidget::closeEvent( QCloseEvent* event )
{
    Q_UNUSED( event );

    closeFile();
}

//set the order of metric/call/system splitter elements
//
void
MainWidget::setDimensionOrder()
{
    SplitterDialog splitterDialog( this, splitterWidget );
    if ( splitterDialog.exec() )
    {
        updateCombos();
    }
    statusBar()->showMessage( tr( "Ready" ) );
}

//determine the current order of the metric/call/system widgets
//
void
MainWidget::getWidgetIndices( int& metricIndex, int& callIndex, int& systemIndex )
{
    if ( ( ( TypedWidget* )( splitterWidget->widget( 0 ) ) )->getType() == METRICWIDGET )
    {
        metricIndex = 0;
    }
    else if ( ( ( TypedWidget* )( splitterWidget->widget( 0 ) ) )->getType() == CALLWIDGET )
    {
        callIndex = 0;
    }
    else
    {
        systemIndex = 0;
    }

    if ( ( ( TypedWidget* )( splitterWidget->widget( 1 ) ) )->getType() == METRICWIDGET )
    {
        metricIndex = 1;
    }
    else if ( ( ( TypedWidget* )( splitterWidget->widget( 1 ) ) )->getType() == CALLWIDGET )
    {
        callIndex = 1;
    }
    else
    {
        systemIndex = 1;
    }

    if ( ( ( TypedWidget* )( splitterWidget->widget( 2 ) ) )->getType() == METRICWIDGET )
    {
        metricIndex = 2;
    }
    else if ( ( ( TypedWidget* )( splitterWidget->widget( 2 ) ) )->getType() == CALLWIDGET )
    {
        callIndex = 2;
    }
    else
    {
        systemIndex = 2;
    }
}


//if the order of the metric/call/system widgets changes,
//the value modi must be adapted;
//that is what this method does
//
void
MainWidget::updateCombos()
{
    //store the current value modi;
    //after update, we try to re-set these modi if available
    QString metricString = metricCombo->itemText( metricCombo->currentIndex() );
    QString callString   = callCombo->itemText( callCombo->currentIndex() );
    QString systemString = systemCombo->itemText( systemCombo->currentIndex() );

    int     metricIndex, callIndex, systemIndex;
    getWidgetIndices( metricIndex, callIndex, systemIndex );

    // disable/enable the box plot tab, if system widget is 1st
    systemTabWidget->enableSystemBoxPlot( systemIndex == 0 );

    //temporarily disconnect the combo signals
    //not to be emitted unnecessarily often
    disconnect( metricCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( onSetMetricValueModus( int ) ) );
    disconnect( callCombo,   SIGNAL( currentIndexChanged( int ) ), this, SLOT( onSetCallValueModus( int ) ) );
    disconnect( systemCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( onSetSystemValueModus( int ) ) );

    //empty the combos

    metricCombo->clear();
    callCombo->clear();
    systemCombo->clear();

    //add the available items into the combos

    metricCombo->addItem( tr( ABSOLUTE_NAME ) );
    callCombo->addItem( tr( ABSOLUTE_NAME ) );
    systemCombo->addItem( tr( ABSOLUTE_NAME ) );

    metricCombo->addItem( tr( OWNROOT_NAME ) );
    if ( metricIndex < callIndex )
    {
        callCombo->addItem( tr( OWNROOT_NAME ) );
    }
    if ( metricIndex < systemIndex )
    {
        systemCombo->addItem( tr( OWNROOT_NAME ) );
    }

    if ( metricIndex == 0 && callIndex == 1 && systemIndex == 2 )
    {
        callCombo->addItem( tr( METRICROOT_NAME ) );
        callCombo->addItem( tr( METRICSELECTED_NAME ) );
        systemCombo->addItem( tr( METRICROOT_NAME ) );
        systemCombo->addItem( tr( METRICSELECTED_NAME ) );
        systemCombo->addItem( tr( CALLROOT_NAME ) );
        systemCombo->addItem( tr( CALLSELECTED_NAME ) );
    }
    else if ( metricIndex == 0 && callIndex == 2 && systemIndex == 1 )
    {
        systemCombo->addItem( tr( METRICROOT_NAME ) );
        systemCombo->addItem( tr( METRICSELECTED_NAME ) );
        callCombo->addItem( tr( METRICROOT_NAME ) );
        callCombo->addItem( tr( METRICSELECTED_NAME ) );
        callCombo->addItem( tr( SYSTEMROOT_NAME ) );
        callCombo->addItem( tr( SYSTEMSELECTED_NAME ) );
    }
    else if ( metricIndex == 1 && callIndex == 0 && systemIndex == 2 )
    {
        systemCombo->addItem( tr( METRICROOT_NAME ) );
        systemCombo->addItem( tr( METRICSELECTED_NAME ) );
    }
    else if ( metricIndex == 1 && callIndex == 2 && systemIndex == 0 )
    {
        callCombo->addItem( tr( METRICROOT_NAME ) );
        callCombo->addItem( tr( METRICSELECTED_NAME ) );
    }

    if ( metricIndex < systemIndex )
    {
        systemCombo->addItem( tr( PEER_NAME ) );
        systemCombo->addItem( tr( PEERDIST_NAME ) );
    }
    if ( metricIndex == 0 )
    {
        metricCombo->addItem( tr( EXTERNAL_NAME ) );
        callCombo->addItem( tr( EXTERNAL_NAME ) );
        systemCombo->addItem( tr( EXTERNAL_NAME ) );
    }


    //re-select the old modi if available,
    //otherwise select the absolute value modus
    int i;
    for ( i = 0; i < metricCombo->count(); i++ )
    {
        if ( metricCombo->itemText( i ).compare( metricString ) == 0 )
        {
            metricCombo->setCurrentIndex( i );
            onSetMetricValueModus( i );
            break;
        }
    }
    if ( i == metricCombo->count() )
    {
        metricCombo->setCurrentIndex( 0 );
        onSetMetricValueModus( 0 );
    }

    for ( i = 0; i < callCombo->count(); i++ )
    {
        if ( callCombo->itemText( i ).compare( callString ) == 0 )
        {
            callCombo->setCurrentIndex( i );
            onSetCallValueModus( i );
            break;
        }
    }
    if ( i == callCombo->count() )
    {
        callCombo->setCurrentIndex( 0 );
        onSetCallValueModus( 0 );
    }

    for ( i = 0; i < systemCombo->count(); i++ )
    {
        if ( systemCombo->itemText( i ).compare( systemString ) == 0 )
        {
            systemCombo->setCurrentIndex( i );
            onSetSystemValueModus( i );
            break;
        }
    }
    if ( i == systemCombo->count() )
    {
        systemCombo->setCurrentIndex( 0 );
        onSetSystemValueModus( 0 );
    }

    //re-compute the values starting from the left-most tab widget
    if ( metricIndex == 0 )
    {
        metricTabWidget->computeValues();
    }
    else if ( callIndex == 0 )
    {
        callTabWidget->computeValues();
    }
    else
    {
        systemTabWidget->computeValues();
    }


    //re-connect the combo signals
    connect( metricCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( onSetMetricValueModus( int ) ) );
    connect( callCombo,   SIGNAL( currentIndexChanged( int ) ), this, SLOT( onSetCallValueModus( int ) ) );
    connect( systemCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( onSetSystemValueModus( int ) ) );
}
//end of updateCombos()


//causes to open a color dialog to allow the user to change color-specific settings
//
void
MainWidget::setColors()
{
    //set the colors by the user; this method can emit the apply()
    //signal of colorWidget, that is connected to the onApply() slot of
    //this object
    colorWidget->setColors();
    statusBar()->showMessage( tr( "Ready" ) );
}

//this slot repaints everything
//
void
MainWidget::onApply()
{
    //update the tabs
    metricTabWidget->displayItems();
    callTabWidget->displayItems();
    systemTabWidget->displayItems();

    //update the value widgets
    metricTabWidget->updateValueWidget();
    callTabWidget->updateValueWidget();
    systemTabWidget->updateValueWidget();
}


//causes to open a precision dialog to allow the user to change precision-specific settings
//
void
MainWidget::setPrecision()
{
    precisionWidget->exec();
    statusBar()->showMessage( tr( "Ready" ) );
}


//causes to open a font dialog to allow the user to change
//font and spacing for tree widgets
//
void
MainWidget::setFont()
{
    FontWidget fontWidget( this, treeFont, spacing );
    connect( &fontWidget, SIGNAL( apply( FontWidget* ) ), this, SLOT( applyFont( FontWidget* ) ) );
    fontWidget.exec();
    disconnect( &fontWidget, SIGNAL( apply( FontWidget* ) ), this, SLOT( applyFont( FontWidget* ) ) );
}


//slot to apply the settings of the font widget
//
void
MainWidget::applyFont( FontWidget* fontWidget )
{
    treeFont = fontWidget->getFont();
    spacing  = fontWidget->getSpacing();
    metricTabWidget->setTreeDisplay( treeFont, spacing );
    callTabWidget->setTreeDisplay( treeFont, spacing );
    systemTabWidget->setTreeDisplay( treeFont, spacing );
    onApply();
}

//selections in trees should be marked by a special background
void
MainWidget::setSelectionBackground()
{
    metricTabWidget->setSelectionSyntax( BACKGROUND_SELECTION );
    callTabWidget->setSelectionSyntax( BACKGROUND_SELECTION );
    systemTabWidget->setSelectionSyntax( BACKGROUND_SELECTION );
}

//selections in trees should be marked by framing
void
MainWidget::setSelectionFrame()
{
    metricTabWidget->setSelectionSyntax( FRAME_SELECTION );
    callTabWidget->setSelectionSyntax( FRAME_SELECTION );
    systemTabWidget->setSelectionSyntax( FRAME_SELECTION );
}



//shows the Onine info for selected metrics
void
MainWidget::onMetricsInfo()
{
    if ( cube == NULL )
    {
        MessageWidget* mess = new MessageWidget( Warning, "No CUBE report is loaded." );
        mess->show();
        return;
    }

    TreeWidget* t_wid =  ( TreeWidget* )( ( ( QScrollArea* )( metricTabWidget->currentWidget() ) )->widget() );

    if ( t_wid == NULL )
    {
        MessageWidget* mess = new MessageWidget( Warning, "No Metrics are loaded." );
        mess->show();
        return;
    }

    std::vector<TreeWidgetItem*> sel_metrics =     t_wid->selectedItems();

    if ( ( unsigned int )sel_metrics.size() == 0 )
    {
        MessageWidget* mess = new MessageWidget( Warning, "There is no selected metrics" );
        mess->show();
        return;
    }
    ;
    for ( std::vector<TreeWidgetItem*>::iterator iter = sel_metrics.begin(); iter != sel_metrics.end(); iter++ )
    {
        t_wid->openUrlOfItem( ( *iter ) );
    }
}



//shows the Onine info for selected metrics
void
MainWidget::onRegionsInfo()
{
    if ( cube == NULL )
    {
        MessageWidget* mess = new MessageWidget( Warning, "No CUBE report is loaded." );
        mess->show();
        return;
    }

    TreeWidget* t_wid =  ( TreeWidget* )( ( ( QScrollArea* )( callTabWidget->currentWidget() ) )->widget() );

    if ( t_wid == NULL )
    {
        MessageWidget* mess = new MessageWidget( Warning, "No Regiond are loaded." );
        mess->show();
        return;
    }

    std::vector<TreeWidgetItem*> sel_regions =     t_wid->selectedItems();

    if ( ( unsigned int )sel_regions.size() == 0 )
    {
        MessageWidget* mess = new MessageWidget( Warning, "There is no selected regions" );
        mess->show();
        return;
    }
    ;
    for ( std::vector<TreeWidgetItem*>::iterator iter = sel_regions.begin(); iter != sel_regions.end(); iter++ )
    {
        t_wid->openUrlOfItem( ( *iter ) );
    }
}




void
MainWidget::introduction()
{
    QString str = "Cube (CUBE Uniform Behavioral Encoding) is a presentation\ncomponent suitable for displaying a wide variety of performance\ndata for parallel programs including MPI and OpenMP applications.\nCube allows interactive exploration of the performance data in a scalable fashion.\nScalability is achieved in two ways: hierarchical decomposition of\nindividual dimensions and aggregation across different dimensions. All\nmetrics are uniformly accommodated in the same display and thus\nprovide the ability to easily compare the effects of different kinds\nof program behavior.\n\nCube has been designed around a high-level data model of program\nbehavior called the Cube performance space. The Cube\nperformance space consists of three dimensions: a metric dimension, a\nprogram dimension, and a system dimension.  The metric dimension\ncontains a set of metrics, such as communication time or\ncache misses.  The program dimension contains the program's\ncall tree, which includes all the call paths onto which metric values\ncan be mapped. The system dimension contains the items\nexecuting in parallel, which can be processes or threads depending on\nthe parallel programming model.  Each point (m, c, s) of the space\ncan be mapped onto a number representing the actual measurement for\nmetric m while the control flow of process/thread s was executing\ncall path c.  This mapping is called the severity of the\nperformance space.\n\nEach dimension of the performance space is organized in a\nhierarchy. First, the metric dimension is organized in an\ninclusion hierarchy where a metric at a lower level is a subset of its\nparent. For example, communication time is below execution time.\nSecond, the program dimension is organized in a call-tree hierarchy.\nHowever, sometimes it can be advantageous to abstract away from the\nhierarchy of the call tree, for example if one is interested in the\nseverities of certain methods, independently of the position of their\ninvocations.  For this purpose Cube supports also flat call\nprofiles, that are represented as a flat sequence of all \nmethods.  Finally, the system dimension is organized in a multi-level\nhierarchy consisting of the levels: machine, SMP node, process,\nand thread.\n\nCube serves with a library to read and write instances of the\npreviously described data model in the form of an XML file. The\nfile representation is divided into a metadata part and a\ndata part. The metadata part describes the structure of the three\ndimensions plus the definitions of various program and system\nresources. The data part contains the actual severity numbers to be\nmapped onto the different elements of the performance space.\n\nThe display component can load such a file and display the\ndifferent dimensions of the performance space using three coupled tree\nbrowsers. The browsers are connected such a way\nthat you can view one dimension with respect to another dimension. The\nconnection is based on selections: in each tree you can select\none or more nodes.\n\nIf the Cube file contains topological information, the\ndistribution of the performance metric across the topology can be\nexamined using the topology view.  Furthermore, the display is\naugmented with a source-code display that shows the position\nof a call site in the source code.\n\nAs performance tuning of parallel applications usually involves\nmultiple experiments to compare the effects of certain optimization\nstrategies, Cube includes a feature designed to simplify\ncross-experiment analysis. The Cube algebra is an extension of\nthe framework for multi-execution performance\ntuning by Karavanic and Miller and offers a set\nof operators that can be used to compare, integrate, and summarize\nmultiple Cube data sets. The algebra allows the combination of\nmultiple Cube data sets into a single one that can be displayed like\nthe original ones.\n\nIf you need further help, press Ctrl+F1 and evtl. select the component\nfor which you need help.";


    QLabel*      label  = new QLabel( str );
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( label );
    QWidget*     widget = new QWidget();
    widget->setLayout( layout );

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget( widget );

    QDialog      dialog;
    QVBoxLayout* layout2 = new QVBoxLayout();
    layout2->addWidget( scrollArea );
    dialog.setLayout( layout2 );

    dialog.exec();
}

//shows the about message
//
void
MainWidget::about()
{
    QDialog dialog( this );
    dialog.setWindowTitle( "About " CUBE_NAME );

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing( 25 );

    QString str1 = "<b>This is ";
    str1.append( CUBE_NAME );
    str1.append( "</b>." );
    QLabel* label1 = new QLabel( str1 );
    QLabel* label2 = new QLabel( "(c) 2013\n" );
    QLabel* label3 = new QLabel( "Juelich Supercomputing Centre,\nForschungszentrum Juelich GmbH" );
    QLabel* label4 = new QLabel( "<b>Home page:</b> www.scalasca.org" );
    QLabel* label5 = new QLabel( "<b>Technical support:</b> scalasca@fz-juelich.de" );
    QLabel* label6 = new QLabel();
    label6->setPixmap( QPixmap( ":/images/fzjlogo.xpm" ) );

    QHBoxLayout* layout2 = new QHBoxLayout();
    layout2->addWidget( label6 );
    layout2->addWidget( label2 );
    layout2->addWidget( label3 );

    layout->addWidget( label1 );
    layout->addLayout( layout2 );
    layout->addWidget( label4 );
    layout->addWidget( label5 );

    layout->setAlignment( label1, Qt::AlignHCenter );
    layout->setAlignment( layout2, Qt::AlignHCenter );
    layout->setAlignment( label4, Qt::AlignHCenter );
    layout->setAlignment( label5, Qt::AlignHCenter );

    dialog.setLayout( layout );
    dialog.exec();

    statusBar()->showMessage( tr( "Ready" ) );
}


void
MainWidget::keyHelp()
{
    QString key1, role1;
    key1.append( "Shift+F1\n\n" );
    role1.append( "Help: What\'s this?\n\n" );
    key1.append( "Ctrl+O\n" );
    role1.append( "Shortcut for menu File -> Open\n" );
    key1.append( "Ctrl+W\n" );
    role1.append( "Shortcut for menu File -> Close\n" );
    key1.append( "Ctrl+Q\n" );
    role1.append( "Shortcut for menu File -> Quit \n" );
    key1.append( "Left click\n" );
    role1.append( "over menu/tool bar: activate menu/function \n" );
    key1.append( "\n" );
    role1.append( "over value mode combo: select value mode\n" );
    key1.append( "\n" );
    role1.append( "over tab: switch to tab \n" );
    key1.append( "\n" );
    role1.append( "in tree: select/deselect/expand/collapse items\n" );
    key1.append( "\n" );
    role1.append( "in topology: select item\n" );
    key1.append( "Right click\n" );
    role1.append( "in tree: context menu\n" );
    key1.append( "\n" );
    role1.append( "in topology: context information \n" );
    key1.append( "Ctrl+Left click\n" );
    role1.append( "in tree: multiple selection/deselection \n" );
    key1.append( "Left drag\n" );
    role1.append( "over scroll bar: scroll \n" );
    key1.append( "\n" );
    role1.append( "in topology: rotate topology \n" );
    key1.append( "Ctrl+Left drag\n" );
    role1.append( "in topology: increase plane distance \n" );
    key1.append( "Shift+Left drag\n" );
    role1.append( "in topology: move topology \n" );
    key1.append( "Mouse wheel\n" );
    role1.append( "in topology: zoom in/out \n" );
    key1.append( "Up arrow\n" );
    role1.append( "in tree: move selection one item up (single-selection only)\n" );
    key1.append( "\n" );
    role1.append( "in topology/scroll area: scroll one unit up \n" );
    key1.append( "Down arrow\n" );
    role1.append( "in tree: move selection one item down (single-selection only) \n" );
    key1.append( "\n" );
    role1.append( "in topology/scroll area:: scroll one unit down \n" );
    key1.append( "Left arrow\n" );
    role1.append( "in scroll area: scroll to the left \n" );
    key1.append( "Right arrow\n" );
    role1.append( "in scroll area: scroll to the right\n" );
    key1.append( "Page up\n" );
    role1.append( "in tree/topology/scroll area: scroll one page up \n" );
    key1.append( "Page down\n" );
    role1.append( "in tree/topology/scroll area: scroll one page down \n" );


    QString key2, role2;
    key2.append( "Up Arrow\n" );
    role2.append( "Move one line up\n" );
    key2.append( "Down Arrow\n" );
    role2.append( "Move one line down\n" );
    key2.append( "Left Arrow\n" );
    role2.append( "Scroll one character to the left (if horizontally scrollable)\n" );
    key2.append( "Right Arrow\n" );
    role2.append( "Scroll one character to the right (if horizontally scrollable)\n" );
    key2.append( "Page Up\n" );
    role2.append( "Move one (viewport) page up \n" );
    key2.append( "PageDown\n" );
    role2.append( "Move one (viewport) page down \n" );
    key2.append( "Home\n" );
    role2.append( "Move to the beginning of the text \n" );
    key2.append( "End\n" );
    role2.append( "Move to the end of the text \n" );
    key2.append( "Mouse wheel\n" );
    role2.append( "Scroll the page vertically\n" );
    key2.append( "Alt+Mouse wheel\n" );
    role2.append( "Scroll the page horizontally (if horizontally scrollable)\n" );
    key2.append( "Ctrl+Mouse wheel\n" );
    role2.append( "Zoom the text \n" );
    key2.append( "Ctrl+A\n" );
    role2.append( "Select all text \n " );


    QString key3, role3;
    key3.append( "Left Arrow\n" );
    role3.append( "Move one character to the left \n" );
    key3.append( "Right Arrow\n" );
    role3.append( "Move one character to the right\n" );
    key3.append( "Backspace\n" );
    role3.append( "Delete the character to the left of the cursor\n" );
    key3.append( "Delete\n" );
    role3.append( "Delete the character to the right of the cursor\n" );
    key3.append( "Ctrl+C\n" );
    role3.append( "Copy the selected text to the clipboard\n" );
    key3.append( "Ctrl+Insert\n" );
    role3.append( "Copy the selected text to the clipboard\n" );
    key3.append( "Ctrl+K\n" );
    role3.append( "Delete to the end of the line\n" );
    key3.append( "Ctrl+V\n" );
    role3.append( "Paste the clipboard text into text edit\n" );
    key3.append( "Shift+Insert\n" );
    role3.append( "Paste the clipboard text into text edit\n" );
    key3.append( "Ctrl+X\n" );
    role3.append( "Delete the selected text and copy it to the clipboard\n" );
    key3.append( "Shift+Delete\n" );
    role3.append( "Delete the selected text and copy it to the clipboard\n" );
    key3.append( "Ctrl+Z\n" );
    role3.append( "Undo the last operation\n" );
    key3.append( "Ctrl+Y\n" );
    role3.append( "Redo the last operation\n" );
    key3.append( "Ctrl+Left arrow\n" );
    role3.append( "Move the cursor one word to the left\n" );
    key3.append( "Ctrl+Right arrow\n" );
    role3.append( "Move the cursor one word to the right\n" );
    key3.append( "Ctrl+Home\n" );
    role3.append( "Move the cursor to the beginning of the text\n" );
    key3.append( "Ctrl+End\n" );
    role3.append( "Move the cursor to the end of the text\n" );
    key3.append( "Hold Shift + some movement\n" );
    role3.append( "Select region \n" );
    key3.append( "(e.g. Right arrow)\n" );


    QLabel*      key1Label  = new QLabel( key1 );
    QLabel*      role1Label = new QLabel( role1 );
    QLabel*      key2Label  = new QLabel( key2 );
    QLabel*      role2Label = new QLabel( role2 );
    QLabel*      key3Label  = new QLabel( key3 );
    QLabel*      role3Label = new QLabel( role3 );

    QHBoxLayout* layout1 = new QHBoxLayout();
    layout1->addWidget( key1Label );
    layout1->addWidget( role1Label );
    layout1->setAlignment( key1Label, Qt::AlignTop | Qt::AlignLeft );
    layout1->setAlignment( role1Label, Qt::AlignTop | Qt::AlignLeft );
    QHBoxLayout* layout2 = new QHBoxLayout();
    layout2->addWidget( key2Label );
    layout2->addWidget( role2Label );
    layout2->setAlignment( key2Label, Qt::AlignTop | Qt::AlignLeft );
    layout2->setAlignment( role2Label, Qt::AlignTop | Qt::AlignLeft );
    QHBoxLayout* layout3 = new QHBoxLayout();
    layout3->addWidget( key3Label );
    layout3->addWidget( role3Label );
    layout3->setAlignment( key3Label, Qt::AlignTop | Qt::AlignLeft );
    layout3->setAlignment( role3Label, Qt::AlignTop | Qt::AlignLeft );

    QLabel*      descr1 = new QLabel( "<b>General control:</b>" );
    QLabel*      descr2 = new QLabel( "<b>For source code editor in read-only mode:</b>" );
    QLabel*      descr3 = new QLabel( "<b>For source code editor in read-and-write mode additionally:</b>" );

    QVBoxLayout* widgetLayout = new QVBoxLayout();
    widgetLayout->addWidget( descr1 );
    widgetLayout->addLayout( layout1 );
    widgetLayout->addWidget( descr2 );
    widgetLayout->addLayout( layout2 );
    widgetLayout->addWidget( descr3 );
    widgetLayout->addLayout( layout3 );

    widgetLayout->setAlignment( descr1, Qt::AlignTop | Qt::AlignLeft );
    widgetLayout->setAlignment( descr2, Qt::AlignTop | Qt::AlignLeft );
    widgetLayout->setAlignment( descr3, Qt::AlignTop | Qt::AlignLeft );
    widgetLayout->setAlignment( layout1, Qt::AlignTop | Qt::AlignLeft );
    widgetLayout->setAlignment( layout2, Qt::AlignTop | Qt::AlignLeft );
    widgetLayout->setAlignment( layout3, Qt::AlignTop | Qt::AlignLeft );

    QWidget* widget = new QWidget();
    widget->setLayout( widgetLayout );

    QScrollArea* scrollWidget = new QScrollArea();
    scrollWidget->setWidget( widget );

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget( scrollWidget );

    QDialog dialog( this );
    dialog.setLayout( layout );
    dialog.exec();

    statusBar()->showMessage( tr( "Ready" ) );
}


//sets the file size threshold above which dynamic loading of metric data is enabled
//
void
MainWidget::setDynloadThreshold()
{
    bool ok;
    int  value = QInputDialog::getInteger( this, "Dynamic loading threshold", "File size threshold (byte) above which files are dynamically loaded: ", dynloadThreshold, 0, 1000000000, 50000000, &ok );
    if ( ok )
    {
        dynloadThreshold = value;
    }
}


//saves a screenshot in a png file
//(it is without the shell frame, I could not find any possibilities to include that frame within qt)
//
void
MainWidget::screenshot()
{
    //this is based on repainting the window into the pixmap
    QPixmap pixmap = QPixmap::grabWidget( this );
    //alternatively direct shot from pixels on the screen:
    //QPixmap pixmap = QPixmap::grabWindow(winId());

    QImage  image = pixmap.toImage();

    QString format      = "png";
    QString initialPath = QDir::currentPath() + tr( "/cube." ) + format;

    QString fileName = QFileDialog::getSaveFileName( this, tr( "Save As" ),
                                                     initialPath,
                                                     tr( "%1 Files (*.%2);;All Files (*)" )
                                                     .arg( format.toUpper() )
                                                     .arg( format ) );
    if ( !fileName.isEmpty() )
    {
        image.save( fileName, format.toAscii() );
    }

    statusBar()->showMessage( tr( "Ready" ) );
}

//sets the busy cursor
//
void
MainWidget::setBusy( bool busy )
{
    setCursor( busy ? Qt::BusyCursor : Qt::ArrowCursor );
}



//distribute the available width between the 3 tabs such that
//from each scroll area the same percentual amount is visible
//
void
MainWidget::distributeWidth()
{
    assert( splitterWidget != NULL );
    //if no data is loaded, do nothing
    if ( metricTabWidget->count() == 0 )
    {
        return;
    }

    //get the current sizes of the splitter's widgets
    QList<int> sizes = splitterWidget->sizes();
    int        sum1  = 0;
    for ( int i = 0; i < 3; i++ )
    {
        sum1 += sizes[ i ];
    }

    //get optimal widths for the splitter's widgets
    sizes[ 0 ] = metricTabWidget->getOptimalWidth();
    sizes[ 1 ] = callTabWidget->getOptimalWidth();
    sizes[ 2 ] = systemTabWidget->getOptimalWidth();

    //distribute the available width
    int sum2 = 0;
    for ( int i = 0; i < 3; i++ )
    {
        sum2 += sizes[ i ];
    }
    for ( int i = 0; i < 3; i++ )
    {
        sizes[ i ] = ( int )( ( double )sum1 * ( double )sizes[ i ] / ( double )sum2 );
    }

    //resize
    splitterWidget->setSizes( sizes );
    repaint();
}
//end of distributeWidth()


//adapt the size of the main window such that
//all informations are visible
//and resize the splitter widget accordingly
//
void
MainWidget::adaptWidth()
{
    assert( splitterWidget != NULL );
    //if no data is loaded, do nothing
    if ( metricTabWidget->count() == 0 )
    {
        return;
    }

    //get the current sizes of the splitter's widgets
    QList<int> sizes = splitterWidget->sizes();
    int        sum   = 0;
    for ( int i = 0; i < 3; i++ )
    {
        sum += sizes[ i ];
    }
    //calculate the main widget's width without the splitter's widgets;
    //needed to take frames etc. into account
    sum = width() - sum;

    //get optimal widths for the splitter's widgets
    sizes[ 0 ] = metricTabWidget->getOptimalWidth();
    sizes[ 1 ] = callTabWidget->getOptimalWidth();
    sizes[ 2 ] = systemTabWidget->getOptimalWidth();

    //add them up
    for ( int i = 0; i < 3; i++ )
    {
        sum += sizes[ i ];
    }

    //resize
    resize( sum, height() );
    splitterWidget->setSizes( sizes );
    repaint();
}
//end of adaptWidth()
