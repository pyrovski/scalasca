/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstring>
#include <iostream>
#include <cassert>
#include <cmath>
#ifndef CUBE_COMPRESSED
#  include <fstream>
#else
#  include "zfstream.h"
#endif

#include <QMouseEvent>
#include <QString>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QHBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QSettings>
#include <QScrollBar>
#include <QtNetwork>
#include <QDialogButtonBox>
#include <QPainter>
#include <QRadioButton>
#include <QFontDatabase>
#include <QRegExp>
#include <QTextEdit>
#include <QClipboard>
#include <QFontDialog>
#include <QFontMetrics>


#include "Environment.h"
#include "TreeWidget.h"
#include "TypedWidget.h"
#include "TabWidget.h"
#include "PrecisionWidget.h"
#include "ValueWidget.h"
#include "HelpBrowser.h"
#include "ScrollArea.h"
#include "StatisticInfo.h"
#include "MessageWidget.h"

#ifdef WITH_EXPERIMENTAL_VIEWS
#  include "2DPlotWidget.h"
#  include "ColorMapPlotWidget.h"
#endif   /* WITH_EXPERIMENTAL_VIEWS */

#include "Cube.h"
#include "Metric.h"
#include "Cnode.h"
#include "Region.h"
#include "Machine.h"
#include "Node.h"
#include "Process.h"
#include "Thread.h"
#include "Vertex.h"
#include "cube_error.h"

using namespace std;


namespace
{
QString
getUrl( TreeWidgetItem* item )
{
    //we must distinguish on the item type to call the right get method
    if ( item->type == METRICITEM )
    {
        return QString::fromStdString( ( ( cube::Metric* )( item->cubeObject ) )->get_url() );
    }
    else if ( item->type == CALLITEM )
    {
        return QString::fromStdString( ( ( cube::Cnode* )( item->cubeObject ) )->get_callee()->get_url() );
    }
    else if ( item->type == REGIONITEM && item->cubeObject != NULL )
    {
        return QString::fromStdString( ( ( cube::Region* )( item->cubeObject ) )->get_url() );
    }

    return QString();
}
}   /* unnamed namespace */


/*****************************************************/
/* constructor, desctructor, and                     */
/* methods called only by them                       */
/*****************************************************/

//
//TreeWidget constructor
//
TreeWidget::TreeWidget( ScrollArea*         parent,
                        TreeWidgetType      type,
                        QString             cubeFileName,
                        cubeparser::Driver* driver,
                        Statistics*         statistics )
    : MyTreeWidget( parent ), helpBrowser( NULL ), driver( driver ),
      cubeFileName( cubeFileName ), statistics( statistics )
{
    //set the type METRICTREE, CALLTREE, CALLFLAT, or SYSTEMTREE of the tree widget
    this->type   = type;
    this->parent = parent;

    //defines the context menu for the tree widget
    //the context menu can be called with right-mouse-click within the widget
    defineContextMenu();

    //the color widget is needed for computing the pixmaps for the tree items
    colorWidget = ( ( ( TabWidget* )( parent->parent() ) )->getColorWidget() );

    //initially coloring is from the minimal to the maximal value in the tree
    userDefinedMinMaxValues = false;
    //initially reference values are invalid
    referenceValue1 = 0.0;
    referenceValue2 = 0.0;
    //no last source code opened yet
    lastSourceCodeName = "";
    //there is no need to sort items
    toSort = false;
    //the first time a source code is not found in the given location,
    //we ask if the user wishes to open another file;
    //after this first time, we do not ask any more but directly open
    //an open file dialog
    firstSourceNotFound = true;

    //for the search in trees
    findText = "";

    clickedItem = NULL;

    textEditSaveButton   = NULL;
    textEditSaveAsButton = NULL;


    setWhatsThis( "The Cube Qt display has three tree browsers, each of them representing a dimension of the performance space.  Per default, the left tree displays the metric dimension, the middle tree displays the program dimension, and the right tree displays the system dimension.\n\nThe nodes in the metric tree represent metrics. The nodes in the program dimension can have different semantics depending on the particular view that has been selected. In the call tree view, they represent call paths forming a call tree. The nodes in the system dimension represent machines, nodes, processes, or threads from top to bottom. \n\nEach node is associated with a value, which is called the severity and is displayed simultaneously using a numerical value as well as a colored square. Colors enable the easy identification of nodes of interest even in a large tree, whereas the numerical values enable the precise comparison of individual values. The sign of a value is visually distinguished by the relief of the colored square. A raised relief indicates a positive sign, a sunken relief indicates a negative sign. \n\nUsers can perform two basic types of actions: selecting a node or expanding/collapsing a node.  Selecting a node in a tree causes the other trees on its right to display values for that selection. Briefly, a tree is always an aggregation over all selected nodes of its neighbor trees to the left. \n\nCollapsed nodes with a subtree that is not shown are marked by a [+] sign, expanded nodes with a visible subtree by a [-] sign.  You can expand/collapse a node by left-clicking on the corresponding [+]/[-] signs.  Collapsed nodes have inclusive values, i.e., their severity is the sum of the severities over the whole collapsed subtree. On the other hand, the displayed values of expanded nodes are their exclusive values.  Note that expanding/collapsing a selected node causes the change of the current values in the trees on its right-hand-side.\n\nEach tree has its own context menu, that can be activated by a right-mouse-click within the tree's window. If you right-click on one of the tree's nodes, this node gets framed, and serves as a reference node for some of the menu items. We call this node the clicked node. The context menu consists, depending on the type of the tree, of some of the following items. If you move the mouse over a context menu item, the status bar displays some explanation of the functionality of that item." );
    fontSourceCode = font();
}


//
//TreeWidget destructor
//
TreeWidget::~TreeWidget()
{
    for ( int i = 0; i < ( int )items.size(); i++ )
    {
        delete items[ i ];
    }
    items.clear();

    delete driver;
    delete helpBrowser;
}



