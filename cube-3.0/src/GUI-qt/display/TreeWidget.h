/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _TREEWIDGET_H
#define _TREEWIDGET_H

#include <QMenu>
#include <QFile>
#include <QHash>

#include "TreeWidgetItem.h"
#include "Action.h"
#include "Coloring.h"
#include "MyTreeWidget.h"
#include "ScrollArea.h"

#include "Cube.h"
#include "Driver.h"

class QTextEdit;
class QDialog;
class QSettings;
class QString;
class QMouseEvent;
class HelpBrowser;

class ValueWidget;

//the TreeWidget class extends MyTreeWidget basically by value computations

class TreeWidget : public MyTreeWidget
{
    Q_OBJECT

signals:

    //this signal is connected to the tab widget's setMessage signal
    //that displays text in the main widget's status menu
    void
    setMessage( QString message );

    // this signal is emitted if the user has defined or deleted a subset
    void
    definedSubsetsChanged( const QString & );

public:

    TreeWidget( ScrollArea*         parent,
                TreeWidgetType      type,
                QString             cubeFileName = "",
                cubeparser::Driver* driver = NULL,
                Statistics*         statistics = 0 );
    ~TreeWidget();

    //sets the type of the widget
    void
    setType( TreeWidgetType type );

    //creates the tree structure
    void
    initialize( cube::Cube* cube );

    //computes the current values for all tree widget types
    void
    computeValues();
    void
    computeValue( TreeWidgetItem* item );
    void
    computeValue( TreeWidgetItem* metricItem,
                  ItemState       metricState,
                  TreeWidgetItem* callItem,
                  ItemState       callState,
                  TreeWidgetItem* systemItem,
                  ItemState       systemState,
                  double&         value );

    //computes the current reference values
    void
    computeReferenceValues();

    //Computes the maximal values for system trees.
    //For metric trees, maximal values are computed for all roots.
    void
    computeMaxValues();

    //(re-)display all items
    void
    displayItems();

    //update the displayed values in the value widget
    //below the tree widget
    void
    updateValueWidget();

    //save and load settings
    void
    saveSettings( QSettings& settings );
    bool
    loadSettings( QSettings& settings );

    const QHash<QString, std::vector<TreeWidgetItem*> > &
    getDefinedSubsets()
    {
        return definedSubsets;
    }
    std::vector<TreeWidgetItem*>
    getActiveSubset();
    int
    getSubsetItemCount( const QString &name );

private slots:

    //hiding slots
    void
    onHideAction( QAction* action );
    void
    onDynamicHiding();
    void
    onHideClicked();
    void
    onShowClicked();
    void
    onStaticMinorHiding();

    //shows the short description for a call tree item
    void
    onDescr();

    //shows the location for a call tree item
    void
    onLocation();

    //shows the location of the callee for a call tree item
    void
    onLocationCallee();

    //opens an editor with the source code for a call tree item
    void
    onSourceCode();

    //opens an editor with the source code of the callee for a call tree item
    void
    onSourceCodeCallee();

    //marks all items containing a user-defined text
    void
    onFindItem();
    void
    onFindNext( bool selectItems = false );

    //removes the marings made ny onFindItem()
    void
    onUnmarkItems();

    //saves a source code file opened by onSourceCode()
    void
    onSaveFile();

    //saves a source code file opened by onSourceCode()
    //under a new name
    void
    onSaveFileAs();

    //this slot allows the user to activate and de-activate minimal and
    //maximal values for the minimal and maximal colors;
    //values outside the user-defined interval get displayed by gray
    void
    onMinMaxValues();

    //sort flat call profile items by value or by name
    void
    onSortByValueDescending();
    void
    onSortByNameAscending();

    //method to toggle the readOnly status of source code editors
    void
    onToggleReadOnly( bool checked );

public slots:
    //shows the online description for a call tree item
    void
    onUrl();

