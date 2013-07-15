/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <QtGui>
#include "SystemTopologyWidget.h"

class SystemTopologyToolBar : public QToolBar
{
    Q_OBJECT
public:
    static SystemTopologyToolBar*
    getInstance();
    void
    setParent( SystemTopologyWidget* parent );

    void
    enableTopologyButtons();
    void
    disableTopologyButtons();

private slots:
    void
    setButtonsToIcon();
    void
    setButtonsToText();
    void
    moveDown();
    void
    moveUp();
    void
    moveRight();
    void
    moveLeft();
    void
    increasePlaneDistance();
    void
    decreasePlaneDistance();
    void
    zoomIn();
    void
    zoomOut();
    void
    reset();
    void
    scale();
    void
    setXAngle( int angle );
    void
    setYAngle( int angle );

    //get user-defined minimal and maximal values for the minimal and maximal colors
    void
    minMaxValues();

private:
    SystemTopologyToolBar();
    static SystemTopologyToolBar* single;
    SystemTopologyWidget*         parent;
    QAction*                      leftAct, * rightAct, * upAct, * downAct;
    QAction*                      dist1Act, * dist2Act, * zoom1Act, * zoom2Act;
    QAction*                      resetAct, * scaleAct, * userAct;
    QSpinBox*                     xSpin, * ySpin;
    QLabel*                       xLabel, * yLabel;
};
