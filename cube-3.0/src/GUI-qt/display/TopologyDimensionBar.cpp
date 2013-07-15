/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "TopologyDimensionBar.h"

TopologyDimensionBar::TopologyDimensionBar( const std::vector<long> &       dims,
                                            const std::vector<std::string> &dimnames,
                                            QWidget*                        parent ) :  QWidget( parent ), dims_( dims )
{
    setWhatsThis( "This widget controls how multi-dimensional topologies are mapped to an (x,y) or (x,y,z) presentation."
                  " It allows either to fold multiple dimensions together on the same axis or"
                  " slice fixing coordinates of some dimensions."
                  " In fold mode, drag the dimension labels to the desired axis row in the table."
                  " In slice mode, two or three dimensions can have all of their coordinates shown"
                  " (on the axis whose label has been dragged beneath them) while a specific coordinate"
                  " can be chosen from the other dimensions using the menu above each dimension label."
                  " When the widget is wider than the panel, the contents can be scrolled horizontally"
                  " with the right and left arrow keys (after it has been selected)." );

    modeSelection = new QStackedLayout();

    // fill dnames with dimension names or enumerate them
    QStringList dnames;
    if ( dimnames.size() == dims.size() )
    {
        for ( uint i = 0; i < dimnames.size(); ++i )
        {
            dnames.append( QString( dimnames.at( i ).c_str() ) );
        }
    }
    else
    {
        for ( uint i = 0; i < dims.size(); ++i )
        {
            dnames.append( QString::number( i + 1 ) );
        }
    }


    QGroupBox* groupBox = new QGroupBox();
    groupBox->setStyleSheet( "border:0;" );
    foldBut = new QRadioButton( "fold", this );
    foldBut->setChecked( true );
    sliceBut = new QRadioButton( "slice", this );
    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->setContentsMargins( 0, 0, 0, 0 );
    vbox->addWidget( foldBut );
    vbox->addWidget( sliceBut );
    groupBox->setLayout( vbox );
    groupBox->setFlat( true );
    connect( foldBut, SIGNAL( clicked() ), this, SLOT( foldingDimensionsChanged() ) );
    connect( sliceBut, SIGNAL( clicked() ), this, SLOT( selectedDimensionsChanged() ) );

    QWidget* stacked = new QWidget( this );
    stacked->setContentsMargins( 0, 0, 0, 0 );
    stacked->setLayout( modeSelection );

    order = new OrderWidget( dims, dnames );
    if ( dims.size() < 2 )    // do not show panel for 1 dimensional topologies
    {
        setLayout( new QHBoxLayout() );
        return;
    }
    else if ( dims.size() == 2 )
    {
        sliceBut->setVisible( false ); // slice mode makes no sence for 2D Topologies
        foldBut->setVisible( false );
    }

    // axis label
    asisLabel = new QLabel();
    setAxisLabel( 3 ); // default for folding mode

    // ------ dimension selection mode -------
    selection = new DimensionSelectionWidget( dims, dnames );
    connect( selection, SIGNAL( selectedDimensionsChanged() ), this, SLOT( selectedDimensionsChanged() ) );

    // minimize layout
    QWidget*     selectionWidget = new QWidget( this );
    QHBoxLayout* selL            = new QHBoxLayout();
    selL->setContentsMargins( 0, 0, 0, 0 );
    selL->addWidget( selection );
    selectionWidget->setLayout( selL );

    // ------ folding mode -------
    connect( order, SIGNAL( foldingDimensionsChanged() ), this, SLOT( foldingDimensionsChanged() ) );

    QWidget*     foldWidget = new QWidget( this );
    QHBoxLayout* fold       = new QHBoxLayout();
    fold->setContentsMargins( 0, 0, 0, 0 );
    fold->addWidget( order );
    QSpacerItem* space2 = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    fold->addItem( space2 );
    foldWidget->setLayout( fold );

    // ------ -------
    modeSelection->addWidget( foldWidget );
    modeSelection->addWidget( selectionWidget );

    // ------ -------
    QHBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins( 0, 0, 0, 0 );
    mainLayout->addWidget( asisLabel );
    mainLayout->addWidget( groupBox );
    mainLayout->addWidget( stacked );
    setLayout( mainLayout );

    this->setMaximumHeight( minimumSizeHint().height() );
}

std::vector<std::vector<int> >
TopologyDimensionBar::getFoldingVector()
{
    return order->getFoldingVector();
}

