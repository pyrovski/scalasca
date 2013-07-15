/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
/*
   Data may be changed by
   - loading new cube file
   - loading settings
   - topology toolbar elements (via 2nd class)
   - selection in system tree widget
   => emit signal to notify view
 */

#include "TreeWidget.h"
#include "Tetragon.h"
#include "SystemTopologyData.h"
#include "PrecisionWidget.h"
#include "ValueWidget.h"

#include "Cartesian.h"
#include "Sysres.h"
#include "Thread.h"
#include "Process.h"
#include "Node.h"
#include <cassert>
#include <qdebug.h>

#define ONEDIM 1

/*************** constructor / destructor *************************/

SystemTopologyData::SystemTopologyData( TreeWidget* systemTreeWidget,
                                        unsigned    topologyId )
{
    //the current dimensions
    dim[ 0 ] = 0;
    dim[ 1 ] = 0;
    dim[ 2 ] = 0;

    this->systemTreeWidget = systemTreeWidget;
    this->topologyId       = topologyId;

    selectMode        = FOLD;
    invalidDimensions = 0;

    userDefinedMinMaxValues = false;
    colors.clear();
    selected_rects.clear();
    cube = 0;
}

SystemTopologyData::~SystemTopologyData()
{
    for ( unsigned i = 0; i < colors.size(); i++ )
    {
        for ( unsigned j = 0; j < colors[ i ].size(); j++ )
        {
            for ( unsigned k = 0; k < colors[ i ][ j ].size(); k++ )
            {
                if ( colors[ i ][ j ][ k ] != NULL )
                {
                    delete colors[ i ][ j ][ k ];
                }
            }
        }
    }
    colors.clear();
    selected_rects.clear();
}

/**
   x,y,z coordinates
 */
unsigned
SystemTopologyData::getSystemId( int x, int y, int z ) const
{
    // coordToSystemId needs coordinates in original dim order
    return coordToSystemId[ x ][ y ][ z ];
}

bool
SystemTopologyData::isSelected( int x, int y, int z ) const
{
    return selected_rects[ x ][ y ][ z ];
}

QColor*
SystemTopologyData::getColor( int x, int y, int z ) const
{
    return colors[ x ][ y ][ z ];
}

/**
 * for each coordinate: check if the 4 neighbors have the same ID
 */
void
SystemTopologyData::determineNeighbors()
{
    neighbors.clear();
    for ( uint x = 0; x < dim[ 0 ]; ++x )
    {
        for ( uint y = 0; y < dim[ 1 ]; ++y )
        {
            for ( uint z = 0; z < dim[ 2 ]; ++z )
            {
                int val = 0;
                int id  = coordToSystemId[ x ][ y ][ z ]; // system id nr of current item
                int idN;                                  // neighbor system id
                if ( x > 0 )
                {
                    idN = coordToSystemId[ x - 1 ][ y  ][ z ]; // P3-P0
                    if ( id == idN )
                    {
                        val |= 1 << 3;
                    }
                }
                if ( x + 1 < dim[ 0 ] )
                {
                    idN = coordToSystemId[ x + 1 ][ y ][ z ]; // P1-P2
                    if ( id == idN )
                    {
                        val |= 1 << 1;
                    }
                }
                if ( y > 0 )
                {
                    idN = coordToSystemId[ x ][ y - 1 ][ z ]; // P2-P3
                    if ( id == idN )
                    {
                        val |= 1 << 0;
                    }
                }
                if ( y + 1 < dim[ 1 ] )
                {
                    idN = coordToSystemId[ x ][ y + 1 ][ z ]; // P0-P1
                    if ( id == idN )
                    {
                        val |= 1 << 2;
                    }
                }
                if ( val != 0 )
                {
                    QString coord;
                    coord.sprintf( "%d,%d,%d", x, y, z );
                    neighbors.insert( coord, val );
                }
            }
        }
    }
}

/**
 * return the neighbors of the given coordinate which have the same systemID
 * @return for each of the four directions, the corresponding bit of the return value is set to 1, if a
 * neighbor with the same systemID exists
 */
int
SystemTopologyData::getNeighbors( int x, int y, int z )
{
    QString coord;
    coord.sprintf( "%d,%d,%d", x, y, z );
    return neighbors.value( coord );
}

/********************** initialization / clean up ********************************/

/** map to 3 dimensions (set systemToCoord and coordToSystemId for the given coord)
    selection method: show 2-3 dimensions and select one index of the rest
    @param coord coordinates in n dimensions
    @param id unique id for each matrix element
 */
