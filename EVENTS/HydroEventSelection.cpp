/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS 
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: fmckenna
// Modified: Ajay B Harish (Feb 2021)

#include "HydroEventSelection.h"


#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QStackedWidget>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <SectionTitle.h>

#include <ExistingSimCenterEvents.h>
#include "Utils/ProgramOutputDialog.h"

#include <UserDefinedApplication.h>
#include <GeoClawOpenFOAM/GeoClawOpenFOAM.h>
#include <WaveDigitalFlume/WaveDigitalFlume.h>
#include <coupledDigitalTwin/CoupledDigitalTwin.h>
#include <MPM/MPM.h>
//*********************************************************************************
// Main Hydro event
//*********************************************************************************


// HydroEventSelection::HydroEventSelection(RandomVariablesContainer *theRandomVariableIW, RemoteService* remoteService, QWidget *parent)
//     : SimCenterAppWidget(parent), theCurrentEvent(0), theRandomVariablesContainer(theRandomVariableIW)
HydroEventSelection::HydroEventSelection(RandomVariablesContainer *theRandomVariableIW,
					 GeneralInformationWidget* generalInfoWidget,
					 QWidget *parent)  : SimCenterAppWidget(parent), theCurrentEvent(0), theRandomVariablesContainer(theRandomVariableIW)
{
    // Unused variables
    (void) generalInfoWidget;

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout();

    // The selection of different events
    QHBoxLayout *theSelectionLayout = new QHBoxLayout();
    SectionTitle *label=new SectionTitle();
    label->setMinimumWidth(250);
    label->setText(QString("Event Type"));

    // Combobox for different simulation types
    eventSelection = new QComboBox();
    eventSelection->setObjectName("LoadingTypeCombox");

    // Load the different event types
    eventSelection->addItem(tr("General Event (GeoClaw and OpenFOAM)"));
    eventSelection->addItem(tr("Digital Twin (GeoClaw and OpenFOAM)"));
    eventSelection->addItem(tr("Digital Twin (OpenFOAM and OpenSees)"));
    eventSelection->addItem(tr("Digital Twin (MPM)"));        

    // Datatips for the different event types
    eventSelection->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    eventSelection->setItemData(0, "Shallow-Water-Equations -> Finite-Volume-Method (GeoClaw -> OpenFOAM) [Multi-CPU]", Qt::ToolTipRole);
    eventSelection->setItemData(1, "Shallow-Water-Equations -> Finite-Volume-Method -> Finite-Element-Analysis (GeoClaw -> OpenFOAM -> OpenSees) [Multi-CPU]", Qt::ToolTipRole);
    eventSelection->setItemData(2, "Finite-Volume-Method <-> Finite-Element-Analysis (OpenFOAM <-> OpenSees) [Multi-CPU]", Qt::ToolTipRole);
    eventSelection->setItemData(3, "Material-Point-Method (ClaymoreUW) [Multi-GPU]", Qt::ToolTipRole);    

    theSelectionLayout->addWidget(label);
    QSpacerItem *spacer = new QSpacerItem(50,10);
    theSelectionLayout->addItem(spacer);
    theSelectionLayout->addWidget(eventSelection, 2);
    theSelectionLayout->addStretch(3);
    layout->addLayout(theSelectionLayout);


    // ---
    // Create the stacked widget for the different event-method types
    theStackedWidget = new QStackedWidget();

    // create the individual load widgets & add to stacked widget
    theGeoClawOpenFOAM = new GeoClawOpenFOAM(theRandomVariablesContainer);
    theWaveDigitalFlume = new WaveDigitalFlume(theRandomVariablesContainer);
    theCoupledDigitalTwin = new CoupledDigitalTwin(theRandomVariablesContainer);
    theMPM = new MPM(theRandomVariablesContainer);

    theStackedWidget->addWidget(theGeoClawOpenFOAM);
    theStackedWidget->addWidget(theWaveDigitalFlume);
    theStackedWidget->addWidget(theCoupledDigitalTwin);
    theStackedWidget->addWidget(theMPM);        
    // --- 


    // Setup the Layout
    layout->addWidget(theStackedWidget);
    this->setLayout(layout);
    // --
    // Set the default event to select at boot-up. For now, it is MPM
    theStackedWidget->setCurrentIndex(0);
    
    connect(eventSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(eventSelectionChanged(int)));
    connect(eventSelection,SIGNAL(currentTextChanged(QString)),this,SLOT(eventSelectionChanged(QString))); // WE-UQ
    
    QString stringForMPM = "Digital Twin (MPM)";
    int indexForMPM = 3;
    eventSelection->setCurrentIndex(indexForMPM);
    eventSelection->setCurrentIndex(indexForMPM);
    theStackedWidget->setCurrentIndex(indexForMPM);
    theStackedWidget->setCurrentIndex(indexForMPM);
    // theCurrentEvent = theMPM;
    // theCurrentEvent = theMPM;


    // Connect signal and slots
    
    // connect(theMPM, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString))); // WE-UQ


    /*
    connect(theGeoClawOpenFOAM, &SimCenterAppWidget::sendErrorMessage, this, [this](QString message) {emit sendErrorMessage(message);});
    connect(theGeoClawOpenFOAM, &SimCenterAppWidget::sendFatalMessage, this, [this](QString message) {emit sendFatalMessage(message);});
    connect(theGeoClawOpenFOAM, &SimCenterAppWidget::sendStatusMessage, this, [this](QString message) {emit sendStatusMessage(message);});
    */
}

