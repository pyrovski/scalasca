/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cassert>

#include "TreeWidgetItem.h"
#include "MyTreeWidget.h"

TreeWidgetItem::TreeWidgetItem( TreeWidgetItem* parent )
{
    name = "";

    framed                    = false;
    statistic_available_frame = false;
    maxItem                   = true;

    id         = 0;
    type       = METRICITEM;
    cubeObject = NULL;

    ownValue   = 0.0;
    totalValue = 0.0;
    rootValue  = 0.0;
    //the minimal and maximal peer values for system tree items
    minValue = 0.0;
    maxValue = 0.0;

    children.clear();

    expanded = false;
    hidden   = false;
    selected = false;

    // if node is not on the very top
    // it is not visible
    if ( parent == NULL )
    {
        visible = true;
    }
    else
    {
        visible = false;
    }

    textExpanded   = "";
    textCollapsed  = "";
    valueExpanded  = 0.0;
    valueCollapsed = 0.0;
    colorExpanded  = Qt::white;
    colorCollapsed = Qt::white;

    parentItem = parent;
    if ( parent != NULL )
    {
        parent->addChild( this );
        depth = parent->getDepth() + 1;
        //set the tree widget
        tree = parent->getTree();
    }
    else
    {
        depth = 0;
        tree  = NULL;
    }

    //found item?
    mark1 = false;
    //parent of found item?
    mark2 = false;
}

TreeWidgetItem::~TreeWidgetItem()
{
}


/***************** set methods ************************/

//set the expansion state
//
void
TreeWidgetItem::setExpanded( bool expanded )
{
    this->expanded = expanded;
}

//set the hidden state
//
void
TreeWidgetItem::setHidden( bool hidden )
{
    this->hidden = hidden;
}

//set the selected state
//
void
TreeWidgetItem::setSelected( bool selected, bool updateTree )
{
    if ( selected != this->selected )
    {
        this->selected = selected;
        //inform the tree widget that this item was inserted
        if ( tree != NULL && updateTree )
        {
            if ( selected )
            {
                tree->select( this );
            }
            else
            {
                tree->deselect( this );
            }
        }
    }
}

//set the text to be displayed in the expansion state "expanded" to
//"text"
//
void
TreeWidgetItem::setText( bool expanded, QString text )
{
    if ( expanded )
    {
        textExpanded = text;
    }
    else
    {
        textCollapsed = text;
    }
}

//set the parent item
//
void
TreeWidgetItem::setParent( TreeWidgetItem* parent )
{
    parentItem = parent;
}

//set the tree widget the item's tree is contained in
//
void
TreeWidgetItem::setTree( MyTreeWidget* tree )
{
    this->tree = tree;
    if ( selected )
    {
        tree->select( this );
    }
    for ( int i = 0; i < ( int )children.size(); i++ )
    {
        children[ i ]->setTree( tree );
    }
}

// set flag for most severe instances
void
TreeWidgetItem::setStatisticFlag()
{
    statistic_available_frame = true;
}
void
TreeWidgetItem::clearStatisticFlag()
{
    statistic_available_frame = false;
}


/***************** get methods **********************/


//get the item's hierarchy depth
//
int
TreeWidgetItem::getDepth()
{
    return depth;
}

//is the item expanded/hidden/selected?
//
bool
TreeWidgetItem::isExpanded()
{
    return expanded;
}
bool
TreeWidgetItem::isHidden()
{
    return hidden;
}
bool
TreeWidgetItem::isSelected()
{
    return selected;
}

//get the text to be displayed in the current state
//
QString
TreeWidgetItem::getText()
{
    return expanded ? textExpanded : textCollapsed;
}

//get the text to be displayed in the expanded state
//if "expanded" is true, and in the collapsed state otherwise
//
QString
TreeWidgetItem::getText( bool expanded )
{
    return expanded ? textExpanded : textCollapsed;
}

//get the item's value in the current state
//
double
TreeWidgetItem::getValue()
{
    return expanded ? valueExpanded : valueCollapsed;
}

//get the color for the value of the item's current state
//
QColor
TreeWidgetItem::getColor()
{
    return expanded ? colorExpanded : colorCollapsed;
}

//return number of children
//
int
TreeWidgetItem::childCount()
{
    return children.size();
}

//return the ith child
//
TreeWidgetItem*
TreeWidgetItem::child( int index )
{
    if ( ( int )children.size() <= index )
    {
        return NULL;
    }
    else
    {
        return children[ index ];
    }
}

//return the parent item
//
TreeWidgetItem*
TreeWidgetItem::parent()
{
    return parentItem;
}

//get the tree widget the item's tree in contained in
//
MyTreeWidget*
TreeWidgetItem::getTree()
{
    return tree;
}

/********** add/remove childen *********/

//add a new child to this item
//
void
TreeWidgetItem::addChild( TreeWidgetItem* item )
{
    assert( !item->isSelected() );
    assert( item->parent() == NULL || item->parent() == this );
    bool found = false;
    //check if the parameter item is already in the children list
    for ( int i = 0; i < ( int )children.size(); i++ )
    {
        if ( children[ i ] == item )
        {
            found = true;
            break;
        }
    }
    //if not, then add it as child
    if ( !found )
    {
        children.push_back( item );
    }
    item->setParent( this );
}

//remove a child from the item's children list
//
void
TreeWidgetItem::removeChild( TreeWidgetItem* item )
{
    assert( item->parent() == this );
    for ( int i = 0; i < ( int )children.size(); i++ )
    {
        if ( children[ i ] == item )
        {
            for (; i < ( int )children.size() - 1; i++ )
            {
                children[ i ] = children[ i + 1 ];
            }
            children.pop_back();
            break;
        }
        assert( i < ( int )children.size() - 1 );
    }
    item->setParent( NULL );
}