void
SystemTopologyData::multiDimSelect( const std::vector<long>& coord, unsigned id )
{
    assert( cube->get_cart( topologyId )->get_dimv().size() >= 3 );

    // initialize selectedDimensions if not set => show first 3 dimensions and index 0 of the rest
    if ( selectedDimensions.size() != coord.size() )
    {
        selectedDimensions.clear();
        for ( uint j = 0; j < coord.size(); j++ )
        {
            if ( j < 3 )
            {
                selectedDimensions.push_back( -( j + 1 ) ); // negative = display all for dimension (-j-1)
            }
            else
            {
                selectedDimensions.push_back( 0 ); // display index 0
            }
        }
    }

    bool notSelected = false;
    int  sdims       = 0; // number dimensions wich are fully displayed (2-3)
    int  sdim[ 3 ];       // index of the dimensions with are fully displayed
    for ( unsigned int j = 0; j < selectedDimensions.size(); j++ )
    {
        if ( selectedDimensions[ j ] >= 0 )              // only one element of this dimension is displayed
        {
            if ( coord[ j ] != selectedDimensions[ j ] ) // current element is not displayed
            {
                notSelected = true;
                break; // dimension is not displayed
            }
        }
        else   // all elements of dimension j are displayed
        {
            long dim = -selectedDimensions[ j ] - 1;
            sdim[ dim ] = j;
            sdims++;
        }
    }

    if ( !notSelected ) // element is visible
    {
        int val[ 3 ];   // x,y,z
        for ( int i = 0; i < sdims; i++ )
        {
            val[ i ] = coord[ sdim[ i ] ];
        }

        if ( sdims == 3 )
        {
            coordToSystemId[ val[ 0 ] ][ val[ 1 ] ][ val[ 2 ] ] = ( int )( id );
        }
        else if ( sdims == 2 ) // special case: slice to 2 dimensions
        {
            coordToSystemId[ val[ 0 ] ][ val[ 1 ] ][ 0 ] = ( int )( id );
        }

        size_t num_of_coords = systemToCoord[  ( int )( id )  ].size();
        for ( int j = 0; j < sdims; j++ )
        {
            systemToCoord[ ( int )( id ) ][ num_of_coords - 1 ].push_back( val[ j ] );
        }
    } // !notSelected
}

/** merge several dimensions into one
 */
void
SystemTopologyData::multiDimFold( const std::vector<long>& coord, unsigned id )
{
    assert( cube->get_cart( topologyId )->get_dimv().size() >= 2 );
    //get the orginal dimensions
    const std::vector<long>& dims = cube->get_cart( topologyId )->get_dimv();

    int                      dimensions = foldingDimensions.size();
    int                      val[ dimensions ]; // x,y,[z]
    for ( int i = 0; i < dimensions; i++ )
    {
        // fold all dimensions of vector foldingDimensions[i] into one
        int elements = 1;
        val[ i ] = 0;
        for ( int j = foldingDimensions[ i ].size() - 1; j >= 0; j-- )
        {
            int dimIndex = foldingDimensions[ i ][ j ];
            val[ i ] += elements * coord[ dimIndex ];
            elements *= dims[ dimIndex ];
        }
    }

    size_t num_of_coords = systemToCoord[  ( int )( id )  ].size();
    coordToSystemId[ val[ 0 ] ][ val[ 1 ] ][ val[ 2 ] ] = ( int )( id );

    for ( int i = 0; i < dimensions; i++ )
    {
        systemToCoord[ ( int )( id ) ][ num_of_coords - 1 ].push_back( val[ i ] );
    }
}


QPair<QString, QString>
SystemTopologyData::coordToString( const std::vector<long>& coord )
{
    QString                        label, value;

    const std::vector<std::string> dimNames = cube->get_cart( topologyId )->get_namedims();
    for ( unsigned j = 0; j < coord.size(); j++ )
    {
        if ( dimNames.size() > 0 )
        {
            label.append( QString::fromStdString( dimNames[ j ] ) );
        }
        else
        {
            label.append( "Coord " ).append( QString::number( j ) );
        }
        value.append( QString::number( coord[ j ] ) );
        if ( j != coord.size() - 1 )
        {
            label.append( ": \n" );
            value.append( "\n" );
        }
    }

    return QPair<QString, QString>( label, value );
}

QPair<QString, QString>
SystemTopologyData::getOriginalCoordString( const std::vector<long>& coord3D )
{
    std::vector<long> coord; // original coords

    if ( selectMode == SELECT )
    {
        int i = 0;
        for ( unsigned int j = 0; j < selectedDimensions.size(); j++ )
        {
            if ( selectedDimensions[ j ] >= 0 )   // only one element of this dimension is displayed
            {
                coord.push_back( selectedDimensions[ j ] );
            }
            else
            {
                coord.push_back( coord3D[ i++ ] );
            }
        }
    }
    else   // FOLD
    {
        int                      dimensions = foldingDimensions.size();
        const std::vector<long>& dims       = cube->get_cart( topologyId )->get_dimv(); // original dims
        coord.resize( dims.size() );
        for ( int i = 0; i < dimensions; i++ )
        {
            int idx3D = coord3D[ i ];
            for ( int j = foldingDimensions[ i ].size() - 1; j >= 0; j-- )
            {
                int dimIndex = foldingDimensions[ i ][ j ];
                coord[ dimIndex ] = idx3D % dims[ dimIndex ];
                idx3D            /= dims[ dimIndex ];
            }
        }
    }
    return coordToString( coord );
}

/* initializes the topology with data from the cube object
 */
void
SystemTopologyData::initialize( cube::Cube* cube )
{
    this->cube    = cube;
    isInitialised = false;
    uniqueIds     = true;

    reinit();
}

