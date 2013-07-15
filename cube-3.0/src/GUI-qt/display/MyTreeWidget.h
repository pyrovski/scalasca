/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _MYTREEWIDGET_H
#define _MYTREEWIDGET_H

#include "TabWidget.h"
#include "ScrollArea.h"

class TreeWidgetItem;


//MyTreeWidget is the basic class for trees;
//TreeWidget inherits MyTreeWidget and extends it with value computations


class MyTreeWidget : public QWidget
{
    Q_OBJECT
signals:
    void
    selectionChanged();
    void
    collapsedChanged();

protected slots:

    //expand all tree items
    void
    expandAll();

    //collapse all tree items
    void
    collapseAll();

    //expand all items in the subtree of the item clicked with the mouse
    void
    expandClicked();

    //collapse all items in the subtree of the item clicked with the mouse
    void
    collapseClicked();

    //expands all peers, i.e., items of the same type,
    //in a system tree
    void
    expandPeers();

    //collapses all peers, i.e., items of the same type,
    //in a system tree
    void
    collapsePeers();

    //starting from the clicked item, this slot recursively
    //expands the largest child and the largest child of the largest child etc.
    //until a leaf is reached
    void
    expandLargest();


public:

    MyTreeWidget( ScrollArea* parent );


    /********************** set methods ************************/


    //set the tab widget the tree is inside of
    void
    setTabWidget( TabWidget* tabWidget );

    //set the spacing between the line in the tree display
    void
    setSpacing( int spacing );


    /******************** get methods ***********************/


    //get list of selected items
    std::vector<TreeWidgetItem*>
    selectedItems();

    //get list of root items
    std::vector<TreeWidgetItem*>
    topLevelItems();

    //get the tab widget the tree is inside of
    TabWidget*
    getTabWidget();

    /*************** miscellaneous ****************/

    //compute the necessary size for the tree and rescale the scroll widget
    //accordingly
    void
    updateSizes();

    //return the optimal width for displaying the tree
    int
    getOptimalWidth();

    //ensure that an item is in the visible rectangle of the scroll widget
    void
    ensureVisible( TreeWidgetItem* item );

    //select an item
    void
    select( TreeWidgetItem* item );

    //deselect an item
    void
    deselect( TreeWidgetItem* item,
              bool            toUpdate = true );

protected:


    /************** protected fields *****************/

    //the type METRICTREE, CALLTREE, CALLFLAT, or SYSTEMTREE of the tree widget
    TreeWidgetType type;

    //the parent scroll area
    ScrollArea* parent;

    //the tab widget the tree is inside of
    TabWidget* tabWidget;

    //vector of all nodes of the tree
    std::vector<TreeWidgetItem*> items;

    //vector of selected items
    std::vector<TreeWidgetItem*> selected;

    //vector of root items
    std::vector<TreeWidgetItem*> top;

    //compute the list of items containing the given text string
    std::vector<TreeWidgetItem*>
    findItems( QString text );

    //for tree search, the current find item (an element from the
    //findItems vector)
    TreeWidgetItem* findItem;

    //for the context menu store over which tree item the user clicked;
    //NULL if no item
    TreeWidgetItem* clickedItem;

    //height of tree lines
    int lineHeight;

    //size of icons of tree items
    int iconSize;

    //spacing between lines of the tree display
    int spacing;

    //indent0: left margin
    //indent1: indention per item depth
    //indent2: space left and right from the icon
    //indent3: 2*indent3 is the width of the expansion rectangle
    int indent0, indent1, indent2, indent3;
    int minIndex, maxIndex;


    /************** protected methods *******************/

    //initialize all tree item values to 0
    void
    initializeValues();

    //return number of root items
    int
    topLevelItemCount();

    //return the index-th root item
    TreeWidgetItem*
    topLevelItem( int index );

    //add a new root item; adding new non-root items happen just by
    //creating a new item with a parent node already in one of the trees
    bool
    addTopLevelItem( TreeWidgetItem* item );

    //remove an item from the tree
    void
    removeItem( TreeWidgetItem* item );

    //expand/collapse a tree item
    void
    expandItem( TreeWidgetItem* item );
    void
    collapseItem( TreeWidgetItem* item );
    void
    setVisibleFlagToSelfAndChildren( TreeWidgetItem* item );
    void
    setInvisibleFlagToChildren( TreeWidgetItem* item );


    //these methods are implemented in the TreeWidget class;
    //they can define additional computations upon selection change
    virtual void
    onSelection() = 0;
    virtual void
    onDeselection() = 0;

    //check if all selected items are visible;
    //for example, if the user collapses a tree item, and there
    //is a selected node in that subtree, then that seleted node would not be visible any more
    void
    checkSelection();

    //sort items by name or by value, ascending or descending
    void
    sortItems( bool sortByName,
               bool ascending );

    //scroll to the top of the tree
    void
    goToTop();

    //compute the width and height of the painted tree
    void
    computeSizes( int&w,
                  int&h );

    //the main painting method
    void
    paintEvent( QPaintEvent* );

    //paint an item
    void
    paintItem( QPainter&       painter,
               TreeWidgetItem* item,
               int             lastIndex,
               bool&           lastExpandable,
               int&            index );

    //return the index-th painted item
    TreeWidgetItem*
    getItemAtIndex( int index );

    //return the item painted below the parameter item
    TreeWidgetItem*
    getNextItem( TreeWidgetItem* item );

    //return the item painted above the parameter item
    TreeWidgetItem*
    getPreviousItem( TreeWidgetItem* item );

    //mouse event handling
    void
    mousePressEvent( QMouseEvent* event );

    //key event handling
    void
    keyPressEvent( QKeyEvent* event );


//  //mouse event handling
//   void mouseMoveEvent(QMouseEvent *event);

// general event handler
// used for tooltip handling
    bool
    event( QEvent* event );

//For handling tooltips position of the mouse pointer will be tracked and saved.
//Used in  ::event(...) to define treeitem under mouse pointer
    QPoint MouseOver;
};

#endif
