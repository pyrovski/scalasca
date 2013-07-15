/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "MainWidget.h"
#include "Statistics.h"

#include <cassert>
#include <iostream>

#include "Action.h"

#include <QThread>

using namespace std;

#if defined(WITH_VAMPIR)
#include "VampirConnectionDialog.h"

class VampirConnectionThread : public QThread
{
    public:
        VampirConnectionThread(string const &host, int port, string const &file,
                               VampirConnectionDialog *dia, Statistics *stat);
        virtual void run();
    private:
        string hostName;
        string fileName;
        int portNumber;
        Statistics *statistics;
        VampirConnectionDialog *dialog;
};

VampirConnectionThread::VampirConnectionThread(string const &host, int port,
      string const &file, VampirConnectionDialog *dia, Statistics *stat)
    : hostName(host), fileName(file), portNumber(port), statistics(stat),
      dialog(dia)
{
}

void VampirConnectionThread::run()
{
    string result = statistics->connectToVampir(hostName, portNumber, fileName);
    if(result != "")
    {
        dialog->setError("Vampir connection", result);
    }
}

VampirConnectionDialog::VampirConnectionDialog(QWidget *par,
      QLineEdit *&fileEdit, Statistics *stat, MainWidget *mainWid)
    : QDialog(par), statistics(stat), connectionThread(0)
{
    int const spacing = 20;

    setWindowTitle("Connect to vampir");

    QFontMetrics fm(font());
    int h = 10 * fm.ascent();
    int w = 4 * fm.width("Connect to vampir:");
    setMinimumSize(w,h);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(spacing);

    QGridLayout *layout1 = new QGridLayout();
    layout1->setSpacing(spacing);

    //define host label and editor
    QLabel *hostLabel = new QLabel();
    hostLabel->setText("Host:");
    hostLabel->setDisabled(true);
    layout1->addWidget(hostLabel,1,0);

    hostLine = new QLineEdit("localhost");
    hostLine->setDisabled(true);
    layout1->addWidget(hostLine,1,1);

    //define port label and editor
    QLabel *portLabel = new QLabel();
    portLabel->setText("Port:");
    portLabel->setDisabled(true);
    layout1->addWidget(portLabel,2,0);

    portLine = new QSpinBox();
    portLine->setRange(0,1000000);
    portLine->setSingleStep(1);
    portLine->setValue(30000);
    portLine->setDisabled(true);
    layout1->addWidget(portLine,2,1);

    //set checkbox to change between a local or remote trace file
    fromServerCheckbox = new QCheckBox("Open local file");
    fromServerCheckbox->setChecked(true);
    layout1->addWidget(fromServerCheckbox,0,0,1,2);
    connect(fromServerCheckbox,SIGNAL(toggled(bool)),portLine,SLOT(setDisabled(bool)));
    connect(fromServerCheckbox,SIGNAL(toggled(bool)),hostLine,SLOT(setDisabled(bool)));
    connect(fromServerCheckbox,SIGNAL(toggled(bool)),portLabel,SLOT(setDisabled(bool)));
    connect(fromServerCheckbox,SIGNAL(toggled(bool)),hostLabel,SLOT(setDisabled(bool)));

    layout->addLayout(layout1);

    //define file label and editor

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->setSpacing(spacing);

    QLabel *fileLabel = new QLabel();
    fileLabel->setText("File:");
    layout2->addWidget(fileLabel);

    fileEdit = fileLine =
                  new QLineEdit(statistics->getDefaultVampirFileName().c_str());
    layout2->addWidget(fileLine);
    fileButton = new QPushButton("Browse");
    layout2->addWidget(fileButton);
    connect(fileButton,SIGNAL(clicked()),mainWid,SLOT(getTraceFileName()));

    layout->addLayout(layout2);

    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    okButton = buttonBox->addButton(QDialogButtonBox::Ok);
    cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);
    connect(okButton, SIGNAL(clicked()), this, SLOT(establishVampirConnection()));  
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    layout->addWidget(buttonBox);

    setLayout(layout);
}

void VampirConnectionDialog::setError(string const &title, string const &message)
{
    errorTitle = title;
    errorMessage = message;
}

void VampirConnectionDialog::printError()
{
    if(errorMessage != "")
    {
        QMessageBox::critical(this, QString::fromStdString(errorTitle),
                              QString::fromStdString(errorMessage));
    }
    setAttribute(Qt::WA_DeleteOnClose);
    accept();
}

void VampirConnectionDialog::establishVampirConnection()
{
    okButton->setEnabled(false);
    cancelButton->setEnabled(false);
    fileButton->setEnabled(false);

    assert(statistics != 0);
    if(fromServerCheckbox->isChecked()){
        connectionThread =
          new VampirConnectionThread("",0,
              fileLine->displayText().toStdString(), this,
              statistics);
    }
    else{
        connectionThread =
          new VampirConnectionThread(hostLine->displayText().toStdString(),
              portLine->value(), fileLine->displayText().toStdString(), this,
              statistics);
    }
    connect(connectionThread, SIGNAL(finished()), this, SLOT(printError()));
    connectionThread->start();
}

VampirConnectionDialog::~VampirConnectionDialog()
{
    delete connectionThread;
}
#endif