/**
   @param resetValue if true, reset angles of topology and plane distance
 */
void
SystemTopologyData::reinit()
{
    assert( systemTreeWidget != NULL );

    QSet<int> idSet;

    //get the dimensions
    const std::vector<long>&       dims     = cube->get_cart( topologyId )->get_dimv();
    const std::vector<std::string> dimNames = cube->get_cart( topologyId )->get_namedims();

    //set the dimensions
    if ( dims.size() == 0 )
    {
        return;
    }
    else if ( dims.size() == ONEDIM )
    {
        setDims( 1, ( unsigned )dims[ 0 ], 1 );
    }
    else // if more than 3 dimensions => reduce to 3 to display
    {    // display 2 ore 3 dimensions and only one item of the rest
        if ( selectMode == SELECT )
        {
            int sdims = 0; // number of displayed dimensions (2-3)
            int sdim[ 3 ]; // indices of the displayed dimensions in original vector (dims)
            // e.g. sdim[0] is the index of the first displayed dimension
            for ( uint j = 0; j < selectedDimensions.size() && sdims < 3; j++ )
            {
                if ( selectedDimensions[ j ] <= -1 )        // negative = show full dimension
                {
                    int dim = -selectedDimensions[ j ] - 1; // dim = position of selected dimension
                    sdim[ dim ] = j;
                    sdims++;
                }
            }
            int val[ sdims ]; // x,y,z
            for ( int i = 0; i < sdims; i++ )
            {
                val[ i ] = dims[ sdim[ i ] ]; // x,y,z -> number of elements
            }
            if ( sdims == 2 )
            {
                setDims( val[ 0 ], val[ 1 ], 1 );
            }
            else if ( sdims >= 3 )
            {
                setDims( val[ 0 ], val[ 1 ], val[ 2 ] );
            }
        }
        // folding is selected => merge dimensions
        else if ( selectMode == FOLD )
        {
            uint dimensions = foldingDimensions.size();
            int  foldingDims[ dimensions ];
            for ( uint i = 0; i < dimensions; i++ )
            {
                int elements = 1;
                for ( uint j = 0; j < foldingDimensions[ i ].size(); j++ )
                {
                    int dimIndex = foldingDimensions[ i ][ j ];
                    ;
                    elements *= dims[ dimIndex ];
                }
                foldingDims[ i ] = elements;
            }

            setDims( foldingDims[ 0 ], foldingDims[ 1 ], foldingDims[ 2 ] );

            // mark merged dimensions (draw separator), if two dimensions are merged
            for ( int i = 0; i < 3; i++ )
            {
                foldingDiv[ i ] = foldingDimensions[ i ].size() == 2 ?
                                  dims[ foldingDimensions[ i ][ 1 ] ] : 1;
            }
            if ( dimensions == 2 )   // merged to 2-dimensional topology
            {
                foldingDiv[ 2 ] = foldingDiv[ 1 ];
                foldingDiv[ 1 ] = foldingDiv[ 0 ];
                foldingDiv[ 0 ] = 0;
            }
        }
    } // if >=3 dims

    assert( dim[ 0 ] > 0 );
    assert( dim[ 1 ] > 0 );
    assert( dim[ 2 ] > 0 );

    //-------------  initialize the color matrix
    for ( unsigned i = 0; i < colors.size(); i++ )
    {
        for ( unsigned j = 0; j < colors[ i ].size(); j++ )
        {
            for ( unsigned k = 0; k < colors[ i ][ j ].size(); k++ )
            {
                if ( colors[ i ][ j ][ k ] != NULL )
                {
                    delete colors[ i ][ j ][ k ];
                }
            }
        }
    }

    colors.clear();
    selected_rects.clear();
    colors.resize( dim[ 0 ] );
    selected_rects.resize( dim[ 0 ] );

    for ( unsigned i = 0; i < dim[ 0 ]; i++ )
    {
        colors[ i ].resize( dim[ 1 ] );
        selected_rects[ i ].resize( dim[ 1 ] );
        for ( unsigned j = 0; j < dim[ 1 ]; j++ )
        {
            colors[ i ][ j ].resize( dim[ 2 ] );
            selected_rects[ i ][ j ].resize( dim[ 2 ] );
            for ( unsigned k = 0; k < dim[ 2 ]; k++ )
            {
                colors[ i ][ j ][ k ]         = NULL;
                selected_rects[ i ][ j ][ k ] = false;
            }
        }
    }

    //initialize the topology;
    //the value -1 denotes that at that node no thread/process was running
    coordToSystemId.resize( dim[ 0 ] );
    for ( unsigned i = 0; i < dim[ 0 ]; i++ )
    {
        coordToSystemId[ i ].resize( dim[ 1 ] );
        for ( unsigned j = 0; j < dim[ 1 ]; j++ )
        {
            coordToSystemId[ i ][ j ].resize( dim[ 2 ] );
            for ( unsigned k = 0; k < dim[ 2 ]; k++ )
            {
                coordToSystemId[ i ][ j ][ k ] = -1;
            }
        }
    }

    const std::multimap<const cube::Sysres*, std::vector<long> >&          cubeSystemToCoord =
        cube->get_cart( topologyId )->get_cart_sys();
    std::multimap<const cube::Sysres*, std::vector<long> >::const_iterator itr;

    systemToCoord.clear();
    systemToCoordStr.clear();
    systemToCoordStr.resize( ( int )( systemTreeWidget->items.size() ) );

    std::vector<TreeWidgetItem*> & items = systemTreeWidget->items;
    for ( unsigned i = 0; i < items.size(); i++ )
    {
        //toCompute tells if the system item is a thread or
        //a process in the single-threaded case
        bool toCompute = false;

        if ( items[ i ]->type == THREADITEM )
        {
            itr       = cubeSystemToCoord.find( ( cube::Sysres* )( items[ i ]->cubeObject ) );
            toCompute = true;
        }
        else if ( items[ i ]->type == PROCESSITEM && items[ i ]->childCount() == 0 )
        {
            //single-threaded processes
            if ( ( ( cube::Process* )( items[ i ]->cubeObject ) )->num_children() == 1 )
            {
                //the topology can be defined either on processes or on threads, we
                //therefore check both...
                itr = cubeSystemToCoord.find( ( cube::Sysres* )( ( ( cube::Process* )( items[ i ]->cubeObject ) )->get_child( 0 ) ) );
                if ( itr == cubeSystemToCoord.end() )
                {
                    itr = cubeSystemToCoord.find( ( cube::Sysres* )( items[ i ]->cubeObject ) );
                }
                toCompute = true;
            }
        }
        //if yes, then check if the item is in the topology, and
        //eventually store its topology position
        if ( toCompute )
        {
            std::multimap <const cube::Sysres*, std::vector<long> >::const_iterator                                                                                     it;
            std::pair< std::multimap<const cube::Sysres*, std::vector<long> >::const_iterator, std::multimap<const cube::Sysres*, std::vector<long> >::const_iterator > ret;

            ret = cubeSystemToCoord.equal_range( itr->first );
            if (  ret.first  != ret.second )
            {
                for ( it = ret.first; it != ret.second; ++it ) // run over all coordinates of this thread
                {
                    const std::vector<long>& coord = it->second;

                    systemToCoord[ ( int )( items[ i ]->id ) ].push_back( std::vector< long > ()  );
                    size_t num_of_coords = systemToCoord[ ( int )( items[ i ]->id ) ].size();

                    // generate info for each displayed element with complete coordinates
                    systemToCoordStr[ ( int )items[ i ]->id ] = coordToString( coord );

                    if ( coord.size() == ONEDIM )
                    {
                        coordToSystemId[ 0 ][ coord[ 0 ] ][ 0 ] = ( int )( items[ i ]->id );
                        systemToCoord[ ( int )( items[ i ]->id ) ][ num_of_coords - 1 ].push_back( 0 );
                        systemToCoord[ ( int )( items[ i ]->id ) ][ num_of_coords - 1 ].push_back( coord[ 0 ] );
                        systemToCoord[ ( int )( items[ i ]->id ) ][ num_of_coords - 1 ].push_back( 0 );
                    }
                    else if ( coord.size() >= 2 )
                    {
                        if ( selectMode == SELECT )
                        {
                            multiDimSelect( coord, items[ i ]->id );
                        }
                        else
                        {
                            multiDimFold( coord, items[ i ]->id );
                        }
                    }
                    if ( !isInitialised ) // check only one time, if ids are unique for each coordinate
                    {
                        int id = ( int )items[ i ]->id;
                        if ( idSet.contains( id ) )
                        {
                            uniqueIds     = false;
                            isInitialised = true;
                        }
                        else
                        {
                            idSet.insert( id );
                        }
                    }
                }
            }
        } //toCompute
    }
    isInitialised = true;

    //we store in the tab widget if unused topology planes should be painted
    if ( !systemTreeWidget->getTabWidget()->getShowUnusedTopologyPlanes() )
    {
        toggleUnused();
    }

    if ( !uniqueIds )
    {
        determineNeighbors();
    }
    updateColors();
    updateSelection();
    emit rescaleRequest();
} // reinit