    //shows the online description for a given tree item
    void
    openUrlOfItem( TreeWidgetItem* );


#ifdef WITH_EXPERIMENTAL_VIEWS
    // experimental: plot some graphics
    void
    plotMetricCallnodeOverSelectedThreads();
    void
    plotMetricCallnodeOverAllThreads();
    void
    plotMetricSubCallnodesOverFirstThread();
    void
    plotMetricSubCallnodesOverAllThreads();
    void
    plotMetricSubCallnodesOverSelectedThreads();

#endif   /* WITH_EXPERIMENTAL_VIEWS */

    // change Font in the source code dialog
    void
    onChangeFont();

protected:

    void
    mousePressEvent( QMouseEvent* event );

    //key event handling
    void
    keyPressEvent( QKeyEvent* event );

    //called when the selection has changed
    void
    onSelection();
    void
    onDeselection();

private:

    friend class SystemTopologyWidget;
    friend class SystemTopologyData;
    friend class SystemBoxPlot;

    /**** basic fields ****/

    //the cube object storing the structures and severities
    cube::Cube* cube;

    //reference values needed for computation
    //for the modi different from absolute value
    double referenceValue1;
    double referenceValue2;

    /*******initialization*********/

    //creates the items of the tree
    template<typename T>
    void
    createItems( const std::vector<T*> &  vec,
                 const TreeWidgetItemType type );

    //determines if a region has any callees
    //(only then there is a child node in the flat profile)
    bool
    hasCallee( cube::Region* region );

    //for call trees and the aggregate mode MAX only:
    //this method marks those items that belong to the main tree
    void
    markMaxItems();

    /******updating and value computation *******/

    //gets the order of the tab widgets in the splitter widget
    void
    getIndices( int& index,
                int& metricIndex,
                int& callIndex,
                int& systemIndex );

    //compute the item values ownValue, totalValue, rootValue,
    //and for system items maxValue
    void
    computeBasicValues();

    //Computes the current value to display.
    //If the value is non-defined, ok is false, otherwise true.
    //A value can be non-defined, if, e.g., the own value should be divided
    //by the reference value 0.
    void
    getValue( TreeWidgetItem* item,
              bool            expanded,
              double&         value,
              bool&           ok );
    void
    getValue( TreeWidgetItem* item,
              bool            expanded,
              double&         value,
              bool&           ok,
              ValueModus      valueModus );

    //Computes the values for the item in the current setting for the cases
    //when it is expanded or not; value1 is the smaller one, value2 is the larger one.
    void
    getMaxValue( TreeWidgetItem* item,
                 double&         value );


    /******* value display **************/

    //returns if the widget's items' values are of type int
    bool
    hasIntegerType( TreeWidgetItem* item = NULL );
    bool
    hasAbsoluteIntegerType( TreeWidgetItem* item = NULL );

    //if integerType holds, the item's value is assumed to be of type int
    void
    displayItem( TreeWidgetItem* item,
                 bool            expanded,
                 bool            integerType );

    /******** coloring *************/

    //the colorWidget is needed for the icon generation
    ColorWidget* colorWidget;

    //Stores the maximal values in the tree for coloring.
    //For metric trees, values are stored for each root, i.e.,
    //coloring is root-dependent.
    std::vector<double> maxValues;
    double              userMinValue, userMaxValue;

    //Used for metric trees only.
    //maxValueIndex stores at position i
    //the position in the vectors maxValues
    //where the maximal value for item[i]
    //is stored
    std::vector<unsigned> maxValueIndex;

    //the user can define the minimal and maximal values that correspond
    //to the minimal and maximal colors;
    //this variable stores if this feature is active
    bool userDefinedMinMaxValues;


    /********context menu**************/

    QMenu contextMenu;

    void
    defineContextMenu();

    //stores the hiding state: dynamic hiding, static hiding or no hiding
    HidingState hidingState;
    //threshold for hiding
    double      hidingThreshold;
    //method for dynamic hiding
    void
    hideMinorValues();

