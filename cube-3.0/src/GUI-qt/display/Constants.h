/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _CONSTANTS_H
#define _CONSTANTS_H

//the application's name
#define CUBE_NAME "Cube 3.4 QT"

//initial margin for the topology display when resetting the display
#define TOPOLOGY_MARGIN    10

//margin and spacing for the main widget's layout 
#define LAYOUT_MARGIN 2
#define LAYOUT_SPACING 2

//names of value modi as displayed in the combos;
//see mainwidget.cpp for further explanation
#define ABSOLUTE_NAME "Absolute"
#define OWNROOT_NAME "Own root percent"
#define METRICSELECTED_NAME "Metric selection percent"
#define CALLSELECTED_NAME "Call selection percent"
#define SYSTEMSELECTED_NAME "System selection percent"
#define METRICROOT_NAME "Metric root percent"
#define CALLROOT_NAME "Call root percent"
#define SYSTEMROOT_NAME "System root percent"
#define PEER_NAME "Peer percent"
#define PEERDIST_NAME "Peer distribution"
#define EXTERNAL_NAME "External percent"

//the internal type for value modus
enum ValueModus {ABSOLUTE,OWNROOT,METRICROOT,CALLROOT,SYSTEMROOT,METRICSELECTED,CALLSELECTED,SYSTEMSELECTED,PEER,PEERDIST,EXTERNAL};

//the internal type for the lines in the topology display
enum LineType {BLACK_LINES, GRAY_LINES, WHITE_LINES, NO_LINES};

//tab widget types
enum TabWidgetType {METRICTAB,CALLTAB,SYSTEMTAB};

//widget types in the splitter widget (containing u.o. the tab widgets)
enum WidgetType {METRICWIDGET, CALLWIDGET, SYSTEMWIDGET};

//All tree widgets are instances of the TreeWidget class.
//TreeWidgetType allows to specify what kind of trees the instances are.
enum TreeWidgetType {METRICTREE=0, CALLTREE, CALLFLAT, SYSTEMTREE};

//We support two precision types:
//FORMAT0 for the tree widgets and
//FORMAT1 for the value widgets (below the tab widgets) and for the
//        topologies
enum PrecisionFormat {FORMAT0=0, FORMAT1};

//In tree widgets there is possibility to hide items. 
//We support 3 hiding modi:
//NO_HIDING     : All items are non-hidden.
//DYNAMIC_HYDING: Entering this modus, all items get non-hidden.
//                The user defines a threshold percent value. All items whose color position
//                is below this threshold on the color scale, get hidden.
//                If the selections and implicitely the current values change,
//                hiding is re-computed (thus items are hidden dynamically depending on the current values).
//STATIC_HIDING : The user can explicitely hide and show states. 
//                When changing into this modus, previously hidden states keep being (statically) hidden.
//                Value changes have no effect on hiding.
//Each tree has its independent hiding modus.
enum HidingState {STATIC_HIDING, DYNAMIC_HIDING, NO_HIDING};

//the type of tree items
enum TreeWidgetItemType {METRICITEM,CALLITEM,REGIONITEM,MACHINEITEM,NODEITEM,PROCESSITEM,THREADITEM};

//available methods to distribute the colors on the color scale
enum ColoringMethod {LINEAR, QUADRATIC1, QUADRATIC2, EXPONENTIAL1,EXPONENTIAL2};

//modi how to compute values for items
//INCLUSIVE            : the total value for the item's subtree, e.g. for collapsed items
//EXCLUSIVE            : the item's own value
//EXCLUSIVE_PLUS_HIDDEN: the item's own value plus the inclusive value of all hidden children 
enum ItemState {EXCLUSIVE, INCLUSIVE, EXCLUSIVE_PLUS_HIDDEN};

//type for defining how selected items in trees should be marked: by background or by framing
enum SelectionSyntax {BACKGROUND_SELECTION, FRAME_SELECTION};

#endif
