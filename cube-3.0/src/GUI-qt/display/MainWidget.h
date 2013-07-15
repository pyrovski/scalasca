/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _MAINWIDGET_H
#define _MAINWIDGET_H

#include <QtGui>

#include "Constants.h"
#include "SystemTopologyToolBar.h"

class Statistics;
class TabWidget;
class Action;
class ColorWidget;
class ValueWidget;
class PrecisionWidget;
class FontWidget;

namespace cube
{
class Cube;
}

//this class defines the main window of the GUI

class MainWidget : public QMainWindow
{
    Q_OBJECT

public:


    /********************public methods *****************/

    MainWidget();
    ~MainWidget();

    //load the contents of a cube fie into a cube object
    //and display the contents
    bool
    readFile( const QString openFileName );

private slots:

    /********************private slots *****************/

    //input a file name to open and open it via readFile(...)
    void
    openFile();

    //re-open the last file, the second last file, etc.
    void
    openLast0();
    void
    openLast1();
    void
    openLast2();
    void
    openLast3();
    void
    openLast4();

    //remove loaded data
    void
    closeFile();

    //input a file name to open as external file for external percentage
    //and open it via readExternalFile(...)
    bool
    openExternalFile();

    //remove loaded external data for external percentage
    void
    closeExternalFile();

    //closes the application after eventually saving settings
    void
    closeApplication();

    //set the order of metric/call/system splitter elements
    void
    setDimensionOrder();

    //displays a short intro to cube
    void
    introduction();

    //shows the about message
    void
    about();

    //list all mouse and keyboard control
    void
    keyHelp();

    //saves a screenshot in a png file
    //(it is without the shell frame, I could not find any possibilities to include that frame within qt)
    void
    screenshot();

    //sets the file size threshold above which dynamic loading of metric data is enabled
    void
    setDynloadThreshold();

    //sets the busy cursor
    void
    setBusy( bool busy );

    //sets the status bar text
    void
    setMessage( const QString message );

    //sets the value modus for the metric/call/system component as defined by the given combo
    void
    onSetMetricValueModus( int modus );
    void
    onSetCallValueModus( int modus );
    void
    onSetSystemValueModus( int modus );

    //causes to open a color dialog to allow the user to change color-specific settings
    void
    setColors();

    //causes to open a precision dialog to allow the user to change precision-specific settings
    void
    setPrecision();

    //causes to open a font dialog to allow the user to change
    //font and spacing for tree widgets
    void
    setFont();

    //slot to apply the settings of the font widget
    void
    applyFont( FontWidget* fontWidget );

    //sets selections in trees to be marked by a special background
    void
    setSelectionBackground();

    //sets selections in trees to be marked by framing
    void
    setSelectionFrame();

    //methods for saving/loading/deleting settings
    void
    saveSettings();
    void
    saveDefaultSettings();
    void
    saveSettings( const QString& settingsName );
    void
    loadSettings( bool showError = true );
    void
    loadSettings( const QString& settingsName );
    void
    deleteSettings();
    void
    deleteSettings( const QString& settingsName );


    //distribute the available width between the 3 tabs such that
    //from each scroll area the same percentual amount is visible
    void
    distributeWidth();

    //adapt the size of the main window such that
    //all informations are visible
    //and resize the splitter widget accordingly
    void
    adaptWidth();

    //since the topology tool bar can be hidden from the tool bar itself, without using the menu for that,
    //and I did not find any signal signalling this event,
    //if the menu is to be shown, I update the status of the topology tool bar menu
    void
    updateToolbarMenu();

    //repaints everything
    void
    onApply();

    // shows performance info for selected metrics
    void
    onMetricsInfo();

    // shows performance info for selected metrics
    void
    onRegionsInfo();

signals:

    /******************** signals *****************/

    //signal emitted when a file was successfully opened
    void
    fileOpened();

    //signal emitted when a file was successfully closed
    void
    fileClosed();

    //signal emitted when an external file was successfully opened
    void
    externalFileOpened();

    //signal emitted when an external file was successfully closed
    void
    externalFileClosed();


protected:
    virtual void
    closeEvent( QCloseEvent* event );


private:


    /******************** private fields **********************/