/************************* coloring  ***********************/

//get the color for the item with the given id
//
QColor*
SystemTopologyData::getColorForId( unsigned id )
{
    assert( systemTreeWidget->items.size() >= id );
    return new QColor( systemTreeWidget->items[ id ]->getColor() );
#if 0
    //todo ????
    // rev 3954, 4 years ago
    bool   ok;
    double value;
    //get the current value from the system tree widget
    systemTreeWidget->getValue( systemTreeWidget->items[ id ], false, value, ok );

    QColor* color;

    //undefined values get the color gray
    if ( !ok )
    {
        color = new QColor( Qt::gray );
    }
    else
    {
        //colors represent the (math) absolute value
        if ( value < 0.0 )
        {
            value = -value;
        }
        assert( value >= 0.0 );

        //compute value for minimal color
        double min;
        if ( userDefinedMinMaxValues )
        {
            min = userMinValue;
        }
        else if ( systemTreeWidget->tabWidget->getValueModus() == ABSOLUTE )
        {
            min = minValue;
        }
        else
        {
            min = 0.0;
        }

        //compute value for maximal color
        double max;
        if ( userDefinedMinMaxValues )
        {
            max = userMaxValue;
        }
        else if ( systemTreeWidget->tabWidget->getValueModus() == ABSOLUTE )
        {
            max = maxValue;
        }
        else
        {
            max = 100.0;
        }

        color = new QColor( colorWidget->getColor( value,
                                                   min,
                                                   max,
                                                   systemTreeWidget->getTabWidget()->getWhiteForZero()
                                                   ) );
    }
    return color;
#endif
}


