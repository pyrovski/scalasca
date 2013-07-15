/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "MainWidget.h"
#include "TabWidget.h"
#include "TypedWidget.h"
#include "PrecisionWidget.h"
#include "MessageWidget.h"
#include "Action.h"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <fstream>

#include <QSplitter>
#include <QToolBar>
#include <QSettings>


//in this file I placed the settings-related methods of MainWidget


//get a name under which settings should be saved 
//and save settings
//
void MainWidget::saveSettings(){

  QSettings settings("FZJ","Cube");

  //get the last used setting name for default name
  QString oldSettingsName = settings.value("current/name","").toString();

  //get the list of all strored settings for choice
  settings.beginGroup("settings");
  QStringList settingsList = settings.childGroups();
  settings.endGroup();

  //search the default setting name in the list
  int index = 0;
  for (int i=0; i<settingsList.size(); i++){
    if (settingsList[i].compare(oldSettingsName)==0){
      index = i;
      break;
    }
  }

  settingsList.push_back("<new setting>");

  //get a name selection from the user
  bool ok;
  QString settingsName = 
    QInputDialog::getItem(this,
			  "Save settings",
			  "Save settings under the name:",
			  settingsList,
			  index,
			  true,
			  &ok);

  //save settings if everything is ok with the input
  if (ok && !settingsName.isEmpty()){
    saveSettings(settingsName);
  }
}


//get a name under which settings should be saved 
//and save settings
//
void MainWidget::saveDefaultSettings()
{

    saveSettings("<default>");
  
}