//*********************************************************************************
// Destructor for the main Hydro event
//*********************************************************************************
HydroEventSelection::~HydroEventSelection()
{

}

//*********************************************************************************
// If Event selection is changed
//*********************************************************************************
void HydroEventSelection::eventSelectionChanged(int arg1)
{
    // switch stacked widgets depending on text
    // note type output in json and name in pull down are not the same and hence the ||
    if (arg1 == 0) {
        theCurrentEvent = theGeoClawOpenFOAM;
        theStackedWidget->setCurrentIndex(0);
    }
    else if (arg1 == 1) {
        theCurrentEvent = theWaveDigitalFlume;
        theStackedWidget->setCurrentIndex(1);
    }
    else if (arg1 == 2) {
        theCurrentEvent = theCoupledDigitalTwin;
        theStackedWidget->setCurrentIndex(2);
    }
    else if (arg1 == 3) {
        MPM* theM = dynamic_cast<MPM*>(theMPM);
        
        theCurrentEvent = theM->isInitialize() ? theMPM 
                         : (theM->initialize() ? theMPM : nullptr); 
        if (theCurrentEvent == nullptr) 
        {
            qDebug() << "ERROR: Hydro-EventSelection failed while attempting to initialize the MPM Event, index: " << arg1;
            return;
        }     
        theStackedWidget->setCurrentIndex(3);
    }        
    else {
        qDebug() << "ERROR: Hydro-EventSelection selection-type unknown: " << arg1;
        return;
    }
    return;
}

void HydroEventSelection::eventSelectionChanged(const QString &arg1)
{
    // qDebug() << arg1;

    //
    // switch stacked widgets depending on text
    // note type output in json and name in pull down are not the same and hence the ||
    //

    if (arg1 == "General Event (GeoClaw and OpenFOAM)" || arg1 == "GeoClawOpenFoam" || arg1 == "theGeoClawOpenFOAM" || arg1== "GeoClawOpenFOAM" ) {
        theCurrentEvent = theGeoClawOpenFOAM;
        theStackedWidget->setCurrentIndex(0);
    }
    else if(arg1 == "Digital Twin (GeoClaw and OpenFOAM)" || arg1 == "WaveDigitalFlume") {
        theCurrentEvent = theWaveDigitalFlume;
        theStackedWidget->setCurrentIndex(1);
    }
    else if(arg1 == "Digital Twin (OpenFOAM and OpenSees)" || arg1 == "CoupledDigitalTwin" || arg1 == "theCoupledDigitalTwin"){
        theCurrentEvent = theCoupledDigitalTwin;
        theStackedWidget->setCurrentIndex(2);
    }
    else if(arg1 == "Digital Twin (MPM)" || arg1 == "MPM" || arg1 == "theMPM" || arg1 == "MPMDigitalTwin" || arg1 == "theMPMDigitalTwin" || arg1 == "MPM Digital Twin") {
        MPM* theM = dynamic_cast<MPM*>(theMPM);
        theCurrentEvent = theM->isInitialize() ? theMPM 
                        : ( theM->initialize() ? theMPM : nullptr ); 
        if (!theCurrentEvent) {
            qDebug() << "ERROR: Hydro-EventSelection failed while attempting to initialize the MPM Event, label: " << arg1;
            return;
        }     
        theStackedWidget->setCurrentIndex(3);
    }
    else {
        qDebug() << "ERROR .. HydroEventSelection selection .. type unknown: " << arg1;
    }
    eventSelection->repaint();
}