    //shows all hidden items and switch off hiding
    void
    showAll();

    //gets as user input a threshold for hiding minor-valued items
    void
    getHidingThreshold( bool& ok );

    //shows all hidden items and switch off hiding
    void
    noHiding();

    //static hiding slots
    void
    staticHiding();

    void
    markTreeItems( std::vector<TreeWidgetItem*> itemList );

    //these actions of the context menu must be stored in the object because
    //they get enabled/disabled when not relevant
    Action* descrAct;
    Action* urlAct;
    Action* locationAct;
    Action* sourceCodeAct;
    Action* locationCalleeAct;
    Action* sourceCodeCalleeAct;
    Action* expandClickedAct;
    Action* collapseClickedAct;
    Action* expandPeersAct;
    Action* collapsePeersAct;
    Action* expandLargestAct;
    Action* hideAct;
    Action* showAct;
    Action* dynamicHidingAct;
    Action* staticHidingAct;
    Action* noHidingAct;

    QMenu*  siteMenu;
    QMenu*  regionMenu;

    QMenu*  expandMenu;
    QMenu*  hiddingMenu;
    QMenu*  infoMenu;


    void
    location( TreeWidgetItem* item,
              bool            takeCallee );
    void
    sourceCode( TreeWidgetItem* item,
                bool            takeCallee );

    //this is the editor to display and edit source code files
    QTextEdit* textEdit;
    // Selected Font
    QFont      fontSourceCode;

    //these are two buttons that get enabled and disabled when toggling the readOnly status
    QPushButton* textEditSaveButton, * textEditSaveAsButton;

    //this is the name of the last opened/stored source code file
    QString fileName;
    //this is the name of the last source code file opened
    QString lastSourceCodeName;
    //the first time a source code is not found in the given location,
    //we ask if the user wishes to open another file;
    //after this first time, we do not ask any more but directly open
    //an open file dialog
    bool firstSourceNotFound;

    //help browser dialog
    HelpBrowser* helpBrowser;

    //source code dialog
    QDialog* dialog;

    //sort flat call profile items by value or by name
    void
    sortByValue( bool ascending );
    void
    sortByName( bool ascending );

    //sortedByName remembers if we sort by name or by value;
    //relevant only for flat call profile
    bool sortedByName;
    //the variable toSort is true only if this is a flat call profile and
    //if we loaded a setting in which the items are sorted by value;
    //upon loading the setting we cannot sort yet, because the values are not yet computed;
    //thus we do so by the first display
    bool                toSort;

    cubeparser::Driver* driver;
    QString             cubeFileName;
    std::vector<bool>   isLoaded;
    void
    loadMissingMetrices();

    //for text search
    //in MyTreeWidget there is a "TreeWidgetItem* findItem" defined
    QString findText;

    //support for statistics and trace browser connections
    //NOTE: also addition constructor argument (statistics)
    Statistics*                                   statistics;
    Action*                                       statisticsAct;

    QHash<QString, std::vector<TreeWidgetItem*> > definedSubsets;
    QHash<QString, int >                          subsetItemCount;

    // returns all subtrees of selected elements.
    void
    getSubTrees( QVector<TreeWidgetItem*>&,
                 TreeWidgetItem* );

#if defined( WITH_VAMPIR ) || defined( WITH_PARAVER )
    Action* traceAct;
#endif

private slots:
    //copy text of selected item to clipboard
    void
    copyToClipboard();

    void
    onShowStatistics();
    void
    createStatisticsContextMenuItems();
    void
    statisticsMousePressEvent();
    void
    updateStatisticsSelection();

    void
    defineSubset();

    std::vector<TreeWidgetItem*>
    getVisitedSubset();

#if defined( WITH_VAMPIR ) || defined( WITH_PARAVER )
    void
    onShowMaxSeverity();

#endif
};


#endif