//save settings under the parameter name settingsName
//
void MainWidget::saveSettings(const QString& settingsName){

  //go into the settings 
  QSettings settings("FZJ","Cube");
  //store that settingsName was the last edited setting;
  //used for default value
  settings.setValue("current/name",settingsName);
  
  //go down into the settings library
  settings.beginGroup("settings");
  //go down into the settings/settingsName library
  settings.beginGroup(settingsName);
  //go down into the settings/settigsName/MainWindow library
  settings.beginGroup("MainWindow");

  //save position and size of the main window
  settings.setValue("coords/size", size());
  settings.setValue("coords/pos", pos());

  //tree widget display
  settings.setValue("spacing",spacing);
  settings.setValue("treeFont",treeFont);
  settings.setValue("treeFontSize",treeFont.pointSize());

  //save precision values
  settings.setValue("precision/prec0",precisionWidget->getPrecision(FORMAT0));
  settings.setValue("precision/round0",precisionWidget->getRoundNr(FORMAT0));
  settings.setValue("precision/exp0",precisionWidget->getUpperExpNr(FORMAT0));
  settings.setValue("precision/prec1",precisionWidget->getPrecision(FORMAT1));
  settings.setValue("precision/round1",precisionWidget->getRoundNr(FORMAT1));
  settings.setValue("precision/exp1",precisionWidget->getUpperExpNr(FORMAT1));

  //save coloring
  settings.setValue("color/lighten",colorWidget->getLightenStart());
  settings.setValue("color/white",colorWidget->getWhiteStart());
  settings.setValue("color/pos0",colorWidget->getColorPos(0));
  settings.setValue("color/pos1",colorWidget->getColorPos(1));
  settings.setValue("color/pos2",colorWidget->getColorPos(2));
  settings.setValue("color/pos3",colorWidget->getColorPos(3));
  settings.setValue("color/pos4",colorWidget->getColorPos(4));
  settings.setValue("color/method",colorWidget->getColoringMethod());

  //save the dimension order
  int metricIndex, callIndex, systemIndex;
  getWidgetIndices(metricIndex,callIndex,systemIndex);
  settings.setValue("indices/metric",metricIndex);
  settings.setValue("indices/call",callIndex);
  settings.setValue("indices/system",systemIndex);

  //save widths of the widgets in the splitter widget
  QList<int> splitterSizes = splitterWidget->sizes();
  QList<QVariant> variantList;
  for (int i=0; i<splitterSizes.size(); i++){
    QVariant v(splitterSizes[i]);
    variantList.push_back(v);
  }
  settings.setValue("splitterSizes",variantList);

  //save the value modi
  settings.setValue("valuemodus/metric",metricCombo->currentIndex());
  settings.setValue("valuemodus/call",callCombo->currentIndex());
  settings.setValue("valuemodus/system",systemCombo->currentIndex());

  //save topology toolbar style
  settings.setValue("toolbarstyle",topologyToolBar->toolButtonStyle());
  settings.setValue("toolbarHidden",topologyToolBar->isHidden());

  //save names of recent files
  for (int i=0; i<(int)openedFiles.size(); i++){
    QString storeUnder = "openedFiles";
    storeUnder.append(QString::number(i));
    settings.setValue(storeUnder,openedFiles[i]);
  }
  {
    QString storeUnder = "openedFiles";
    storeUnder.append(QString::number(openedFiles.size()));
    settings.setValue(storeUnder,"");
  }
  
  //save external file name
  settings.setValue("lastExternalFileName",lastExternalFileName);
  //save file size threshold above which dynamic loading is enabled
  settings.setValue("dynloadThreshold",dynloadThreshold);
  //save the status of the actions in menu Display/Topology
  settings.setValue("emptyActChecked",emptyAct->isChecked());
  settings.setValue("antialiasingActChecked",antialiasingAct->isChecked());

  //go up into the settings/settigsName library
  settings.endGroup();

  //check if data-related settings should be stored
  bool saveWithData;
  if (cube==NULL) saveWithData = false;
  else saveWithData = (QMessageBox::question(this, 
					     tr("Settings"),
					     "Save also data settings?",
					     QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes);
  //go down into the settings/settigsName/data library
  settings.beginGroup("data");
  //store data settings
  if (!saveWithData){
    settings.setValue("withdata",false);
  } else {
    settings.setValue("withdata",true);
    settings.setValue("filename",openedFiles[0]);

    metricTabWidget->saveSettings(settings);
    callTabWidget->saveSettings(settings);
    systemTabWidget->saveSettings(settings);
  }
  settings.endGroup();

  settings.endGroup();
  settings.endGroup();
}


//input setting name from the user and load the setting
//
void MainWidget::loadSettings(bool showError){

  QSettings settings("FZJ","Cube");
  //get default setting name, i.e., the name of the last edited setting
  QString oldSettingsName = settings.value("current/name","").toString();

  //get list of available settings
  settings.beginGroup("settings");
  QStringList settingsList = settings.childGroups();
  settings.endGroup();

  if (settingsList.size()==0){
    if (showError) 
        {
            MessageWidget * wwidget = new MessageWidget(Warning, "No settings found.");
            wwidget->show();
//             QMessageBox::warning(this,"Load settings error","No settings found.");
        }
    return;
  }

  //search for the setting which was used at latest
  int index = 0;
  for (int i=0; i<settingsList.size(); i++){
    if (settingsList[i].compare(oldSettingsName)==0){
      index = i;
      break;
    }
  }

  //for the display in the input dialog, extend the settings names
  //with the cube file name if also data was stored
  QStringList settingsList2;
  settings.beginGroup("settings");
  for (int i=0; i<settingsList.size(); i++){

    QString str = settingsList[i];

    settings.beginGroup(settingsList[i]);

    if (settings.value("data/withdata",false).toBool()){
      QString path = settings.value("data/filename","").toString();
      QChar pathSeparator = QDir::separator();
      if (path.at(path.length()-1) == pathSeparator)
	path.chop(1);
      int lastSeparator = path.lastIndexOf(pathSeparator);
      QString fileName = path.mid(lastSeparator+1);
      QString lastDirectory = path.left(lastSeparator);
      lastSeparator = lastDirectory.lastIndexOf(pathSeparator);
      if (lastSeparator==-1){
	lastDirectory = "";
      } else {
	lastDirectory = lastDirectory.mid(lastSeparator+1);
      }
    
      str.append(" (");
      str.append(lastDirectory);
      str.append(pathSeparator);
      str.append(fileName);
      str.append(")");
    }
    settings.endGroup();
    settingsList2.push_back(str);
  }
  settings.endGroup();

  //get setting name
  bool ok;
  QString settingsName = 
    QInputDialog::getItem(this,
			  "Load settings",
			  "Load settings with name:",
			  settingsList2,
			  index,
			  false,
			  &ok);

  //load setting if all right with the name
  if (ok && !settingsName.isEmpty()){
    int index = 0;
    for (int i=0; i<settingsList2.size(); i++){
      if (settingsList2[i].compare(settingsName)==0){
	index = i;
	break;
      }
    }
    loadSettings(settingsList[index]);
  }
}



//load a setting named settingsName
//
void MainWidget::loadSettings(const QString& settingsName){

  QSettings settings("FZJ","Cube");
  //store the name as last edited setting
  settings.setValue("current/name",settingsName);
  
  //go down into the settings library
  settings.beginGroup("settings");
  //go down into settings/settingsName
  settings.beginGroup(settingsName);
  //go down into settings/settingsName/MainWindow
  settings.beginGroup("MainWindow");

  //restore position and size of the main window
  resize(settings.value("coords/size",QSize(600,500)).toSize());
  move(settings.value("coords/pos",QPoint(200,200)).toPoint());

  //tree widget display
  spacing = settings.value("spacing",5).toInt();
  treeFont = settings.value("treeFont",font()).toString();
  treeFont.setPointSize(settings.value("treeFontSize",9).toInt());

  metricTabWidget->setTreeDisplay(treeFont,spacing);
  callTabWidget->setTreeDisplay(treeFont,spacing);
  systemTabWidget->setTreeDisplay(treeFont,spacing);

  //restore precision values
  precisionWidget->setPrecision(settings.value("precision/prec0",2).toInt(),FORMAT0);
  precisionWidget->setUpperExpNr(settings.value("precision/exp0",4).toInt(),FORMAT0);
  precisionWidget->setRoundNr(settings.value("precision/round0",7).toInt(),FORMAT0);
  precisionWidget->setPrecision(settings.value("precision/prec1",2).toInt(),FORMAT1);
  precisionWidget->setUpperExpNr(settings.value("precision/exp1",4).toInt(),FORMAT1);
  precisionWidget->setRoundNr(settings.value("precision/round1",7).toInt(),FORMAT1);

  //restore coloring
  colorWidget->setLightenStart(settings.value("color/lighten",0.0).toDouble());
  colorWidget->setWhiteStart(settings.value("color/white",0.0).toDouble());
  colorWidget->setColorPos(0,settings.value("color/pos0",0.0).toDouble());
  colorWidget->setColorPos(1,settings.value("color/pos1",0.1).toDouble());
  colorWidget->setColorPos(2,settings.value("color/pos2",0.2).toDouble());
  colorWidget->setColorPos(3,settings.value("color/pos3",0.3).toDouble());
  colorWidget->setColorPos(4,settings.value("color/pos4",1.0).toDouble());
  colorWidget->setColoringMethod((ColoringMethod)(settings.value("color/method",EXPONENTIAL2).toInt()));
  colorWidget->update();
  colorWidget->repaint();

  //restoring the dimension order
  int metricIndex = settings.value("indices/metric",0).toInt();
  int callIndex = settings.value("indices/call",1).toInt();
  QWidget* metricWidget, *callWidget, *systemWidget;

  if (((TypedWidget*)(splitterWidget->widget(0)))->getType()==METRICWIDGET)
    metricWidget = splitterWidget->widget(0);
  else if (((TypedWidget*)(splitterWidget->widget(1)))->getType()==METRICWIDGET)
    metricWidget = splitterWidget->widget(1);
  else metricWidget = splitterWidget->widget(2);

  if (((TypedWidget*)(splitterWidget->widget(0)))->getType()==CALLWIDGET)
    callWidget = splitterWidget->widget(0);
  else if (((TypedWidget*)(splitterWidget->widget(1)))->getType()==CALLWIDGET)
    callWidget = splitterWidget->widget(1);
  else callWidget = splitterWidget->widget(2);

  if (((TypedWidget*)(splitterWidget->widget(0)))->getType()==SYSTEMWIDGET)
    systemWidget = splitterWidget->widget(0);
  else if (((TypedWidget*)(splitterWidget->widget(1)))->getType()==SYSTEMWIDGET)
    systemWidget = splitterWidget->widget(1);
  else systemWidget = splitterWidget->widget(2);

  if (metricIndex==0) splitterWidget->insertWidget(0,metricWidget);
  else if (callIndex==0) splitterWidget->insertWidget(0,callWidget);
  else splitterWidget->insertWidget(0,systemWidget);

  if (metricIndex==1) splitterWidget->insertWidget(1,metricWidget);
  else if (callIndex==1) splitterWidget->insertWidget(1,callWidget);
  else splitterWidget->insertWidget(1,systemWidget);

  if (metricIndex==2) splitterWidget->insertWidget(2,metricWidget);
  else if (callIndex==2) splitterWidget->insertWidget(2,callWidget);
  else splitterWidget->insertWidget(2,systemWidget);

  //update value modi
  updateCombos();
  metricCombo->setCurrentIndex(settings.value("valuemodus/metric",0).toInt());
  callCombo->setCurrentIndex(settings.value("valuemodus/call",0).toInt());
  systemCombo->setCurrentIndex(settings.value("valuemodus/system",0).toInt());

  //restoring the width of the widgets in the splitter widget 
  QList<QVariant> v = settings.value("splitterSizes",QList<QVariant>()).toList();
  if (v.size()>0){
    QList<int> splitterSizes;
    for (int i=0; i<v.size(); i++){
      splitterSizes.push_back(v[i].toInt());
    }
    splitterWidget->setSizes(splitterSizes);
  }

  //read in data for external percentage
  lastExternalFileName = settings.value("lastExternalFileName","").toString();
  if (lastExternalFileName!=""){
    if (!readExternalFile(lastExternalFileName)){
      closeExternalFile();
    }
  }

  //restore the threshold above which dynamic metric loading is enabled
  dynloadThreshold = settings.value("dynloadThreshold",1000000000).toInt();

  //restore the status of the actions under Display/Topology
  if (settings.value("emptyActChecked",true).toBool() != emptyAct->isChecked())
    emptyAct->trigger();
  if (settings.value("antialiasingActChecked",false).toBool() != antialiasingAct->isChecked())
    antialiasingAct->trigger();

  //update topology toolbar style
  int style = settings.value("toolbarstyle",Qt::ToolButtonIconOnly).toInt();
  topologyToolBar->setToolButtonStyle((Qt::ToolButtonStyle)style);
  if (settings.value("toolbarHidden",false).toBool())
    topologyToolBar->hide();

  //restore recent files
  QString currentFile = (openedFiles.size()==0 ? "" : openedFiles[0]);
  QList<QString> fileNames;
  for (int i=0; ; i++){
    QString storeUnder = "openedFiles";
    storeUnder.append(QString::number(i));
    QString openedFile = settings.value(storeUnder,"").toString();
    if (openedFile.isEmpty()) break;
    fileNames.push_back(openedFile);
  }
  for (int i=(int)fileNames.size()-1; i>=0; i--)
    rememberFileName(fileNames[i]);


  settings.endGroup();  //MainWindow

  //go down into settings/settingsName/data
  settings.beginGroup("data");

  //load data-related settings
  bool result = true;
  bool saveWithData = settings.value("withdata",false).toBool();
  if (saveWithData){

    QString fileName = settings.value("filename","").toString();
    if (cube==NULL) 
      result = readFile(fileName);
    else {
      assert(openedFiles.size()>0);
      //load data only if not already loaded
      if (fileName.compare(currentFile)!=0) 
	result = readFile(fileName);
    }

    if (!result){
            MessageWidget * wwidget = new MessageWidget(Error, "Unable to load data." );
            wwidget->show();
//       QMessageBox::critical(this, 
// 			    tr("Settings"),
// 			    "Unable to load data.");
    } else {

      if (result) result = metricTabWidget->loadSettings(settings);
      if (result) result = callTabWidget->loadSettings(settings);
      if (result) result = systemTabWidget->loadSettings(settings);
      
      if (result){
	if (((TypedWidget*)(splitterWidget->widget(0)))->getType()==METRICWIDGET)
	  metricTabWidget->displayItems();
	else if (((TypedWidget*)(splitterWidget->widget(0)))->getType()==CALLWIDGET)
	  callTabWidget->displayItems();
	else systemTabWidget->displayItems();
      } else {
                MessageWidget * wwidget = new MessageWidget(Error, "Probably data has been changed since settings was stored.\n Cannot load data settings.");
                wwidget->show();
/*	QMessageBox::critical(this, 
			      tr("Settings"),
			      "Probably data has been changed since settings was stored.\n Cannot load data settings.");*/
      }
    }

    //if something went wrong, clean up
    if (!result) closeFile();
  }

  settings.endGroup(); //data
  settings.endGroup(); //settingsname
  settings.endGroup(); //settings



  //update the menu status reflecting the loaded properties

  //how selected tree items should be marked (Display/T
  //(menu Display/Trees/Selection marking)
  if (systemTabWidget->getSelectionSyntax()==BACKGROUND_SELECTION && !selection1Act->isChecked()){
    selection1Act->setChecked(true);
  } else if (systemTabWidget->getSelectionSyntax()==FRAME_SELECTION && !selection2Act->isChecked()){
    selection2Act->setChecked(true);
  }
  //how zero-valued items in the topology widget should be colored
  //(menu Display/Coloring/Topology coloring)
  if (systemTabWidget->getWhiteForZero() && !white1Act->isChecked()){
    white1Act->setChecked(true);
  } else if (!systemTabWidget->getWhiteForZero() && !white2Act->isChecked()){
    white2Act->setChecked(true);
  }
  //colors for lines in the topology display
  //(menu Display/Coloring/Topology line coloring)
  LineType lineType = systemTabWidget->getLineType();
  if (lineType==BLACK_LINES && !blackLinesAct->isChecked()) blackLinesAct->setChecked(true);
  else if (lineType==GRAY_LINES && !grayLinesAct->isChecked()) grayLinesAct->setChecked(true);
  else if (lineType==WHITE_LINES && !whiteLinesAct->isChecked()) whiteLinesAct->setChecked(true);
  else if (lineType==NO_LINES && !noLinesAct->isChecked()) noLinesAct->setChecked(true);

  statusBar()->showMessage(tr("Ready"));

}

//input a name which setting should be deleted and delete it
//
void MainWidget::deleteSettings(){
  QSettings settings("FZJ","Cube");
  //get default name
  QString oldSettingsName = settings.value("current/name","").toString();

  //get the list of available settings
  settings.beginGroup("settings");
  QStringList settingsList = settings.childGroups();
  settings.endGroup();

  if (settingsList.size()==0){
    MessageWidget * wwidget = new MessageWidget(Warning, "No settings found." );
    wwidget->show();
//     QMessageBox::warning(this,"Delete settings error","No settings found.");
    return;
  }

  int index = 0;
  for (int i=0; i<settingsList.size(); i++){
    if (settingsList[i].compare(oldSettingsName)==0){
      index = i;
      break;
    }
  }

  //get user choice
  bool ok;
  QString settingsName = 
    QInputDialog::getItem(this,
			  "Delete settings",
			  "Delete settings with name:",
			  settingsList,
			  index,
			  false,
			  &ok);

  //delete setting if everything is ok with the name
  if (ok && !settingsName.isEmpty()){
    QString sure = "Really delete settings ";
    sure.append(settingsName);
    sure.append("?");
    if (QMessageBox::question(this,"Delete settings",sure)==QMessageBox::Ok)
      deleteSettings(settingsName);
  }
}

//delete a setting
//
void MainWidget::deleteSettings(const QString& settingsName){
  QSettings settings("FZJ","Cube");
  
  settings.beginGroup("settings");
  settings.remove(settingsName);
  settings.endGroup();
}