void
TopologyDimensionBar::setAxisLabel( int dims )
{
    QString img;
    if ( dims == 2 ) // two dimensions
    {
        img = ":/images/yx_small.png";
    }
    else  // three dimensions or invalid selection
    {
        img = ":/images/yzx_small.png";
    }
    QPixmap       pix( img );
    const QBitmap mask = pix.createMaskFromColor( QColor( 255, 255, 255, 0 ).rgb() ); // transparent background for white
    pix.setMask( mask );
    asisLabel->setPixmap( pix );
}

/**
   emit dimensionsChanged, if the selected values are valid (3 full dimensions)
 */
void
TopologyDimensionBar::selectedDimensionsChanged()
{
    modeSelection->setCurrentIndex( foldBut->isChecked() ? 0 : 1 );
    std::vector<long> val = selection->getSelectionVector();
    emit              selectedDimensionsChanged( val );

    // count number of selected dimensions
    int dims = 0;
    for ( uint i = 0; i < val.size(); i++ )
    {
        if ( val[ i ] != 0 )
        {
            dims++;
        }
    }
    setAxisLabel( dims );
}
/**
   emit dimensionsChanged, if new combinations of dimensions are selected
 */
void
TopologyDimensionBar::foldingDimensionsChanged()
{
    modeSelection->setCurrentIndex( foldBut->isChecked() ? 0 : 1 );
    std::vector<std::vector<int> > val = order->getFoldingVector();
    emit                           foldingDimensionsChanged( val );

    setAxisLabel( 3 ); // always show 3 dimensions in folding mode
}



/************************ settings ***************************/

//save topology settings
//
void
TopologyDimensionBar::saveSettings( QSettings& settings, int topologyId  )
{
    if ( selection == 0 )
    {
        return;
    }

    QString groupName( "systemTopology" );
    groupName.append( QString::number( topologyId ) );
    settings.beginGroup( groupName );

    settings.setValue( "selectMode", sliceBut->isChecked() );
    std::vector<long> selectedDimensions = selection->getSelectionVector();
    QList<QVariant>   save1;
    for ( uint i = 0; i < selectedDimensions.size(); ++i )
    {
        save1.append( ( int )selectedDimensions[ i ] );
    }
    settings.setValue( "selectedDimensions", QVariant( save1 ) );

    QList<QVariant>                save2;
    std::vector<std::vector<int> > merge = order->getFoldingVector();
    for ( uint i = 0; i < merge.size(); ++i )
    {
        QList<QVariant> line;
        for ( uint j = 0; j < merge[ i ].size(); ++j )
        {
            line.append( merge[ i ][ j ] );
        }
        save2.append( QVariant( line ) );
    }
    settings.setValue( "mergedDimensions", QVariant( save2 ) );

    //settings.setValue( "userDefinedMinMaxValues", userDefinedMinMaxValues );
    //settings.setValue( "userMinValue", userMinValue );
    //settings.setValue( "userMaxValue", userMaxValue );

    settings.endGroup();
}

bool
TopologyDimensionBar::loadSettings( QSettings& settings, int topologyId )
{
    QString groupName = "systemTopology";
    groupName.append( QString::number( topologyId ) );
    settings.beginGroup( groupName );

    typedef enum { SELECT, FOLD } Mode;
    bool              slice = settings.value( "selectMode" ).toBool();

    QList<QVariant>   list = settings.value( "selectedDimensions" ).toList();

    std::vector<long> selectedDimensions;
    for ( int i = 0; i < list.size(); i++ )
    {
        selectedDimensions.push_back( list[ i ].toInt() );
    }

    QList<QVariant>                list2 = settings.value( "mergedDimensions" ).toList();
    std::vector<std::vector<int> > foldingVec;
    for ( int i = 0; i < list2.size(); ++i )
    {
        QList<QVariant>  line = list2[ i ].toList();
        std::vector<int> foldLine;
        for ( int j = 0; j < line.size(); ++j )
        {
            foldLine.push_back( line[ j ].toInt() );
        }
        foldingVec.push_back( foldLine );
    }

    settings.endGroup();

    if ( selectedDimensions.size() == 0 )
    {
        return true;
    }
    selection->setSelectionVector( selectedDimensions );
    order->setFoldingVector( foldingVec );

    if ( slice )
    {
        sliceBut->setChecked( true );
        selectedDimensionsChanged();
    }
    else
    {
        foldingDimensionsChanged();
    }
    //userDefinedMinMaxValues = settings.value( "userDefinedMinMaxValues", false ).toBool();
    //userMinValue            = settings.value( "userMinValue", 0.0 ).toDouble();
    //userMaxValue            = settings.value( "userMaxValue", 0.0 ).toDouble();

    return true;
}
