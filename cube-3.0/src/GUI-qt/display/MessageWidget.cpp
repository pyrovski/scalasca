/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef _MESSAGEWIDGET_CPP
#define _MESSAGEWIDGET_CPP

#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QMouseEvent>
#include <QApplication>

#include <iostream>

#include "MessageWidget.h"


MessageWidget::MessageWidget(HintKind kind, QString message):QWidget(0, Qt::FramelessWindowHint)
{
    mykind = kind;
    unsigned ms = 10000;



    QColor background;
    setAutoFillBackground ( true );
    QPalette p( palette());

    switch (kind)
    {
        case Message:
            warning_message = message;
            background=QColor(206, 240,255);
            ms = 6000;
            break;

        case Hint :
            warning_message = "HINT: " + message;
            background=Qt::yellow;
            ms = 5000;
            // duplicate messaage in STDERR output stream.
            std::cout << warning_message.toUtf8().data() << std::endl;
            break;
        case Warning :
            warning_message = "WARNING: " + message;
            background=QColor(250, 200,200);
            ms = 6000;
            // duplicate messaage in STDERR output stream.
            std::cerr << warning_message.toUtf8().data() << std::endl;
            break;
        case Error :
            warning_message = "ERROR: " + message;
            background=QColor(250, 100,100);
            ms = 10000;
            // duplicate messaage in STDERR output stream.
            std::cerr << warning_message.toUtf8().data() << std::endl;
            break;
        default:
            warning_message = message;
            background=p.color(QPalette::Window);
            break;
    }



    setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout * w_layout = new QVBoxLayout();
    QLabel * label = new QLabel(warning_message);
    w_layout->addWidget(label);
    label->setWordWrap(true);
//     w_layout->setSpacing(30);
    setLayout(w_layout);
//    resize(1,1);


    p.setColor(QPalette::Window, background);
    setPalette( p);
    w_layout->activate();

    // getting place.

    QPoint place;
    QSize ssize;

    // getting size and place of the top window
   foreach (QWidget *widget, QApplication::topLevelWidgets()) 
    {
         if (!widget->isHidden())
            {
                place = widget->pos();
                ssize = widget->size();
                break; // assuming, first window is the CUBE GUI application window
            }
    }




    // Calculation of the placefor the hint.
    QPoint hint_place;
/*
    if (place.x() > 10) 
        hint_place.setX(place.x() - 10);
    else
        hint_place.setX(place.x() + ssize.width()- width() + 10);

    if (place.y() > 10) 
        hint_place.setY(place.y() - 10);
    else
        hint_place.setY( place.y() + ssize.height() - height() + 20);
*/
    if ( true || mykind == Message) // true - makes that all messages appear in the middle of the window
    {
        // Messages should be shown in teh center of man widget 
        hint_place.setY( place.y() + (unsigned)((double)(ssize.height() - height())/2.) );
        hint_place.setX (place.x() + (unsigned)((double)(ssize.width()  - width())/2.));

    }
    
    move(hint_place);
//     show();
    QTimer::singleShot(ms, this, SLOT(close()));
}



void MessageWidget::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    this->close();
}
     

#endif