void MainWidget::createTraceBrowserMenu()
{
    traceBrowserMenu = 0;
#if defined(WITH_VAMPIR) || defined(WITH_PARAVER)
    traceBrowserMenu = fileMenu->addMenu("Connect to trace browser");
    traceBrowserMenu->setStatusTip(tr("Ready"));
    traceBrowserMenu->setEnabled(false);
    traceBrowserMenu->menuAction()->setVisible(false);
#endif

#if defined(WITH_VAMPIR)
    vampirAct = 0;
    vampirAct = new Action(tr("Connect to &vampir..."), this);
    vampirAct->setStatusTip(tr("Connect to vampir and display a trace file"));
    vampirAct->setWhatsThis("Connect to vampir and display a trace file");
    connect(vampirAct, SIGNAL(triggered()), this, SLOT(vampirMenu()));
    traceBrowserMenu->addAction(vampirAct);
    vampirAct->setEnabled(false);
    vampirAct->setVisible(false);
#endif

#if defined(WITH_PARAVER)
    paraverAct = 0;
    paraverAct = new Action(tr("Connect to &paraver..."), this);
    paraverAct->setStatusTip(tr("Connect to paraver and display a trace file"));
    paraverAct->setWhatsThis("Connect to paraver and display a trace file");
    connect(paraverAct, SIGNAL(triggered()), this, SLOT(paraverMenu()));
    traceBrowserMenu->addAction(paraverAct);
    paraverAct->setEnabled(false);
    paraverAct->setVisible(false);
#endif
}

void MainWidget::initializeTraceBrowserMenu(QString const &)
{
    //initialize statistics info
    if(statistics->existsMaxSeverities())
    {
        toggleTraceBrowserMenuVisibility(true);
    }
}

void MainWidget::toggleTraceBrowserMenuVisibility(bool visible)
{
    if(traceBrowserMenu != 0)
    {
        traceBrowserMenu->setEnabled(visible);
        traceBrowserMenu->menuAction()->setVisible(visible);
    }
#if defined(WITH_VAMPIR)
    if(vampirAct != 0)
    {
        vampirAct->setEnabled(visible);
        vampirAct->setVisible(visible);
    }
#endif
#if defined(WITH_PARAVER)
    if(paraverAct != 0)
    {
        paraverAct->setEnabled(visible);
        paraverAct->setVisible(visible);
    }
#endif
}

void MainWidget::disableTraceBrowserMenu()
{
    if(statistics != 0 && statistics->existsMaxSeverities())
    {
        toggleTraceBrowserMenuVisibility(false);
    }
}

#if defined(WITH_VAMPIR)
//slot for connecting to vampir and displaying traces there
void MainWidget::vampirMenu()
{
    VampirConnectionDialog *dialog = new VampirConnectionDialog(this, fileLine,
                                         statistics, this);

    dialog->setModal(false);
    dialog->show();
}
#endif

#define MAKE_STR(x) _MAKE_STR(x)
#define _MAKE_STR(x) #x

#if defined(WITH_PARAVER)
void MainWidget::paraverMenu()
{
    int const spacing = 20;

    QDialog dialog(this);
    dialog.setWindowTitle("Connect to paraver");

    QFontMetrics fm(font());
    int h = 10 * fm.ascent();
    int w = 4 * fm.width("Connect to paraver:");
    dialog.setMinimumSize(w,h);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(spacing);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->setSpacing(spacing);

    //define configuration file label and editor
    QLabel *configLabel = new QLabel();
    configLabel->setText("Configuration file:");
    layout1->addWidget(configLabel);

    configLine = new QLineEdit(MAKE_STR(PV_DEF_CFG));
    layout1->addWidget(configLine);
    QPushButton *configFileButton = new QPushButton("Browse");
    layout1->addWidget(configFileButton);
    connect(configFileButton, SIGNAL(clicked()), this, SLOT(getConfigFileName()));

    layout->addLayout(layout1);

    //define file label and editor
    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->setSpacing(spacing);

    QLabel *fileLabel = new QLabel();
    fileLabel->setText("Trace file:");
    layout2->addWidget(fileLabel);

    fileLine = new QLineEdit(statistics->getDefaultParaverFileName().c_str());
    layout2->addWidget(fileLine);
    QPushButton *fileButton = new QPushButton("Browse");
    layout2->addWidget(fileButton);
    connect(fileButton,SIGNAL(clicked()),this,SLOT(getTraceFileName()));

    layout->addLayout(layout2);

    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    QPushButton *okButton = buttonBox->addButton(QDialogButtonBox::Ok);
    QPushButton *cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);
    connect(okButton, SIGNAL(clicked()), &dialog, SLOT(accept()));  
    connect(cancelButton, SIGNAL(clicked()), &dialog, SLOT(reject()));
    layout->addWidget(buttonBox);

    dialog.setLayout(layout);

    if(dialog.exec())
    {
        assert(statistics != 0);
        std::string result =
          statistics->connectToParaver(configLine->displayText().toStdString(),
                                       fileLine->displayText().toStdString());
        if(result != "")
        {
            QMessageBox::critical(this, tr("Paraver connection"),
                                  QString::fromStdString(result));
        }
    }
}
#endif

#undef _MAKE_STR
#undef MAKE_STR

void MainWidget::getTraceFileName()
{

    QString openFileName =
      QFileDialog::getOpenFileName(this, tr("Choose a file to open"),
        fileLine->displayText(), 
        tr("Trace files (*.esd *.elg *.otf *.prv);;All files (*.*);;All files (*)"));
    if(openFileName.length() == 0)
    {
        statusBar()->showMessage(tr("Ready"));
        return;
    }
    fileLine->setText(openFileName);
}

#if defined(WITH_PARAVER)
void MainWidget::getConfigFileName()
{

    QString configFileName = QFileDialog::getOpenFileName(this,
      tr("Choose a file to open"), configLine->displayText(), 
      tr("Config files (*.cfg);;All files (*.*);;All files (*)"));
    if(configFileName.length() == 0)
    {
        statusBar()->showMessage(tr("Ready"));
        return;
    }
    configLine->setText(configFileName);
}
#endif

