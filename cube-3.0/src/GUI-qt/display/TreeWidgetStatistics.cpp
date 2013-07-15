/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <cassert>
#include <string>

#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>

#include "TreeWidget.h"
#include "MessageWidget.h"

#include "StatisticInfo.h"
#include "Statistics.h"
#include "BoxPlot.h"

#include "Cube.h"
#include "Metric.h"
#include "Cnode.h"

using namespace std;

void
TreeWidget::createStatisticsContextMenuItems()
{
    //for metric trees create an action for expert statistics
    //and for connections to a trace browser
    if ( statistics != 0 && statistics->existsStatFile() )
    {
        if ( type == METRICTREE )
        {
            contextMenu.addSeparator();

            statisticsAct = new Action( tr( "Statistics" ), this );
            statisticsAct->setStatusTip( tr( "Shows metric statistics" ) );
            statisticsAct->setWhatsThis( tr( "Shows metric statistics. Only available if a statistics file for the current cube file exists and if statistical information for the selected metric is provided." ) );
            connect( statisticsAct, SIGNAL( triggered() ), this, SLOT( onShowStatistics() ) );
            contextMenu.addAction( statisticsAct );
        }
      #if defined( WITH_VAMPIR ) || defined( WITH_PARAVER )
        if ( type == METRICTREE || type == CALLTREE )
        {
            traceAct = new Action( tr( "Max severity in trace browser" ), this );
            traceAct->setStatusTip( tr( "Shows the most severe instance of pattern in trace browser" ) );
            traceAct->setWhatsThis( tr( "Shows the most severe instance of pattern in trace browser. Only available if a statistics file for the current cube file exists and if a connection to a trace browser was established." ) );
            connect( traceAct, SIGNAL( triggered() ), this, SLOT( onShowMaxSeverity() ) );
            contextMenu.addAction( traceAct );
            if ( !statistics->existsStatFile() )
            {
                traceAct->setVisible( false );
            }
        }
      #endif
    }
}

void
TreeWidget::onShowStatistics()
{
    assert( statistics != 0 );
    assert( clickedItem != NULL );
    assert( type == METRICTREE );

    QDialog* dialog = new QDialog( this );
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->setWindowTitle( "Statistics info" );
    BoxPlot*                       theBoxPlot = new BoxPlot( dialog, statistics->getPrecisionWidget() );

    vector<TreeWidgetItem*> const &vec   = selectedItems();
    bool                           added = false;
    for ( vector<TreeWidgetItem*>::const_iterator it = vec.begin(); it != vec.end(); ++it )
    {
        if ( statistics->existsStatistics( static_cast<cube::Metric*>( ( *it )->cubeObject ) ) )
        {
            theBoxPlot->Add( statistics->getItem( static_cast<cube::Metric*>( ( *it )->cubeObject ) ) );
            if ( *it == clickedItem )
            {
                added = true;
            }
        }
    }
    if ( !added )
    {
        theBoxPlot->Add( statistics->getItem( static_cast<cube::Metric*>( clickedItem->cubeObject ) ) );
    }

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( theBoxPlot );

    QPushButton* okButton = new QPushButton( "Close", dialog );
    connect( okButton, SIGNAL( pressed() ), dialog, SLOT( accept() ) );
    layout->addWidget( okButton );

    dialog->setLayout( layout );

    dialog->setModal( false );
    dialog->show();
}

#if defined( WITH_VAMPIR ) || defined( WITH_PARAVER )
void
TreeWidget::onShowMaxSeverity()
{
    assert( statistics != 0 );
    assert( statistics->isConnected() );
    assert( clickedItem != 0 );
    assert( type == METRICTREE || type == CALLTREE );
    string str;
    if ( type == METRICTREE )
    {
        str = statistics->showMaxSeverity( static_cast<cube::Metric*>( clickedItem->cubeObject ) );
    }
    else if ( type == CALLTREE )
    {
        cube::Metric* metric = static_cast<cube::Metric*>( tabWidget->getMetricTree()->selectedItems().at( 0 )->cubeObject );
        str = statistics->showMaxSeverity( metric, static_cast<cube::Cnode*>( clickedItem->cubeObject ) );
    }
    if ( str != "" )
    {
        MessageWidget* wwidget = new MessageWidget( Error, str.c_str() );
        wwidget->show();
//         QMessageBox::critical(this, "Statistics", str.c_str());
    }
}
#endif