void
SystemTopologyData::updateColors()
{
    if ( dim[ 0 ] == 0 || dim[ 1 ] == 0 || dim[ 2 ] == 0 )
    {
        return;
    }

    assert( colors.size() == dim[ 0 ] );
    assert( colors[ 0 ].size() == dim[ 1 ] );
    assert( colors[ 0 ][ 0 ].size() == dim[ 2 ] );

    bool   first    = true;
    bool   firstAbs = true;
    bool   ok;
    double value;

    //since we consider threads or single-threaded processes only,
    //the min/max values differ from that of the system tree,
    //thus we must compute them;
    minValue    = 0.0;
    maxValue    = 0.0;
    minAbsValue = 0.0;
    maxAbsValue = 0.0;

    //compute minimal and maximal values

    for ( unsigned i0 = 0; i0 < dim[ 0 ]; i0++ )
    {
        for ( unsigned i1 = 0; i1 < dim[ 1 ]; i1++ )
        {
            for ( unsigned i2 = 0; i2 < dim[ 2 ]; i2++ )
            {
                //get the id of the item at the current topology position
                int systemId = coordToSystemId[ i0 ][ i1 ][ i2 ];
                if ( systemId >= 0 )
                {
                    //get its absolute value from the system topology widget
                    systemTreeWidget->getValue( systemTreeWidget->items[ systemId ], false, value, ok, ABSOLUTE );
                    //if the value is defined, update the variables storing the
                    //minimal and maximal absolute values
                    if ( ok )
                    {
                        if ( firstAbs )
                        {
                            firstAbs    = false;
                            minAbsValue = value;
                            maxAbsValue = value;
                        }
                        else
                        {
                            if ( minAbsValue > value )
                            {
                                minAbsValue = value;
                            }
                            else if ( maxAbsValue < value )
                            {
                                maxAbsValue = value;
                            }
                        }
                    }
                    //if the current value modus is not the absolute value modus
                    if ( !systemTreeWidget->getTabWidget()->getValueModus() == ABSOLUTE )
                    {
                        //get the current value for the item at the current
                        //topology position from the system tree widget
                        systemTreeWidget->getValue( systemTreeWidget->items[ systemId ], false, value, ok );
                        //if the value is defined, then update the variables for
                        //the minimal and maximal values
                        if ( ok )
                        {
                            if ( first )
                            {
                                first    = false;
                                minValue = value;
                                maxValue = value;
                            }
                            else
                            {
                                if ( minValue > value )
                                {
                                    minValue = value;
                                }
                                else if ( maxValue < value )
                                {
                                    maxValue = value;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if ( minValue < 0.0 )
    {
        minValue = 0.0;
    }
    if ( minAbsValue < 0.0 )
    {
        minAbsValue = 0.0;
    }

    if ( systemTreeWidget->getTabWidget()->getValueModus() == ABSOLUTE )
    {
        minValue = minAbsValue;
        maxValue = maxAbsValue;
    }

    //determine the colors
    for ( unsigned i0 = 0; i0 < dim[ 0 ]; i0++ )
    {
        for ( unsigned i1 = 0; i1 < dim[ 1 ]; i1++ )
        {
            for ( unsigned i2 = 0; i2 < dim[ 2 ]; i2++ )
            {
                if ( colors[ i0 ][ i1 ][ i2 ] != NULL )
                {
                    delete colors[ i0 ][ i1 ][ i2 ];
                }
                int systemId = coordToSystemId[ i0 ][ i1 ][ i2 ];
                if ( systemId < 0 )
                {
                    colors[ i0 ][ i1 ][ i2 ] = new QColor( Qt::lightGray );
                }
                else
                {
                    colors[ i0 ][ i1 ][ i2 ] = getColorForId( ( unsigned )systemId );
                }
            }
        }
    }
}

/**
   initialize selected_rects from systemTreeWidget->selectedItems()
 */
bool
SystemTopologyData::updateSelection()
{
    bool changed = false;
    for ( unsigned i0 = 0; i0 < dim[ 0 ]; i0++ )
    {
        for ( unsigned i1 = 0; i1 < dim[ 1 ]; i1++ )
        {
            for ( unsigned i2 = 0; i2 < dim[ 2 ]; i2++ )
            {
                int systemId = coordToSystemId[ i0 ][ i1 ][ i2 ];
                if ( systemId < 0 )
                {
                    selected_rects[ i0 ][ i1 ][ i2 ] = false;
                }
                else
                {
                    bool                         marked        = false;
                    std::vector<TreeWidgetItem*> _marked_items = systemTreeWidget->selectedItems();
                    for ( std::vector<TreeWidgetItem*>::iterator iter = _marked_items.begin(); iter != _marked_items.end(); iter++ )
                    {
                        if ( ( int )( ( *iter )->id ) == systemId )
                        {
                            marked = true;
                            break;
                        }
                    }
                    if ( selected_rects[ i0 ][ i1 ][ i2 ] != marked )
                    {
                        changed = true;
                    }
                    selected_rects[ i0 ][ i1 ][ i2 ] = marked;
                }
            }
        }
    }
    return changed;
}

unsigned
SystemTopologyData::getDim( int i ) const
{
    if ( i <= 3 )
    {
        return dim[ i ];
    }
    else
    {
        return 0;
    }
}


/**************************** miscellaneous **********************/


//get user-defined minimal and maximal values for the minimal and
//maximal colors
void
SystemTopologyData::setMinMaxValues( double minValue, double maxValue )
{
    userDefinedMinMaxValues = true;
    userMinValue            = minValue;
    userMaxValue            = maxValue;
    emit viewChanged(); // todo correct?
}

//set the line type for topologies: black, gray, white, or no lines
//
void
SystemTopologyData::setLineType( LineType lineType )
{
    this->lineType = lineType;
    if ( cube != NULL )
    {
        emit viewChanged();
    }
}


//toggle the state if unused planes should be displayed or not
//
void
SystemTopologyData::toggleUnused()
{
    if ( dim[ 0 ] == 0 || dim[ 1 ] == 0 || dim[ 2 ] == 0 )
    {
        return;
    }

    //first case: if unused topology planes are not shown
    if ( !systemTreeWidget->getTabWidget()->getShowUnusedTopologyPlanes() )
    {
        //check the first dimension for unused planes
        for ( unsigned i = 0; i < dim[ 0 ]; i++ )
        {
            bool unused = true;
            for ( unsigned j = 0; j < dim[ 1 ]; j++ )
            {
                for ( unsigned k = 0; k < dim[ 2 ]; k++ )
                {
                    if ( coordToSystemId[ i ][ j ][ k ] != -1 )
                    {
                        unused = false;
                        break;
                    }
                }
                if ( !unused )
                {
                    break;
                }
            }
            if ( unused )
            {
                for ( unsigned i2 = i + 1; i2 < dim[ 0 ]; i2++ )
                {
                    coordToSystemId[ ( int )i2 - 1 ] = coordToSystemId[ i2 ];
                }
                setDims( dim[ 0 ] - 1, dim[ 1 ], dim[ 2 ] );
                coordToSystemId.resize( dim[ 0 ] );
                i--;
            }
        }

        //check the second dimension for unused planes
        for ( unsigned j = 0; j < dim[ 1 ]; j++ )
        {
            bool unused = true;
            for ( unsigned i = 0; i < dim[ 0 ]; i++ )
            {
                for ( unsigned k = 0; k < dim[ 2 ]; k++ )
                {
                    if ( coordToSystemId[ i ][ j ][ k ] != -1 )
                    {
                        unused = false;
                        break;
                    }
                }
                if ( !unused )
                {
                    break;
                }
            }

            if ( unused )
            {
                for ( unsigned i = 0; i < dim[ 0 ]; i++ )
                {
                    for ( unsigned j2 = j + 1; j2 < dim[ 1 ]; j2++ )
                    {
                        coordToSystemId[ i ][ ( int )j2 - 1 ] = coordToSystemId[ i ][ j2 ];
                    }
                    coordToSystemId[ i ].resize( dim[ 1 ] - 1 );
                }
                setDims( dim[ 0 ], dim[ 1 ] - 1, dim[ 2 ] );
                j--;
            }
        }

        //check the third dimension for unused planes
        for ( unsigned k = 0; k < dim[ 2 ]; k++ )
        {
            bool unused = true;
            for ( unsigned i = 0; i < dim[ 0 ]; i++ )
            {
                for ( unsigned j = 0; j < dim[ 1 ]; j++ )
                {
                    if ( coordToSystemId[ i ][ j ][ k ] != -1 )
                    {
                        unused = false;
                        break;
                    }
                }
                if ( !unused )
                {
                    break;
                }
            }
            if ( unused )
            {
                for ( unsigned i = 0; i < dim[ 0 ]; i++ )
                {
                    for ( unsigned j = 0; j < dim[ 1 ]; j++ )
                    {
                        for ( unsigned k2 = k + 1; k2 < dim[ 2 ]; k2++ )
                        {
                            coordToSystemId[ i ][ j ][ ( int )k2 - 1 ] = coordToSystemId[ i ][ j ][ k2 ];
                        }
                        coordToSystemId[ i ][ j ].resize( dim[ 2 ] - 1 );
                    }
                }
                setDims( dim[ 0 ], dim[ 1 ], dim[ 2 ] - 1 );
                k--;
            }
        }
        for ( unsigned i = 0; i < dim[ 0 ]; i++ )
        {
            for ( unsigned j = 0; j < dim[ 1 ]; j++ )
            {
                for ( unsigned k = 0; k < dim[ 2 ]; k++ )
                {
                    int id = coordToSystemId[ i ][ j ][ k ];
                    if ( id >= 0 )
                    {
                        systemToCoord[ id ].clear();
                    }
                }
            }
        }
        for ( unsigned i = 0; i < dim[ 0 ]; i++ )
        {
            for ( unsigned j = 0; j < dim[ 1 ]; j++ )
            {
                for ( unsigned k = 0; k < dim[ 2 ]; k++ )
                {
                    int    id = coordToSystemId[ i ][ j ][ k ];
                    systemToCoord[ id ].push_back( std::vector< long > () );
                    size_t num_of_coords = systemToCoord[ id ].size();
                    if ( id >= 0 )
                    {
                        systemToCoord[ id ][ num_of_coords - 1 ].resize( 3 );
                        systemToCoord[ id ][ num_of_coords - 1 ][ 0 ] = i;
                        systemToCoord[ id ][ num_of_coords - 1 ][ 1 ] = j;
                        systemToCoord[ id ][ num_of_coords - 1 ][ 2 ] = k;
                    }
                }
            }
        }

        //update the color dimensions

        for ( unsigned i = 0; i < colors.size(); i++ )
        {
            for ( unsigned j = 0; j < colors[ i ].size(); j++ )
            {
                for ( unsigned k = 0; k < colors[ i ][ j ].size(); k++ )
                {
                    if ( colors[ i ][ j ][ k ] != NULL )
                    {
                        delete colors[ i ][ j ][ k ];
                    }
                }
            }
        }
        colors.clear();
        selected_rects.clear();
        colors.resize( dim[ 0 ] );
        selected_rects.resize( dim[ 0 ] );
        for ( unsigned i = 0; i < dim[ 0 ]; i++ )
        {
            colors[ i ].resize( dim[ 1 ] );
            selected_rects[ i ].resize( dim[ 1 ] );
            for ( unsigned j = 0; j < dim[ 1 ]; j++ )
            {
                colors[ i ][ j ].resize( dim[ 2 ] );
                selected_rects[ i ][ j ].resize( dim[ 2 ] );
                for ( unsigned k = 0; k < dim[ 2 ]; k++ )
                {
                    colors[ i ][ j ][ k ]         = NULL;
                    selected_rects[ i ][ j ][ k ] = false;
                }
            }
        }
        updateColors();
        updateSelection();
        emit rescaleRequest();
    }
    else
    {
        //second case: unused planes are displayed, too
        //in this case just re-initialize
        reinit();
    }
}


/**************************** dimension settings *****************/

//set the original dimensions
void
SystemTopologyData::setDims( unsigned dim0, unsigned dim1, unsigned dim2 )
{
    dim[ 0 ] = dim0;
    dim[ 1 ] = dim1;
    dim[ 2 ] = dim2;
}


/**
   used for topologies with more than 3 dimensions
   the method is called, if the user has changed the displayed dimensions
 */
void
SystemTopologyData::selectedDimensionsChanged( const std::vector<long> &dims )
{
    if ( dims.size() > 0 )
    {
        invalidDimensions  = false;
        selectedDimensions = dims;
        selectMode         = SELECT;
        reinit();
    }
    else
    {
        invalidDimensions = true;
    }
    emit dataChanged();
}

void
SystemTopologyData::foldingDimensionsChanged( std::vector<std::vector<int> > fold )
{
    if ( fold.size() > 0 )
    {
        foldingDimensions = fold;
        invalidDimensions = false;
        selectMode        = FOLD;
        reinit(); // count/size of dimensions may have changed by folding
    }
    else
    {
        invalidDimensions = true;
    }
    emit dataChanged();
}

QStringList
SystemTopologyData::getTooltipText( int x, int y, int z )
{
    QStringList ret;

    if ( ( x >= 0 ) && ( y >= 0 ) && ( z >= 0 ) && ( x < ( int )dim[ 0 ] ) &&
         ( y < ( int )dim[ 1 ] ) && ( z < ( int )dim[ 2 ] ) )
    {
        QString nameStr, valueStr, absValueStr, rankStr, idStr, nodeStr, numberOfElemStr;

        //though we always store internally 3 dimensions, in the output
        //only the real dimensions should be displayed
        int systemId = this->getSystemId( x, y, z );

        //generate the strings of the info box
        std::vector<long> coord3D;
        coord3D.push_back( x );
        coord3D.push_back( y );
        coord3D.push_back( z );

        QPair<QString, QString> coordInfo;
        if ( systemId < 0 )
        {
            coordInfo       = getOriginalCoordString( coord3D );
            nameStr         = "-";
            valueStr        = "-";
            absValueStr     = "-";
            rankStr         = "-";
            idStr           = "-";
            nodeStr         = "-";
            numberOfElemStr = "-";
        }
        else
        {
            coordInfo = systemToCoordStr[ systemId ];
            std::vector< std::vector<long> >& all_coords = systemToCoord[ systemId ];
            numberOfElemStr.append( QString::number( all_coords.size() ) );
            std::vector<long>                 coord;
            for ( std::vector< std::vector<long> >::iterator iter = all_coords.begin(); iter != all_coords.end(); iter++ )
            {
                std::vector<long> _coord = *iter;
                if (    ( long )x == _coord[ 0 ]
                        && ( long )y == _coord[ 1 ]
                        && ( long )z == _coord[ 2 ]
                        )
                {
                    coord = _coord;
                    break;
                }
            }

            TreeWidgetItem* item   = systemTreeWidget->items[ systemId ];
            cube::Sysres*   sysres = ( ( cube::Sysres* )( item->cubeObject ) );

            nameStr.append( QString::fromStdString( sysres->get_name() ) );

            double value, absValue;
            bool   ok;
            //get the current total value
            systemTreeWidget->getValue( item, false, value, ok );
            //get the absolute total value
            systemTreeWidget->getValue( item, false, absValue, ok, ABSOLUTE );

            //compute the value string
            if ( !ok )
            {
                valueStr = "-";
            }
            else
            {
                valueStr = systemTreeWidget->getTabWidget()->getPrecisionWidget()->numberToQString( value, systemTreeWidget->hasIntegerType(), FORMAT1 );
                //compute percentage of the value on the color scale
                double percent = ( userDefinedMinMaxValues ? userMaxValue - userMinValue : getMaxValue() - getMinValue() );
                percent = ( percent == 0.0 ? 100.0 : 100.0 * ( value - ( userDefinedMinMaxValues ? userMinValue : getMinValue() ) ) / percent );
                valueStr.append( " (" );
                valueStr.append( systemTreeWidget->getTabWidget()->getPrecisionWidget()->numberToQString( percent, false, FORMAT1 ) );
                valueStr.append( "%)" );
            }

            //compute the absolute value string
            if ( !ok )
            {
                absValueStr = "-";
            }
            else if ( systemTreeWidget->getTabWidget()->getValueModus() != ABSOLUTE )
            {
                absValueStr = systemTreeWidget->getTabWidget()->getPrecisionWidget()->numberToQString( absValue, systemTreeWidget->hasAbsoluteIntegerType(), FORMAT1 );
                //compute percentage of the absolute value on the color scale
                double absPercent = ( getMaxAbsValue() - getMinAbsValue() );
                absPercent = ( absPercent == 0.0 ? 100.0 : 100.0 * ( absValue - getMinAbsValue() ) / absPercent );
                absValueStr.append( " (" );
                absValueStr.append( systemTreeWidget->getTabWidget()->getPrecisionWidget()->numberToQString( absPercent, false, FORMAT1 ) );
                absValueStr.append( "%)" );
            }

            //get rank, node, and id
            if ( item->type == THREADITEM )
            {
                assert( ( ( cube::Thread* )sysres )->get_parent() != NULL );
                assert( ( ( cube::Thread* )sysres )->get_parent()->get_parent() != NULL );
                rankStr.append( QString::number( ( ( cube::Thread* )sysres )->get_parent()->get_rank() ) );
                idStr.append( QString::number( ( ( cube::Thread* )sysres )->get_rank() ) );
                nodeStr.append( QString::fromStdString( ( ( cube::Thread* )sysres )->get_parent()->get_parent()->get_name() ) );
            }
            else
            {
                assert( item->type == PROCESSITEM );
                rankStr.append( QString::number( ( ( cube::Process* )sysres )->get_rank() ) );
                if ( ( ( cube::Process* )sysres )->num_children() == 0 )
                {
                    idStr.append( "-" );
                }
                else
                {
                    assert( ( ( cube::Process* )sysres )->num_children() == 1 );
                    idStr.append( QString::number( ( ( cube::Process* )sysres )->get_child( 0 )->get_rank() ) );
                }
                nodeStr.append( QString::fromStdString( ( ( cube::Process* )sysres )->get_parent()->get_name() ) );
            }
        }

        QString coordLabel = coordInfo.first;
        QString coordStr   = coordInfo.second;
        QString text1;

        text1.append( coordLabel );
        text1.append( "\n" );

        text1.append( "Node:" );
        text1.append( "\n" );

        text1.append( "Name:" );
        text1.append( "\n" );

        text1.append( "MPI rank:" );
        text1.append( "\n" );

        text1.append( "Thread id:" );
        text1.append( "\n" );

        text1.append( "Value:" );

        if ( systemTreeWidget->getTabWidget()->getValueModus() != ABSOLUTE )
        {
            text1.append( "\n" );
            text1.append( "Absolute:" );
        }
        text1.append( "\n" );
        text1.append( "Number of elements:" );
        text1.append( "\n" );

        QString text2;

        text2.append( coordStr );
        text2.append( "\n" );

        text2.append( nodeStr );
        text2.append( "\n" );

        text2.append( nameStr );
        text2.append( "\n" );

        text2.append( rankStr );
        text2.append( "\n" );

        text2.append( idStr );
        text2.append( "\n" );

        text2.append( valueStr );

        if ( systemTreeWidget->getTabWidget()->getValueModus() != ABSOLUTE )
        {
            text2.append( "\n" );
            text2.append( absValueStr );
        }
        text2.append( "\n" );
        text2.append( numberOfElemStr );
        text2.append( "\n" );

        ret.append( text1 );
        ret.append( text2 );
    }

    return ret;
}