//
//defines the context menu
//
void
TreeWidget::defineContextMenu()
{
    urlAct              = NULL;
    descrAct            = NULL;
    locationAct         = NULL;
    locationCalleeAct   = NULL;
    sourceCodeAct       = NULL;
    sourceCodeCalleeAct = NULL;
    if ( type == METRICTREE )
    {
        //add actions specific to the metric tree

        //create an action for showing the short metric info of the clicked item,
        //by default disabled, it will be enabled for items for which the info is defined
        descrAct = new Action( tr( "Info" ), this );
        descrAct->setStatusTip( tr( "Shows a short description of the clicked item" ) );
        connect( descrAct, SIGNAL( triggered() ), this, SLOT( onDescr() ) );
        contextMenu.addAction( descrAct );
        descrAct->setEnabled( false );
        descrAct->setWhatsThis( "For all trees (for call trees under \"Called region\"). Gives some short information about the clicked node. Disabled if you did not click over a node or if no information is available for the clicked node." );

        //create an action for showing the online metric info of the clicked item,
        //by default disabled, it will be enabled for items for which the url is defined
        urlAct = new Action( tr( "Online description" ), this );
        urlAct->setStatusTip( tr( "Shows the online description of the clicked item" ) );
        connect( urlAct, SIGNAL( triggered() ), this, SLOT( onUrl() ) );
        contextMenu.addAction( urlAct );
        urlAct->setEnabled( false );
        urlAct->setWhatsThis( "For metric trees and flat call profiles (for call trees see under \"Called region\"). Shows some (usually more extensive) online description for the clicked node. For metrics it might point to an online documentation explaining their semantics. Disabled if no node is clicked or if no online information is available." );
    }
    else if ( type == CALLFLAT )
    {
        //actions specific for the flat call view

        // Collect all "information" actions in to the submenu to make the top level menu lighter.
        // Fast access to the "Online Description" is provided by action "Onine description" on top of the top contex menu. Marmot feedback.
        infoMenu = contextMenu.addMenu( tr( "Info" ) );
        infoMenu->setWhatsThis( "Information about specific metric." );

        //create an action for showing the short cnode info of the clicked item,
        //by default disabled, it will be enabled for items for which the info is defined
        descrAct = new Action( tr( "Info" ), this );
        descrAct->setStatusTip( tr( "Shows a short description of the clicked" ) );
        connect( descrAct, SIGNAL( triggered() ), this, SLOT( onDescr() ) );
        infoMenu->addAction( descrAct );
        descrAct->setEnabled( false );
        descrAct->setWhatsThis( "For all trees (for call trees under \"Called region\"). Gives some short information about the clicked node. Disabled if you did not click over a node or if no information is available for the clicked node." );

        //create an action for showing the online cnode info of the clicked item,
        //by default disabled, it will be enabled for items for which the url is defined
        urlAct = new Action( tr( "Online description" ), this );
        urlAct->setStatusTip( tr( "Shows the online description of the clicked item" ) );
        connect( urlAct, SIGNAL( triggered() ), this, SLOT( onUrl() ) );
        infoMenu->addAction( urlAct );
        urlAct->setEnabled( false );
        urlAct->setWhatsThis( "For metric trees and flat call profiles (for call trees see under \"Called region\"). Shows some (usually more extensive) online description for the clicked node. For regions representing library functions it might point to the corresponding library documentation. Disabled if no node is clicked or if no online information is available." );

        //create an action for showing the location of the clicked item,
        //by default disabled, it will be enabled for items for which the location is defined
        locationAct = new Action( tr( "Location" ), this );
        locationAct->setStatusTip( tr( "Shows the location of the clicked item" ) );
        connect( locationAct, SIGNAL( triggered() ), this, SLOT( onLocation() ) );
        infoMenu->addAction( locationAct );
        locationAct->setEnabled( false );
        locationAct->setWhatsThis( "For flat profiles only. Disabled if no node is clicked. Displays information about the module and position within the module (line numbers) where the method is defined." );

        //create an action for listing the source code of the clicked item in an editor,
        //by default disabled, it will be enabled for items for which the source code is defined
        sourceCodeAct = new Action( tr( "Source code" ), this );
        sourceCodeAct->setStatusTip( tr( "Shows the source code of the clicked item" ) );
        connect( sourceCodeAct, SIGNAL( triggered() ), this, SLOT( onSourceCode() ) );
        infoMenu->addAction( sourceCodeAct );
        sourceCodeAct->setEnabled( false );
        sourceCodeAct->setWhatsThis( "For flat call profiles only (for call trees see \"Call site\" and \"Called region\"). Disabled if no node is clicked. Opens an editor for displaying, editing, and saving the source code of the method/region for which the clicked node stays for.  The begin and the end of the method/region are highlighted. If the specified source file is not found, you are asked to chose a file to open." );
    }
    else if ( type == SYSTEMTREE )
    {
        //actions specific for the system tree

        //create an action for showing the short info for the clicked item,
        //by default disabled, it will be enabled for items for which the info is defined
        descrAct = new Action( tr( "Info" ), this );
        descrAct->setStatusTip( tr( "Shows a short description of the clicked item" ) );
        connect( descrAct, SIGNAL( triggered() ), this, SLOT( onDescr() ) );
        contextMenu.addAction( descrAct );
        descrAct->setEnabled( false );
        descrAct->setWhatsThis( "For all trees (for call trees under \"Called region\"). Gives some short information about the clicked node. Disabled if you did not click over a node or if no information is available for the clicked node." );
    }
    else if ( type == CALLTREE )
    {
        //actions specific for the call tree

        siteMenu = contextMenu.addMenu( tr( "Call site" ) );
        siteMenu->setWhatsThis( "This menu is enabled if you clicked over an item, and offers menu items to display information about the caller of the clicked node." );

        //create an action for showing the location of the clicked item,
        //by default disabled, it will be enabled for items for which the location is defined
        locationAct = new Action( tr( "Location" ), this );
        locationAct->setStatusTip( tr( "Shows the location of the clicked item" ) );
        connect( locationAct, SIGNAL( triggered() ), this, SLOT( onLocation() ) );
        siteMenu->addAction( locationAct );
        locationAct->setWhatsThis( "Disabled if no node is clicked. Displays information about the module and position within the module (line numbers) of the caller of the clicked node." );

        //create an action for listing the source code of the clicked item in an editor,
        //by default disabled, it will be enabled for items for which the source code is defined
        sourceCodeAct = new Action( tr( "Source code" ), this );
        sourceCodeAct->setStatusTip( tr( "Shows the source code of the clicked item" ) );
        connect( sourceCodeAct, SIGNAL( triggered() ), this, SLOT( onSourceCode() ) );
        siteMenu->addAction( sourceCodeAct );
        sourceCodeAct->setEnabled( false );
        sourceCodeAct->setWhatsThis( "Disabled if no node is clicked. Opens an editor for displaying, editing, and saving the source code where the call for which the clicked node stays for happens. The begin and the end of the relevant source code region are highlighted. If the specified source file is not found, you are asked to chose a file to open." );


        //create a sub-menu for the region of the call tree node
        regionMenu = contextMenu.addMenu( tr( "Called region" ) );
        regionMenu->setWhatsThis( "This menu is enabled if you clicked over an item, and offers menu items to display information about the callee of the clicked node." );

        //create an action for showing the short info for the region of the clicked item,
        //by default disabled, it will be enabled for items for which the info is defined
        descrAct = new Action( tr( "Info" ), this );
        descrAct->setStatusTip( tr( "Shows a short description of the clicked item" ) );
        connect( descrAct, SIGNAL( triggered() ), this, SLOT( onDescr() ) );
        regionMenu->addAction( descrAct );
        descrAct->setEnabled( false );
        descrAct->setWhatsThis( "For all trees. Gives some short information about the callee of the clicked node. Disabled if you did not click over a node or if no information is available for the clicked node." );

        //create an action for showing the online info for the region of the clicked item,
        //by default disabled, it will be enabled for items for which the url is defined
        urlAct = new Action( tr( "Online description" ), this );
        urlAct->setStatusTip( tr( "Shows the online description of the clicked item" ) );
        connect( urlAct, SIGNAL( triggered() ), this, SLOT( onUrl() ) );
        regionMenu->addAction( urlAct );
        urlAct->setEnabled( false );
        urlAct->setWhatsThis( "For metric trees, flat call profiles, and call trees. Shows some (usually more extensive) online description for the callee of the clicked node. Disabled if no node is clicked or if no online information is available." );

        //create an action for showing the location of the clicked item's region,
        //by default disabled, it will be enabled for items for which the location is defined
        locationCalleeAct = new Action( tr( "Location" ), this );
        locationCalleeAct->setStatusTip( tr( "Shows the location of the region of the clicked item" ) );
        connect( locationCalleeAct, SIGNAL( triggered() ), this, SLOT( onLocationCallee() ) );
        regionMenu->addAction( locationCalleeAct );
        locationCalleeAct->setWhatsThis( "Displays information about the module and position within the module (line numbers) where the callee method of the clicked node is defined." );

        //create an action for listing the source code of the clicked item's region in an editor,
        //by default disabled, it will be enabled for items for which the source code is defined
        sourceCodeCalleeAct = new Action( tr( "Source code" ), this );
        sourceCodeCalleeAct->setStatusTip( tr( "Shows the source code of the clicked item" ) );
        connect( sourceCodeCalleeAct, SIGNAL( triggered() ), this, SLOT( onSourceCodeCallee() ) );
        regionMenu->addAction( sourceCodeCalleeAct );
        sourceCodeCalleeAct->setEnabled( false );
        sourceCodeCalleeAct->setWhatsThis( "Opens an editor for displaying, editing, and saving the source code of the callee of the clicked node. Begin and end of the relevant region are highlighted. If the specified source code does not exists, you are asked to chose a file to open." );
    }

    contextMenu.addSeparator();

    // Create an "expand/collapse" submenu
    expandMenu = contextMenu.addMenu( tr( "Expand/collapse" ) );
    expandMenu->setWhatsThis( "Collapse or expand the tree." );

    //create an action for expanding all items in the tree
    Action* expandAllAct = new Action( tr( "Expand all" ), this );
    expandAllAct->setStatusTip( tr( "Expand all nodes" ) );
    connect( expandAllAct, SIGNAL( triggered() ), this, SLOT( expandAll() ) );
    expandMenu->addAction( expandAllAct );
    expandAllAct->setWhatsThis( "For all trees. Expands all nodes in the tree." );

    //create an action for expanding all items in the subtree of the clicked item
    expandClickedAct = new Action( tr( "Expand subtree" ), this );
    expandClickedAct->setStatusTip( tr( "Expand all nodes in the clicked subtree" ) );
    connect( expandClickedAct, SIGNAL( triggered() ), this, SLOT( expandClicked() ) );
    expandMenu->addAction( expandClickedAct );
    expandClickedAct->setWhatsThis( "For all trees.  Enabled only if you clicked above a node. Expands all nodes in the subtree of the clicked node (inclusively the clicked node)." );

    if ( type == SYSTEMTREE )
    {
        //create an action for expanding all peers, i.e., the same kind of items,
        //of the clicked item in the system tree
        expandPeersAct = new Action( tr( "Expand peers" ), this );
        expandPeersAct->setStatusTip( tr( "Expand all peers of clicked node" ) );
        connect( expandPeersAct, SIGNAL( triggered() ), this, SLOT( expandPeers() ) );
        expandMenu->addAction( expandPeersAct );
        expandPeersAct->setWhatsThis( "For system trees only.  Enabled only if you clicked above a node. Expands all peer nodes of the clicked node, i.e., all nodes at the same hierarchy depth." );
    }
    else
    {
        expandPeersAct = NULL;
    }

    //create an action for expanding the clicked item and
    //recursively expanding the child having the largest value,
    //starting from the clicked item untill a leaf item is reached
    expandLargestAct = new Action( tr( "Expand largest" ), this );
    expandLargestAct->setStatusTip( tr( "Expand path with largest nodes below clicked" ) );
    connect( expandLargestAct, SIGNAL( triggered() ), this, SLOT( expandLargest() ) );
    expandMenu->addAction( expandLargestAct );
    expandLargestAct->setWhatsThis( "For all trees.  Enabled only if you clicked above a node. Starting at the clicked node, expands its child with the largest inclusive value (if any), and continues recursively with that child until it finds a leaf. It is recommended to collapse all nodes before using this function in order to be able to see the path along the largest values." );

    expandMenu->addSeparator();

    //create an action for collapsing all items in the tree
    Action* collapseAllAct = new Action( tr( "Collapse all" ), this );
    collapseAllAct->setStatusTip( tr( "Collapse all nodes" ) );
    connect( collapseAllAct, SIGNAL( triggered() ), this, SLOT( collapseAll() ) );
    expandMenu->addAction( collapseAllAct );
    collapseAllAct->setWhatsThis( "For all trees. Collapses all nodes in the tree." );

    //create an action for collapsing all items in the subtree of the clicked item
    collapseClickedAct = new Action( tr( "Collapse subtree" ), this );
    collapseClickedAct->setStatusTip( tr( "Collapse all nodes in the clicked subtree" ) );
    connect( collapseClickedAct, SIGNAL( triggered() ), this, SLOT( collapseClicked() ) );
    expandMenu->addAction( collapseClickedAct );
    collapseClickedAct->setWhatsThis( "For all trees.  Enabled only if you clicked above a node. It collapses all nodes in the subtree of the clicked node (inclusively the clicked node)." );

    if ( type == SYSTEMTREE )
    {
        //create an action for collapsing all peers, i.e., the same kind of items,
        //of the clicked item in the system tree
        collapsePeersAct = new Action( tr( "Collapse peers" ), this );
        collapsePeersAct->setStatusTip( tr( "Collapse all peers of clicked node" ) );
        connect( collapsePeersAct, SIGNAL( triggered() ), this, SLOT( collapsePeers() ) );
        expandMenu->addAction( collapsePeersAct );
        collapsePeersAct->setWhatsThis( "For system trees only.  Enabled only if you clicked above a node. Collapses all peer nodes of the clicked node, i.e., all nodes at the same hierarchy depth." );
    }
    else
    {
        collapsePeersAct = NULL;
    }


    // Add "hiding" submenu
    if ( type != METRICTREE )
    {
        // Put all hiding operations in submenu to make the top menu lighter.
        hiddingMenu = contextMenu.addMenu( tr( "Hiding" ) );
        hiddingMenu->setWhatsThis( "Hide selected subtrees." );


        //create an action for dynamically hiding subtrees with minor values
        dynamicHidingAct = new Action( tr( "Dynamic hiding" ), this );
        dynamicHidingAct->setCheckable( true );
        dynamicHidingAct->setChecked( false );
        dynamicHidingAct->setStatusTip( tr( "Dynamically hide nodes with minor total values" ) );
        hiddingMenu->addAction( dynamicHidingAct );
        dynamicHidingAct->setWhatsThis( "Not available for metric trees. This menu item activates dynamic hiding. All currently hidden nodes get shown. You are asked to define a percentage threshold between 0.0 and 100.0. All nodes whose color position on the color scale (in percent) is below this threshold get hidden. As default value, the color percentage position of the clicked node is suggested, if you right-clicked over a node. If not, the default value is the last threshold. The hiding is called dynamic, because upon value changes (caused for example by changing the node selection) hiding is re-computed for the new values. With other words, value changes may change the visibility of the nodes.\n\n During dynamic hiding, for expanded nodes with some hidden children and for nodes with all of its children hidden, their displayed (exclusive) value includes the hidden children's inclusive value. After this sum we display in brackets the percentage of the hidden children's value in it." );

        //create an action for dynamically hiding subtrees with minor values
        Action* dynamicMinorHidingAct = new Action( tr( "   Redefine threshold" ), this );
        dynamicMinorHidingAct->setStatusTip( tr( "Dynamically hide nodes with minor total values" ) );
        connect( dynamicMinorHidingAct, SIGNAL( triggered() ), this, SLOT( onDynamicHiding() ) );
        hiddingMenu->addAction( dynamicMinorHidingAct );
        dynamicMinorHidingAct->setWhatsThis( "This menu item is enabled if dynamic hiding is already activated. This function allows to re-define the dynamic hiding threshold." );

        connect( dynamicHidingAct, SIGNAL( toggled( bool ) ), dynamicMinorHidingAct, SLOT( setEnabled( bool ) ) );

        //create an action for hiding the clicked node
        staticHidingAct = new Action( tr( "Static hiding" ), this );
        staticHidingAct->setCheckable( true );
        staticHidingAct->setChecked( false );
        staticHidingAct->setStatusTip( tr( "Start static hiding (keep hidden nodes)" ) );
        hiddingMenu->addAction( staticHidingAct );
        staticHidingAct->setWhatsThis( "Not available for metric trees. This menu item activates static hiding. All currently hidden nodes keep being hidden.\n\nLike for dynamic hiding, for expanded nodes with some hidden children and for nodes with all of its children hidden, their displayed (exclusive) value includes the hidden children's inclusive value.  After this sum we display in brackets the percentage of the hidden children's value in it." );

        //create an action for statically hiding subtrees with minor values
        Action* staticMinorHidingAct = new Action( tr( "   Static hiding of minor values" ), this );
        staticMinorHidingAct->setStatusTip( tr( "Statically hide nodes with minor total values" ) );
        connect( staticMinorHidingAct, SIGNAL( triggered() ), this, SLOT( onStaticMinorHiding() ) );
        hiddingMenu->addAction( staticMinorHidingAct );
        staticMinorHidingAct->setWhatsThis( "This menu item is enabled in the static hiding mode only. Here you can define the threshold for static hiding. All nodes whose current color position on the color scale is below this percentage threshold get hidden. However, in contrast to dynamic hiding, these hidings are static: Even if after some value changes the color position of a hidden node gets above the threshold, the node keeps being hidden." );

        //create an action for hiding the clicked node
        hideAct = new Action( tr( "   Hide this" ), this );
        hideAct->setStatusTip( tr( "Statically hide the clicked node" ) );
        connect( hideAct, SIGNAL( triggered() ), this, SLOT( onHideClicked() ) );
        hiddingMenu->addAction( hideAct );
        hideAct->setWhatsThis( "Enabled only if you clicked above a node in the static hiding mode. Hides the clicked node." );

        //create an action for hiding the clicked node
        showAct = new Action( tr( "   Show children of this" ), this );
        showAct->setStatusTip( tr( "Show all hidden children of the clicked node" ) );
        connect( showAct, SIGNAL( triggered() ), this, SLOT( onShowClicked() ) );
        hiddingMenu->addAction( showAct );
        showAct->setWhatsThis( "Enabled only if you clicked above a node in the static hiding mode. Shows all hidden children of the clicked node, if any." );

        //create an action for showing all hidden items
        noHidingAct = new Action( tr( "No hiding" ), this );
        noHidingAct->setCheckable( true );
        noHidingAct->setChecked( true );
        noHidingAct->setStatusTip( tr( "Swith off hiding and show all hidden items" ) );
        hiddingMenu->addAction( noHidingAct );
        noHidingAct->setWhatsThis( "Not available for metric trees. This menu item deactivates any hiding, and shows all hidden nodes." );

        dynamicMinorHidingAct->setEnabled( false );
        connect( dynamicHidingAct, SIGNAL( toggled( bool ) ), dynamicMinorHidingAct, SLOT( setEnabled( bool ) ) );

        hideAct->setEnabled( false );
        showAct->setEnabled( false );
        staticMinorHidingAct->setEnabled( false );

        connect( staticHidingAct, SIGNAL( toggled( bool ) ), hideAct, SLOT( setEnabled( bool ) ) );
        connect( staticHidingAct, SIGNAL( toggled( bool ) ), showAct, SLOT( setEnabled( bool ) ) );
        connect( staticHidingAct, SIGNAL( toggled( bool ) ), staticMinorHidingAct, SLOT( setEnabled( bool ) ) );

        QActionGroup* hideActionGroup = new QActionGroup( this );
        hideActionGroup->setExclusive( true );
        hideActionGroup->addAction( dynamicHidingAct );
        hideActionGroup->addAction( staticHidingAct );
        hideActionGroup->addAction( noHidingAct );

        connect( hideActionGroup, SIGNAL( triggered( QAction* ) ), this, SLOT( onHideAction( QAction* ) ) );
    }
    else
    {
        hideAct = NULL;
        showAct = NULL;
    }
    hidingState     = NO_HIDING;
    hidingThreshold = 0.0;

    contextMenu.addSeparator();

    //creates an action for marking all items whose name contains a user-defined string
    Action* findAct = new Action( tr( "&Find items" ), this );
    findAct->setStatusTip( tr( "Marks all visible items containing the given text" ) );
    //  findAct->setShortcut(tr("Ctrl+F"));
    connect( findAct, SIGNAL( triggered() ), this, SLOT( onFindItem() ) );
    contextMenu.addAction( findAct );
    findAct->setWhatsThis( "For all trees. Opens a dialog to get a text input from the user. If the user called the context menu over an item, the default text is the name of the clicked item, otherwise it is the last text which was searched for.\n\nThe function marks by a yellow background all non-hidden nodes whose names contain the given text, and by a light yellow background all collapsed nodes whose subtree contains such a non-hidden node. The current found node, that is initialized to the first found node, is marked by a distinguishable yellow hue." );

    //creates an action for find next
    Action* findNextAct = new Action( tr( "Find &Next" ), this );
    findNextAct->setStatusTip( tr( "Find next visible items containing the given text" ) );
    //  findNextAct->setShortcut(tr("Ctrl+N"));
    connect( findNextAct, SIGNAL( triggered() ), this, SLOT( onFindNext() ) );
    contextMenu.addAction( findNextAct );
    findNextAct->setWhatsThis( "For all trees. Changes the current found node to the next found node. If you did not start a search yet, then you are previously asked for the string to search for." );

    //removes the markings made by findAct
    Action* removeFindAct = new Action( tr( "Clear found items" ), this );
    removeFindAct->setStatusTip( tr( "Clears background of all items" ) );
    connect( removeFindAct, SIGNAL( triggered() ), this, SLOT( onUnmarkItems() ) );
    contextMenu.addAction( removeFindAct );
    removeFindAct->setWhatsThis( "For all trees. Removes the background markings of the preceding find functions." );

    contextMenu.addSeparator();

    if ( type == SYSTEMTREE )
    {
        Action* defineSubsetAct = new Action( tr( "Define subset" ), this );
        defineSubsetAct->setStatusTip( tr( "Define a named subset with all selected items" ) );
        connect( defineSubsetAct, SIGNAL( triggered() ), this, SLOT( defineSubset() ) );
        contextMenu.addAction( defineSubsetAct );

        /*
           Action * deleteSubsetAct = new Action(tr("Delete subset"),this);
           deleteSubsetAct->setStatusTip(tr("todo"));
           connect(deleteSubsetAct, SIGNAL(triggered()), this, SLOT(todo()));
           contextMenu.addAction(deleteSubsetAct);
           deleteSubsetAct->setWhatsThis("todo");
         */
    }

    contextMenu.addSeparator();

    //create an action for copying the text of the selected item to the
    //clipboard
    Action* copyToClipboardAct = new Action( tr( "Copy to clipboard" ), this );
    copyToClipboardAct->setStatusTip( tr( "Copies the selected node name to the clipboard" ) );
    connect( copyToClipboardAct, SIGNAL( triggered() ), this, SLOT( copyToClipboard() ) );
    contextMenu.addAction( copyToClipboardAct );
    copyToClipboardAct->setWhatsThis( "For all trees. Copies the text of the selected node to the clipboard." );

    //actions that occur for special tree kinds only are first set to undefined
    //and defined for those special tree kinds

    if ( type != METRICTREE )
    {
        contextMenu.addSeparator();

        //for all trees but the metric tree
        //the user may define the minimum and maximum values that should correspond
        //to the minimal and maximal color values
        Action* userAct = new Action( tr( "Min/max values" ), this );
        userAct->setStatusTip( tr( "Sets user-defined minimal/maximal values that should correspond to the color extremes." ) );
        connect( userAct, SIGNAL( triggered() ), this, SLOT( onMinMaxValues() ) );
        contextMenu.addAction( userAct );
        userAct->setWhatsThis( "Not for metric trees. Here you can activate and deactivate the application of user-defined minimal and maximal values for the color extremes, i.e., the values corresponding to the left and right end of the color legend. If you activate user-defined values for the color extremes, you are asked to define two values that should correspond to the minimal and to the maximal colors. All values outside of this interval will get the color gray. Note that canceling any of the input windows causes no changes in the coloring method. If user-defined min/max values are activated, the selected value information widget displays a ``(u)'' for ``user-defined'' behind the minimal and maximal color values." );
    }

    if ( type == CALLFLAT )
    {
        contextMenu.addSeparator();

        //flat call view items can be sorted by value or by name

        Action* sortVAct = new Action( tr( "Sort by value (descending)" ), this );
        sortVAct->setStatusTip( tr( "" ) );
        connect( sortVAct, SIGNAL( triggered() ), this, SLOT( onSortByValueDescending() ) );
        contextMenu.addAction( sortVAct );
        sortVAct->setWhatsThis( "For flat call profiles only. Sorts the nodes by their current values in descending order. Note that if an item is expanded, its exclusive value is taken for sorting, otherwise its inclusive value." );

        Action* sortNAct = new Action( tr( "Sort by name (ascending)" ), this );
        sortNAct->setStatusTip( tr( "" ) );
        connect( sortNAct, SIGNAL( triggered() ), this, SLOT( onSortByNameAscending() ) );
        contextMenu.addAction( sortNAct );
        sortNAct->setWhatsThis( "For flat call profiles only. Sorts the nodes alphabetically by name in ascending order." );
    }

#ifdef WITH_EXPERIMENTAL_VIEWS
    if ( type == CALLTREE || type == SYSTEMTREE )
    {
        //     create a sub-menu for the region of the call tree node
        QMenu* plotsMenu = contextMenu.addMenu( tr( "Plot the first selected metric over ... " ) );
        plotsMenu->setWhatsThis( "This menu allows you to plot projections in the cube on different way." );

        contextMenu.addSeparator();
        Action* plotValues1 = new Action( tr( "selected callsubtree for first selected thread as 2D " ), this );
        plotValues1->setStatusTip( tr( "Plot metric on thread over calltree" ) );
        connect( plotValues1, SIGNAL( triggered() ), this, SLOT( plotMetricSubCallnodesOverFirstThread() ) );
        plotsMenu->addAction( plotValues1 );
        plotValues1->setWhatsThis( "For flat call profiles only. Sorts the nodes alphabetically by name in ascending order." );

        Action* plotValues2 = new Action( tr( "selected threads for first selected callnode as 2D " ), this );
        plotValues2->setStatusTip( tr( "Plot metric on thread over threads" ) );
        connect( plotValues2, SIGNAL( triggered() ), this, SLOT( plotMetricCallnodeOverSelectedThreads() ) );
        plotsMenu->addAction( plotValues2 );
        plotValues2->setWhatsThis( "For flat call profiles only. Sorts the nodes alphabetically by name in ascending order." );

        plotValues2 = new Action( tr( "all threads for first selected callnode as 2D " ), this );
        plotValues2->setStatusTip( tr( "Plot metric on thread over threads" ) );
        connect( plotValues2, SIGNAL( triggered() ), this, SLOT( plotMetricCallnodeOverAllThreads() ) );
        plotsMenu->addAction( plotValues2 );
        plotValues2->setWhatsThis( "For flat call profiles only. Sorts the nodes alphabetically by name in ascending order." );


        plotsMenu->addSeparator();
        Action* plotValues3 = new Action( tr( "selected callsubtree and  selected threads  as a colormap " ), this );
        plotValues3->setStatusTip( tr( "Plot metric on thread over calltree" ) );
        connect( plotValues3, SIGNAL( triggered() ), this, SLOT( plotMetricSubCallnodesOverSelectedThreads() ) );
        plotsMenu->addAction( plotValues3 );
        plotValues3->setWhatsThis( "For flat call profiles only. Sorts the nodes alphabetically by name in ascending order." );

        Action* plotValues4 = new Action( tr( "selected callsubtree and all threads as a colormap " ), this );
        plotValues4->setStatusTip( tr( "Plot metric on thread over calltree" ) );
        connect( plotValues4, SIGNAL( triggered() ), this, SLOT( plotMetricSubCallnodesOverAllThreads() ) );
        plotsMenu->addAction( plotValues4 );
        plotValues4->setWhatsThis( "For flat call profiles only. Sorts the nodes alphabetically by name in ascending order." );
    }
#endif   /* WITH_EXPERIMENTAL_VIEWS */

    createStatisticsContextMenuItems();
}
//end of defineContextMenu()



void
TreeWidget::onHideAction( QAction* action )
{
    if ( action == dynamicHidingAct )
    {
        onDynamicHiding();
    }
    else if ( action == staticHidingAct )
    {
        staticHiding();
    }
    else
    {
        noHiding();
    }
}


/*****************************************************/
/*             mouse event handling                  */
/*****************************************************/

//
//this method defines what happens when a mouse button is pressed
//
void
TreeWidget::mousePressEvent( QMouseEvent* event )
{
    // remove dashed frame from previous clicked item
    if ( clickedItem != NULL )
    {
        clickedItem->framed = false;
    }
    //right mouse button calls the context menu
    int index = event->y() / lineHeight;
    clickedItem = getItemAtIndex( index );

    if ( event->button() == Qt::RightButton )
    {
        //check which context actions can be enabled and which must be disabled

        statisticsMousePressEvent();

        //if the user did not click over an item, then disable
        //item-specific action
        if ( clickedItem == NULL )
        {
            expandClickedAct->setEnabled( false );
            collapseClickedAct->setEnabled( false );
            if ( expandPeersAct != NULL )
            {
                expandPeersAct->setEnabled( false );
            }
            if ( collapsePeersAct != NULL )
            {
                collapsePeersAct->setEnabled( false );
            }
            expandLargestAct->setEnabled( false );

            if ( descrAct != NULL )
            {
                descrAct->setEnabled( false );
            }
            if ( urlAct != NULL )
            {
                urlAct->setEnabled( false );
            }
            if ( locationAct != NULL )
            {
                locationAct->setEnabled( false );
            }
            if ( sourceCodeAct != NULL )
            {
                sourceCodeAct->setEnabled( false );
            }
            if ( sourceCodeCalleeAct != NULL )
            {
                sourceCodeCalleeAct->setEnabled( false );
            }
            if ( locationCalleeAct != NULL )
            {
                locationCalleeAct->setEnabled( false );
            }
        }
        else
        {
            //we frame this item to denote over which item the click happened
            clickedItem->framed = true;
            repaint();

            //enable item-specific actions
            expandClickedAct->setEnabled( true );
            collapseClickedAct->setEnabled( true );
            if ( expandPeersAct != NULL )
            {
                expandPeersAct->setEnabled( true );
            }
            if ( collapsePeersAct != NULL )
            {
                collapsePeersAct->setEnabled( true );
            }
            expandLargestAct->setEnabled( true );

            if ( descrAct != NULL )
            {
                std::string descr = "";
                if ( clickedItem->type == METRICITEM )
                {
                    descr = ( ( cube::Metric* )( clickedItem->cubeObject ) )->get_descr();
                }
                else if ( clickedItem->type == CALLITEM )
                {
                    descr = ( ( cube::Cnode* )( clickedItem->cubeObject ) )->get_callee()->get_descr();
                }
                else if ( clickedItem->type == REGIONITEM && clickedItem->cubeObject != NULL )
                {
                    descr = ( ( cube::Region* )( clickedItem->cubeObject ) )->get_descr();
                }
                else if ( clickedItem->type == MACHINEITEM )
                {
                    descr = ( ( cube::Machine* )( clickedItem->cubeObject ) )->get_desc();
                }
                if ( !descr.empty() )
                {
                    descrAct->setEnabled( true );
                }
                else
                {
                    descrAct->setEnabled( false );
                }
            }

            if ( urlAct != NULL )
            {
                if ( cube->get_mirrors().size() > 0 )
                {
                    std::string url;
                    if ( clickedItem->type == METRICITEM )
                    {
                        url = ( ( cube::Metric* )( clickedItem->cubeObject ) )->get_url();
                    }
                    else if ( clickedItem->type == CALLITEM )
                    {
                        url = ( ( cube::Cnode* )( clickedItem->cubeObject ) )->get_callee()->get_url();
                    }
                    else if ( clickedItem->type == REGIONITEM && clickedItem->cubeObject != NULL )
                    {
                        url = ( ( cube::Region* )( clickedItem->cubeObject ) )->get_url();
                    }
                    if ( !url.empty() )
                    {
                        urlAct->setEnabled( true );
                    }
                    else
                    {
                        urlAct->setEnabled( false );
                    }
                }
                else
                {
                    urlAct->setEnabled( false );
                }
            }


            if ( locationAct != NULL )
            {
                if ( type == CALLFLAT && clickedItem->cubeObject == NULL )
                {
                    locationAct->setEnabled( false );
                }
                else
                {
                    locationAct->setEnabled( true );
                }
            }

            if ( locationCalleeAct != NULL )
            {
                if ( type == CALLFLAT )
                {
                    locationCalleeAct->setEnabled( false );
                }
                else
                {
                    locationCalleeAct->setEnabled( true );
                }
            }


            if ( sourceCodeAct != NULL )
            {
                std::string mod = "";
                if ( type == CALLTREE )
                {
                    mod = ( ( cube::Cnode* )( clickedItem->cubeObject ) )->get_mod();
                }
                else
                if ( type == CALLFLAT && clickedItem->cubeObject != NULL )
                {
                    mod = ( ( cube::Region* )( clickedItem->cubeObject ) )->get_mod();
                }
                if ( !mod.empty() &&
                     strcmp( mod.c_str(), "MPI" ) != 0 &&
                     strcmp( mod.c_str(), "INTERNAL" ) != 0 &&
                     strcmp( mod.c_str(), "OMP" ) != 0
                     )
                {
                    sourceCodeAct->setEnabled( true );
                }
                else
                {
                    // call site either undefined, of MPI, or INTERNAL of OMP.
                    // no source code for this case.
                    sourceCodeAct->setEnabled( false );
                    // if call site is undefined, no location have to be showed.
                    // therefore submenu "Location" gets disabled
                    if ( mod.empty() )
                    {
                        locationAct->setEnabled( false );
                    }
                }
            }


            if ( sourceCodeCalleeAct != NULL )
            {
                std::string mod = "";
                if ( type == CALLTREE )
                {
                    mod = ( ( cube::Cnode* )( clickedItem->cubeObject ) )->get_callee()->get_mod();
                }
                if ( !mod.empty() &&
                     strcmp( mod.c_str(), "MPI" ) != 0 &&
                     strcmp( mod.c_str(), "INTERNAL" ) != 0 &&
                     strcmp( mod.c_str(), "OMP" ) != 0
                     )
                {
                    sourceCodeCalleeAct->setEnabled( true );
                }
                else
                {
                    // call site either undefined, of MPI, or INTERNAL of OMP.
                    // no source code for this case.
                    sourceCodeCalleeAct->setEnabled( false );
                    // if call site is undefined, no location have to be showed.
                    // therefore submenu "Location" gets disabled
                    if ( mod.empty() )
                    {
                        locationCalleeAct->setEnabled( false );
                    }
                }
            }
        }

        //check now the parent menues

        if ( type == CALLTREE )
        {
            if ( locationAct->isEnabled() || sourceCodeAct->isEnabled() )
            {
                siteMenu->setEnabled( true );
            }
            else
            {
                siteMenu->setEnabled( false );
            }

            if ( descrAct->isEnabled() || urlAct->isEnabled() || locationCalleeAct->isEnabled() || sourceCodeCalleeAct->isEnabled() )
            {
                regionMenu->setEnabled( true );
            }
            else
            {
                regionMenu->setEnabled( false );
            }
        }

        //if no item is clicked and item-specific hiding options are enabled, then
        //disable them
        bool disabled = false;
        if ( type != METRICTREE )
        {
            assert( hideAct != NULL && showAct != NULL );
            if ( clickedItem == NULL && hideAct->isEnabled() )
            {
                assert( showAct->isEnabled() );
                disabled = true;
                hideAct->setEnabled( false );
                showAct->setEnabled( false );
            }
        }

        //execute the context menu
        contextMenu.exec( event->globalPos() );

        //remove the frame around the clicked item
        if ( clickedItem != NULL )
        {
            clickedItem->framed = false;
            repaint();
            clickedItem = NULL;
        }

        if ( disabled )
        {
            assert( hideAct != NULL && showAct != NULL );
            hideAct->setEnabled( true );
            showAct->setEnabled( true );
        }

        emit setMessage( QString( "Ready" ) );
    }
    else
    {
        // set a status line of the container widget (main widget) with the title of the selected item.
        if ( clickedItem != NULL )
        {
            std::string selection = clickedItem->getText().toStdString();
            selection.erase( 0, selection.find( " " ) + 1 ); // remove prepended value
            emit this->setMessage( QString( "Selected \"" ).append( QString::fromStdString( selection ) ).append( "\"" ) );
        }

        //other buttons should cause behaviour inherited from MyTreeWidget

        MyTreeWidget::mousePressEvent( event );
    }

    updateStatisticsSelection();
}