void
TreeWidget::statisticsMousePressEvent()
{
    //the statistics action serves with some expert statistics if available
    if ( statistics != 0 && statistics->existsStatFile() )
    {
        if ( type == METRICTREE )
        {
            bool enableStatistics = false;
            if ( clickedItem != 0 && statistics != 0
                 && statistics->existsStatistics( static_cast<cube::Metric*>( clickedItem->cubeObject ) ) )
            {
                enableStatistics = true;
            }
            statisticsAct->setEnabled( enableStatistics );
        }

        #if defined( WITH_VAMPIR ) || defined( WITH_PARAVER )
        //the trace action offers the ability to connect to let the trace
        //browser show the most severe instance of a pattern
        if ( type == METRICTREE )
        {
            if ( clickedItem != 0 && statistics != 0 && statistics->isConnected()
                 && statistics->existsMaxSeverity( static_cast<cube::Metric*>( clickedItem->cubeObject ) ) )
            {
                traceAct->setEnabled( true );
            }
            else
            {
                traceAct->setEnabled( false );
            }
        }
        if ( type == CALLTREE )
        {
            cube::Metric* metric = static_cast<cube::Metric*>( tabWidget->getMetricTree()->selectedItems().at( 0 )->cubeObject );
            if ( clickedItem != 0 && statistics != 0 && statistics->isConnected() && tabWidget->getMetricTree()->selectedItems().size() == 1
                 && statistics->existsMaxSeverity( metric, static_cast<cube::Cnode*>( clickedItem->cubeObject ) ) )
            {
                traceAct->setEnabled( true );
            }
            else
            {
                traceAct->setEnabled( false );
            }
        }
        #endif
    }
}

void
TreeWidget::updateStatisticsSelection()
{
    if ( statistics != 0 && statistics->existsStatFile() )
    {
        if ( type == METRICTREE || type == CALLTREE )
        {
            std::vector<TreeWidgetItem*> metrics = tabWidget->getMetricTree()->selectedItems();

            std::vector<TreeWidgetItem*> cnodes_with_stat;
            std::vector<TreeWidgetItem*> cnodes = tabWidget->getCallTree()->items;

            // clear stat flag for all items in the call tree
            for ( std::vector<TreeWidgetItem*>::iterator c_iter = cnodes.begin();
                  c_iter != cnodes.end(); c_iter++ )
            {
                ( *c_iter )->clearStatisticFlag();
            }

            // collect for selected metric cnodes with statistics
            for ( std::vector<TreeWidgetItem*>::iterator m_iter = metrics.begin();
                  m_iter != metrics.end(); m_iter++ )
            {
                cube::Metric* metric = static_cast<cube::Metric*>( ( *m_iter )->cubeObject );

                for ( std::vector<TreeWidgetItem*>::iterator c_iter = cnodes.begin();
                      c_iter != cnodes.end(); c_iter++ )
                {
                    if ( statistics->existsMaxSeverity( metric, static_cast<cube::Cnode*>( ( *c_iter )->cubeObject ) ) )
                    {
                        cnodes_with_stat.push_back( *c_iter );
                    }
                }
            }

            // set up stat flag for all cnodes with statistics
            // in order to mark them with frames later
            for ( std::vector<TreeWidgetItem*>::iterator c_iter = cnodes_with_stat.begin();
                  c_iter != cnodes_with_stat.end(); c_iter++ )
            {
                TreeWidgetItem* _item = *c_iter;
                while ( !_item->isVisible() )
                {
                    _item = _item->parent();
                }
                _item->setStatisticFlag();
            }
            tabWidget->getCallTree()->update();
        }
    }
}