// void
// HydroEventSelection::sendStatusMessage(QString message) {
//     ProgramOutputDialog *theDialog=ProgramOutputDialog::getInstance();
//     theDialog->appendInfoMessage(message);
// }

// void
// HydroEventSelection::sendErrorMessage(QString message) {
//     ProgramOutputDialog *theDialog=ProgramOutputDialog::getInstance();
//     theDialog->appendErrorMessage(message);
// }

// void
// HydroEventSelection::sendFatalMessage(QString message) {
//     ProgramOutputDialog *theDialog=ProgramOutputDialog::getInstance();
//     theDialog->appendErrorMessage(message);
// }


//*********************************************************************************
// Input data from JSON
//*********************************************************************************
bool HydroEventSelection::inputFromJSON(QJsonObject &jsonObject) {

    QString type;
    QJsonObject theEvent;

    if (jsonObject.contains("Events"))
    {
        QJsonArray theEvents = jsonObject["Events"].toArray();
        QJsonValue theValue = theEvents.at(0);
        if (theValue.isNull())
        {
            qDebug() << "HydroEventSelection::inputFromJSON no Event in Events";
            return false;
        }
        theEvent = theValue.toObject();
        // From WE-UQ, adds type
        if (theEvent.contains("type")) {
            QJsonValue theName = theEvent["type"];
            type = theName.toString();
        } else {
            qDebug() << "HydroEventSelection::inputFromJSON no type in Event";
            return false;
        }
    }
    else
    {
        qDebug() << "HydroEventSelection::inputFromJSON Events found";
        return false;
    }

    int index = 0;
    qDebug() << "TYPE: " << type;

    if (type.contains(QString("GeoClawOpenFOAM")) || (type == QString("General Event (GeoClaw and OpenFOAM)")) || (type == QString("General Event")) || (type == QString("GeoClawOpenFoam")) || (type == QString("theGeoClawOpenFOAM"))) {
        index = 0;
    } else if ((type == QString("WaveDigitalFlume")) || (type == QString("Digital Twin (GeoClaw and OpenFOAM)")) || (type == QString("Digital Twin")) || (type == QString("WaveDigitalFlume")) || (type == QString("theWaveDigitalFlume"))){
        index = 1;
    } else if  ((type == QString("CoupledDigitalTwin")) || (type == QString("Digital Twin (OpenFOAM and OpenSees)")) || (type == QString("Digital Twin")) || (type == QString("CoupledDigitalTwin")) || (type == QString("theCoupledDigitalTwin"))){
        index = 2;
    } else if ((type == QString("MPM")) || (type == QString("Material Point Method")) || (type == QString("Digital Twin (MPM)")) || (type == QString("MPMDigitalTwin")) || (type == QString("theMPM")) || (type == QString("MPM Digital Twin")) || (type == QString("MPMDigitalTwin")) || (type == QString("theMPMDigitalTwin"))){
        index = 3;
    }
    else 
    {
        qDebug() << "HydroEventSelection::inputFromJSON type unknown: " << type;
        return false;
    }
    qDebug() << "TYPE: " << type << "INDEX: " << index;

    eventSelection->setCurrentIndex(index);

    // if worked, just invoke method on new type


    if (theCurrentEvent != 0)
    {
        return theCurrentEvent->inputFromJSON(theEvent);
    }

    qDebug() << "HydroEventSelection::inputFromJSON theCurrentEvent is nullptr, return false";
    return false;
}

//*********************************************************************************
// Output data to JSON
//*********************************************************************************
bool HydroEventSelection::outputToJSON(QJsonObject &jsonObject)
{
    QJsonArray eventArray;
    QJsonObject singleEventData;
    bool result = theCurrentEvent->outputToJSON(singleEventData);
    if (!result) {
        qDebug() << "HydroEventSelection::outputToJSON - theCurrentEvent->outputToJSON returned false";
        return result;
    }

    eventArray.append(singleEventData);
    jsonObject["Events"]=eventArray;
    return result;
}