void
TreeWidget::keyPressEvent( QKeyEvent* event )
{
    if ( Qt::Key_C == event->key() && Qt::ControlModifier == event->modifiers() )
    {
        emit copyToClipboard();
        event->accept();
    }
    else
    {
        MyTreeWidget::keyPressEvent( event );
    }
}


/*****************************************************/
/* private slots                                     */
/*****************************************************/


/******slots for displaying descriptions and locations******/

//this slot shows a short info for the clicked item,
//the action is enabled only if the info is defined
//
void
TreeWidget::onDescr()
{
    TreeWidgetItem* item = clickedItem;
    if ( item == NULL )
    {
        return;
    }

    assert( descrAct != NULL );

    //store the description text in the string "descr";
    //we get the description from the cube object "cubeObject" of the clicked item,
    //where we have to distinguish on its type
    std::string title;
    std::string descr;
    if ( item->type == METRICITEM )
    {
        std::string   unit;
        cube::Metric* metric = ( ( cube::Metric* )( item->cubeObject ) );

        title = "Metric";
        descr = metric->get_descr();
        if ( metric->get_uom() == "sec" )
        {
            unit = "Seconds";
        }
        else if ( metric->get_uom() == "occ" )
        {
            unit = "Counts";
        }
        else if ( metric->get_uom() == "bytes" )
        {
            unit = "Bytes";
        }

        if ( !unit.empty() )
        {
            descr += "\n\nUnit: " + unit;
        }
    }
    else if ( item->type == CALLITEM )
    {
        title = "Cnode";
        descr = ( ( cube::Cnode* )( item->cubeObject ) )->get_callee()->get_descr();
    }
    else if ( item->type == REGIONITEM && item->cubeObject != NULL )
    {
        title = "Region";
        descr = ( ( cube::Region* )( item->cubeObject ) )->get_descr();
    }
    else if ( item->type == MACHINEITEM )
    {
        title = "Machine";
        descr = ( ( cube::Machine* )( item->cubeObject ) )->get_desc();
    }
    assert( !descr.empty() );

    //display the info
    MessageWidget* wwidget = new MessageWidget( Message, QString::fromStdString( descr ) );
    wwidget->show();

//   QMessageBox messageBox(QMessageBox::Information,
//                       QString::fromStdString(title) + QString(" info"),
//                       QString::fromStdString(descr),
//                       QMessageBox::Ok);
//   messageBox.exec();
}


//this slot is called for displaying the location of an item
//
void
TreeWidget::onLocation()
{
    TreeWidgetItem* item = clickedItem;
    if ( item == NULL )
    {
        return;
    }
    assert( locationAct != NULL );

    //display the location of the clicked item
    location( item, false );
}



//this slot displays the location of the clicked item's callee
//
void
TreeWidget::onLocationCallee()
{
    TreeWidgetItem* item = clickedItem;
    if ( item == NULL )
    {
        return;
    }
    assert( locationAct != NULL );

    //display the location of the clicked item's callee
    location( item, true );
}





/***************** online description *********/



//this slot displays the online description for the clicked item
//
void
TreeWidget::onUrl()
{
    openUrlOfItem( clickedItem );
}



//this slot displays the online description for the clicked item
//
void
TreeWidget::openUrlOfItem( TreeWidgetItem* clItem )
{
    if ( clItem == NULL )
    {
        return;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );
    if ( !helpBrowser )
    {
        helpBrowser = new HelpBrowser( tr( "Online description" ), this );
    }

    // Check whether HTTP access to online documentation should be disabled
    bool                                no_http = env_str2bool( getenv( "CUBE_DISABLE_HTTP_DOCS" ) );
    bool                                success = false;
    const std::vector<std::string>&     mirrors = cube->get_mirrors();
    vector<std::string>::const_iterator it      = mirrors.begin();
    while ( !success && it != mirrors.end() )
    {
        QString url = getUrl( clItem );
        url.replace( QString( "@mirror@" ), QString::fromStdString( *it ) );

        if ( no_http && ( url.startsWith( "http://" ) || url.startsWith( "https://" ) ) )
        {
            ++it;
            continue;
        }
        success = helpBrowser->showUrl( QUrl( url ) );

        ++it;
    }
    QApplication::restoreOverrideCursor();

    if ( !success )
    {
        //if there is no working url defined,
        //give a warning and return

        MessageWidget* wwidget = new MessageWidget( Warning, QString( "No accessible mirror found" ) );
        wwidget->show();

//     QMessageBox messageBox(QMessageBox::Warning,
//             QString("Online decription"),
//             QString("No accessible mirror found"),
//             QMessageBox::Ok);
//     messageBox.exec();
    }
}



/******slots used for source code display***********/


//this slot displays the source code for the clicked item in an editor;
//if the source code cannot be found, the user may chose a file to open
//
void
TreeWidget::onSourceCode()
{
    TreeWidgetItem* item = clickedItem;
    if ( item == NULL )
    {
        return;
    }
    assert( sourceCodeAct != NULL );

    //display the source code of item;
    sourceCode( item, false );
}


//this slot displays the source code for the clicked item's callee in an editor;
//if the source code cannot be found, the user may chose a file to open
//
void
TreeWidget::onSourceCodeCallee()
{
    TreeWidgetItem* item = clickedItem;
    if ( item == NULL )
    {
        return;
    }
    assert( sourceCodeCalleeAct != NULL );

    //display the source code of the item's callee
    sourceCode( item, true );
}


//this slot is called by the editor for source code;
//it stores the eventually modified source code
//
void
TreeWidget::onSaveFile()
{
    //"fileName" stores the name for the last source code opening/storing
    QFile file( fileName );

    //if the file cannot be opened for writing
    //display a warning message and return without storing
    if ( !file.open( QFile::WriteOnly | QFile::Text ) )
    {
        MessageWidget* wwidget = new MessageWidget( Warning, tr( "Cannot write file %1:\n%2." )
                                                    .arg( fileName )
                                                    .arg( file.errorString() ) );
        wwidget->show();
/*
    QMessageBox::warning(this, tr("Application"),
                         tr("Cannot write file %1:\n%2.")
                         .arg(fileName)
                         .arg(file.errorString()));*/
        return;
    }

    {
        //write source code into the opened file
        QTextStream out( &file );
        QApplication::setOverrideCursor( Qt::WaitCursor );
        out << textEdit->toPlainText();
        QApplication::restoreOverrideCursor();
        out.flush();
    }
}


//this slot is called by the editor opened for source code;
//it stores the eventually modified source code under a new name
//
void
TreeWidget::onSaveFileAs()
{
    //get the new file name by user input
    QString tmpName = QFileDialog::getSaveFileName( this );
    //tmpName can be empty, if the user canceled the input;
    //in this case just return without storing
    if ( tmpName.isEmpty() )
    {
        return;
    }
    //otherwise store the new name in the object field "fileName"
    fileName = tmpName;
    //and call the slot for storing source code
    onSaveFile();
    //update the window title of the dialog widget containing the text editor
    dialog->setWindowTitle( fileName );
}





/*** methods for hiding ***/


//show all hidden items
//
void
TreeWidget::showAll()
{
    //set all hidden nodes to non-hidden
    for ( int i = items.size() - 1; i >= 0; i-- )
    {
        if ( items[ i ]->isHidden() )
        {
            items[ i ]->setHidden( false );
        }
    }
}


//this slot sets all hidden nodes to non-hidden
//
void
TreeWidget::noHiding()
{
    hidingState = NO_HIDING;
    showAll();
    onSelection();
    displayItems();
    updateSizes();
}



void
TreeWidget::getHidingThreshold( bool& ok )
{
    double clickedValue;
    double defaultThreshold;

    //first determine a default (suggested) threshold for hiding; this
    //threshold is a percentual value: we will hide items whose colors
    //are under this percentage on the color scale

    if ( clickedItem != NULL )
    {
        //get the current value of the clicked item
        getValue( clickedItem, false, clickedValue, ok );
        if ( ok )
        {
            //take the absolute value
            if ( clickedValue < 0.0 )
            {
                clickedValue = -clickedValue;
            }

            //if the clickedValue of the current node is near by 0, then set it to 0
            if ( clickedValue <= tabWidget->getPrecisionWidget()->getRoundThreshold( FORMAT0 ) )
            {
                clickedValue = 0.0;
            }

            //get values corresponding to the minimal and maximak colors
            double min = 0.0;
            double max = 0.0;
            if ( !userDefinedMinMaxValues )
            {
                min = 0.0;
                if ( tabWidget->getValueModus() == ABSOLUTE )
                {
                    max = maxValues[ ( type == METRICTREE ? maxValueIndex[ clickedItem->id ] : 0 ) ];
                }
                else
                {
                    max = 100.0;
                }
            }
            else
            {
                min = userMinValue;
                max = userMaxValue;
            }

            //the default threshold will be the percentual position of the
            //clicked value between the min and max values
            if ( max - min == 0.0 )
            {
                defaultThreshold = 0.0;
            }
            else
            {
                defaultThreshold = 100.0 * ( clickedValue - min ) / ( max - min );
                if ( defaultThreshold < 0.0 )
                {
                    defaultThreshold = 0.0;
                }
                else if ( defaultThreshold > 100.0 )
                {
                    defaultThreshold = 100.0;
                }
                else
                {
                    defaultThreshold += 1e-17;
                }
            }
        }
        else
        {
            defaultThreshold = hidingThreshold;
        }
    }
    else
    {
        defaultThreshold = hidingThreshold;
    }

    //get a user-defined threshold, using defaultThreshold as default
    double newThreshold;
    newThreshold =
        QInputDialog::getDouble( this, "Dynamic hiding", "Dynamically hide nodes below (less or equal) this percentage on the color scale:", defaultThreshold, 0.0, 100.0, 17, &ok );
    ;
    if ( !ok )
    {
        return;
    }

    hidingThreshold = newThreshold;
}


//this slot inputs threshold for hiding subtrees below that value and
//hides those subtrees
//
void
TreeWidget::onDynamicHiding()
{
    bool ok;
    //get a user-defined hidin threshold
    getHidingThreshold( ok );
    //if the user canceled the input, re-set the checked states of
    //actions to the old values (before calling the context menu)
    if ( !ok )
    {
        if ( hidingState == STATIC_HIDING )
        {
            staticHidingAct->setChecked( true );
        }
        else
        {
            noHidingAct->setChecked( true );
        }
        return;
    }
    hidingState = DYNAMIC_HIDING;
    showAll();
    hideMinorValues();
}


//hide subtrees with a total value smaller or equal the hidingThreshold
//
void
TreeWidget::hideMinorValues()
{
    //if the widget is empty, then nothing to do
    if ( items.size() == 0 )
    {
        return;
    }

    double value = 0.0;
    bool   ok;

    for ( int i = items.size() - 1; i >= 0; i-- )
    {
        if ( items[ i ]->isHidden() )
        {
            continue;
        }

        getValue( items[ i ], false, value, ok );
        if ( ok )
        {
            //take the absolute value
            if ( value < 0.0 )
            {
                value = -value;
            }

            //if the value of the current node is near by 0
            if ( value <= tabWidget->getPrecisionWidget()->getRoundThreshold( FORMAT0 ) )
            {
                value = 0.0;
            }

            double min = 0.0;
            double max = 0.0;
            if ( !userDefinedMinMaxValues )
            {
                min = 0.0;
                if ( tabWidget->getValueModus() == ABSOLUTE )
                {
                    max = maxValues[ ( type == METRICTREE ? maxValueIndex[ items[ i ]->id ] : 0 ) ];
                }
                else
                {
                    max = 100.0;
                }
            }
            else
            {
                min = userMinValue;
                max = userMaxValue;
            }
            double percent;
            if ( max - min == 0.0 )
            {
                percent = 0.0;
            }
            else
            {
                percent = 100.0 * ( value - min ) / ( max - min );
            }
            if ( percent < 0.0 )
            {
                percent = 0.0;
            }
            else if ( percent > 100.0 )
            {
                percent = 100.0;
            }

            //we hide the item if its color is below hidingThreshold on the color scale
            if ( percent <= hidingThreshold )
            {
                //and if the node is leaf or if all of its children are hidden
                bool hide = true;
                for ( int j = 0; j < items[ i ]->childCount(); j++ )
                {
                    if ( !items[ i ]->child( j )->isHidden() )
                    {
                        hide = false;
                        break;
                    }
                }
                if ( hide )
                {
                    //then hide the current node
                    items[ i ]->setHidden( true );
                }
            }
        }
    }
    checkSelection();
    displayItems();
}
//end of hideMinorValues()


void
TreeWidget::staticHiding()
{
    hidingState = STATIC_HIDING;
}

void
TreeWidget::copyToClipboard()
{
    QString                                text;

    std::vector<TreeWidgetItem*>::iterator it    = selected.begin();
    unsigned                               index = 0; // used to catch prelast element
    while ( it != selected.end() )
    {
        text.append( ( *it )->name );
        if ( index++ < selected.size() - 1 )
        {
            text.append( "\n" );
        }
        ++it;
    }

    QApplication::clipboard()->setText( text );
}

void
TreeWidget::onHideClicked()
{
    assert( hidingState == STATIC_HIDING );
    if ( clickedItem != NULL )
    {
        if ( !clickedItem->isHidden() )
        {
            clickedItem->setHidden( true );
            checkSelection();
            displayItems();
        }
    }
}

void
TreeWidget::onShowClicked()
{
    assert( hidingState == STATIC_HIDING );
    if ( clickedItem != NULL )
    {
        for ( int i = 0; i < clickedItem->childCount(); i++ )
        {
            if ( clickedItem->child( i )->isHidden() )
            {
                clickedItem->child( i )->setHidden( false );
            }
        }
        checkSelection();
        displayItems();
    }
}

void
TreeWidget::onStaticMinorHiding()
{
    assert( hidingState == STATIC_HIDING );

    bool ok;
    getHidingThreshold( ok );
    if ( !ok )
    {
        return;
    }

    hideMinorValues();
}

/***** slots for sorting flat call tree items *****/

//this slot sorts flat call tree items by descending values
void
TreeWidget::onSortByValueDescending()
{
    sortItems( false, false );
}

//this slot sorts flat call tree items in ascending alphabetic order
void
TreeWidget::onSortByNameAscending()
{
    sortItems( true, true );
}



/***** slots for signals emitted for changes in ********/
/***** item selection or expansion/collapsing   ********/



//this slot is called when an item got deselected
//
void
TreeWidget::onDeselection()
{
    if ( selectedItems().size() > 0 )
    {
        onSelection();
    }
}


