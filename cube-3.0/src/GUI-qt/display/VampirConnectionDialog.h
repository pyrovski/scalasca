/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef VAMPIR_CONNECTION_DIALOG_H
#define VAMPIR_CONNECTION_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>

#include "Statistics.h"

class VampirConnectionThread;

class VampirConnectionDialog : public QDialog
{
    Q_OBJECT
    public:
        VampirConnectionDialog(QWidget *par, QLineEdit *&fileEdit,
                               Statistics *stat, MainWidget *mainWid);
        void setError(std::string const &title, std::string const &message);
        virtual ~VampirConnectionDialog();
    private:
        QLineEdit *fileLine;
        QLineEdit *hostLine;
        QSpinBox *portLine;
        QPushButton *fileButton;
        QPushButton *cancelButton;
        QPushButton *okButton;
        QCheckBox *fromServerCheckbox;
        Statistics *statistics;
        VampirConnectionThread *connectionThread;
        std::string errorTitle, errorMessage;
    private slots:
        void establishVampirConnection();
        void printError();
};

#endif