//*********************************************************************************
// Input AppData to JSON
//*********************************************************************************
bool HydroEventSelection::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    QJsonObject theEvent;
    QString type;
    QString subtype;

    // from Events get the single event
    if (jsonObject.contains("Events"))
    {
        QJsonArray theEvents = jsonObject["Events"].toArray();
        QJsonValue theValue = theEvents.at(0);
        if (theValue.isNull())
        {
            qDebug() << "HydroEventSelection::inputAppDataFromJSON no Event in Events";
          return false;
        }
        theEvent = theValue.toObject();
        if (theEvent.contains("Application"))
        {
            QJsonValue theName = theEvent["Application"];
            type = theName.toString();
            if(theEvent.contains("subtype"))
                subtype = theEvent["subtype"].toString();

            qDebug() << "HydroEventSelection::inputAppDataFromJSON type: " << type;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    // Need the || (or) statements to map the different names of the same event and display names for convenience. Ideally we would have a proper map or enum
    // The folder for the event, the event header file, the event input file, the event's python script folder in SimCenterCommon, etc. will usually match these
    int index = 0;
    if (((type == "GeneralEvent")
	|| (type == "General Event")) || ((type == "General") || (type == "General Event (GeoClaw and OpenFOAM)") || (type == "GeoClawOpenFoam") || (type == "GeoClawOpenFOAM"))) {

      theCurrentEvent = theGeoClawOpenFOAM;
      index = 0;
    }
    else if (((type == "DigitalTwin")
	|| (type == "Digital Twin") || (type == "Digital Twin (GeoClaw and OpenFOAM)") || (type == "WaveDigitalFlume"))) {
      
      theCurrentEvent = theWaveDigitalFlume;
      index = 1;
    }
    else if (((type == "CoupledDigitalTwin")
	|| (type == "Coupled Digital Twin")) || (type == "Digital Twin (OpenFOAM and OpenSees)") || (type == "coupledDigitalTwin")) {
      
      theCurrentEvent = theCoupledDigitalTwin;
      index = 2;
    }
    else if (((type == "MPM")
	|| (type == "Material Point Method")) || ((type == "General Event (MPM)") || (type == "Digital Twin (MPM)")) || (type == "MPMDigitalTwin") || (type == "theMPM")) {
      
      theCurrentEvent = theMPM;
      index = 3;
    } else {
      qDebug() << "HydroEventSelection::inputAppDataFromJSON type unknown: " << type;
      theCurrentEvent = theMPM;
      index = 3;
    //   return false;
    }

    eventSelection->setCurrentIndex(index);
    eventSelection->setCurrentIndex(index);
    
    // invoke inputAppDataFromJSON on new type
    if (theCurrentEvent != 0 && !theEvent.isEmpty())
    {
      return theCurrentEvent->inputAppDataFromJSON(theEvent);
    }
    
    if (theCurrentEvent != 0 ) qDebug() << "HydroEventSelection::inputAppDataFromJSON theCurrentEvent is nullptr, return false";
    if (theEvent.isEmpty()) qDebug() << "HydroEventSelection::inputAppDataFromJSON theEvent is empty, return false";
    return false;
}

//*********************************************************************************
// Output AppData to JSON
//*********************************************************************************
bool HydroEventSelection::outputAppDataToJSON(QJsonObject &jsonObject)
{
    QJsonArray eventArray;
    QJsonObject singleEventData;
    theCurrentEvent->outputAppDataToJSON(singleEventData);
    eventArray.append(singleEventData);
    jsonObject["Events"]=eventArray;
    return true;
}

//*********************************************************************************
// Copy files
//*********************************************************************************
bool HydroEventSelection::copyFiles(QString &destDir) {

    if (theCurrentEvent != 0)
    {
        return  theCurrentEvent->copyFiles(destDir);
    }

    return false;
}

bool HydroEventSelection::supportsLocalRun()
{
    // return theCurrentEvent->supportsLocalRun();
    // if theCurrentEvent
    return false;
}