void
TreeWidget::loadMissingMetrices()
{
    if ( type == METRICTREE && driver != NULL )
    {
        //check if data loading is necessary

        bool toLoad = false;
        for ( int i = 0; !toLoad && i < ( int )selectedItems().size(); i++ )
        {
            TreeWidgetItem* item = selectedItems().at( i );
            if ( !isLoaded[ item->id ] )
            {
                toLoad = true;
            }
            else if ( item->isExpanded() )
            {
                for ( int j = 0; !toLoad && j < item->childCount(); j++ )
                {
                    if ( !isLoaded[ item->child( j )->id ] )
                    {
                        toLoad = true;
                    }
                }
            }
        }

        //if no data loading is necessary, nothing to do
        if ( !toLoad )
        {
            return;
        }

        //unload unused metrics
        for ( int i = 0; i < ( int )items.size(); i++ )
        {
            if ( !isLoaded[ i ] )
            {
                continue;
            }
            if ( items[ i ]->isSelected() )
            {
                continue;
            }
            if ( items[ i ]->parent() != NULL )
            {
                if ( items[ i ]->parent()->isSelected() && items[ i ]->parent()->isExpanded() )
                {
                    continue;
                }
            }
            ( ( cube::Metric* )( items[ i ]->cubeObject ) )->clear();
            isLoaded[ i ] = false;
        }

        //open the cube file
#ifndef CUBE_COMPRESSED
        std::ifstream in( cubeFileName.toStdString().c_str() );
#else
        gzifstream    in( cubeFileName.toStdString().c_str(), ios_base::in | ios_base::binary );
#endif
        if ( !in )
        {
            MessageWidget* wwidget = new MessageWidget( Error, QString( "Error while opening " ).arg( cubeFileName ) );
            wwidget->show();
//       std::cerr << "Error while opening " << cubeFileName.toStdString() << std::endl;
        }

        //load metrics for all selected metric items, and if they are
        //expanded, also for their children
        for ( int i = 0; i < ( int )selectedItems().size(); i++ )
        {
            TreeWidgetItem* item = selectedItems().at( i );
            try {
                if ( !isLoaded[ item->id ] )
                {
                    driver->parse_matrix( in, *cube, ( ( cube::Metric* )( item->cubeObject ) )->get_id() );
                    isLoaded[ item->id ] = true;
                }
                if ( item->isExpanded() )
                {
                    for ( int i = 0; i < item->childCount(); i++ )
                    {
                        if ( !isLoaded[ item->child( i )->id ] )
                        {
                            driver->parse_matrix( in, *cube, ( ( cube::Metric* )( item->child( i )->cubeObject ) )->get_id() );
                            isLoaded[ item->child( i )->id ] = true;
                        }
                    }
                }
            }
            catch ( cube::RuntimeError e )
            {
                MessageWidget* wwidget = new MessageWidget( Error, QString( "Parse error.  " ).arg( QString::fromStdString( e.get_msg() ) ) );
                wwidget->show();
//      QMessageBox::critical(this, "Parse error", QString::fromStdString(e.get_msg()));
                return;
            }
        }
    }
}


//this slot is called when a new item got selected
//
void
TreeWidget::onSelection()
{
    //if there is no item selected, then the tree is empty and we
    //only have to update the value widget (that gets gray)
    if ( selectedItems().size() == 0 )
    {
        updateValueWidget();
        return;
    }

    loadMissingMetrices();

    int index       = 0;
    int metricIndex = 0;
    int callIndex   = 0;
    int systemIndex = 0;

    //get the own position and the position of the metric/call/system
    //tab in the splitter widget
    getIndices( index, metricIndex, callIndex, systemIndex );

    //if this tree is not the most-right one then the tree on its right,
    //must be updated since this tree's selection has changed
    //and that has an effect on the values in the tree on the right
    if ( index < 2 )
    {
        ++index;
        TabWidget* nextTabWidget;
        if ( index == metricIndex )
        {
            nextTabWidget = tabWidget->getMetricTabWidget();
        }
        else if ( index == callIndex )
        {
            nextTabWidget = tabWidget->getCallTabWidget();
        }
        else
        {
            nextTabWidget = tabWidget->getSystemTabWidget();
        }
        if ( nextTabWidget != NULL )
        {
            nextTabWidget->computeValues();
        }
    }


    //update the value widget at the bottom of the tree
    updateValueWidget();
}

