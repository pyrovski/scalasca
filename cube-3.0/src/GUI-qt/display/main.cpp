/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "MainWidget.h"

#include <iostream>

//this is the main method
//handles command-line arguments and starts the GUI

int main(int argc, char *argv[])
{

  QApplication app(argc, argv);

  if (argc>2){
    std::cout << "Usage: cube3-qt [filename]\n";
    return 0;
  }

  MainWidget mainWidget;
  mainWidget.show();
  if (argc==2){
      QString name = QString::fromStdString(argv[1]);
      //if the command line argument is a directory then 
      //open a file dialog for getting the file name
      if (!QFile(name).exists() || QDir(name).exists()){
	name = QFileDialog::getOpenFileName(NULL,
					    app.tr("Choose a file to open"),
					    name,
#ifndef CUBE_COMPRESSED
					    app.tr("Cube files (*.cube);;All files (*.*);;All files (*)"));
#else
					    app.tr("Cube files (*.cube.gz *.cube);;All files (*.*);;All files (*)"));
#endif
      }
      if (name.length()>0) mainWidget.readFile(name);
  } 

  return app.exec();
}