    //the cube database for loaded data
    cube::Cube* cube;
    //the cube database for loaded external data for external percentage
    cube::Cube* cubeExternal;

    //the splitter widget containing the 3 widgets for the metric/call/system components
    QSplitter* splitterWidget;

    //the three tab widgets
    TabWidget* metricTabWidget;
    TabWidget* callTabWidget;
    TabWidget* systemTabWidget;

    //combos for the value modi
    QComboBox* metricCombo;
    QComboBox* callCombo;
    QComboBox* systemCombo;

    //value widgets for displaying the usually more exact values and
    //minimum/maximum values, eventually also the absolute values
    //for the current selection
    ValueWidget* metricValueWidget;
    ValueWidget* callValueWidget;
    ValueWidget* systemValueWidget;

    //color widget for setting/storing color specifications and getting colors for values
    ColorWidget* colorWidget;

    //precision widget for setting/storing precision and converting numbers into text
    PrecisionWidget* precisionWidget;

    //stores at most 5 last opened files
    std::vector<QString>  openedFiles;
    //stores the name of the current external file
    QString               lastExternalFileName;
    //stores the actions for opening the last at most 5 files
    std::vector<QAction*> lastFileAct;
    //file menu containing u.o. the last file actions
    QMenu*                fileMenu;

    //the tool bar for topology
    SystemTopologyToolBar* topologyToolBar;

    //actions modifying the tool bar outlook
    Action* textAct;
    Action* iconAct;
    Action* hideToolbarAct;
    Action* white1Act, * white2Act;
    Action* blackLinesAct, * grayLinesAct, * whiteLinesAct, * noLinesAct;
    Action* selection1Act, * selection2Act;
    Action* emptyAct, * antialiasingAct;
    Action* splitterOrderAct;
#if defined( WITH_VAMPIR )
    Action* vampirAct;
#endif
#if defined( WITH_PARAVER )
    Action* paraverAct;
#endif
#if defined( WITH_VAMPIR ) || defined( WITH_PARAVER )
    QMenu* traceBrowserMenu;
#endif

    //threshold for file size above which dynamic metric loading is enabled
    int dynloadThreshold;

    //the font for the tree display, user-definable
    QFont treeFont;
    //spacing for the tree display, user-definable
    int   spacing;

    /******************** private methods **********************/

    //read the contents of a cube file into a temporary cube object for
    //the external percentage modus and compute the relevant values for the display
    bool
    readExternalFile( const QString fileName );

    //returns if currently a file is loaded
    bool
    fileLoaded();

    //remember the names of the last 5 files that were opened;
    //they can be re-opened via the display menu
    void
    rememberFileName( QString fileName );

    //creates the pull-down menu
    void
    createMenu();

    //update the title of the application to show infos to the loaded file and external file
    void
    updateWidgetTitle();

    //return the internal type for a value modus with the given displayed name
    ValueModus
    getValueModus( QString text );

    //return the displayed name for a given value modus
    QString
    getValueModusText( ValueModus modus );

    //if the order of the metric/call/system widgets changes,
    //the value modi must be adapted;
    //that is what this method does
    void
    updateCombos();

    //determine the current order of the metric/call/system widgets
    void
    getWidgetIndices( int& metricIndex,
                      int& callIndex,
                      int& systemIndex );

    //an object serving metric statistics
    Statistics* statistics;

#if defined( WITH_VAMPIR ) || defined( WITH_PARAVER )
    //needed for the trace browser dialog's file input
    QLineEdit* fileLine;
    QLineEdit* configLine;
    void
    createTraceBrowserMenu();
    void
    initializeTraceBrowserMenu( QString const &fileName );
    void
    disableTraceBrowserMenu();
    void
    toggleTraceBrowserMenuVisibility( bool visible );

private slots:
  #if defined( WITH_VAMPIR )
    //connect to vampir and display trace data there
    void
    vampirMenu();

  #endif
  #if defined( WITH_PARAVER )
    //connect to paraver and display trace data there
    void
    paraverMenu();

    //slot for a config browser file name input
    void
    getConfigFileName();

  #endif
    //slot for a trace browser file name input
    void
    getTraceFileName();

#endif
};



#endif