class FindDialog : public QDialog
{
public:
    FindDialog( QWidget* parent ) : QDialog( parent )
    {
        this->setWindowTitle( "Find" );
        QLabel* label = new QLabel( tr( "Regular expression to search for:" ) );
        lineEdit = new QLineEdit;
        label->setBuddy( lineEdit );

        selectCheckBox = new QCheckBox( tr( "select items" ) );
        selectCheckBox->setChecked( true );

        QPushButton* findButton = new QPushButton( tr( "&Find" ) );
        findButton->setDefault( true );

        QPushButton* cancelButton = new QPushButton( tr( "&Cancel" ) );
        cancelButton->setDefault( true );

        QDialogButtonBox* buttonBox = new QDialogButtonBox( Qt::Horizontal );
        buttonBox->addButton( findButton, QDialogButtonBox::ActionRole );
        buttonBox->addButton( cancelButton, QDialogButtonBox::ActionRole );

        QVBoxLayout* topLayout = new QVBoxLayout;
        this->setLayout( topLayout );
        topLayout->addWidget( label );
        topLayout->addWidget( lineEdit );
        topLayout->addWidget( selectCheckBox );
        topLayout->addWidget( buttonBox );

        connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
        connect( findButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    }
    QString
    getText() const
    {
        return lineEdit->text();
    }
    void
    setText( const QString &txt )
    {
        lineEdit->setText( txt );
    }
    bool
    selectItems()
    {
        return selectCheckBox->isChecked();
    }
private:
    QCheckBox* selectCheckBox;
    QLineEdit* lineEdit;
};

//this slot marks all items containing a user-defined string in their name
//
void
TreeWidget::onFindItem()
{
    FindDialog* findDialog = new FindDialog( this );
    findDialog->setText( clickedItem == NULL ? findText : clickedItem->name );
    bool        ok          = findDialog->exec();
    QString     text        = findDialog->getText();
    bool        selectItems = findDialog->selectItems();

    if ( !ok || text.isEmpty() )
    {
        return;
    }

    QRegExp regExp( text );
    if ( !regExp.isValid() )
    {
        MessageWidget* wwidget = new MessageWidget( Warning, "The entered string is not a valid regular expression!"  );
        wwidget->show();
        return;
    }

    //store the text for the next find
    findText = text;
    findItem = NULL;

    onFindNext( selectItems );
}

void
TreeWidget::onFindNext( bool selectItems )
{
    if ( findText.isEmpty() )
    {
        onFindItem();
        return;
    }

    //get the items containing the user-defined string
    const std::vector<TreeWidgetItem*> & itemList = findItems( findText );

    if ( itemList.size() > 0 )
    {
        if ( selectItems ) // select found items
        {
            int index, metricIndex, callIndex, systemIndex;
            getIndices( index, metricIndex, callIndex, systemIndex );
            if ( index == systemIndex )
            {
                TreeWidget*                             tree = tabWidget->getSystemTabWidget()->getSystemTree();
                vector<TreeWidgetItem*>                 list = tree->selectedItems();
                vector<TreeWidgetItem*>::const_iterator it;
                for ( it = list.begin(); it != list.end(); ++it )
                {
                    tree->deselect( *it );
                }
                for ( int i = 0; i < ( int )itemList.size(); i++ )
                {
                    tree->select( itemList[ i ] );
                }
            }
        }

        // todo: both? mark and select?
        markTreeItems( itemList );

        int id = ( findItem == NULL ? 0 : findItem->id + 1 );
        if ( id == ( int )items.size() )
        {
            id = 0;
        }
        while ( true )
        {
            assert( id >= 0 && id < ( int )items.size() );
            if ( items[ id ]->mark1 || ( !items[ id ]->isExpanded() && items[ id ]->mark2 ) )
            {
                TreeWidgetItem* item = items[ id ]->parent();
                while ( item != NULL )
                {
                    if ( !item->isExpanded() )
                    {
                        break;
                    }
                    item = item->parent();
                }
                if ( item == NULL )
                {
                    findItem = items[ id ];
                    break;
                }
            }
            if ( id == ( int )items.size() - 1 )
            {
                id = 0;
            }
            else
            {
                id++;
            }
        }
        ensureVisible( findItem );
    }
    else
    {
        MessageWidget* wwidget = new MessageWidget( Message, "Nothing found." );
        wwidget->show();
        findItem = NULL;
    }

    repaint();
}
//end of onFindNext()

void
TreeWidget::markTreeItems( std::vector<TreeWidgetItem*> itemList )
{
    if ( itemList.size() == 0 )
    {
        return;
    }

    //remove eventual earlier markings
    for ( unsigned i = 0; i < items.size(); i++ )
    {
        items[ i ]->mark1 = false;
        items[ i ]->mark2 = false;
    }

    std::vector<TreeWidgetItem*> v;
    for ( int i = 0; i < ( int )itemList.size(); i++ )
    {
        itemList[ i ]->mark1 = true;
        if ( itemList[ i ]->parent() != NULL )
        {
            v.push_back( itemList[ i ]->parent() );
        }
    }

    //mark all parents of the found items ;
    //this way also non-visible hits can be denoted
    for ( unsigned i = 0; i < v.size(); i++ )
    {
        v[ i ]->mark2 = true;
        if ( v[ i ]->parent() != NULL )
        {
            v.push_back( v[ i ]->parent() );
        }
    }
}

//this slot removes all markings made by onFindItems()
//
void
TreeWidget::onUnmarkItems()
{
    for ( unsigned i = 0; i < items.size(); i++ )
    {
        items[ i ]->mark1 = false;
        items[ i ]->mark2 = false;
    }
    findItem = NULL;
    repaint();
}


//this slot allows the user to activate and de-activate minimal and
//maximal values for the minimal and maximal colors;
//values outside the user-defined interval get displayed by gray
//
void
TreeWidget::onMinMaxValues()
{
    bool   ok = true;
    double minValue, maxValue;

    //ask the user if user-defined minimal and maximal values should be used
    QMessageBox::StandardButton answer =
        QMessageBox::question( this, "Min/max values", "Use user-defined minimal and maximal values for coloring?",
                               QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes );

    //if the user canceled the input then do nothing
    if ( answer == QMessageBox::Cancel )
    {
        return;
    }

    //if the user de-activated user-defined min/max values for coloring,
    //then re-display with the default coloring
    else if ( answer == QMessageBox::No )
    {
        if ( userDefinedMinMaxValues )
        {
            userDefinedMinMaxValues = false;
            updateValueWidget();
            displayItems();
        }
    }
    else
    {
        //if the user activated user-defined coloring, then ask for the minimal and maximal values
        //for the minimal and maximal colors;
        //ok gets false if the user canceled the input, in this case do nothing

        minValue = QInputDialog::getDouble( this, "Minimal value", "Minimal value for coloring:", ( userDefinedMinMaxValues ? userMinValue : 0.0 ), 0.0, 1e+15, 15, &ok );

        if ( ok )
        {
            maxValue = QInputDialog::getDouble( this, "Maximal value", "Maximal value for coloring:", ( userDefinedMinMaxValues ? userMaxValue : maxValues[ 0 ] ), 0.0, 1e+15, 15, &ok );
        }

        if ( ok )
        {
            userDefinedMinMaxValues = true;
            userMinValue            = minValue;
            userMaxValue            = maxValue;
            //re-display with the new setting
            updateValueWidget();
            displayItems();
        }
    }
}
//end of onMinMaxValues()


/****************************************************/
/*  public methods                                  */
/****************************************************/

//sets the type field
void
TreeWidget::setType( TreeWidgetType type )
{
    this->type = type;
}


//this method initializes the tree for a new cube input
//
void
TreeWidget::initialize( cube::Cube* cube )
{
    //store the input object
    this->cube = cube;
    //clear the list of items
    assert( items.size() == 0 );

    //create the tree items
    if ( type == METRICTREE )
    {
        std::vector<cube::Metric*> v = cube->get_root_metv();
        for ( int i = 0; i < ( int )v.size(); i++ )
        {
            for ( int j = 0; j < ( int )v[ i ]->num_children(); j++ )
            {
                v.push_back( v[ i ]->get_child( j ) );
            }
        }
        createItems<cube::Metric>( v, METRICITEM );
    }
    else if ( type == CALLTREE )
    {
        std::vector<cube::Cnode*> v = cube->get_root_cnodev();
        for ( int i = 0; i < ( int )v.size(); i++ )
        {
            for ( int j = 0; j < ( int )v[ i ]->num_children(); j++ )
            {
                v.push_back( v[ i ]->get_child( j ) );
            }
        }
        createItems<cube::Cnode>( v, CALLITEM );
    }
    else if ( type == CALLFLAT )
    {
        createItems<cube::Region>( cube->get_regv(), REGIONITEM );
    }
    else
    {
        std::vector<cube::Machine*> mv = cube->get_machv();
        std::vector<cube::Node*>    nv;
        std::vector<cube::Process*> pv;
        std::vector<cube::Thread*>  tv;
        for ( int i = 0; i < ( int )mv.size(); i++ )
        {
            for ( int j = 0; j < ( int )mv[ i ]->num_children(); j++ )
            {
                nv.push_back( mv[ i ]->get_child( j ) );
            }
        }
        for ( int i = 0; i < ( int )nv.size(); i++ )
        {
            for ( int j = 0; j < ( int )nv[ i ]->num_children(); j++ )
            {
                pv.push_back( nv[ i ]->get_child( j ) );
            }
        }
        for ( int i = 0; i < ( int )pv.size(); i++ )
        {
            for ( int j = 0; j < ( int )pv[ i ]->num_children(); j++ )
            {
                tv.push_back( pv[ i ]->get_child( j ) );
            }
        }

        createItems<cube::Machine>( mv, MACHINEITEM );
        createItems<cube::Node>   ( nv, NODEITEM );
        createItems<cube::Process>( pv, PROCESSITEM );
        createItems<cube::Thread> ( tv, THREADITEM );
    }

    if ( items.size() == 0 )
    {
        return;
    }

    if ( type == METRICTREE )
    {
        //in metric trees, the roots are not related, and
        //each root has its own min/max values and coloring
        maxValueIndex.resize( items.size() );
        maxValues.resize( topLevelItemCount() );
    }
    else
    {
        //in all other trees we have one coloring scheme
        maxValues.resize( 1 );
    }
    userDefinedMinMaxValues = false;

    assert( topLevelItemCount() > 0 );

    topLevelItem( 0 )->setSelected( true );

    //for call trees in the MAX aggregate mode with more than one root
    //we determine the main tree;
    // all other trees are seen as subtrees of the main tree
    //and are not involved when computing the sum of the tree node values
    //
    if ( type == CALLTREE && topLevelItemCount() > 1 && strcmp( cube->get_attr( "CUBE_CT_AGGR" ).c_str(), "MAX" ) == 0 )
    {
        markMaxItems();
    }

    updateSizes();


    //for flat call profiles we sort the items in ascending alphabetic order
    if ( type == CALLFLAT )
    {
        sortItems( true, true );
        sortedByName = true;
    }

    //the first time a source code is not found, we ask if another file should be opened;
    //after this first time, we do not ask any more, just open a file open dialog
    firstSourceNotFound = true;
}
//end of initialize()



//(re-)computes the inclusive/exclusive values for all tree items
//
void
TreeWidget::computeValues()
{
    if ( items.size() == 0 || selectedItems().size() == 0 )
    {
        //set all values of all items to 0.0
        initializeValues();
        return;
    }

    loadMissingMetrices();
    computeBasicValues();
    //probably the line below is superflous, becase the method will be
    //called at the end of computeReferenceValues(); please check :-)
    computeMaxValues();
    //compute the reference values for percentage-based value modi
    computeReferenceValues();

    //for dynamic hiding,
    //show again all items (since some can be hidden that were
    //minor-valued but now are not any more) and
    //hide the items of minor-valued subtrees
    if ( hidingState == DYNAMIC_HIDING )
    {
        showAll();
        hideMinorValues();
    }

    onSelection();
}



//this method computes the reference values for percentage-based value modi
//
void
TreeWidget::computeReferenceValues()
{
    ValueModus valueModus = tabWidget->getValueModus();

    //since we divide the absolute value by the reference value,
    //the reference value 0.0 would result in the value "undefined"
    referenceValue1 = 0.0;


    //get the proper value of the required selected, root, or external item
    if ( valueModus == METRICSELECTED )
    {
        TreeWidget* metricTreeWidget = tabWidget->getMetricTree();
        for ( int i = 0; i < ( int )metricTreeWidget->selectedItems().size(); i++ )
        {
            TreeWidgetItem* item = ( TreeWidgetItem* )( metricTreeWidget->selectedItems().at( i ) );
            if ( item->isExpanded() )
            {
                referenceValue1 += item->ownValue;
            }
            else
            {
                referenceValue1 += item->totalValue;
            }
        }
    }
    else if ( valueModus == CALLSELECTED )
    {
        TreeWidget* callTreeWidget = tabWidget->getCallTree();
        for ( int i = 0; i < ( int )callTreeWidget->selectedItems().size(); i++ )
        {
            TreeWidgetItem* item = ( TreeWidgetItem* )( callTreeWidget->selectedItems().at( i ) );
            if ( item->isExpanded() )
            {
                referenceValue1 += item->ownValue;
            }
            else
            {
                referenceValue1 += item->totalValue;
            }
        }
    }
    else if ( valueModus == SYSTEMSELECTED )
    {
        TreeWidget* systemTreeWidget = tabWidget->getSystemTree();
        for ( int i = 0; i < ( int )systemTreeWidget->selectedItems().size(); i++ )
        {
            TreeWidgetItem* item = ( TreeWidgetItem* )( systemTreeWidget->selectedItems().at( i ) );
            if ( item->isExpanded() )
            {
                referenceValue1 += item->ownValue;
            }
            else
            {
                referenceValue1 += item->totalValue;
            }
        }
    }
    else if ( valueModus == METRICROOT )
    {
        //for metrics, only items with the same root can be selected simultaneously;
        //thus we take this common root's value
        TreeWidget* metricTreeWidget = tabWidget->getMetricTree();
        if ( metricTreeWidget->selectedItems().size() > 0 )
        {
            referenceValue1 = ( ( TreeWidgetItem* )( metricTreeWidget->selectedItems().at( 0 ) ) )->rootValue;
        }
    }
    else if ( valueModus == CALLROOT )
    {
        //for the call tree, items with different roots can be selected;
        //in this case we take the sum of all roots as reference value
        TreeWidget* callTreeWidget = tabWidget->getCallTree();
        bool*       seen           = new bool[ callTreeWidget->items.size() ];
        for ( int i = 0; i < ( int )callTreeWidget->items.size(); i++ )
        {
            seen[ i ] = false;
        }
        for ( int i = 0; i < ( int )callTreeWidget->selectedItems().size(); i++ )
        {
            TreeWidgetItem* item = callTreeWidget->selectedItems().at( i );
            while ( item->parent() != NULL )
            {
                item = item->parent();
            }
            if ( !seen[ item->id ] )
            {
                referenceValue1 += item->rootValue;
                seen[ item->id ] = true;
            }
        }
        delete[] seen;
    }
    else if ( valueModus == SYSTEMROOT )
    {
        //like for call tree, we take as ref.value the sum of all roots of selected items
        TreeWidget* systemTreeWidget = tabWidget->getSystemTree();
        bool*       seen             = new bool[ systemTreeWidget->items.size() ];
        for ( int i = 0; i < ( int )systemTreeWidget->items.size(); i++ )
        {
            seen[ i ] = false;
        }
        for ( int i = 0; i < ( int )systemTreeWidget->selectedItems().size(); i++ )
        {
            TreeWidgetItem* item = systemTreeWidget->selectedItems().at( i );
            while ( item->parent() != NULL )
            {
                item = item->parent();
            }
            if ( !seen[ item->id ] )
            {
                referenceValue1 += item->rootValue;
                seen[ item->id ] = true;
            }
        }
        delete[] seen;
    }
    else if ( valueModus == EXTERNAL )
    {
        //again, for metric trees, all selected items have the same root
        //thus we take the external value for that root item
        //as reference value in external value modus

        TreeWidget* metricTreeWidget = tabWidget->getMetricTree();
        if ( metricTreeWidget->selectedItems().size() > 0 )
        {
            TreeWidgetItem* item = ( TreeWidgetItem* )( metricTreeWidget->selectedItems().at( 0 ) );
            while ( item->parent() != NULL )
            {
                item = ( TreeWidgetItem* )( item->parent() );
            }
            cube::Metric* metric     = ( cube::Metric* )( item->cubeObject );
            QString       metricName = QString::fromStdString( metric->get_uniq_name() );
            for ( int i = 0; i < ( int )( tabWidget->getMetricTabWidget()->numExternalMetricItems() ); i++ )
            {
                QString name = QString::fromStdString( tabWidget->getMetricTabWidget()->getExternalMetricName( i ) );
                if ( metricName.compare( name ) == 0 )
                {
                    referenceValue1 = tabWidget->getMetricTabWidget()->getExternalMetricItem( i )->totalValue;
                    break;
                }
            }
        }
    }

    //round values very close to 0.0 down to 0.0
    if ( referenceValue1 <= tabWidget->getPrecisionWidget()->getRoundThreshold( FORMAT0 ) &&
         referenceValue1 >= -tabWidget->getPrecisionWidget()->getRoundThreshold( FORMAT0 ) )
    {
        referenceValue1 = 0.0;
    }

    //compute minimal and maximal values for coloring and
    //for the peer percentage modus
    computeMaxValues();
}
//end of computeReferenceValues()



//compute the maximal item values
//
void
TreeWidget::computeMaxValues()
{
    double value;
    bool   first    = true;
    double maxValue = 100.0;

    //for non-metric trees just search for the minmal/maximal values
    //in the whole tree
    if ( type != METRICTREE )
    {
        for ( unsigned i = 0; i < items.size(); i++ )
        {
            getMaxValue( items[ i ], value );
            if ( first )
            {
                first    = false;
                maxValue = value;
            }
            else
            {
                // we save max absolute value (for the case of differences of cube)
                if ( fabs( maxValue ) < fabs( value ) )
                {
                    maxValue = value;
                }
            }
        }
        maxValues[ 0 ] = maxValue;
    }
    else
    {
        //for metric trees we compute the max values for each root's subtree
        std::vector<TreeWidgetItem*> subTree;

        for ( unsigned i = 0; i < ( unsigned )topLevelItemCount(); i++ )
        {
            subTree.clear();
            subTree.push_back( ( TreeWidgetItem* )( topLevelItem( i ) ) );
            maxValue = 100.0;
            first    = true;

            for ( unsigned j = 0; j < subTree.size(); j++ )
            {
                maxValueIndex[ subTree[ j ]->id ] = i;
                for ( int k = 0; k < subTree[ j ]->childCount(); k++ )
                {
                    subTree.push_back( ( TreeWidgetItem* )( subTree[ j ]->child( k ) ) );
                }
                getMaxValue( subTree[ j ], value );
                if ( first )
                {
                    first    = false;
                    maxValue = value;
                }
                else
                {
                    // we save max absolute value (for the case of differences of cube)
                    if ( fabs( maxValue ) < fabs( value ) )
                    {
                        maxValue = value;
                    }
                }
            }
            maxValues[ i ] = maxValue;
        }
    }
}
//end of computeMaxValues()




//(re-)displays all items
//
void
TreeWidget::displayItems()
{
    //the absolute values must be up-to-date upon call;
    //we recompute the reference values for the case that the modus has changed
    computeReferenceValues();

    if ( type == METRICTREE )
    {
        //in the metric tree we must determine the type of each item on its own,
        //because in different metrics we can have different types
        for ( unsigned i = 0; i < items.size(); i++ )
        {
            bool integerType = hasIntegerType( items[ i ] );
            displayItem( items[ i ], true, integerType );
            displayItem( items[ i ], false, integerType );
        }
    }
    else
    {
        //for the other trees all items have the same type
        bool integerType = hasIntegerType();
        for ( unsigned i = 0; i < items.size(); i++ )
        {
            displayItem( items[ i ], true, integerType );
            displayItem( items[ i ], false, integerType );
        }
    }

    //the variable toSort is true only if this is a flat call profile and
    //if we loaded a setting in which the items are sorted by value;
    //upon loading the setting we cannot sort yet, because the values are not yet computed;
    //thus we do so by the first display
    if ( toSort )
    {
        assert( !sortedByName );
        onSortByValueDescending();
        toSort = false;
        //since this is the first display, we select the first item;
        //note that there was already a selected item, namely the first item before sorting
        TreeWidgetItem* newSelection = topLevelItem( 0 );
        TreeWidgetItem* oldSelection = selectedItems().at( 0 );
        if ( oldSelection != newSelection )
        {
            newSelection->setSelected( true );
            oldSelection->setSelected( false );
        }
        goToTop();
    }

    if ( type == SYSTEMTREE )
    {
        vector<TreeWidgetItem*> subsetItems = getActiveSubset();
        subsetItemCount.insert( tabWidget->getSubsetName(), subsetItems.size() );

        if ( subsetItems.size() == items.size() ) // dont't mark everything
        {
            subsetItems.clear();
        }

        markTreeItems( subsetItems );
    }

    repaint();
}
//end of displayItems()

//save the current tree settings
//
void
TreeWidget::saveSettings( QSettings& settings )
{
    //create a sub-group corresponding to the tree's type
    QString groupName;
    if ( type == METRICTREE )
    {
        groupName = "metrictree";
    }
    else if ( type == CALLTREE )
    {
        groupName = "calltree";
    }
    else if ( type == CALLFLAT )
    {
        groupName = "callflat";
    }
    else if ( type == SYSTEMTREE )
    {
        groupName = "systemtree";
    }
    else
    {
        assert( false );
    }
    settings.beginGroup( groupName );

    //store the number of items and the positions of the
    //expanded items
    QList<QVariant> expandedItems;
    QVariant        numItems( ( int )items.size() );
    expandedItems.push_back( numItems );
    for ( int i = 0; i < ( int )items.size(); i++ )
    {
        if ( items[ i ]->isExpanded() )
        {
            QVariant v( i );
            expandedItems.push_back( v );
        }
    }
    settings.setValue( "expItems", expandedItems );


    //store which items are selected
    settings.setValue( "numSelectedItems", ( int )selectedItems().size() );
    for ( int i = 0; i < ( int )selected.size(); i++ )
    {
        QString name = "selectedItem";
        name.append( QString::number( i ) );
        settings.setValue( name, selected[ i ]->id );
    }

    //store if minor-valued subtrees should be displayed
    assert( hidingState == DYNAMIC_HIDING ||
            hidingState == STATIC_HIDING ||
            hidingState == NO_HIDING );
    settings.setValue( "hidingState", hidingState );
    settings.setValue( "hidingThreshold", hidingThreshold );

    //store which items are hidden
    int n = 0;
    for ( int i = 0; i < ( int )items.size(); i++ )
    {
        if ( items[ i ]->isHidden() )
        {
            QString name = "hiddenItem";
            name.append( QString::number( n ) );
            settings.setValue( name, items[ i ]->id );
            n++;
        }
    }
    settings.setValue( "numHiddenItems", n );

    //store the name of the last opened source code file
    settings.setValue( "lastSourceCodeName", lastSourceCodeName );
    //store if we sort by name or by value;
    //relevant only for flat call profile
    settings.setValue( "sortedByName", sortedByName );

    settings.setValue( "findText", findText );
    if ( findItem == NULL )
    {
        settings.setValue( "findItem", "" );
    }
    else
    {
        settings.setValue( "findItem", findItem->id );
    }

    settings.setValue( "userDefinedMinMaxValues", userDefinedMinMaxValues );
    settings.setValue( "userMinValue", userMinValue );
    settings.setValue( "userMaxValue", userMaxValue );

    assert( parent != NULL );
    assert( parent->horizontalScrollBar() != NULL );
    assert( parent->verticalScrollBar() != NULL );
    settings.setValue( "hScrollValue", parent->horizontalScrollBar()->value() );
    settings.setValue( "vScrollValue", parent->verticalScrollBar()->value() );

    settings.endGroup(); //tree
}
//end of saveSettings()


//load settings
//
bool
TreeWidget::loadSettings( QSettings& settings )
{
    //open the sub-group corresponding to the tree type
    QString groupName;
    assert( METRICTREE == 0 );
    if ( type == METRICTREE )
    {
        groupName = "metrictree";
    }
    else if ( type == CALLTREE )
    {
        groupName = "calltree";
    }
    else if ( type == CALLFLAT )
    {
        groupName = "callflat";
    }
    else if ( type == SYSTEMTREE )
    {
        groupName = "systemtree";
    }
    else
    {
        assert( false );
    }

    settings.beginGroup( groupName );

    //re-store which items should be expanded
    const QList<QVariant> expandedItems = settings.value( "expItems", QVariant( QList<QVariant>() ) ).toList();

    //expandedItems[0] should contain the number of items;
    //if this does not match with the current number of items, then we do not
    //load the item-specific settings
    bool result = ( expandedItems[ 0 ].toInt() == ( int )items.size() );

    if ( result )
    {
        //expand items as in the setting
        for ( int i = 1; i < ( int )expandedItems.size(); i++ )
        {
            int index = expandedItems[ i ].toInt();
            assert( index < ( int )items.size() );
            expandItem( items[ index ] );
        }

        //select items as in the setting
        while ( selectedItems().size() > 0 )
        {
            deselect( selectedItems().at( 0 ) );
        }
        int numSelectedItems = settings.value( "numSelectedItems", 0 ).toInt();
        for ( int i = 0; i < numSelectedItems; i++ )
        {
            QString name = "selectedItem";
            name.append( QString::number( i ) );
            select( items[ settings.value( name, 0 ).toInt() ] );
        }

        if ( type != METRICTREE )
        {
            //restore if minor-valued subtrees should be hidden

            noHiding();

            hidingState     = ( HidingState )( settings.value( "hidingState", NO_HIDING ).toUInt() );
            hidingThreshold = settings.value( "hidingThreshold", 0.0 ).toDouble();

            if ( hidingState == DYNAMIC_HIDING )
            {
                dynamicHidingAct->setChecked( true );
            }
            else if ( hidingState == STATIC_HIDING )
            {
                staticHidingAct->setChecked( true );
            }
            else if ( hidingState == NO_HIDING )
            {
                noHidingAct->setChecked( true );
            }
            else
            {
                assert( false );
            }
        }

        int numHiddenItems = settings.value( "numHiddenItems", 0 ).toInt();
        for ( int i = 0; i < numHiddenItems; i++ )
        {
            QString name = "hiddenItem";
            name.append( QString::number( i ) );
            items[ settings.value( name, 0 ).toInt() ]->setHidden( true );
        }

        //restore name of the last opene source code file
        lastSourceCodeName = settings.value( "lastSourceCodeName", "" ).toString();

        if ( type == CALLFLAT )
        {
            //for flat call profiles, restore the sorting criterium
            sortedByName = settings.value( "sortedByName", true ).toBool();
            if ( !sortedByName )
            {
                toSort = true;
            }
        }

        findText = settings.value( "findText", "" ).toString();
        findItem = NULL;
        if ( !findText.isEmpty() && !settings.value( "findItem", "" ).toString().isEmpty() )
        {
            onFindNext();
            if ( findItem != NULL )
            {
                int id      = settings.value( "findItem" ).toInt();
                int startId = findItem->id;
                while ( true )
                {
                    if ( ( int )findItem->id == id )
                    {
                        break;
                    }
                    onFindNext();
                    if ( ( int )findItem->id == startId )
                    {
                        findItem = NULL;
                        break;
                    }
                }
            }
        }

        userDefinedMinMaxValues = settings.value( "userDefinedMinMaxValues", false ).toBool();
        userMinValue            = settings.value( "userMinValue", 0.0 ).toDouble();
        userMaxValue            = settings.value( "userMaxValue", 0.0 ).toDouble();

        parent->horizontalScrollBar()->setValue( settings.value( "hScrollValue", 0 ).toInt() );
        parent->verticalScrollBar()->setValue( settings.value( "vScrollValue", 0 ).toInt() );
    }

    //close the sub-group
    settings.endGroup();

    return result;
} //end of loadSettings()


/*****************************************************/
/*    private methods                                */
/*****************************************************/

//display the location of a call or region item
//
void
TreeWidget::location( TreeWidgetItem* item, bool takeCallee )
{
    QString mod;
    int     line  = -1;
    int     line2 = -1;
    QString lineStr, line2Str;
    if ( item->type == CALLITEM )
    {
        if ( !takeCallee )
        {
            mod  = QString::fromStdString( ( ( cube::Cnode* )( item->cubeObject ) )->get_mod() );
            line = ( ( cube::Cnode* )( item->cubeObject ) )->get_line();
        }
        else
        {
            mod   = QString::fromStdString( ( ( cube::Cnode* )( item->cubeObject ) )->get_callee()->get_mod() );
            line  = ( ( cube::Cnode* )( item->cubeObject ) )->get_callee()->get_begn_ln();
            line2 = ( ( cube::Cnode* )( item->cubeObject ) )->get_callee()->get_end_ln();
        }
    }
    else if ( item->type == REGIONITEM && item->cubeObject != NULL )
    {
        mod   = QString::fromStdString( ( ( cube::Region* )( item->cubeObject ) )->get_mod() );
        line  = ( ( cube::Region* )( item->cubeObject ) )->get_begn_ln();
        line2 = ( ( cube::Region* )( item->cubeObject ) )->get_end_ln();
    }

    if ( mod.isEmpty() )
    {
        mod = "Undefined";
    }
    if ( line == -1 )
    {
        lineStr = "Undefined";
    }
    else
    {
        lineStr = QString::number( line );
    }
    if ( line2 == -1 )
    {
        line2Str = "Undefined";
    }
    else
    {
        line2Str = QString::number( line2 );
    }

    QString text;
    text.append( "\nModule:          " );
    text.append( mod );
    text.append( "\n\nStarts at line:  " );
    text.append( lineStr );
    if ( type == CALLFLAT || takeCallee )
    {
        text.append( "\n\nEnds at line:    " );
        text.append( line2Str );
    }
    text.append( "\n" );

    MessageWidget* wwidget = new MessageWidget( Message, text );
    wwidget->show();
//   QMessageBox messageBox(QMessageBox::Information,
//                       QString("Location info"),
//                       text,
//                       QMessageBox::Ok);

//   messageBox.exec();
}
//end of location()


//this method displays the source code of a call or region item
//or its callee in a dialog
//
void
TreeWidget::sourceCode( TreeWidgetItem* item, bool takeCallee )
{
    std::string stdFileName = "";
    if ( type == CALLTREE )
    {
        if ( !takeCallee )
        {
            stdFileName = ( ( cube::Cnode* )( item->cubeObject ) )->get_mod();
        }
        else
        {
            stdFileName = ( ( cube::Cnode* )( item->cubeObject ) )->get_callee()->get_mod();
        }
    }
    else if ( type == CALLFLAT && item->cubeObject != NULL )
    {
        stdFileName = ( ( cube::Region* )( item->cubeObject ) )->get_mod();
    }
    if ( strcmp( stdFileName.c_str(), "MPI" ) == 0 ||
         strcmp( stdFileName.c_str(), "INTERNAL" ) == 0 ||
         strcmp( stdFileName.c_str(), "OMP" ) == 0 )
    {
        stdFileName = "";
    }

    fileName = QString::fromStdString( stdFileName );
    QFile* file = new QFile( fileName );

    while ( !file->exists() || !file->open( QFile::ReadWrite | QFile::Text ) )
    {
        if ( firstSourceNotFound )
        {
            QString message;
            if ( !file->exists() )
            {
                message = tr( "File \" %1 \" does not exist." ).arg( fileName );
            }
            else
            {
                message = tr( "Cannot read file %1:\n%2." ).arg( fileName ).arg( file->errorString() );
            }
            message.append( "\n\nDo you want to open another file?" );
            if ( QMessageBox::question( this,
                                        tr( "Source file" ),
                                        message,
                                        QMessageBox::Open | QMessageBox::Cancel ) == QMessageBox::Cancel )
            {
                delete file;
                return;
            }
            firstSourceNotFound = false;
        }

        //take the path of the last opened source code file
        //and append the current file name to it;
        //that will be the default name for the open file dialog
        QString name = lastSourceCodeName;
        if ( name.compare( QString( "" ) ) != 0 )
        {
            int pos = name.lastIndexOf( "/" );
            if ( pos >= 0 )
            {
                name = name.left( pos );
            }
        }
        else
        {
            name = QDir::homePath();
        }

        QString name2 = fileName;
        {
            int pos = name2.lastIndexOf( "/" );
            if ( pos >= 0 )
            {
                name2 = name2.right( pos );
            }
        }

        name.append( "/" );
        name.append( name2 );

        QString caption = "";
        fileName = QFileDialog::getOpenFileName( this, caption, name );
        if ( fileName.isEmpty() )
        {
            delete file;
            return;
        }
        delete file;
        file               = new QFile( fileName );
        lastSourceCodeName = fileName;
    }

    {
        QTextStream in( file );
        textEdit = new QTextEdit();
        textEdit->setLineWrapMode( QTextEdit::NoWrap );
        textEdit->setLineWrapColumnOrWidth( 80 );

        textEdit->setFont( fontSourceCode );
        textEdit->setPlainText( in.readAll() );
        delete file;
    }




    //get the position of the item in the source code
    int line  = -1;
    int line2 = -1;
    if ( item->type == CALLITEM )
    {
        if ( !takeCallee )
        {
            line = ( ( cube::Cnode* )( item->cubeObject ) )->get_line();
        }
        else
        {
            line  = ( ( cube::Cnode* )( item->cubeObject ) )->get_callee()->get_begn_ln();
            line2 = ( ( cube::Cnode* )( item->cubeObject ) )->get_callee()->get_end_ln();
        }
    }
    else if ( item->type == REGIONITEM && item->cubeObject != NULL )
    {
        line  = ( ( cube::Region* )( item->cubeObject ) )->get_begn_ln();
        line2 = ( ( cube::Region* )( item->cubeObject ) )->get_end_ln();
    }

    //mark the start line of the item's code by a dark green background
    QList<QTextEdit::ExtraSelection> extras;

    if ( line2 != -1 )
    {
        QTextEdit::ExtraSelection highlight2;
        highlight2.format.setBackground( Qt::darkGreen );
        highlight2.cursor = textEdit->textCursor();
        highlight2.cursor.movePosition( QTextCursor::Start );
        for ( int i = 1; i < line2; i++ )
        {
            highlight2.cursor.movePosition( QTextCursor::Down );
        }
        highlight2.cursor.movePosition( QTextCursor::Down, QTextCursor::KeepAnchor );
        extras << highlight2;
    }

    //mark the final line of the item's code by a green background
    if ( line > 0 )
    {
        QTextEdit::ExtraSelection highlight;
        highlight.format.setBackground( Qt::green );
        highlight.cursor = textEdit->textCursor();
        highlight.cursor.movePosition( QTextCursor::Start );
        for ( int i = 1; i < line; i++ )
        {
            highlight.cursor.movePosition( QTextCursor::Down );
        }
        highlight.cursor.movePosition( QTextCursor::StartOfLine );
        textEdit->setTextCursor( highlight.cursor );
        highlight.cursor.movePosition( QTextCursor::Down, QTextCursor::KeepAnchor );

        extras << highlight;
    }

    textEdit->setExtraSelections( extras );


    dialog = new QDialog();
    QFontMetrics fmetric( textEdit->font() );
    int          wsize = fmetric.boundingRect( "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" ).width();
    dialog->resize( max( wsize, 500 ), 500 );

    dialog->setWindowTitle( fileName );
    QVBoxLayout* layout = new QVBoxLayout();

    //add the text editor to the dialog
    layout->addWidget( textEdit );
    textEdit->setReadOnly( true );





    //add save/save as /close buttons to the dialog
    textEditSaveButton   = new QPushButton( "Save", this );
    textEditSaveAsButton = new QPushButton( "Save as", this );
    QPushButton* closeButton = new QPushButton( "Close", this );

//    QFontDialog * fontSelection = new QFontDialog(textEdit->font(), 0);
// Select some another font
    QPushButton*  fontButton = new QPushButton( "Font...", this );

    QRadioButton* readOnlyAct = new QRadioButton( tr( "Read only" ), this );
    readOnlyAct->setChecked( true );
    onToggleReadOnly( true );
    connect( readOnlyAct, SIGNAL( toggled( bool ) ), this, SLOT( onToggleReadOnly( bool ) ) );

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget( readOnlyAct );
    buttonLayout->addWidget( textEditSaveButton );
    buttonLayout->addWidget( textEditSaveAsButton );
    buttonLayout->addWidget( fontButton );
    buttonLayout->addWidget( closeButton );

    layout->addLayout( buttonLayout );
    dialog->setLayout( layout );

    connect( textEditSaveButton, SIGNAL( clicked( bool ) ), this, SLOT( onSaveFile() ) );
    connect( textEditSaveAsButton, SIGNAL( clicked( bool ) ), this, SLOT( onSaveFileAs() ) );
    connect( closeButton, SIGNAL( clicked( bool ) ), dialog, SLOT( close() ) );
    connect( fontButton, SIGNAL( clicked( bool ) ), this, SLOT( onChangeFont() ) );


    //execute the dialog
    dialog->exec();

    disconnect( textEditSaveButton, SIGNAL( clicked( bool ) ), this, SLOT( onSaveFile() ) );
    disconnect( textEditSaveAsButton, SIGNAL( clicked( bool ) ), this, SLOT( onSaveFileAs() ) );
    disconnect( closeButton, SIGNAL( clicked( bool ) ), dialog, SLOT( close() ) );
    disconnect( fontButton, SIGNAL( clicked( bool ) ), this, SLOT( onChangeFont() ) );

    delete dialog;
}
//end of sourceCode()


void
TreeWidget::onToggleReadOnly( bool checked )
{
    if ( !checked )
    {
        textEdit->setReadOnly( false );
        textEditSaveButton->setEnabled( true );
        textEditSaveAsButton->setEnabled( true );
    }
    else
    {
        textEdit->setReadOnly( true );
        textEditSaveButton->setEnabled( false );
        textEditSaveAsButton->setEnabled( false );
    }
}


//only for call trees with the aggregate mode MAX:
//select the items of the maximal tree;
//all other trees are supposed to be subtrees of the maximal tree
//
void
TreeWidget::markMaxItems()
{
    int                               maxIndex = 0;
    double                            maxSum   = 0.0;
    double                            sum      = 0.0;

    const std::vector<cube::Metric*>& metricVec = cube->get_metv();
    const std::vector<cube::Thread*>& threadVec = cube->get_thrdv();
    std::vector<TreeWidgetItem*>      callVec;

    TreeWidgetItem*                   callItem;

    //determine the root whose tree has the largest value sum;
    //this will be the main tree
    for ( int i = 0; i < topLevelItemCount(); i++ )
    {
        callVec.clear();
        callVec.push_back( ( TreeWidgetItem* )topLevelItem( i ) );
        for ( unsigned j = 0; j < callVec.size(); j++ )
        {
            callItem = callVec[ j ];
            for ( int k = 0; k < callItem->childCount(); k++ )
            {
                callVec.push_back( ( TreeWidgetItem* )( callItem->child( k ) ) );
            }
            sum = 0.0;
            for ( unsigned k = 0; k < metricVec.size(); k++ )
            {
                for ( unsigned l = 0; l < threadVec.size(); l++ )
                {
                    sum += cube->get_sev( metricVec[ k ],
                                          ( cube::Cnode* )( callItem->cubeObject ),
                                          threadVec[ l ] );
                }
            }
        }
        if ( i == 0 || sum > maxSum )
        {
            maxSum   = sum;
            maxIndex = i;
        }
    }

    //for TreeWidgetItems, the instance variable maxItem is true by default;
    //thus set it to false in all non-max trees
    for ( int i = 0; i < topLevelItemCount(); i++ )
    {
        if ( i != maxIndex )
        {
            callVec.clear();
            callVec.push_back( ( TreeWidgetItem* )topLevelItem( i ) );
            for ( unsigned j = 0; j < callVec.size(); j++ )
            {
                callItem          = callVec[ j ];
                callItem->maxItem = false;
                for ( int k = 0; k < callItem->childCount(); k++ )
                {
                    callVec.push_back( ( TreeWidgetItem* )( callItem->child( k ) ) );
                }
            }
        }
    }
}
//end of markMaxItems()


//create tree items of different kinds
//input: vec: vector of cube objects for which we create nodes
//       type: type of the items we create
template<typename T>
void
TreeWidget::createItems( const std::vector<T*> & vec, const TreeWidgetItemType type )
{
    //for non-system trees
    //we store the created items in the vector tmpItems at the position of their cube id
    //to find the parents easily
    std::vector<TreeWidgetItem*> tmpItems;

    TreeWidgetItem*              item;

    if ( type == NODEITEM )
    {
        unsigned id;
        for ( int i = items.size() - 1; i >= 0; i-- )
        {
            if ( items[ i ]->type == MACHINEITEM )
            {
                id = ( ( cube::Vertex* )( items[ i ]->cubeObject ) )->get_id();
                while ( tmpItems.size() <= id )
                {
                    tmpItems.push_back( NULL );
                }
                tmpItems[ id ] = items[ i ];
            }
            else
            {
                break;
            }
        }
    }
    else if ( type == PROCESSITEM )
    {
        unsigned id;
        for ( int i = items.size() - 1; i >= 0; i-- )
        {
            if ( items[ i ]->type == NODEITEM )
            {
                id = ( ( cube::Vertex* )( items[ i ]->cubeObject ) )->get_id();
                while ( tmpItems.size() <= id )
                {
                    tmpItems.push_back( NULL );
                }
                tmpItems[ id ] = items[ i ];
            }
            else
            {
                break;
            }
        }
    }
    else if ( type == THREADITEM )
    {
        unsigned id;
        for ( int i = items.size() - 1; i >= 0; i-- )
        {
            if ( items[ i ]->type == PROCESSITEM )
            {
                id = ( ( cube::Vertex* )( items[ i ]->cubeObject ) )->get_id();
                while ( tmpItems.size() <= id )
                {
                    tmpItems.push_back( NULL );
                }
                tmpItems[ id ] = items[ i ];
            }
            else
            {
                break;
            }
        }
    }

    for ( unsigned i = 0; i < vec.size(); i++ )
    {
        cube::Vertex* vertex = ( cube::Vertex* )( vec[ i ] );
        assert( vertex->get_parent() == NULL || type != REGIONITEM );

        if ( type == METRICITEM )
        {
            //if the metric item is VOID, we must skip it
            cube::Metric* metric     = ( cube::Metric* )vertex;
            bool          voidValued = false;
            while ( true )
            {
                if ( std::strcmp( metric->get_val().c_str(), "VOID" ) == 0 )
                {
                    voidValued = true;
                    break;
                }
                metric = metric->get_parent();
                if ( metric == NULL )
                {
                    break;
                }
            }
            if ( voidValued )
            {
                continue;
            }
        }

        //get the name of the item
        std::string name;
        if ( type == METRICITEM )
        {
            name = ( ( cube::Metric* )vertex )->get_disp_name();
        }
        else if ( type == CALLITEM )
        {
            name = ( ( cube::Cnode* )vertex )->get_callee()->get_name();
        }
        else if ( type == REGIONITEM )
        {
            name = ( ( cube::Region* )vertex )->get_name();
        }
        else
        {
            name = ( ( cube::Sysres* )vertex )->get_name();
        }

        unsigned id = vertex->get_id();
        if ( vertex->get_parent() == NULL )
        {
            //top-level item without parent
            item = new TreeWidgetItem( ( TreeWidgetItem* )NULL );
            addTopLevelItem( item );
            assert( topLevelItemCount() > 0 );
            assert( topLevelItem( topLevelItemCount() - 1 ) == item );
        }
        else
        {
            //metric, cnode or machine items with parent
            assert( tmpItems.size() > vertex->get_parent()->get_id() );
            item = new TreeWidgetItem( tmpItems[ vertex->get_parent()->get_id() ] );
        }
        item->name.append( QString::fromStdString( name ) );
        item->type       = type;
        item->id         = items.size();
        item->cubeObject = vertex;
        items.push_back( item );

        if ( type == METRICITEM )
        {
            isLoaded.push_back( false );
        }

        if ( type == METRICITEM || type == CALLITEM || type == MACHINEITEM )
        {
            while ( tmpItems.size() <= id )
            {
                tmpItems.push_back( NULL );
            }
            tmpItems[ id ] = item;
        }

        //for flat call profile, add also a "subroutines" child items
        if ( type == REGIONITEM )
        {
            if ( hasCallee( ( cube::Region* )vertex ) )
            {
                item = new TreeWidgetItem( item );
                name = "Subroutines";
                item->name.append( QString::fromStdString( name ) );
                item->type       = type;
                item->id         = items.size();
                item->cubeObject = NULL;
                items.push_back( item );
            }
        }
    }

    //for single-threaded system trees hide the threads
    if ( type == THREADITEM )
    {
        bool singleThreaded = true;
        for ( int i = 0; i < ( int )items.size(); i++ )
        {
            if ( items[ i ]->type == THREADITEM )
            {
                if ( items[ i ]->parent() != NULL )
                {
                    if ( items[ i ]->parent()->childCount() > 1 )
                    {
                        singleThreaded = false;
                        break;
                    }
                }
                else
                {
                    singleThreaded = false;
                    break;
                }
            }
        }
        if ( singleThreaded )
        {
            int index = 0;
            for ( int i = 0; i < ( int )items.size(); i++ )
            {
                if ( items[ i ]->type == THREADITEM )
                {
                    removeItem( items[ i ] );
                    delete items[ i ];
                }
                else
                {
                    items[ index++ ] = items[ i ];
                }
            }
            items.resize( index );
        }
    }
}
//end of createItems()


//returns if the region parameter has any callee in the call tree
//called from createItems to determine if in the flat call profile
//a region item should have "subroutines" child item
//
bool
TreeWidget::hasCallee( cube::Region* region )
{
    const std::vector<cube::Cnode*>& cnodeVector = cube->get_cnodev();
    for ( unsigned i = 0; i < cnodeVector.size(); i++ )
    {
        if ( cnodeVector[ i ]->get_caller() == region )
        {
            return true;
        }
    }
    return false;
}



//determine the indices of this tree and the metric/call/system trees
//in the splitter widget and return them in the parameters
//
void
TreeWidget::getIndices( int& index, int& metricIndex, int& callIndex, int& systemIndex )
{
    WidgetType widgetType = ( ( TypedWidget* )( tabWidget->getSplitterWidget()->widget( 0 ) ) )->getType();
    if ( widgetType == METRICWIDGET )
    {
        metricIndex = 0;
    }
    else if ( widgetType == CALLWIDGET )
    {
        callIndex = 0;
    }
    else
    {
        systemIndex = 0;
    }

    widgetType = ( ( TypedWidget* )( tabWidget->getSplitterWidget()->widget( 1 ) ) )->getType();
    if ( widgetType == METRICWIDGET )
    {
        metricIndex = 1;
    }
    else if ( widgetType == CALLWIDGET )
    {
        callIndex = 1;
    }
    else
    {
        systemIndex = 1;
    }

    widgetType = ( ( TypedWidget* )( tabWidget->getSplitterWidget()->widget( 2 ) ) )->getType();
    if ( widgetType == METRICWIDGET )
    {
        metricIndex = 2;
    }
    else if ( widgetType == CALLWIDGET )
    {
        callIndex = 2;
    }
    else
    {
        systemIndex = 2;
    }

    if ( type == METRICTREE )
    {
        index = metricIndex;
    }
    else if ( type == CALLTREE || type == CALLFLAT )
    {
        index = callIndex;
    }
    else
    {
        index = systemIndex;
    }
}



//special case:
//compute the current value for a METRIC tree widget item
//if the metric tree is on LEFT
//
void
TreeWidget::computeValue( TreeWidgetItem* item )
{
    assert( item->type == METRICITEM );

    cube::Metric* metric = ( ( cube::Metric* )( item->cubeObject ) );
    if ( driver == NULL )
    {
        item->totalValue = metric->sum();
    }
    else
    {
        item->totalValue = driver->metricSum[ metric->get_id() ];
    }
    item->ownValue = item->totalValue;
    if ( item->parent() != NULL )
    {
        item->parent()->ownValue -= item->totalValue;
        item->rootValue = item->parent()->rootValue;
    }
    else
    {
        item->rootValue = item->totalValue;
    }
    for ( int i = 0; i < ( int )( item->childCount() ); i++ )
    {
        computeValue( item->child( i ) );
    }
}

//compute the value for the selection as specified by the parameters
//
void
TreeWidget::computeValue( TreeWidgetItem* metricItem, ItemState metricState,
                          TreeWidgetItem* callItem,   ItemState callState,
                          TreeWidgetItem* systemItem, ItemState systemState,
                          double& value )
{
    value = 0.0;
    double tmp;

    //analyze metric item

    if ( metricState == EXCLUSIVE )
    {
        computeValue( metricItem, INCLUSIVE, callItem, callState, systemItem, systemState, value );
        for ( int i = 0; i < metricItem->childCount(); i++ )
        {
            computeValue( metricItem->child( i ), INCLUSIVE, callItem, callState, systemItem, systemState, tmp );
            value -= tmp;
        }
        return;
    }
    else if ( metricState == EXCLUSIVE_PLUS_HIDDEN )
    {
        computeValue( metricItem, INCLUSIVE, callItem, callState, systemItem, systemState, value );
        for ( int i = 0; i < metricItem->childCount(); i++ )
        {
            if ( !metricItem->child( i )->isHidden() )
            {
                computeValue( metricItem->child( i ), INCLUSIVE, callItem, callState, systemItem, systemState, tmp );
                value -= tmp;
            }
        }
        return;
    }
    cube::Metric* metric = ( ( cube::Metric* )( metricItem->cubeObject ) );

    //analyze call item

    if ( callItem->type == REGIONITEM )
    {
        std::vector<TreeWidgetItem*>& callItems = tabWidget->getCallTree()->items;
        cube::Region*                 region    = ( cube::Region* )( callItem->cubeObject );

        if ( region == NULL )
        {
            //callItem is a "subroutines" item
            region = ( cube::Region* )( callItem->parent()->cubeObject );

            //collect in v all subroutines of cnodes with this region
            std::vector<TreeWidgetItem*> v;

            for ( unsigned i = 0; i < callItems.size(); i++ )
            {
                //if the call item's callee is the selected region
                //then take all its subroutines
                if ( ( ( cube::Cnode* )( callItems[ i ]->cubeObject ) )->get_callee() == region )
                {
                    for ( int j = 0; j < callItems[ i ]->childCount(); j++ )
                    {
                        if ( ( ( cube::Cnode* )( callItems[ i ]->child( j )->cubeObject ) )->get_callee() != region )
                        {
                            v.push_back( callItems[ i ]->child( j ) );
                        }
                    }
                }
            }
            for ( int i = 0; i < ( int )v.size(); i++ )
            {
                for ( int j = 0; j < v[ i ]->childCount(); j++ )
                {
                    if ( ( ( cube::Cnode* )( v[ i ]->child( j )->cubeObject ) )->get_callee() != region )
                    {
                        v.push_back( v[ i ]->child( j ) );
                    }
                }
            }

            for ( int i = 0; i < ( int )v.size(); i++ )
            {
                computeValue( metricItem, metricState, v[ i ], EXCLUSIVE, systemItem, systemState, tmp );
                value += tmp;
            }
        }
        else
        {
            //collect in v all cnodes with this region
            //and evtl. their children
            std::vector<TreeWidgetItem*> v;

            for ( unsigned i = 0; i < callItems.size(); i++ )
            {
                //if the call item's callee is the selected region
                //then take it
                if ( ( ( cube::Cnode* )( callItems[ i ]->cubeObject ) )->get_callee() == region )
                {
                    v.push_back( callItems[ i ] );
                }
            }

            bool takeChild = false;
            if ( callState == INCLUSIVE )
            {
                takeChild = true;
            }
            else if ( callState == EXCLUSIVE_PLUS_HIDDEN )
            {
                if ( callItem->childCount() == 1 )
                {
                    if ( callItem->child( 0 )->isHidden() )
                    {
                        takeChild = true;
                    }
                }
            }

            if ( takeChild )
            {
                for ( int i = 0; i < ( int )v.size(); i++ )
                {
                    for ( int j = 0; j < v[ i ]->childCount(); j++ )
                    {
                        if ( ( ( cube::Cnode* )( v[ i ]->child( j )->cubeObject ) )->get_callee() != region )
                        {
                            v.push_back( v[ i ]->child( j ) );
                        }
                    }
                }
            }

            for ( int i = 0; i < ( int )v.size(); i++ )
            {
                computeValue( metricItem, metricState, v[ i ], EXCLUSIVE, systemItem, systemState, tmp );
                value += tmp;
            }
        }
        return;
    }

    assert( callItem->type == CALLITEM );

    if ( callState == INCLUSIVE )
    {
        double tmp;
        computeValue( metricItem, metricState, callItem, EXCLUSIVE, systemItem, systemState, value );
        for ( int i = 0; i < callItem->childCount(); i++ )
        {
            computeValue( metricItem, metricState, callItem->child( i ), INCLUSIVE, systemItem, systemState, tmp );
            value += tmp;
        }
        return;
    }
    else if ( callState == EXCLUSIVE_PLUS_HIDDEN )
    {
        double tmp;
        computeValue( metricItem, metricState, callItem, EXCLUSIVE, systemItem, systemState, value );
        for ( int i = 0; i < callItem->childCount(); i++ )
        {
            if ( callItem->child( i )->isHidden() )
            {
                computeValue( metricItem, metricState, callItem->child( i ), INCLUSIVE, systemItem, systemState, tmp );
                value += tmp;
            }
        }
        return;
    }

    cube::Cnode* cnode = ( cube::Cnode* )( callItem->cubeObject );


    //analyze the system item

    if ( ( systemItem->type == PROCESSITEM && systemItem->childCount() > 0 ) || systemItem->type == MACHINEITEM || systemItem->type == NODEITEM )
    {
        if ( systemState == INCLUSIVE )
        {
            for ( int i = 0; i < systemItem->childCount(); i++ )
            {
                computeValue( metricItem, metricState, callItem, callState, systemItem->child( i ), INCLUSIVE, tmp );
                value += tmp;
            }
        }
        else if ( systemState == EXCLUSIVE_PLUS_HIDDEN )
        {
            for ( int i = 0; i < systemItem->childCount(); i++ )
            {
                if ( systemItem->child( i )->isHidden() )
                {
                    computeValue( metricItem, metricState, callItem, callState, systemItem->child( i ), INCLUSIVE, tmp );
                    value += tmp;
                }
            }
        }
        return;
    }

    cube::Thread* thread = NULL;

    if ( systemItem->type == PROCESSITEM )
    {
        assert( systemItem->childCount() == 0 );
        if ( ( ( cube::Process* )( systemItem->cubeObject ) )->num_children() == 1 )
        {
            thread = ( ( cube::Process* )( systemItem->cubeObject ) )->get_child( 0 );
        }
        else
        {
            return;
        }
    }
    else
    {
        assert( systemItem->type == THREADITEM );
        thread = ( cube::Thread* )( systemItem->cubeObject );
    }


    //get the value
    value = metric->get_sev( cnode, thread );
}


//compute all absolute values for this tree's items
//
void
TreeWidget::computeBasicValues()
{
    //set all values of all items to 0.0
    initializeValues();

    if ( tabWidget->getMetricTree()->items.size() == 0 ||
         tabWidget->getCallTree()->items.size() == 0 ||
         tabWidget->getSystemTree()->items.size() == 0 )
    {
        return;
    }
    if ( tabWidget->getMetricTree()->selectedItems().size() == 0 ||
         tabWidget->getCallTree()->selectedItems().size() == 0 ||
         tabWidget->getSystemTree()->selectedItems().size() == 0 )
    {
        return;
    }

    int index, metricIndex, callIndex, systemIndex;
    getIndices( index, metricIndex, callIndex, systemIndex );

    if ( metricIndex > index )
    {
        return;
    }

    //get the absolute values for all metric-call-thread combinations
    //and compute the values for this tree's items

    if ( metricIndex == 0 && type == METRICTREE )
    {
        for ( int i = 0; i < ( int )top.size(); i++ )
        {
            computeValue( top[ i ] );
        }
    }
    else
    {
        double value;

        //the third case, namely when metricIndex > index, never matches,
        //because in this case we already returned above
        const std::vector<TreeWidgetItem*> & metricItems = ( metricIndex <  index ? tabWidget->getMetricTree()->selectedItems() :
                                                             ( metricIndex == index ? tabWidget->getMetricTree()->items :
                                                               tabWidget->getMetricTree()->topLevelItems() ) );
        const std::vector<TreeWidgetItem*> & callItems = ( callIndex <    index ? tabWidget->getCurrentCallTree()->selectedItems() :
                                                           ( callIndex ==   index ? tabWidget->getCurrentCallTree()->items :
                                                             tabWidget->getCallTree()->topLevelItems() ) );
        const std::vector<TreeWidgetItem*> & systemItems = ( systemIndex <  index ? tabWidget->getSystemTree()->selectedItems() :
                                                             ( systemIndex == index ? tabWidget->getSystemTree()->items :
                                                               tabWidget->getSystemTree()->topLevelItems() ) );

        TreeWidgetItem* metricItem;
        TreeWidgetItem* callItem;
        TreeWidgetItem* systemItem;
        ItemState       metricState, callState, systemState;

        for ( int i = 0; i < ( int )metricItems.size(); i++ )
        {
            metricItem = metricItems[ i ];
            if ( metricIndex < index )
            {
                if ( metricItem->isExpanded() )
                {
                    metricState = EXCLUSIVE_PLUS_HIDDEN;
                }
                else
                {
                    metricState = INCLUSIVE;
                }
            }
            else
            {
                metricState = INCLUSIVE;
            }

            for ( int j = 0; j < ( int )callItems.size(); j++ )
            {
                callItem = callItems[ j ];

                if ( callItem->maxItem || callIndex <= index )
                {
                    if ( callIndex < index )
                    {
                        if ( !callItem->isExpanded() )
                        {
                            callState = INCLUSIVE;
                        }
                        else
                        {
                            callState = EXCLUSIVE_PLUS_HIDDEN;
                        }
                    }
                    else if ( callIndex == index )
                    {
                        callState =  EXCLUSIVE;
                    }
                    else
                    {
                        callState = INCLUSIVE;
                    }

                    for ( int k = 0; k < ( int )systemItems.size(); k++ )
                    {
                        systemItem = systemItems[ k ];
                        if ( systemIndex != index ||
                             ( systemItem->type == PROCESSITEM && systemItem->childCount() == 0 ) ||
                             systemItem->type == THREADITEM )
                        {
                            if ( systemIndex < index )
                            {
                                if ( systemItem->isExpanded() )
                                {
                                    systemState = EXCLUSIVE_PLUS_HIDDEN;
                                }
                                else
                                {
                                    systemState = INCLUSIVE;
                                }
                            }
                            else if ( systemIndex == index )
                            {
                                systemState = EXCLUSIVE;
                            }
                            else
                            {
                                systemState = INCLUSIVE;
                            }

                            computeValue( metricItem, metricState, callItem, callState, systemItem, systemState, value );

                            if ( type == METRICTREE )
                            {
                                metricItem->totalValue += value;
                            }
                            else if ( type == CALLTREE || type == CALLFLAT )
                            {
                                callItem->ownValue += value;
                            }
                            else if ( type == SYSTEMTREE )
                            {
                                systemItem->ownValue += value;
                            }
                        }
                    }
                }
            }
        }

        //for non-metric trees, compute the total/root values
        if ( type != METRICTREE )
        {
            for ( int i = items.size() - 1; i >= 0; i-- )
            {
                items[ i ]->totalValue += items[ i ]->ownValue;
                if ( items[ i ]->parent() != NULL )
                {
                    items[ i ]->parent()->totalValue += items[ i ]->totalValue;
                }
            }
        }
        else
        {
            for ( int i = items.size() - 1; i >= 0; i-- )
            {
                items[ i ]->ownValue += items[ i ]->totalValue;
                if ( items[ i ]->parent() != NULL )
                {
                    items[ i ]->parent()->ownValue -= items[ i ]->totalValue;
                }
            }
        }

        //store for each item the total value of its root
        //for the root percent value modi
        for ( int i = 0; i < ( int )items.size(); i++ )
        {
            items[ i ]->rootValue = ( items[ i ]->parent() == NULL ?
                                      items[ i ]->totalValue :
                                      ( ( TreeWidgetItem* )( items[ i ]->parent() ) )->rootValue );
        }

        //for system trees, we compute the minimal/maximal peer values for the
        //peer percent value modus
        if ( type == SYSTEMTREE )
        {
            double maxMachine = 0.0;
            double minMachine = 0.0;
            double maxNode    = 0.0;
            double minNode    = 0.0;
            double maxProcess = 0.0;
            double minProcess = 0.0;
            double maxThread  = 0.0;
            double minThread  = 0.0;
            double tmp        = 0.0;

            bool   machineSeen = false;
            bool   nodeSeen    = false;
            bool   processSeen = false;
            bool   threadSeen  = false;

            for ( unsigned i = 0; i < items.size(); i++ )
            {
                tmp = items[ i ]->totalValue;
                if ( items[ i ]->type == MACHINEITEM )
                {
                    if ( !machineSeen )
                    {
                        machineSeen = true;
                        minMachine  = tmp;
                        maxMachine  = tmp;
                    }
                    else if ( tmp > maxMachine )
                    {
                        maxMachine = tmp;
                    }
                    else if ( tmp < minMachine )
                    {
                        minMachine = tmp;
                    }
                }
                else if ( items[ i ]->type == NODEITEM )
                {
                    if ( !nodeSeen )
                    {
                        nodeSeen = true;
                        minNode  = tmp;
                        maxNode  = tmp;
                    }
                    else if ( tmp > maxNode )
                    {
                        maxNode = tmp;
                    }
                    else if ( tmp < minNode )
                    {
                        minNode = tmp;
                    }
                }
                else if ( items[ i ]->type == PROCESSITEM )
                {
                    if ( !processSeen )
                    {
                        processSeen = true;
                        minProcess  = tmp;
                        maxProcess  = tmp;
                    }
                    else if ( tmp > maxProcess )
                    {
                        maxProcess = tmp;
                    }
                    else if ( tmp < minProcess )
                    {
                        minProcess = tmp;
                    }
                }
                else if ( items[ i ]->type == THREADITEM )
                {
                    if ( !threadSeen )
                    {
                        threadSeen = true;
                        minThread  = tmp;
                        maxThread  = tmp;
                    }
                    else if ( tmp > maxThread )
                    {
                        maxThread = tmp;
                    }
                    else if ( tmp < minThread )
                    {
                        minThread = tmp;
                    }
                }
            }

            for ( unsigned i = 0; i < items.size(); i++ )
            {
                if ( items[ i ]->type == MACHINEITEM )
                {
                    items[ i ]->minValue = minMachine;
                    items[ i ]->maxValue = maxMachine;
                }
                else if ( items[ i ]->type == NODEITEM )
                {
                    items[ i ]->minValue = minNode;
                    items[ i ]->maxValue = maxNode;
                }
                else if ( items[ i ]->type == PROCESSITEM )
                {
                    items[ i ]->minValue = minProcess;
                    items[ i ]->maxValue = maxProcess;
                }
                else if ( items[ i ]->type == THREADITEM )
                {
                    items[ i ]->minValue = minThread;
                    items[ i ]->maxValue = maxThread;
                }
            }
        }
    }
}
//end of computeBasicValues()



//this method determines if the current values of this tree are of integer type;
//for metric trees it returns if the item's value is of type int (item is otherwise a dummy)
//
bool
TreeWidget::hasIntegerType( TreeWidgetItem* item )
{
    if ( tabWidget->getValueModus() != ABSOLUTE )
    {
        return false;
    }
    else
    {
        return hasAbsoluteIntegerType( item );
    }
}

bool
TreeWidget::hasAbsoluteIntegerType( TreeWidgetItem* item )
{
    int ownIndex, metricIndex, callIndex, systemIndex;
    getIndices( ownIndex, metricIndex, callIndex, systemIndex );

    if ( ownIndex > metricIndex )
    {
        //if the metric tab is on the left, then we return if the
        //selected metric has integer type
        cube::Metric* metric;
        if ( tabWidget->getMetricTree()->selectedItems().size() == 0 )
        {
            if ( cube->get_metv().size() > 0 )
            {
                metric = cube->get_metv()[ 0 ];
            }
            else
            {
                return true;
            }
        }
        else
        {
            TreeWidgetItem* selectedMetricItem =
                ( TreeWidgetItem* )( tabWidget->getMetricTree()->selectedItems().at( 0 ) );
            metric = ( ( cube::Metric* )( selectedMetricItem->cubeObject ) );
        }
        assert( metric != NULL );
        return strcmp( metric->get_dtype().c_str(), "INTEGER" ) == 0;
    }
    else if ( ownIndex < metricIndex )
    {
        //otherwise we return true if all metrics are of type integer
        const std::vector<cube::Metric*>& metricVec = cube->get_metv();
        for ( unsigned i = 0; i < metricVec.size(); i++ )
        {
            if ( strcmp( metricVec[ i ]->get_dtype().c_str(), "INTEGER" ) != 0 )
            {
                return false;
            }
        }
        return true;
    }
    else
    {
        return strcmp( ( ( cube::Metric* )( item->cubeObject ) )->get_dtype().c_str(), "INTEGER" ) == 0;
    }
}
//end of hasIntegerType()




//the methods stores the current value of item in the current value modus
//in the parameter "value" and sets "ok" to true if it is defined;
//in case the value is not defined, ok gets set to false
//
void
TreeWidget::getValue( TreeWidgetItem* item, bool expanded, double& value, bool& ok )
{
    getValue( item, expanded, value, ok, tabWidget->getValueModus() );
}

void
TreeWidget::getValue( TreeWidgetItem* item, bool expanded, double& value, bool& ok, ValueModus valueModus )
{
    int metricIndex, callIndex, systemIndex, index;
    getIndices( index, metricIndex, callIndex, systemIndex );

    //if the metric tab is on the right, then the value is undefined
    if ( index < metricIndex || ( ( type == SYSTEMTREE && expanded && item->childCount() > 0 ) ) )
    {
        ok = false;
        return;
    }

    ok = true;

    //get the absolute value
    value =  ( expanded ? item->ownValue : item->totalValue );
    double roundThreshold = tabWidget->getPrecisionWidget()->getRoundThreshold( FORMAT0 );

    //round to 0.0 if very close to it
    if ( value <= roundThreshold && value >= -roundThreshold )
    {
        value = 0.0;
    }

    //compute the value in the current modus
    if ( valueModus != ABSOLUTE )
    {
        if ( valueModus == OWNROOT )
        {
            referenceValue1 = item->rootValue;
        }
        else if ( valueModus == PEER )
        {
            assert( type == SYSTEMTREE );
            referenceValue1 = item->maxValue;
        }
        else if ( valueModus == PEERDIST )
        {
            assert( type == SYSTEMTREE );
            referenceValue1 = item->maxValue;
            referenceValue2 = item->minValue;
        }
        else if ( type == METRICTREE && valueModus == EXTERNAL )
        {
            referenceValue1 = 0.0;
            while ( item->parent() != NULL )
            {
                item = ( TreeWidgetItem* )( item->parent() );
            }
            cube::Metric* metric     = ( cube::Metric* )( item->cubeObject );
            QString       metricName = QString::fromStdString( metric->get_uniq_name() );
            for ( int i = 0; i < ( int )( tabWidget->getMetricTabWidget()->numExternalMetricItems() ); i++ )
            {
                QString name = QString::fromStdString( tabWidget->getMetricTabWidget()->getExternalMetricName( i ) );
                if ( metricName.compare( name ) == 0 )
                {
                    referenceValue1 = tabWidget->getMetricTabWidget()->getExternalMetricItem( i )->totalValue;
                    break;
                }
            }
        }

        if ( ok )
        {
            if ( valueModus != PEERDIST )
            {
                if ( referenceValue1 == 0.0 )
                {
                    if ( value != 0.0 )
                    {
                        ok = false;
                    }
                }
                else
                {
                    value = 100.0 * value / referenceValue1;
                }
            }
            else
            {
                double diff = ( referenceValue1 - referenceValue2 );
                if ( diff == 0.0 )
                {
                    if ( value - referenceValue2 != 0 )
                    {
                        ok = false;
                    }
                    else
                    {
                        value = 0.0;
                    }
                }
                else
                {
                    value = 100.0 * ( value - referenceValue2 ) / diff;
                }
            }
        }
        if ( ok )
        {
            if ( value < roundThreshold && value > -roundThreshold )
            {
                value = 0.0;
            }
        }
    }
}
//end of getValue()


//get the larger one from the item's current own and total value
//
void
TreeWidget::getMaxValue( TreeWidgetItem* item, double& value )
{
    // we compare just the absolut value. In the case of the differences of the cubes may appear negatives values.
    value = ( fabs( item->ownValue ) < fabs( item->totalValue ) ? item->totalValue : item->ownValue );
    double roundThreshold = tabWidget->getPrecisionWidget()->getRoundThreshold( FORMAT0 );
// if (abs(value)<=abs(roundThreshold) && value>=-roundThreshold)

    if ( fabs( value ) <= fabs( roundThreshold ) )
    {
        value = 0.0;
    }
}


//this method computes an item's text and color,
//in the given expansion state and with the given value type
//
void
TreeWidget::displayItem( TreeWidgetItem* item, bool expanded, bool integerType )
{
    int index       = 0;
    int metricIndex = 0;
    int callIndex   = 0;
    int systemIndex = 0;

    //get the own position and the position of the metric/call/system
    //tab in the splitter widget
    getIndices( index, metricIndex, callIndex, systemIndex );


    //we store the current value in the item's field
    double& value = ( expanded ? item->valueExpanded : item->valueCollapsed );
    QString text  = "";
    QColor& color = ( expanded ? item->colorExpanded : item->colorCollapsed );

    bool    ok = true;

    //get the value of the item in the current modus;
    //if the value is undefined, ok gets false
    value = 0.0;

    getValue( item, expanded, value, ok );

    //add up hidden children's value to the own value
    int    hiddenChildren = 0;
    double hiddenValue    = 0.0;
    double childValue;
    bool   ok2 = true;
    for ( int i = 0; i < item->childCount(); i++ )
    {
        if ( item->child( i )->isHidden() )
        {
            hiddenChildren++;
            if ( ok2 )
            {
                getValue( item->child( i ), false, childValue, ok2 );
            }
            if ( ok2 )
            {
                hiddenValue += childValue;
            }
        }
    }

    if ( expanded )
    {
        value += hiddenValue;
    }

    if ( !ok && ( !expanded || hiddenChildren == 0 || !ok2 ) )
    {
        text.append( "-" );
    }
    else
    {
        text.append( tabWidget->getPrecisionWidget()->numberToQString( value, integerType, FORMAT0 ) );
    }
    text.append( " " );
    text.append( item->name );

    //write info about hidden children's value
    if ( hiddenChildren > 0 && ( expanded || ( item->childCount() == hiddenChildren ) ) )
    {
        text.append( " (" );
        if ( ok || ( expanded && hiddenChildren > 0 && ok2 ) )
        {
            if ( value == 0.0 )
            {
                if ( hiddenValue != 0.0 )
                {
                    ok2 = false;
                }
            }
            else
            {
                hiddenValue = 100.0 * hiddenValue / value;
            }

            if ( ok2 )
            {
                text.append( tabWidget->getPrecisionWidget()->numberToQString( hiddenValue, false, FORMAT0 ) );
                text.append( "%" );
            }
            else
            {
                text.append( "-" );
            }
        }
        else
        {
            text.append( "-" );
        }
        text.append( ")" );
    }

    item->setText( expanded, text );

    //set the colors for the item
    if ( !ok && ( !expanded || hiddenChildren == 0 || !ok2 ) )
    {
        color = colorWidget->getColor( 0, 0, 0 );
    }
    else if ( !userDefinedMinMaxValues )
    {
        double min = 0.0;
        double max;
        if ( tabWidget->getValueModus() == ABSOLUTE )
        {
            max = maxValues[ ( type == METRICTREE ? maxValueIndex[ item->id ] : 0 ) ];
        }
        else
        {
            max = 100.0;
        }
        // in the case, calculated min is greater than max in its ablut value, we swap them here
        if ( fabs( min ) > fabs( max ) )
        {
            double tmp = min;
            min = max;
            max = tmp;
        }

        // coloring is only according to the absolut values of the min and max.
        color = colorWidget->getColor( fabs( value ), fabs( min ), fabs( max ) );

//	old code.
//     color = colorWidget->getColor((value<0.0 ? -value : value),min,max);
    }
    else
    {
        // coloring is only according to the absolut values of the min and max.
        color = colorWidget->getColor( fabs( value ), fabs( userMinValue ), fabs( userMaxValue ) );

//	old code
//      color = colorWidget->getColor((value<0.0 ? -value : value),userMinValue,userMaxValue);
    }
}
//end of displayItem()



//this method updates the value widget
//
void
TreeWidget::updateValueWidget()
{
    double   _sigma    = 0.0;
    double   _variance = 0.0;
    double   _mean     = 0.0;

    double   tmp = 0.;
    bool     _ok = true;

    unsigned N = 0;
    for ( int i = 0; i < ( int )items.size(); i++ )
    {
        TreeWidgetItem* item = items.at( i );
        if ( item->childCount() != 0 )
        {
            continue;
        }

        getValue( item, item->isExpanded(), tmp, _ok, ABSOLUTE );

        _mean += tmp;
        N++;
    }


    _mean = ( N != 0 ) ? _mean / N : 0.;


    for ( int i = 0; i < ( int )items.size(); i++ )
    {
        TreeWidgetItem* item = items.at( i );
        if ( item->childCount() != 0 )
        {
            continue;
        }

        getValue( item, item->isExpanded(), tmp, _ok, ABSOLUTE );

        _variance += ( _mean - tmp ) * ( _mean - tmp );
    }

    _sigma = ( N != 0 ) ? sqrt( _variance / N ) : 0.;

    if ( selectedItems().size() == 0 )
    {
        tabWidget->getValueWidget()->clear();
    }
    else
    {
        double value = 0.0;
        double tmp;
        bool   ok            = true;
        bool   wasOk         = false;
        bool   intType       = false;
        double maxValue      = 0.0;
        double absoluteValue = 0.0;

        for ( int i = 0; i < ( int )selectedItems().size(); i++ )
        {
            TreeWidgetItem* item = selectedItems().at( i );
            if ( i == 0 )
            {
                intType  = hasAbsoluteIntegerType( item );
                maxValue = maxValues[ ( type == METRICTREE ? maxValueIndex[ item->id ] : 0 ) ];
            }

            getValue( item, item->isExpanded(), tmp, ok, ABSOLUTE );
            if ( ok )
            {
                absoluteValue += tmp;
                wasOk          = true;
            }

            if ( ok && tabWidget->getValueModus() != ABSOLUTE )
            {
                getValue( item, item->isExpanded(), tmp, ok );
                if ( ok )
                {
                    value += tmp;
                    wasOk  = true;
                }
            }
        }

        if ( !wasOk )
        {
            if ( userDefinedMinMaxValues )
            {
                tabWidget->getValueWidget()->update( userMinValue, userMaxValue, intType, true, _mean, _sigma );
            }
            else
            {
                tabWidget->getValueWidget()->update( 0.0, ( tabWidget->getValueModus() == ABSOLUTE ? maxValue : 100.0 ), intType, false, _mean, _sigma );
            }
        }
        else if ( tabWidget->getValueModus() == ABSOLUTE )
        {
            if ( userDefinedMinMaxValues )
            {
                tabWidget->getValueWidget()->update( userMinValue, userMaxValue, absoluteValue, intType, true, _mean, _sigma );
            }
            else
            {
                tabWidget->getValueWidget()->update( 0.0, maxValue, absoluteValue, intType, false, _mean, _sigma );
            }
        }
        else
        {
            if ( userDefinedMinMaxValues )
            {
                tabWidget->getValueWidget()->update( userMinValue, userMaxValue, value, 0.0, maxValue, absoluteValue, intType, true, _mean, _sigma );
            }
            else
            {
                tabWidget->getValueWidget()->update( 0.0, 100.0, value, 0.0, maxValue, absoluteValue, intType, false, _mean, _sigma );
            }
        }
    }
}






// Selection of another font in source code dialog.
void
TreeWidget::onChangeFont()
{
    fontSourceCode = QFontDialog::getFont( 0, textEdit->font() );
    textEdit->setFont( fontSourceCode );
}


// returns all subtrees of the selected set of items
void
TreeWidget::getSubTrees( QVector<TreeWidgetItem*> & items, TreeWidgetItem* item )
{
    if ( item == NULL )
    {
        return;
    }
    items.append( item );
    if ( item->childCount() > 0 )
    {
        for ( int i = 0; i < item->childCount(); i++ )
        {
            getSubTrees( items, item->child( i ) );
        }
    }
}




#ifdef WITH_EXPERIMENTAL_VIEWS
// // // ----- experimental tools - 2D plots and colormaps
// // // plot some graphics

// plot selected metric and callnode ovr selected threads.
void
TreeWidget::plotMetricCallnodeOverSelectedThreads()
{
    QString                  str( "" );

    vector<TreeWidgetItem* > metricSelection = tabWidget->getMetricTree()->selectedItems();
    vector<TreeWidgetItem* > callSelection   = tabWidget->getCallTree()->selectedItems();
    vector<TreeWidgetItem* > systemSelection = tabWidget->getSystemTree()->selectedItems();

    DataFor2DPlot*           data = new DataFor2DPlot;
    for ( unsigned i = 0; i < systemSelection.size(); i++ )
    {
        cube::Thread* thread = ( cube::Thread* )( ( TreeWidgetItem* )systemSelection[ i ] )->cubeObject;

        if ( thread != NULL )
        {
            cube::Process* process = thread->get_parent();

            if ( process != NULL )
            {
                data->append(
                    LabeledTuple(
                        QString( "Proc. " ).
                        append(
                            QString::number( ( unsigned )( process->get_rank() ), 'g', 2 ) )
                        .append( ", Thrd. " ).append( QString::number( ( unsigned )( thread->get_rank() ), 'g', 2 ) ),
                        Tuple( ( double )i
                               ,
                               cube->get_sev(
                                   ( cube::Metric* )( ( TreeWidgetItem* )metricSelection[ 0 ] )->cubeObject,
                                   ( cube::Cnode* )( ( TreeWidgetItem* )callSelection[ 0 ] )->cubeObject,
                                   ( ( cube::Thread* )( ( TreeWidgetItem* )systemSelection[ i ] )->cubeObject )
                                   )
                               )
                        )
                    );
            }
        }
    }


    QWidget*        dialog = new QWidget( 0 );
    My2DPlotWidget* plot   =  new My2DPlotWidget( dialog );
    plot->setData( data );

    plot->resize( plot->sizeHint() );
    QVBoxLayout* layout = new QVBoxLayout();

    layout->addWidget( new QLabel( QString( tr( "Plot of the Metric \"" ).append(
                                                QString(
                                                    ( ( ( cube::Metric* )( ( TreeWidgetItem* )metricSelection[ 0 ] )->cubeObject )->get_disp_name() ).c_str()
                                                    ) ).append( "\"." ) ) ) );
    layout->addWidget( plot );
    layout->addWidget( new QLabel( QString( tr( "More functionality one gets in the next release of CUBE4." ) ) ) );

    QPushButton* close = new QPushButton( QString( tr( "Close plot" ) ) );

    connect( close, SIGNAL( pressed() ), dialog, SLOT( close() ) );
    layout->addWidget( close );

    dialog->setLayout( layout );
    dialog->adjustSize();
    dialog->show();
}

void
TreeWidget::plotMetricCallnodeOverAllThreads()
{
    QString                           str( "" );

    vector<TreeWidgetItem* >          metricSelection = tabWidget->getMetricTree()->selectedItems();
    vector<TreeWidgetItem* >          callSelection   = tabWidget->getCallTree()->selectedItems();

    const std::vector<cube::Thread*>& threads = cube->get_thrdv();
    DataFor2DPlot*                    data    = new DataFor2DPlot;
    for ( unsigned i = 0; i < threads.size(); i++ )
    {
        data->append(
            LabeledTuple(
                QString( "Proc. " ).append(
                    QString::number(
                        ( ( ( cube::Process* )( threads[ i ]->get_parent() ) )->get_rank() ), 'g', 3
                        ) ).append( ", Thrd. " ).append(
                    QString::number( threads[ i ]->get_rank(), 'g', 2 ) ),
                Tuple( ( double )i
                       ,
                       cube->get_sev(
                           ( cube::Metric* )( ( TreeWidgetItem* )metricSelection[ 0 ] )->cubeObject,
                           ( cube::Cnode* )( ( TreeWidgetItem* )callSelection[ 0 ] )->cubeObject,
                           threads[ i ]
                           )
                       )
                )
            );
    }
    // nonmodal window.
    QWidget*        dialog = new QWidget( 0 );
    My2DPlotWidget* plot   =  new My2DPlotWidget( dialog );
    plot->setData( data );


    plot->resize( plot->sizeHint() );
    QVBoxLayout* layout = new QVBoxLayout();

    layout->addWidget( new QLabel( QString( tr( "Plot of the Metric \"" ).append(
                                                QString(
                                                    ( ( ( cube::Metric* )( ( TreeWidgetItem* )metricSelection[ 0 ] )->cubeObject )->get_disp_name() ).c_str()
                                                    ) ).append( "\"." ) ) ) );
    layout->addWidget( plot );
    layout->addWidget( new QLabel( QString( tr( "More functionality one gets in the next release of CUBE4." ) ) ) );

    QPushButton* close = new QPushButton( QString( tr( "Close plot" ) ) );

    connect( close, SIGNAL( pressed() ), dialog, SLOT( close() ) );
    layout->addWidget( close );

    dialog->setLayout( layout );
    dialog->adjustSize();
    dialog->show();
}


void
TreeWidget::plotMetricSubCallnodesOverFirstThread()
{
    vector<TreeWidgetItem* >  metricSelection = tabWidget->getMetricTree()->selectedItems();
    vector<TreeWidgetItem* >  callSelection   = tabWidget->getCallTree()->selectedItems();
    vector<TreeWidgetItem* >  systemSelection = tabWidget->getSystemTree()->selectedItems();

    QVector<TreeWidgetItem* > cnodes;
    DataFor2DPlot*            data = new DataFor2DPlot;

    for ( unsigned i = 0; i < callSelection.size(); i++ )
    {
        getSubTrees( cnodes, callSelection[ i ] );
    }
    for ( int i = 0; i < cnodes.size(); i++ )
    {
        cube::Cnode* cnode = ( cube::Cnode* )( ( TreeWidgetItem* )cnodes[ i ] )->cubeObject;

        data->append(
            LabeledTuple(
                QString( ( ( cube::Region* )( cnode )->get_callee() )->get_name().c_str() ),
                Tuple( ( double )i
                       ,
                       cube->get_sev(
                           ( cube::Metric* )( ( TreeWidgetItem* )metricSelection[ 0 ] )->cubeObject,
                           cnode,
                           ( ( cube::Thread* )( ( TreeWidgetItem* )systemSelection[ 0 ] )->cubeObject )
                           )
                       )
                )
            );
    }


// nonmodal window.
    QWidget*        dialog = new QWidget( 0 );
    My2DPlotWidget* plot   =  new My2DPlotWidget( dialog );
    plot->setData( data );


    plot->resize( plot->sizeHint() );
    QVBoxLayout* layout = new QVBoxLayout();

    layout->addWidget( new QLabel( QString( tr( "Plot of the Metric \" " ).append(
                                                QString(
                                                    ( ( ( cube::Metric* )( ( TreeWidgetItem* )metricSelection[ 0 ] )->cubeObject )->get_disp_name() ).c_str()
                                                    ) ).append( "\"." ) ) ) );
    layout->addWidget( plot );
    layout->addWidget( new QLabel( QString( tr( "More functionality one gets in the next release of CUBE4." ) ) ) );

    QPushButton* close = new QPushButton( QString( tr( "Close plot" ) ) );

    connect( close, SIGNAL( pressed() ), dialog, SLOT( close() ) );
    layout->addWidget( close );

    dialog->setLayout( layout );
    dialog->adjustSize();
    dialog->show();
}





void
TreeWidget::plotMetricSubCallnodesOverAllThreads()
{
    QString                           str( "" );

    vector<TreeWidgetItem* >          metricSelection = tabWidget->getMetricTree()->selectedItems();
    vector<TreeWidgetItem* >          callSelection   = tabWidget->getCallTree()->selectedItems();
    const std::vector<cube::Thread*>& threads         = cube->get_thrdv();

    QVector<TreeWidgetItem* >         cnodes;

    LabeledMatrix*                    data = new LabeledMatrix;

    for ( unsigned i = 0; i < callSelection.size(); i++ )
    {
        getSubTrees( cnodes, callSelection[ i ] );
    }

    for ( unsigned ti = 0; ti < threads.size(); ti++ )
    {
        LabeledRow lrow;
        lrow.first = QString( "Proc. " ).append(
            QString::number(
                ( ( ( cube::Process* )( threads[ ti ]->get_parent() ) )->get_rank() ), 'g', 3
                )
            ).append( ", Thrd. " ).append(
            QString::number( threads[ ti ]->get_rank(), 'g', 2 ) );

        for ( int ci = 0; ci < cnodes.size(); ci++ )
        {
            cube::Cnode* cnode = ( cube::Cnode* )( ( TreeWidgetItem* )cnodes[ ci ] )->cubeObject;
            lrow.second.append(
                Triple(
                    Tuple( ( double )ci, ( double )ti  ),
                    cube->get_sev(
                        ( cube::Metric* )( ( TreeWidgetItem* )metricSelection[ 0 ] )->cubeObject,
                        cnode,
                        threads[ ti ]
                        )
                    )
                );
        }
        data->second.append( lrow );
    }
    for ( int ci = 0; ci < cnodes.size(); ci++ )
    {
        cube::Cnode* cnode = ( cube::Cnode* )( ( TreeWidgetItem* )cnodes[ ci ] )->cubeObject;
        data->first.append( QString( ( ( cube::Region* )( cnode )->get_callee() )->get_name().c_str() ) );
    }

    // nonmodal window.
    QWidget*              dialog = new QWidget( 0 );
    MyColorMapPlotWidget* plot   =  new MyColorMapPlotWidget( dialog );
    plot->setData( data );


    plot->resize( plot->sizeHint() );
    QVBoxLayout* layout = new QVBoxLayout();

    layout->addWidget( new QLabel( QString( tr( "ColorMap of the Metric \"" ).append(
                                                QString(
                                                    ( ( ( cube::Metric* )( ( TreeWidgetItem* )metricSelection[ 0 ] )->cubeObject )->get_disp_name() ).c_str()
                                                    ) ).append( "\"." ) ) ) );
    layout->addWidget( plot );
    layout->addWidget( new QLabel( QString( tr( "More functionality one gets in the next release of CUBE4." ) ) ) );

    QPushButton* close = new QPushButton( QString( tr( "Close plot" ) ) );

    connect( close, SIGNAL( pressed() ), dialog, SLOT( close() ) );
    layout->addWidget( close );
    dialog->setLayout( layout );
    dialog->adjustSize();
    dialog->show();
}








void
TreeWidget::plotMetricSubCallnodesOverSelectedThreads()
{
    vector<TreeWidgetItem* >  metricSelection = tabWidget->getMetricTree()->selectedItems();
    vector<TreeWidgetItem* >  callSelection   = tabWidget->getCallTree()->selectedItems();
    vector<TreeWidgetItem* >  systemSelection = tabWidget->getSystemTree()->selectedItems();

    QVector<TreeWidgetItem* > cnodes;

    LabeledMatrix*            data = new LabeledMatrix;
    for ( unsigned i = 0; i < callSelection.size(); i++ )
    {
        getSubTrees( cnodes, callSelection[ i ] );
    }

    for ( unsigned ti = 0; ti < systemSelection.size(); ti++ )
    {
        cube::Thread* thread = ( cube::Thread* )( ( ( TreeWidgetItem* )systemSelection[ ti ] )->cubeObject );
        if ( thread->get_parent() != NULL )
        {
            LabeledRow lrow;
            lrow.first = QString( "Proc. " ).append(
                QString::number(
                    ( ( ( cube::Process* )( thread->get_parent() ) )->get_rank() ), 'g', 3
                    )
                ).append( ", Thrd. " ).append(
                QString::number( thread->get_rank(), 'g', 2 ) );
            for ( int ci = 0; ci < cnodes.size(); ci++ )
            {
                cube::Cnode* cnode = ( cube::Cnode* )( ( TreeWidgetItem* )cnodes[ ci ] )->cubeObject;
                lrow.second.append(
                    Triple(
                        Tuple( ( double )ci, ( double )ti  ),
                        cube->get_sev(
                            ( cube::Metric* )( ( TreeWidgetItem* )metricSelection[ 0 ] )->cubeObject,
                            cnode,
                            thread
                            )
                        )
                    );
            }
            data->second.append( lrow );
        }
    }
    for ( int ci = 0; ci < cnodes.size(); ci++ )
    {
        cube::Cnode* cnode = ( cube::Cnode* )( ( TreeWidgetItem* )cnodes[ ci ] )->cubeObject;
        data->first.append( QString( ( ( cube::Region* )( cnode )->get_callee() )->get_name().c_str() ) );
    }

    // nonmodal window.
    QWidget*              dialog = new QWidget( 0 );
    MyColorMapPlotWidget* plot   =  new MyColorMapPlotWidget( dialog );
    plot->setData( data );

    plot->resize( plot->sizeHint() );
    QVBoxLayout* layout = new QVBoxLayout();

    layout->addWidget( new QLabel( QString( tr( "ColorMap of the Metric \"" ).append(
                                                QString(
                                                    ( ( ( cube::Metric* )( ( TreeWidgetItem* )metricSelection[ 0 ] )->cubeObject )->get_disp_name() ).c_str()
                                                    ) ).append( "\"." ) ) ) );
    layout->addWidget( plot );
    layout->addWidget( new QLabel( QString( tr( "More functionality one gets in the next release of CUBE4." ) ) ) );

    QPushButton* close = new QPushButton( QString( tr( "Close plot" ) ) );

    connect( close, SIGNAL( pressed() ), dialog, SLOT( close() ) );
    layout->addWidget( close );

    dialog->setLayout( layout );
    dialog->adjustSize();
    dialog->show();
}

#endif   /* WITH_EXPERIMENTAL_VIEWS */

/**
 * add all selected items to a subset
 */
void
TreeWidget::defineSubset()
{
    const vector<TreeWidgetItem*> &items = selectedItems();
    if ( items.size() < 3 )
    {
        MessageWidget* wwidget = new MessageWidget( Warning, "At least 3 selected items are necessary to define a subset"  );
        wwidget->show();
        return;
    }

    //ok gets false if the user canceled the input
    bool    ok;
    QString name = QInputDialog::getText( this, tr( "Find" ),
                                          tr( "Define named subset" ), QLineEdit::Normal,
                                          findText, &ok );

    if ( name.length() > 0 )
    {
        definedSubsets.insert( name, items );
        emit definedSubsetsChanged( name );
    }
}

/**
 * @brief TreeWidget::getActiveSubset
 * @return the TreeWidgetItems which belong the the currently selected subset
 */
std::vector<TreeWidgetItem*>
TreeWidget::getActiveSubset()
{
    QString name = tabWidget->getSubsetName();
    if ( name == "All" )
    {
        return items;
    }
    else if ( name == "Visited" )
    {
        return getVisitedSubset();
    }
    else
    {
        return definedSubsets.value( name );
    }
}

int
TreeWidget::getSubsetItemCount( const QString &subsetName )
{
    int count = 0;
    if ( subsetName == "Visited" )
    {
        vector<TreeWidgetItem*> subsetItems = getVisitedSubset(); // todo: avoid unnecessary calls
        count = subsetItems.size();
    }
    else if ( subsetName == "All" )
    {
        for ( unsigned i = 0; i < items.size(); ++i )
        {
            if ( ( items[ i ]->type == PROCESSITEM && items[ i ]->childCount() == 0 )
                 || ( items[ i ]->type == THREADITEM ) )
            {
                count++;
            }
        }
    }
    return count;
}

/**
 * @brief TreeWidget::getVisitedSubset
 * @return predefined subset: all thread with visited > 0
 */
std::vector<TreeWidgetItem*>
TreeWidget::getVisitedSubset()
{
    std::vector<TreeWidgetItem*> visited;
    double                       val;
    double                       sum;
    TreeWidget*                  callTree   = tabWidget->getCurrentCallTree();
    TreeWidget*                  systemTree = tabWidget->getSystemTree();
    TreeWidgetItem*              visit      =  tabWidget->getMetricTree()->findItems( "Visit" ).at( 0 );

    for ( unsigned i = 0; i < systemTree->items.size(); ++i )
    {
        if ( ( systemTree->items[ i ]->type == PROCESSITEM && systemTree->items[ i ]->childCount() == 0 )
             || ( systemTree->items[ i ]->type == THREADITEM ) )
        {
            sum = 0;
            for ( unsigned j = 0; j < callTree->selectedItems().size(); ++j )
            {
                TreeWidgetItem* callItem = callTree->selectedItems().at( j );

                ItemState       callTreeState; // todo: EXCLUSIVE_PLUS_HIDDEN?
                if ( !callItem->isExpanded() )
                {
                    callTreeState = INCLUSIVE;
                }
                else
                {
                    callTreeState =  EXCLUSIVE;
                }

                systemTree->computeValue( visit, INCLUSIVE,
                                          callItem, callTreeState,
                                          systemTree->items[ i ], INCLUSIVE, val );
                sum += val;
            }
            if ( sum > 0 )
            {
                visited.push_back( systemTree->items[ i ] );
            }
        }
    }
    return visited;
}
