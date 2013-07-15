/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _TREEWIDGETITEM_H
#define _TREEWIDGETITEM_H

#include <QTreeWidgetItem>
#include <QBrush>

#include "Vertex.h"

#include "Constants.h"

class MyTreeWidget;

//TreeWidgetItem is the type of tree nodes


class TreeWidgetItem
{
public:

    TreeWidgetItem( TreeWidgetItem* parent );

    ~TreeWidgetItem();

    /************* fields *************/

    //name of the item
    QString name;

    //values attached to the item
    double ownValue;
    double totalValue;
    double rootValue;

    //the minimal and maximal peer values for system tree items
    double minValue;
    double maxValue;

    //item values/colors for the current state; note that valueExpanded
    //can differ from ownValue if the item has hidden children, in which
    //case the total values of the hidden children are added up to the
    //own value
    double valueExpanded, valueCollapsed;
    QColor colorExpanded, colorCollapsed;

    //this flag is used in the aggregate mode "MAX"
    //to mark if the node belongs to the maximal cnode tree
    bool maxItem;

    //the index of the node in the item vector
    //of the treewidget object it is contained in
    unsigned id;

    //the type of the node;
    //needed to distinguish how to handle nodes
    TreeWidgetItemType type;

    //the corresponding cube object
    cube::Vertex* cubeObject;

    //should the item be framed?
    bool framed;
    //flag for most severe instances
    bool statistic_available_frame;

    // indicates, that parent is expanded
    bool visible;
    void
    setVisible()
    {
        visible = true;
    }
    void
    setInvisible()
    {
        visible = false;
    }
    bool
    isVisible()
    {
        return visible;
    }

    //node found by "find items"?
    bool mark1;
    //is there a node in this subtree found by "find items"?
    bool mark2;

    /************* set methods **************/

    //set the item's displayed text (name, value, etc.) for the "expanded" state
    void
    setText( bool    expanded,
             QString text );

    //set and get expansion state
    void
    setExpanded( bool expanded );

    //set and get hidden state
    void
    setHidden( bool hidden );

    //set and get selected state
    void
    setSelected( bool selected,
                 bool updateTree = true );

    //set the tree widget (if any) the item is contained in
    void
    setTree( MyTreeWidget* tree );

    //set parent item
    void
    setParent( TreeWidgetItem* item );

    /********************** get methods ********************/

    //get the item's hierarchy depth
    int
    getDepth();

    //get the text to be displayed in the current expansion mode
    QString
    getText();

    //get the text to be displayed in the expansion mode "expanded"
    QString
    getText( bool expanded );

    //get color for the current value of the item
    QColor
    getColor();

    //get current value of the item
    double
    getValue();

    //get the tree widget (if any) the item is contained in
    MyTreeWidget*
    getTree();

    //get parent item
    TreeWidgetItem*
    parent();

    //number of children
    int
    childCount();

    //return the ith child
    TreeWidgetItem*
    child( int index );

    //is the item expanded/hidden/selected?
    bool
    isExpanded();
    bool
    isHidden();
    bool
    isSelected();

    //set and clear statistics flags
    void
    setStatisticFlag();
    void
    clearStatisticFlag();

    /********** add/remove childen *********/

    //remove a child
    void
    removeChild( TreeWidgetItem* item );

    //add a new child
    void
    addChild( TreeWidgetItem* item );


private:

    bool                         expanded;
    bool                         hidden;
    bool                         selected;

    TreeWidgetItem*              parentItem;
    std::vector<TreeWidgetItem*> children;

    int                          depth;
    MyTreeWidget*                tree;

    QString                      textExpanded, textCollapsed;
};

#endif
