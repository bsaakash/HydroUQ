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

// Written: fmk, JustinBonus

#include "SPH.h"
#include <QScrollArea>
#include <QLineEdit>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QJsonObject>
#include <QJsonArray>
#include <QSvgWidget>
#include <QString>
#include <QIcon>
#include <QPixmap>
#include <QToolButton>
#include <QStackedWidget>
#include "slidingstackedwidget.h"

#include <SC_DoubleLineEdit.h>
#include <SettingsMPM.h>
#include <BodiesMPM.h>
#include <BoundariesMPM.h>
#include <SensorsMPM.h>
#include <OutputsMPM.h>

#include <CaseSPH.h>
#include <ExecutionSPH.h>

#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QPhongAlphaMaterial>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DRender/QMesh>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DRender/QCamera>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QEntity>
#include <QQuaternion>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>


#include "QVector3D"
#include <qcustomplot.h>
#include <QJsonDocument>
#include <QFileDialog>
#include <SectionTitle.h>
#include <QFileInfo>
#include <QMovie>
#include <QPixmap>
#include <RandomVariablesContainer.h>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QVector>
#include <LineEditRV.h>
#include <QDebug>
// #include <QOpenGLWidget>
#include <SimCenterPreferences.h>
#include <GeneralInformationWidget.h>
#include <QProcess>
#include <QDir>
#include <QTextEdit>
#include <QFormLayout>
#include <Qt3DRender/QMesh>


// Trying out
// #include <SimCenterAppWidget.h>
// #include <WorkflowAppHydroUQ.h>
// #include <MainWindowWorkflowApp.h>
// #include <HydroEventSelection.h>
// #include <LocalApplication.h>
// #include <RemoteApplication.h>
// #include <RemoteJobManager.h>
// #include <RunWidget.h>
// #include <UQ_EngineSelection.h>
// #include <UQ_Results.h>
// #include <SC_ToolDialog.h>
// #include <SC_RemoteAppTool.h>

// SPH::SPH(RandomVariablesContainer *theRandomVariableIW, QWidget *parent)
//     :  SimCenterAppWidget(parent), theRandomVariablesContainer(theRandomVariableIW)
// {

SPH::SPH(QWidget *parent)
    :  SimCenterAppWidget(parent)
{
    int windowWidth = 800;
    int windowWidthMin = 250;
    QWidget     *mainGroup = new QWidget();
    QGridLayout *mainLayout = new QGridLayout();

    mainWindowLayout = new QHBoxLayout(); // WE-UQ


    // ==================== Digital Twin Description and Selection ====================
    QStackedWidget *parentStackedWidget = new QStackedWidget();

    SlidingStackedWidget *stackedWidget = new SlidingStackedWidget(parentStackedWidget);
    stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    stackedWidget->setMinimumWidth(windowWidthMin);
    stackedWidget->setMaximumWidth(windowWidth);
    stackedWidget->setStyleSheet("QStackedWidget {background-color:  rgb(79, 83, 89); color: #000000; border: 1px solid #000000; border-radius: 0px;}"
                                 "QStackedWidget:disabled {background-color:  rgb(79, 83, 89); color: #000000; border: 1px solid #000000; border-radius: 0px;}");
    QWidget *page1 = new QWidget();
    QWidget *page2 = new QWidget();
    QWidget *page3 = new QWidget();
    QWidget *page4 = new QWidget();
    QWidget *page5 = new QWidget();
    stackedWidget->addWidget(page1);
    stackedWidget->addWidget(page2);
    stackedWidget->addWidget(page3);
    stackedWidget->addWidget(page4);
    stackedWidget->addWidget(page5);
    stackedWidget->setCurrentIndex(0); // Open to OSU LWF

    // Each page gets a different image pixmap and description of the digital twin in photo
    QPixmap page1Pixmap(":/images/OSU_LWF_Pic_MTS_HighRes.png");
    QPixmap page2Pixmap(":/images/OSU_DWB_Pic_Square.png");
    QPixmap page3Pixmap(":/images/UW_WASIRF_Pic_Square.png");
    QPixmap page4Pixmap(":/images/Waseda_Flume_Picture.jpg");
    QPixmap page5Pixmap(":/images/USGS_Iverson_2020_RFI.PNG");
    uint minWidthDigitalTwinPhoto  = 200;
    uint minHeightDigitalTwinPhoto = 200;
    uint maxWidthDigitalTwinPhoto  = 275;
    uint maxHeightDigitalTwinPhoto = 275;

    // OSU LWF (Winter 2019, Shekhar et al 2020, Mascarenas 2022, Bonus 2023)
    QLabel *page1Label = new QLabel();
    page1Label->setPixmap(page1Pixmap);
    page1Label->setScaledContents(true);
    page1Label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    page1Label->setMinimumHeight(minHeightDigitalTwinPhoto);
    page1Label->setMinimumWidth(minWidthDigitalTwinPhoto);
    page1Label->setMaximumHeight(maxHeightDigitalTwinPhoto);
    page1Label->setMaximumWidth(maxWidthDigitalTwinPhoto);
    page1Label->setAlignment(Qt::AlignCenter);
    QLabel *page1DescriptionLabel = new QLabel(" Oregon State University's Large Wave Flume - OSU LWF"
                                               "\n Supported by NSF Award No. 2037914 and the Pacific Marine Energy Center."
                                               "\n "
                                               "\n Largest of its kind in North America, operating in high Reynolds regimes"
                                               "\n The flume is ideally suited for:"
                                               "\n > Wave runup, reflection, and overtopping"
                                               "\n > Wave forces on offshore and coastal structures"
                                               "\n > Nearshore hydrodynamics, wave breaking, swash, and undertow"
                                               "\n > Tsunami inundation and overland flow"
                                               "\n > Tsunami structure impact, debris, and scour"
                                               "\n "
                                               "\n Wave Flume Size: 104 m (342ft), 3.7 m (12ft), 4.6 m (15ft)"
                                               "\n Bathymetry:    Movable and adjustable"
                                               "\n Max depth: 2 m (6.5 ft) for tsunami, 2.7 m (9 ft) for wind/storm waves"
                                               "\n Wave Maker:    Piston-type, Hydraulic Actuator Assembly"
                                               "\n Wave Types:    Regular, Irregular, Tsunami, and User-Defined"
                                               "\n Max Wave:      1.7 m (5.6 ft) @ 5 sec in max 2.7 m water"
                                               "\n Max Tsunami:   1.4 m (3.9ft) in max 2.0 m water"
                                               "\n Max Stroke:    4 m (13.1 ft) at 4 m/s (13.1 ft/s)."
                                               "\n Period Range: 0.8 to 12+ seconds");
                                              //  "\n > Cross-shore sediment suspension and transport, pollutant mixing and transport"
                                              //  "\n > Scour, pipeline stability, outfalls, liquefaction, and cohesive sediments"
    
    page1DescriptionLabel->setWordWrap(true);
    page1DescriptionLabel->setStyleSheet("QLabel {background-color:  rgb(79, 83, 89); color: #ffffff; border: 0px solid #000000; border-radius: 0px;}"
                                         "QLabel:disabled {background-color:  rgb(79, 83, 89); color: #ffffff; border: 0px solid #000000; border-radius: 0px;}");
    
    // OSU DWB (Park et al. 2021)
    QLabel *page2Label = new QLabel();
    page2Label->setPixmap(page2Pixmap);
    page2Label->setScaledContents(true);
    page2Label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    page2Label->setMinimumHeight(minHeightDigitalTwinPhoto);
    page2Label->setMinimumWidth(minWidthDigitalTwinPhoto);
    page2Label->setMaximumHeight(maxHeightDigitalTwinPhoto);
    page2Label->setMaximumWidth(maxWidthDigitalTwinPhoto);
    page2Label->setAlignment(Qt::AlignCenter);
    QLabel *page2DescriptionLabel = new QLabel(" Oregon State University's Directional Wave Basin - OSU DWB"
                                               "\n Supported by NSF Award No. 2037914 and the Pacific Marine Energy Center."
                                               "\n "
                                               "\n Generate directional waves with a wide range of wave periods and heights."
                                               "\n The flume is ideally suited for:"
                                               "\n > Wave runup, reflection, and overtopping"
                                               "\n > Wave forces on offshore and coastal structures"
                                               "\n > Nearshore hydrodynamics, wave breaking, swash, and undertow"
                                               "\n > Tsunami inundation and overland flow"
                                               "\n > Tsunami structure impact, debris, and scour"
                                               "\n "
                                               "\n Wave Flume Size: 48.8 m long, 26.5 m wide, 2.7 m deep." 
                                               "\n Wave maker: 2 m wide, 1.2 m high piston-type wavemaker.");
    page2DescriptionLabel->setWordWrap(true);
    page2DescriptionLabel->setStyleSheet("QLabel {background-color:  rgb(79, 83, 89); color: #ffffff; border: 0px solid #000000; border-radius: 0px;}"
                                         "QLabel:disabled {background-color:  rgb(79, 83, 89); color: #ffffff; border: 0px solid #000000; border-radius: 0px;}");
    
    // UW WASIRF (Lewis 2023)
    QLabel *page3Label = new QLabel();
    page3Label->setPixmap(page3Pixmap);
    page3Label->setScaledContents(true);
    page3Label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    page3Label->setMinimumHeight(minHeightDigitalTwinPhoto);
    page3Label->setMinimumWidth(minWidthDigitalTwinPhoto);
    page3Label->setMaximumHeight(maxHeightDigitalTwinPhoto);
    page3Label->setMaximumWidth(maxWidthDigitalTwinPhoto);
    page3Label->setAlignment(Qt::AlignCenter);
    QLabel *page3DescriptionLabel = new QLabel(" University of Washington's Wind-Air-Sea Interatction Research Facility - UW WASIRF"
                                               "\n "
                                               "\n Steady and Quasi-Steady State Flows of Water and Wind with Stochastic Reproducibility."
                                               "\n "
                                               "\n Wave Flume Size: 12 m long, 1.2 m wide, 0.9 m deep"
                                               "\n Wave Maker: Circulated, reversible, pump-driven flow");
    page3DescriptionLabel->setWordWrap(true);
    page3DescriptionLabel->setStyleSheet("QLabel {background-color:  rgb(79, 83, 89); color: #ffffff; border: 0px solid #000000; border-radius: 0px;}"
                                         "QLabel:disabled {background-color:  rgb(79, 83, 89); color: #ffffff; border: 0px solid #000000; border-radius: 0px;}");

    // WU TWB (Goseberg et al 2016, Nistor 2016)
    QLabel *page4Label = new QLabel();
    page4Label->setPixmap(page4Pixmap);
    page4Label->setScaledContents(true);
    page4Label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    page4Label->setMinimumHeight(minHeightDigitalTwinPhoto);
    page4Label->setMinimumWidth(minWidthDigitalTwinPhoto);
    page4Label->setMaximumHeight(maxHeightDigitalTwinPhoto);
    page4Label->setMaximumWidth(maxWidthDigitalTwinPhoto);
    page4Label->setAlignment(Qt::AlignCenter);
    QLabel *page4DescriptionLabel = new QLabel(" Waseda University's Tsunami Wave Basin - WU TWB"
                                               "\n "
                                               "\n Vacuum-Pump Controlled Reservoir Tank for Tsunami-Like Wave Generation."
                                               "\n "
                                               "\n Wave Flume Size: 9 m long, 4 m wide, 1 m deep"
                                               "\n Wave Maker: Vacuum-Pump Controlled Reservoir Tank."
                                               "\n Includes a frictional harbor apron, quay wall, and square columns.");
    page4DescriptionLabel->setWordWrap(true);
    page4DescriptionLabel->setStyleSheet("QLabel {background-color:  rgb(79, 83, 89); color: #ffffff; border: 0px solid #000000; border-radius: 0px;}"
                                         "QLabel:disabled {background-color:  rgb(79, 83, 89); color: #ffffff; border: 0px solid #000000; border-radius: 0px;}");

    // USGS DFF (Iverson 2020)
    QLabel *page5Label = new QLabel();
    page5Label->setPixmap(page5Pixmap);
    page5Label->setScaledContents(true);
    page5Label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    page5Label->setMinimumHeight(minHeightDigitalTwinPhoto);
    page5Label->setMinimumWidth(minWidthDigitalTwinPhoto);
    page5Label->setMaximumHeight(maxHeightDigitalTwinPhoto);
    page5Label->setMaximumWidth(maxWidthDigitalTwinPhoto);
    page5Label->setAlignment(Qt::AlignCenter);
    QLabel *page5DescriptionLabel = new QLabel(" United States Geological Survey's Debris Flow Flume - USGS DFF"
                                               "\n "
                                               "\n Large slope flume for studying granular flows and run-out."
                                               "\n "
                                               "\n Flow Flume Size: 100 m long, 2 m wide, 2 m deep"
                                               "\n Flow Maker: Gravity-Driven after retaining gates release."
                                               "\n Includes a 31 degree slope, swing gates, and run-out plane.");
    page5DescriptionLabel->setWordWrap(true);
    page5DescriptionLabel->setStyleSheet("QLabel {background-color:  rgb(79, 83, 89); color: #ffffff; border: 0px solid #000000; border-radius: 0px;}"
                                         "QLabel:disabled {background-color:  rgb(79, 83, 89); color: #ffffff; border: 0px solid #000000; border-radius: 0px;}");

    page1->setLayout(new QHBoxLayout());
    page2->setLayout(new QHBoxLayout());
    page3->setLayout(new QHBoxLayout());
    page4->setLayout(new QHBoxLayout());
    page5->setLayout(new QHBoxLayout());
    page1->layout()->addWidget(page1Label);
    page2->layout()->addWidget(page2Label);
    page3->layout()->addWidget(page3Label);
    page4->layout()->addWidget(page4Label);
    page5->layout()->addWidget(page5Label);
    page1->layout()->addWidget(page1DescriptionLabel);
    page2->layout()->addWidget(page2DescriptionLabel);
    page3->layout()->addWidget(page3DescriptionLabel);
    page4->layout()->addWidget(page4DescriptionLabel);
    page5->layout()->addWidget(page5DescriptionLabel);

    // -- Previous slide button
    QToolButton *prev = new QToolButton();
    // prev->setText("Prev");
    // prev->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    prev->setArrowType(Qt::LeftArrow);
    prev->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    prev->setMinimumSize(25, 50);
    prev->setMaximumSize(25, 50);
    prev->setStyleSheet("QToolButton {background-color: #ffffff; color: #000000; border: 1px solid #000000; border-radius: 5px; font-size: 16px; font-weight: bold;}"
                        "QToolButton:disabled {background-color: #d3d3d3; color: #000000; border: 1px solid #000000; border-radius: 5px; font-size: 16px; font-weight: bold;}");
    
    // -- Next slide button
    QToolButton *next = new QToolButton();
    // next->setText("Next");
    // next->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    next->setArrowType(Qt::RightArrow);
    next->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    next->setMinimumSize(25, 50);
    next->setMaximumSize(25, 50);
    next->setStyleSheet("QToolButton {background-color: #ffffff; color: #000000; border: 1px solid #000000; border-radius: 5px; font-size: 16px; font-weight: bold;}"
                        "QToolButton:disabled {background-color: #d3d3d3; color: #000000; border: 1px solid #000000; border-radius: 5px; font-size: 16px; font-weight: bold;}");
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(prev);
    buttonLayout->addWidget(stackedWidget);
    buttonLayout->addWidget(next);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setSpacing(0);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(buttonLayout, 1, 0);

    
    stackedWidget->setAnimation(QEasingCurve::Type::OutQuart);
    stackedWidget->setSpeed(500);
    connect(prev,&QAbstractButton::clicked,[this, stackedWidget, prev, next]{
        if(stackedWidget->slideInPrev()){
            prev->setEnabled(false);
            next->setEnabled(false);
        }
    });
    connect(next,&QAbstractButton::clicked,[this, stackedWidget, prev, next]{
        if(stackedWidget->slideInNext()){
            prev->setEnabled(false);
           next->setEnabled(false);
        }
    });
    connect(stackedWidget,&SlidingStackedWidget::animationFinished,[this, prev, next]{
        prev->setEnabled(true);
        next->setEnabled(true);
    });

    QLabel *aboveTabs = new QLabel("\nSelect a NHERI SimCenter Digital Twin Above To Begin\n");
    aboveTabs->setAlignment(Qt::AlignCenter);
    // aboveTabs->setStyleSheet("QLabel {background-color:  rgb(79, 83, 89); color: #ffffff; border: 0px solid #000000; border-radius: 0px;}"
    //                          "QLabel:disabled {background-color:  rgb(79, 83, 89); color: #ffffff; border: 0px solid #000000; border-radius: 0px;}");
    mainLayout->addWidget(aboveTabs, 2, 0);

    // ==================== Simulation Set-Up ====================
    mpmSettings = new SettingsMPM();
    mpmBodies = new BodiesMPM();
    mpmBoundaries = new BoundariesMPM();
    mpmSensors = new SensorsMPM();
    mpmOutputs = new OutputsMPM();
    sphCase = new CaseSPH();
    sphExecution = new ExecutionSPH();

    QTabWidget *theTabWidget = new QTabWidget();
    theTabWidget->addTab(mpmSettings, QIcon(QString(":/icons/settings-black.svg")), "Settings");
    theTabWidget->addTab(mpmBodies, QIcon(QString(":/icons/deform-black.svg")), "Bodies");
    theTabWidget->addTab(mpmBoundaries, QIcon(QString(":/icons/man-door-black.svg")), "Boundaries");
    theTabWidget->addTab(mpmSensors, QIcon(QString(":/icons/dashboard-black.svg")), "Sensors");
    theTabWidget->addTab(mpmOutputs, QIcon(QString(":/icons/file-settings-black.svg")), "Outputs");    
    theTabWidget->addTab(sphCase, QIcon(QString(":/icons/user-black.svg")), "Case");
    theTabWidget->addTab(sphExecution, QIcon(QString(":/icons/user-black.svg")), "Execution");

    int sizePrimaryTabs =20;
    theTabWidget->setIconSize(QSize(sizePrimaryTabs,sizePrimaryTabs));
    // theTabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    // -----------------------------------------------------------------------------------
    // Create a 3D window and container widget and set the 3D window as its layout
    // Based on code by Alex44, 2018; https://stackoverflow.com/questions/23231012/how-to-render-in-qt3d-in-standard-gui-application)
    auto rootEntity = new Qt3DCore::QEntity();
    auto view = new Qt3DExtras::Qt3DWindow();
    QWidget *container = QWidget::createWindowContainer(view);

    // background color
    view->defaultFrameGraph()->setClearColor(QColor(QRgb(0xFFFFFF)));

    // Create a camera and set its position
    // auto camera = new Qt3DRender::QCamera(rootEntity);
    auto cameraEntity = view->camera();
    cameraEntity->setUpVector(QVector3D(0, 1.f, 0));
    cameraEntity->setPosition(QVector3D(-5.0f, 5.0f, 5.0f));
    cameraEntity->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
    cameraEntity->viewAll();
    // Create a cube mesh
    Qt3DExtras::QCuboidMesh *cubeMesh = new Qt3DExtras::QCuboidMesh();
    Qt3DExtras::QCuboidMesh *fluidMesh = new Qt3DExtras::QCuboidMesh();
    Qt3DExtras::QCuboidMesh *pistonMesh = new Qt3DExtras::QCuboidMesh();
    Qt3DRender::QMesh *twinMesh = new Qt3DRender::QMesh();
    twinMesh->setSource(QUrl(QStringLiteral("qrc:/OSU_LWF_Bathymetry.obj")));
    Qt3DRender::QMesh *hydroMesh = new Qt3DRender::QMesh();
    hydroMesh->setSource(QUrl(QStringLiteral("qrc:/HydroUQ_Icon_Color.obj")));

    // Create a transform and set its scale
    auto cubeTransform = new Qt3DCore::QTransform();
    auto fluidTransform = new Qt3DCore::QTransform();
    auto pistonTransform = new Qt3DCore::QTransform();
    auto twinTransform = new Qt3DCore::QTransform();
    auto hydroTransform = new Qt3DCore::QTransform();
    cubeMesh->setXExtent(1.016f);
    cubeMesh->setYExtent(0.615f);
    cubeMesh->setZExtent(1.016f);
    cubeTransform->setScale(1.f);
    cubeTransform->setTranslation(QVector3D(45.8f+1.016f/2.f, 2.0f+0.615f/2.f, 1.825f));
    cubeTransform->setRotation(QQuaternion::fromAxisAndAngle(1.f, 1.f, 1.f, 0.f));

    fluidMesh->setXExtent(84.0f);
    fluidMesh->setYExtent(2.0f);
    fluidMesh->setZExtent(3.65f);
    fluidTransform->setScale(1.f);
    fluidTransform->setTranslation(QVector3D(42.0f+1.915f, 1.0f, 1.825f));
    fluidTransform->setRotation(QQuaternion::fromAxisAndAngle(1.f, 1.f, 1.f, 0.f));

    pistonMesh->setXExtent(0.25f);
    pistonMesh->setYExtent(4.6f);
    pistonMesh->setZExtent(3.65f);
    pistonTransform->setScale(1.f);
    pistonTransform->setTranslation(QVector3D(1.9f-0.25f/2.f, 4.6f/2.f, 3.65f/2.f));
    pistonTransform->setRotation(QQuaternion::fromAxisAndAngle(1.f, 1.f, 1.f, 0.f));


    twinTransform->setScale(1.f);
    twinTransform->setTranslation(QVector3D(0.0f, 0.0f, 0.0f));
    twinTransform->setRotation(QQuaternion::fromAxisAndAngle(1.f, 1.f, 1.f, 0.f));
    twinTransform->setRotation(QQuaternion::fromEulerAngles(90.f, 0.f, 0.f));

    hydroTransform->setScale(120.f);
    hydroTransform->setTranslation(QVector3D(30.0f, 5.0f, -10.0f));
    // hydroTransform->setRotation(QQuaternion::fromAxisAndAngle(1.f, 1.f, 1.f, 0.f));
    hydroTransform->setRotation(QQuaternion::fromEulerAngles(90.f, 0.f, 0.f));

    // Allow for camera controls
    auto camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setCamera(cameraEntity);


    // Create a material and set its color
    auto cubeMaterial = new Qt3DExtras::QPhongMaterial();
    auto fluidMaterial = new Qt3DExtras::QPhongAlphaMaterial();
    auto pistonMaterial = new Qt3DExtras::QPhongMaterial();
    auto twinMaterial = new Qt3DExtras::QPhongMaterial();
    auto hydroMaterial = new Qt3DExtras::QPhongMaterial();
    cubeMaterial->setDiffuse(QColor(QRgb(0xCC5500)));
    twinMaterial->setDiffuse(QColor(QRgb(0xFFFFFF)));
    // Give twin an ambient conrete color
    twinMaterial->setAmbient(QColor(QRgb(0xCCCCCC)));

    // Give fluid material transparecny and blue color
    fluidMaterial->setDiffuse(QColor(QRgb(0x0000FF)));
    fluidMaterial->setAlpha(0.6f);
    fluidMaterial->setAmbient(QColor(QRgb(0x0000FF)));

    // Give piston material a teal color
    pistonMaterial->setDiffuse(QColor(QRgb(0x00FFFF)));

    hydroMaterial->setDiffuse(QColor(QRgb(0x005FFF)));
    hydroMaterial->setAmbient(QColor(QRgb(0x005FFF)));
    
    // Create a cube entity and add the mesh, transform and material components
    auto twinEntity = new Qt3DCore::QEntity(rootEntity);
    auto cubeEntity = new Qt3DCore::QEntity(rootEntity);
    auto fluidEntity = new Qt3DCore::QEntity(rootEntity);
    auto pistonEntity = new Qt3DCore::QEntity(rootEntity);
    auto hydroEntity = new Qt3DCore::QEntity(rootEntity);

    cubeEntity->addComponent(cubeMesh);
    cubeEntity->addComponent(cubeMaterial);
    cubeEntity->addComponent(cubeTransform);


    fluidEntity->addComponent(fluidMesh);
    fluidEntity->addComponent(fluidMaterial);
    fluidEntity->addComponent(fluidTransform);

    pistonEntity->addComponent(pistonMesh);
    pistonEntity->addComponent(pistonMaterial);
    pistonEntity->addComponent(pistonTransform);

    twinEntity->addComponent(twinMesh);
    twinEntity->addComponent(twinMaterial);
    twinEntity->addComponent(twinTransform);

    hydroEntity->addComponent(hydroMesh);
    hydroEntity->addComponent(hydroMaterial);
    hydroEntity->addComponent(hydroTransform);

    // Set the root entity of the 3D window
    view->setRootEntity(rootEntity);


    // Make lambda function to update the position of cuboid design structure
    auto updateFluid = [=]() {
      QJsonObject bodiesObjectJSON; 
      QJsonArray bodiesArrayJSON;
      bodiesObjectJSON["bodies"] = bodiesArrayJSON;
      mpmBodies->outputToJSON(bodiesObjectJSON);
      int fluidBodyID = 0;
      int fluidGeometryID = 0;
      double swl = bodiesObjectJSON["bodies"].toArray()[fluidBodyID].toObject()["geometry"].toArray()[fluidGeometryID].toObject()["standing_water_level"].toDouble();
      double originX = bodiesObjectJSON["bodies"].toArray()[fluidBodyID].toObject()["geometry"].toArray()[fluidGeometryID].toObject()["offset"].toArray()[0].toDouble();
      double originY = bodiesObjectJSON["bodies"].toArray()[fluidBodyID].toObject()["geometry"].toArray()[fluidGeometryID].toObject()["offset"].toArray()[1].toDouble();
      double originZ = bodiesObjectJSON["bodies"].toArray()[fluidBodyID].toObject()["geometry"].toArray()[fluidGeometryID].toObject()["offset"].toArray()[2].toDouble();
      double lengthX = bodiesObjectJSON["bodies"].toArray()[fluidBodyID].toObject()["geometry"].toArray()[fluidGeometryID].toObject()["span"].toArray()[0].toDouble();
      double lengthY = swl;
      double lengthZ = bodiesObjectJSON["bodies"].toArray()[fluidBodyID].toObject()["geometry"].toArray()[fluidGeometryID].toObject()["span"].toArray()[2].toDouble();
      originX = lengthX/2.f + originX;  
      originY = swl/2.f + originY;
      originZ = lengthZ/2.f + originZ;
      fluidMesh->setXExtent(lengthX);
      fluidMesh->setYExtent(lengthY);
      fluidMesh->setZExtent(lengthZ);
      // double wave_maker_neutral = 1.915f;
      fluidTransform->setTranslation(QVector3D(originX, originY, originZ));
    };


    auto updateBoundaryStructureSize = [=]() {
      double lengthX = mpmBoundaries->getDimensionX(mpmBoundaries->getStructureBoundary());
      double lengthY = mpmBoundaries->getDimensionY(mpmBoundaries->getStructureBoundary());
      double lengthZ = mpmBoundaries->getDimensionZ(mpmBoundaries->getStructureBoundary());
      cubeMesh->setXExtent(lengthX);
      cubeMesh->setYExtent(lengthY);
      cubeMesh->setZExtent(lengthZ);
    };

    // Make lambda function to update the position of cuboid design structure
    auto updateBoundaryStructurePosition = [=]() {
      double originX = mpmBoundaries->getOriginX(mpmBoundaries->getStructureBoundary());
      double originY = mpmBoundaries->getOriginY(mpmBoundaries->getStructureBoundary());
      double originZ = mpmBoundaries->getOriginZ(mpmBoundaries->getStructureBoundary());
      double lengthX = mpmBoundaries->getDimensionX(mpmBoundaries->getStructureBoundary());
      double lengthY = mpmBoundaries->getDimensionY(mpmBoundaries->getStructureBoundary());
      double lengthZ = mpmBoundaries->getDimensionZ(mpmBoundaries->getStructureBoundary());
      originX = lengthX/2.f + originX;  
      originY = lengthY/2.f + originY;
      originZ = lengthZ/2.f + originZ;
      cubeTransform->setTranslation(QVector3D(originX, originY, originZ));
    };

    auto updateBoundaryPaddleSize = [=]() {
      double lengthX = mpmBoundaries->getDimensionX(mpmBoundaries->getPaddleBoundary());
      double lengthY = mpmBoundaries->getDimensionY(mpmBoundaries->getPaddleBoundary());
      double lengthZ = mpmBoundaries->getDimensionZ(mpmBoundaries->getPaddleBoundary());
      pistonMesh->setXExtent(lengthX);
      pistonMesh->setYExtent(lengthY);
      pistonMesh->setZExtent(lengthZ);
    };

    // Make lambda function to update the position of cuboid design structure
    auto updateBoundaryPaddlePosition = [=]() {
      double originX = mpmBoundaries->getOriginX(mpmBoundaries->getPaddleBoundary());
      double originY = mpmBoundaries->getOriginY(mpmBoundaries->getPaddleBoundary());
      double originZ = mpmBoundaries->getOriginZ(mpmBoundaries->getPaddleBoundary());
      double lengthX = mpmBoundaries->getDimensionX(mpmBoundaries->getPaddleBoundary());
      double lengthY = mpmBoundaries->getDimensionY(mpmBoundaries->getPaddleBoundary());
      double lengthZ = mpmBoundaries->getDimensionZ(mpmBoundaries->getPaddleBoundary());
      originX = lengthX/2.f + originX;  
      originY = lengthY/2.f + originY;
      originZ = lengthZ/2.f + originZ;
      pistonTransform->setTranslation(QVector3D(originX, originY, originZ));
    };


    auto updateDigitalTwin = [=](int index) {
      if (index == 0) {
        twinEntity->setEnabled(true);
        cubeEntity->setEnabled(true);
        fluidEntity->setEnabled(true);
        pistonEntity->setEnabled(true);
        twinTransform->setScale3D(QVector3D(1.f,1.f,1.f));
        // hydroEntity->setEnabled(true);
      } else if (index == 1) {
        twinEntity->setEnabled(true);
        cubeEntity->setEnabled(true);
        fluidEntity->setEnabled(true);
        pistonEntity->setEnabled(true);
        twinTransform->setScale3D(QVector3D(0.6f,7.25f,1.f/1.75f));
        // hydroEntity->setEnabled(false);
      } else if (index == 2) {
        twinEntity->setEnabled(false);
        cubeEntity->setEnabled(true);
        fluidEntity->setEnabled(true);
        pistonEntity->setEnabled(false);
        // hydroEntity->setEnabled(false);
      } else if (index == 3) {
        twinEntity->setEnabled(false);
        cubeEntity->setEnabled(true);
        fluidEntity->setEnabled(true);
        pistonEntity->setEnabled(true);
        // hydroEntity->setEnabled(false);
      } else if (index == 4) {
        twinEntity->setEnabled(false);
        cubeEntity->setEnabled(false);
        fluidEntity->setEnabled(false);
        pistonEntity->setEnabled(true);
        // hydroEntity->setEnabled(true);
      }
      updateFluid();
      updateBoundaryStructureSize();
      updateBoundaryStructurePosition();
      updateBoundaryPaddleSize();
      updateBoundaryPaddlePosition();
    };


    // Add a push button that will redraw the bodies
    QPushButton *updateBodiesButton = new QPushButton("Redraw Bodies");
    connect(updateBodiesButton, &QPushButton::clicked, [=](void){
      updateFluid();
      updateDigitalTwin(stackedWidget->currentIndex());
    });

    // TODO: Refactor so that we just pass a reference to a container/rootEntity to each of the classes/subclasses. Using widget getters is tedious
    // Could template this to avoid repeating code
    connect(mpmBoundaries->getDimensionXWidget(mpmBoundaries->getStructureBoundary()), &QLineEdit::textChanged, [=](QString text){
      updateBoundaryStructurePosition();
      updateBoundaryStructureSize();
    });
    connect(mpmBoundaries->getDimensionYWidget(mpmBoundaries->getStructureBoundary()), &QLineEdit::textChanged, [=](QString text){
      updateBoundaryStructurePosition();
      updateBoundaryStructureSize();
    });

    connect(mpmBoundaries->getDimensionZWidget(mpmBoundaries->getStructureBoundary()), &QLineEdit::textChanged, [=](QString text){
      updateBoundaryStructurePosition();
      updateBoundaryStructureSize();
    });

    connect(mpmBoundaries->getOriginXWidget(mpmBoundaries->getStructureBoundary()), &QLineEdit::textChanged, [=](QString text){
      updateBoundaryStructurePosition();
      updateBoundaryStructureSize();
    });

    connect(mpmBoundaries->getOriginYWidget(mpmBoundaries->getStructureBoundary()), &QLineEdit::textChanged, [=](QString text){
      updateBoundaryStructurePosition();
      updateBoundaryStructureSize();
    });

    connect(mpmBoundaries->getOriginZWidget(mpmBoundaries->getStructureBoundary()), &QLineEdit::textChanged, [=](QString text){
      updateBoundaryStructurePosition();
      updateBoundaryStructureSize();
    });

    // Paddle
    connect(mpmBoundaries->getDimensionXWidget(mpmBoundaries->getPaddleBoundary()), &QLineEdit::textChanged, [=](QString text){
      updateBoundaryPaddlePosition();
      updateBoundaryPaddleSize();
    });
    connect(mpmBoundaries->getDimensionYWidget(mpmBoundaries->getPaddleBoundary()), &QLineEdit::textChanged, [=](QString text){
      updateBoundaryPaddlePosition();
      updateBoundaryPaddleSize();
    });

    connect(mpmBoundaries->getDimensionZWidget(mpmBoundaries->getPaddleBoundary()), &QLineEdit::textChanged, [=](QString text){
      updateBoundaryPaddlePosition();
      updateBoundaryPaddleSize();
    });

    connect(mpmBoundaries->getOriginXWidget(mpmBoundaries->getPaddleBoundary()), &QLineEdit::textChanged, [=](QString text){
      updateBoundaryPaddlePosition();
      updateBoundaryPaddleSize();
    });

    connect(mpmBoundaries->getOriginYWidget(mpmBoundaries->getPaddleBoundary()), &QLineEdit::textChanged, [=](QString text){
      updateBoundaryPaddlePosition();
      updateBoundaryPaddleSize();
    });

    connect(mpmBoundaries->getOriginZWidget(mpmBoundaries->getPaddleBoundary()), &QLineEdit::textChanged, [=](QString text){
      updateBoundaryPaddlePosition();
      updateBoundaryPaddleSize();
    });

    // -----------------------------------------------------------------------------------
    
    mainLayout->addWidget(updateBodiesButton, 3, 0);
    mainLayout->addWidget(theTabWidget, 4, 0);
    mainGroup->setLayout(mainLayout);
    mainGroup->setMinimumWidth(windowWidthMin);
    mainGroup->setMaximumWidth(windowWidth);
    mainLayout->setRowStretch(0, 5);
    
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setLineWidth(1);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidget(mainGroup);
    scrollArea->setMinimumWidth(windowWidthMin + 25);
    scrollArea->setMaximumWidth(windowWidth + 25);


    // Add digital twin + scene builder on left. Add 3D visualizer on right
    QHBoxLayout *horizontalPanelLayout = new QHBoxLayout();
    QWidget *horizontalPanels = new QWidget();
    horizontalPanels->setLayout(horizontalPanelLayout);
    horizontalPanelLayout->addWidget(scrollArea);

    // horizontalPanelLayout->addWidget(visualizationGroup);
    horizontalPanelLayout->addWidget(container);

    // QVBoxLayout *layout = new QVBoxLayout();
    // mainWindowLayout->addWidget(scrollArea);
    // mainWindowLayout->addWidget(updateBodiesButton);
    // mainWindowLayout->addWidget(container);
    mainWindowLayout->addWidget(horizontalPanels);
    this->setLayout(mainWindowLayout);


    connect(stackedWidget, &SlidingStackedWidget::animationFinished, [=](void){
      int index = stackedWidget->currentIndex();
      mpmBodies->setDigitalTwin(index);
      mpmBoundaries->setDigitalTwin(index);
      updateDigitalTwin(index);
    });


}


SPH::~SPH()
{

}


void SPH::updateJSON()
{
    //Write it to JSON becase it is needed for the mesh generation before the final simulation is run.
    //In future only one JSON file in temp.SimCenter directory might be enough
    QString inputFilePath = caseDir() + QDir::separator() + "constant" + QDir::separator() + "simCenter"
                            + QDir::separator() + "input" + QDir::separator() + "SPH.json";


    QFile jsonFile(inputFilePath);
    if (!jsonFile.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug() << "Cannot find the path: " << inputFilePath;
    }

    QJsonObject jsonObject;

    outputToJSON(jsonObject);

    QJsonDocument jsonDoc = QJsonDocument(jsonObject);

    jsonFile.write(jsonDoc.toJson());

    jsonFile.close();
    return;
}

void SPH::writeSPHFiles()
{

    updateJSON();

    // //Run python script to prepare case directory
    QString scriptPath = pyScriptsPath() + "MPM.py";// "/setup_case.py";
    QString jsonPath = caseDir() + QDir::separator() + "constant" + QDir::separator() + "simCenter" + QDir::separator() + "input";
    QString templatePath = templateDictDir();
    QString outputPath = caseDir();

    // QString program = SimCenterPreferences::getInstance()->getPython();
    // QStringList arguments;

    // arguments << scriptPath << jsonPath << templatePath << outputPath;

    // QProcess *process = new QProcess(this);

    // process->start(program, arguments);

    // process->waitForFinished(-1);

    // QMessageBox msgBox;
    // msgBox.setText(process->readAllStandardOutput() + "\n" + process->readAllStandardError());
    // msgBox.exec();

    // process->close();
    return;
}

// From WE-UQ 
void SPH::readCaseData()
{
    //Write it to JSON becase it is needed for the mesh generation before the final simulation is run.
    //In future only one JSON file in temp.SimCenter directory might be enough
    QString inputFilePath = caseDir() + QDir::separator() + "constant" + QDir::separator() + "simCenter"
                            + QDir::separator() + "input" + QDir::separator() + "SPH.json";


    QFile jsonFile(inputFilePath);
    if (!jsonFile.open(QFile::ReadOnly | QFile::Text))
    {
       qDebug() << "Cannot find the path: " << inputFilePath;
    }


    QString val = jsonFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObject = doc.object();

    inputFromJSON(jsonObject);

    // // close file
    jsonFile.close();

    removeOldFiles();
}

void SPH::onBrowseCaseDirectoryButtonClicked(void)
{
    QString fileName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), caseDir(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    QDir newCaseDir(fileName);

    if (!newCaseDir.exists())
    {
       return;
    }

    caseDirectoryPathWidget->setText(fileName);


    if(!isCaseConfigured())
    {
        setupCase();
        // snappyHexMesh->onRunBlockMeshClicked();
        // snappyHexMesh->snappyHexMeshCompleted = false;
        // reloadMesh();
        return;
    }
    // if(!isMeshed())
    // {
    //     snappyHexMesh->onRunBlockMeshClicked();
    //     snappyHexMesh->snappyHexMeshCompleted = false;
    //     reloadMesh();
    //     return;
    // }
    else
    {
        // Need to have SPH.json in LocalWorkDir/SPH/constant/simCenter/input
        // readCaseData();

        //Change it back if the case file is pointing to somethings else
        caseDirectoryPathWidget->setText(fileName);
        // reloadMesh();
        return;
    }

    return;
}

void SPH::clear(void)
{

}



bool SPH::inputFromJSON(QJsonObject &jsonObject)
{
  this->clear();
  caseDirectoryPathWidget->setText(jsonObject["caseDirectoryPath"].toString());

    // openFoamVersion->setCurrentText(jsonObject["OpenFoamVersion"].toString());
  // mpmSettings->inputFromJSON(jsonObject);
  // mpmBodies->inputFromJSON(jsonObject);
  // mpmBoundaries->inputFromJSON(jsonObject);
  // mpmSensors->inputFromJSON(jsonObject);
  // mpmOutputs->inputFromJSON(jsonObject);
  
  return true;
}

bool SPH::outputToJSON(QJsonObject &jsonObject)
{
  jsonObject["EventClassification"] = "Hydro";
  jsonObject["Application"] = "MPM";
  jsonObject["type"] = "MPM";
  jsonObject["subtype"] = "MPM";
  // jsonObject["programFile"] = "fbar"; 
  // The JSON object-or-array that defines each main tab (i.e. Settings, Bodies, Boundaries, Sensors, Outputs)
  QJsonObject settingsObject;  
  QJsonArray bodiesArray;
  QJsonArray boundariesArray;
  QJsonArray sensorsArray;
  QJsonObject outputsObject;

  // Pass in the objects or array object wrappers to the outputToJSON functions
  QJsonObject bodiesObjectWrapper;
  QJsonObject boundariesObjectWrapper;
  QJsonObject sensorsObjectWrapper;
  QJsonArray  bodiesArrayWrapper;
  QJsonArray  boundariesArrayWrapper;
  QJsonArray  sensorsArrayWrapper;

  // Some of the outputToJSON functions will add to the arrays in the wrappers
  // We pass in objects wrapping the arrays to avoid two outputToJSON functions per class (could also template)
  bodiesObjectWrapper["bodies"] = bodiesArrayWrapper;  
  boundariesObjectWrapper["boundaries"] = boundariesArrayWrapper;
  sensorsObjectWrapper["sensors"] = sensorsArrayWrapper;

  // Call the outputToJSON functions in the sub-widget classes (i.e. tabs)
  mpmSettings->outputToJSON(settingsObject);
  mpmBodies->outputToJSON(bodiesObjectWrapper);
  mpmBoundaries->outputToJSON(boundariesObjectWrapper);
  mpmSensors->outputToJSON(sensorsObjectWrapper);
  mpmOutputs->outputToJSON(outputsObject);

  // ==================== Settings ====================
  // Settings (simulation in ClaymoreUW currently)
  if (settingsObject.contains("simulation") && settingsObject["simulation"].isObject()) {
    // Read in the simulation object from the settings object
    jsonObject["simulation"] = settingsObject["simulation"];

    // Move some values from the outputs object to the simulation settings object
    QJsonObject my_sim = jsonObject["simulation"].toObject(); 
    if (outputsObject.contains("save_suffix") && outputsObject["save_suffix"].isString()) {
      my_sim["save_suffix"] = outputsObject["save_suffix"].toString(); // for ClaymoreUW, simulation:save_suffix = outputs:bodies_save_suffix
    }
    if (outputsObject.contains("fps") && outputsObject["fps"].isDouble()) {
      my_sim["fps"] = outputsObject["fps"].toDouble(); // for ClaymoreUW, simulation:fps = outputs:outputBodies_Dt
    }
    if (outputsObject.contains("particles_output_exterior_only") && outputsObject["particles_output_exterior_only"].isBool()) {
      my_sim["particles_output_exterior_only"] = outputsObject["particles_output_exterior_only"].toBool(); // for ClaymoreUW, simulation:particles_output_exterior_only = outputs:bodies_OutputExteriorOnly
    }
    // To be an output option, not a simulation option
    if ((my_sim.contains("duration") && my_sim["duration"].isDouble()) && (my_sim.contains("fps") && my_sim["fps"].isDouble())) {
      my_sim["frames"] = my_sim["duration"].toDouble() * my_sim["fps"].toDouble(); // for ClaymoreUW, simulation:frames = simulation:duration * simulation:fps
    } else {
      my_sim["frames"] = my_sim["fps"]; // Assumes 1 second total simulation as a fallback
    }
    jsonObject["simulation"] = my_sim;
  }
  // Computer, as in hardware and how it is compiled for by application if relevant (e.g. number of GPUs)
  if (settingsObject.contains("computer") && settingsObject["computer"].isObject()) {
    jsonObject["computer"] = settingsObject["computer"];
  }
  // Similitude scaling
  if (settingsObject.contains("scaling") && settingsObject["scaling"].isObject()) {
    jsonObject["scaling"] = settingsObject["scaling"]; 
  }

  // ==================== Bodies ====================
  // Bodies (models in ClaymoreUW currently)
  if (bodiesObjectWrapper.contains("bodies") && bodiesObjectWrapper["bodies"].toArray().size() > 0) {
    bodiesArray = bodiesObjectWrapper["bodies"].toArray();
    int numBodies = bodiesArray.size();
    
    // Assign each body an output_attribs array (array of strings that define what attributes to output each frame per particle)
    QJsonArray outputAttribsArray = outputsObject["outputs"].toObject()["output_attribs"].toArray(); // for ClaymoreUW, outputs:numOutputBodies = bodies:numBodies
    int numOutputAttribs = outputAttribsArray.size();
    for (int i = 0; i < numBodies; i++) {
      // If there are more bodies than output attribs, just output IDs of particles
      if (i >= numOutputAttribs) {
        outputAttribsArray.append(QJsonArray::fromStringList(QStringList() << "ID"));
      }
      QJsonObject body = bodiesArray[i].toObject();
      QJsonArray bodyAttribsArray = outputAttribsArray[i].toArray();
      body["output_attribs"] = bodyAttribsArray;
      bodiesArray[i] = body;
    }

    // Unravel partition array per body into additional bodies (maybe move this into ClaymoreUW itself)
    for (int i = 0; i < numBodies; i++) {
      QJsonObject body = bodiesArray[i].toObject();
      
      QJsonArray partitionArray = body["partition"].toArray();
      int numPartitions = partitionArray.size();
      for (int j = 0; j < numPartitions; j++) {
        QJsonObject newBody = body;
        QJsonObject partition = partitionArray[j].toObject();
        newBody["gpu"] = partition["gpu"]; 
        newBody["model"] = partition["model"]; // TODO: Update schema to body maybe
        newBody["partition_start"] = partition["partition_start"]; // TODO: Update schema to body maybe
        newBody["partition_end"] = partition["partition_end"]; // TODO: Update schema to body maybe
        if (j == 0) {
          bodiesArray[i] = newBody;
          continue;
        }
        bodiesArray.append(newBody); // TODO: Insert instead of append
      }
    }
    // Add the bodies array to the jsonObject
    jsonObject["bodies"] = bodiesArray; // for the future schema
  }

  // ==================== Boundaries ====================
  // Boundaries (grid-boundaries in ClaymoreUW currently, TODO: Deprecate and change to "boundaries")
  if (boundariesObjectWrapper.contains("boundaries") && boundariesObjectWrapper["boundaries"].isArray()) {
    // boundaries is an array of objects, each is an individual boundary
    jsonObject["boundaries"] = boundariesObjectWrapper["boundaries"]; // for the future schema
  }

  // ==================== Sensors ====================
  // Sensors (grid-targets, particle-targets in ClaymoreUW currently, TODO: Deprecate and change to "sensors")
  // sensors is an array of objects, each is an individual sensor
  if (0) {
    if (sensorsObjectWrapper.contains("sensors") && sensorsObjectWrapper["sensors"].isArray()) 
      jsonObject["sensors"] = sensorsObjectWrapper["sensors"];
  } else {
    if (sensorsObjectWrapper.contains("particle-sensors") && sensorsObjectWrapper["particle-sensors"].isArray()) 
      jsonObject["particle-sensors"] = sensorsObjectWrapper["particle-sensors"]; // for the future schema
    if (sensorsObjectWrapper.contains("grid-sensors") && sensorsObjectWrapper["grid-sensors"].isArray()) 
      jsonObject["grid-sensors"] = sensorsObjectWrapper["grid-sensors"]; // for the future schema
  }
  // ==================== Outputs ====================
  // Outputs (not a separate object in ClaymoreUW currently, must move some fields to other objects manually for ClaymoreUW)
  if (outputsObject.contains("outputs") && outputsObject["outputs"].isObject()) {
    jsonObject["outputs"] = outputsObject["outputs"]; // for future schema, not used in ClaymoreUW currently
  }

  
  return true;
}

bool SPH::outputAppDataToJSON(QJsonObject &jsonObject) {

    //
    // Only the "parameters" that are needed for the tapis apps (e.g. ClaymoreUW-ls6.bonusj.json for app ClaymoreUW-ls6.bonusj-1.0.0) wrapper script (e.g. wrapper-ls6.sh) to run should be added
    // To see the parameter list for a tapis app, try: 
    // > tapis apps show ClaymoreUW-ls6.bonusj-1.0.0 -f json
    // 
    // Everything output by this function is going to become a key-value pair in the "parameters" array
    // Default "parameters" : [driverFile, errorFile, modules, inputFile, outputFile]
    // Extra "parameters" : [..., programFile]
    // I only added the "programFile" parameter for my tapis app, don't add more unless the app json required parameters is changed or the app is changed
    // ${programFile} --file=${inputFile}
    //
    // per API, need to add name of application to be called in Application
    // and all data to be used in ApplicationDate
    // See SimCenterCommon/Workflow/TOOLS/SC_RemoteAppTool.cpp (should be folder adjacent to HydroUQ, i.e. typically in ~/SimCenter/)
    //

    // jsonObject["EventClassification"] = "Hydro";
    // jsonObject["Application"] = "SPH";
    jsonObject["programFile"] = "fbar"; // <- ClaymoreUW SPH executable filename on remote machine. Can be changed depending on compiled optimizations, versions, digital twin, etc.
  

    // QJsonObject dataObj;
    // dataObj["modules"] = "python3";
    // dataObj["sceneFile"] = "scene-OSU_LWF-ls6.json";
    // dataObj["programFile"] = "fbar";
    // dataObj["inputFile"] = "scInput.json";
    
    // jsonObject["ApplicationData"] = dataObj;

    return true;
}
bool SPH::inputAppDataFromJSON(QJsonObject &jsonObject) {

    Q_UNUSED(jsonObject);
    return true;
}


bool SPH::copyFiles(QString &destDir) {
  
    writeSPHFiles();

    QString caseName = "SPH";

    bool result = this->copyPath(caseDir(), destDir + QDir::separator() + caseName, false);
    //Remove the 'constant/polyMesh' directory
    // Makes it slow to transfer the mesh to DesignSafe
    // The mesh will be run on the remote machine anyway
    // QDir polyMeshDir(destDir + QDir::separator() + caseName + QDir::separator() + "constant" + QDir::separator() + "polyMesh");
    // polyMeshDir.removeRecursively();

    if (result == false) {
        QString errorMessage; errorMessage = "SPH - failed to copy file: " + caseDir() + " to: " + destDir;
        emit sendFatalMessage(errorMessage);
        qDebug() << errorMessage;
    }


  // if (mpmSettings->copyFiles(destDir) == false)
  //   return false;
  // if (mpmBodies->copyFiles(destDir) == false)
  //   return false;
  // if (mpmBoundaries->copyFiles(destDir) == false)
  //   return false;
  // if (mpmSensors->copyFiles(destDir) == false)
  //   return false;
  // if (mpmOutputs->copyFiles(destDir) == false)
  //   return false;

  // return result;
    return true;

 }

// From WE-UQ EmptyDomainCFD
bool SPH::cleanCase()
{
    QDir zeroDir(caseDir() + QDir::separator() + "0");
    QDir constDir(caseDir() + QDir::separator() + "constant");
    QDir systemDir(caseDir() + QDir::separator() + "system");

    zeroDir.removeRecursively();
    constDir.removeRecursively();
    systemDir.removeRecursively();

    QFile logFile(caseDir() + QDir::separator() + "log.txt");

    logFile.remove();

    return true;
}

// From WE-UQ EmptyDomainCFD
bool SPH::removeOldFiles()
{
    // //Clean extra files if exist in 0 folder
    // QFile nSurfaceLayersFile(caseDir() + QDir::separator() + "0" + QDir::separator() + "nSurfaceLayers");
    // QFile pointLevelFile(caseDir() + QDir::separator() + "0" + QDir::separator() + "pointLevel");
    // QFile thicknessFile(caseDir() + QDir::separator() + "0" + QDir::separator() + "thickness");
    // QFile thicknessFractionFile(caseDir() + QDir::separator() + "0" + QDir::separator() + "thicknessFraction");
    // QFile cellLevelFile(caseDir() + QDir::separator() + "0" + QDir::separator() + "cellLevel");

    // nSurfaceLayersFile.remove();
    // pointLevelFile.remove();
    // thicknessFile.remove();
    // thicknessFractionFile.remove();
    // cellLevelFile.remove();

    return true;
}



// From WE-UQ EmptyDomainCFD
bool SPH::setupCase()
{
    cleanCase();

    QDir targetDir(caseDir());

    if (!targetDir.exists())
    {
        targetDir.mkpath(caseDir());
    }

    targetDir.mkpath("0");
    targetDir.mkpath("constant");
    targetDir.mkpath("constant/geometry");
    targetDir.mkpath("constant/simCenter");
    targetDir.mkpath("constant/simCenter/output");
    targetDir.mkpath("constant/simCenter/input");
    targetDir.mkpath("constant/boundaryData");
    targetDir.mkpath("constant/boundaryData/inlet");
    targetDir.mkpath("system");

    // QFile visFoam(caseDir() + "/vis.foam");
    // visFoam.open(QIODevice::WriteOnly);

    // //Write dictionary files
    writeSPHFiles();

    return true;
}

// From WE-UQ EmptyDomainCFD
QVector<QVector<double>> SPH::readTxtData(QString fileName)
{
    QVector<QVector<double>>  data;

    int colCount  = 0;

    QFile inputFileTest(fileName);

    if (inputFileTest.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFileTest);


       while (!in.atEnd())
       {
            QString line = in.readLine();

            QStringList  fields = line.split(" ");

            colCount  = fields.size();
            break;
       }
       inputFileTest.close();
    }

    for (int i=0; i < colCount; i++)
    {
        QVector<double> row;
        data.append(row);
    }

    int count  = 0;

    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);

       while (!in.atEnd())
       {
            QString line = in.readLine();

            QStringList  fields = line.split(" ");

            for (int i=0; i < colCount; i++)
            {
                data[i].append(fields[i].toDouble());
            }
       }
       inputFile.close();
    }

    return data;
}

// From WE-UQ EmptyDomainCFD
bool SPH::isCaseConfigured()
{
    QDir zeroDir(caseDir() + QDir::separator() +  "0");
    QDir constDir(caseDir() + QDir::separator() + "constant");
    QDir systemDir(caseDir() + QDir::separator() + "system");
    // QFile contrlDict(caseDir() + QDir::separator() + "system" + QDir::separator() + "controlDict");
    // QFile blockDict(caseDir() + QDir::separator() + "system" +  QDir::separator() + "blockMeshDict");
    // QFile snappyDict(caseDir() + QDir::separator() + "system" + QDir::separator() + "snappyHexMeshDict");

    // //Better if we check other files too, for now these are enougg to run a mesh
    // return zeroDir.exists() && constDir.exists() && systemDir.exists() &&
    //        contrlDict.exists() && blockDict.exists() && snappyDict.exists();
    return zeroDir.exists() && constDir.exists() && systemDir.exists();
}



bool SPH::initialize()
{
    // mainWindowLayout = new QHBoxLayout();
    caseDirectoryGroup = new QGroupBox("Case Directory");
    caseDirectoryLayout = new QGridLayout();

    QLabel *casePathLabel = new QLabel("Path: ");
    QPushButton* browseCaseDirectoryButton  = new QPushButton("Browse");

    caseDirectoryPathWidget = new QLineEdit();
    QString currentAppDir = QCoreApplication::applicationDirPath();

    QDir workingDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if (!workingDir.exists())
        workingDir.mkpath(".");

    QString workingDirPath = workingDir.filePath(QCoreApplication::applicationName() + QDir::separator()
                                                 + "LocalWorkDir" + QDir::separator()
                                                 + "SPH");

    if (!workingDir.exists(workingDirPath))
        workingDir.mkpath(workingDirPath);

    caseDirectoryPathWidget->setText(workingDirPath);


    caseDirectoryLayout->addWidget(casePathLabel, 0, 0);
    caseDirectoryLayout->addWidget(caseDirectoryPathWidget, 0, 1);
    caseDirectoryLayout->addWidget(browseCaseDirectoryButton, 0, 2);



    // QLabel *citeLabel = new QLabel("\nParts of the workflow for this SPH event are developed based on the work of Wang et al. (2020) and Bonus (2023).\n"
    //                                "The user should cite the work as follows:\n"
    //                                "\nWang, Xinlei and Qiu Yuxing, et al. (2020). “A massively parallel and scalable multi-GPU material point method.”\n"
    //                                "\nBonus, Justin (2023). “Evaluation of Fluid-Driven Debris Impacts in a High-Performance Multi-GPU Material Point Method.”\n"
    //                                "PhD thesis, University of Washington, Seattle, WA.");

    // QFont citeFont("Arial", 8);
    // citeFont.setPointSize(7);
    // citeFont.setItalic(true);

    // citeLabel->setFont(citeFont);

    caseDirectoryGroup->setLayout(caseDirectoryLayout);
    caseDirectoryGroup->setMaximumWidth(200); // small test

    //Populate each tab
    // auto layout = this.layout();
    // layout->addWidget(caseDirectoryGroup);
    // layout->addWidget(citeLabel);
    // layout->addStretch();
    
    // mainWindowLayout->addWidget(caseDirectoryGroup); // Before ?
    // connect(browseCaseDirectoryButton, SIGNAL(clicked()), this, SLOT(onBrowseCaseDirectoryButtonClicked()));



    // mainWindowLayout->addWidget(caseDirectoryGroup);
    // mainWindowLayout->addWidget(citeLabel);
    // mainWindowLayout->addStretch();

    // connect(browseCaseDirectoryButton, SIGNAL(clicked()), this, SLOT(onBrowseCaseDirectoryButtonClicked()));
    //=====================================================
    // Setup the case directory
    //=====================================================

    if(!isCaseConfigured())
    {
        setupCase(); // Check if directories for the case files exist, if not create them
    }

    //Read all the case data from const/simCenter

    // readCaseData();
    
    // caseInitialized = true;
    caseInitialized = true; 


    // Update the GI Tab once the data is read
    // GeneralInformationWidget *theGI = GeneralInformationWidget::getInstance();
    // theGI->setLengthUnit("m");
//    theGI->setNumStoriesAndHeight(numberOfFloors(), buildingHeight());
//    theGI->setBuildingDimensions(buildingWidth(), buildingDepth(), buildingWidth()*buildingDepth());

    this->adjustSize();

    return true;
}


QString SPH::caseDir()
{
    return caseDirectoryPathWidget->text();
}

QString SPH::pyScriptsPath()
{
    QString backendAppDir = SimCenterPreferences::getInstance()->getAppDir() + QDir::separator()
             + QString("applications") + QDir::separator() + QString("createEVENT") + QDir::separator()
             + QString("MPM");
    return backendAppDir;
}

QString SPH::templateDictDir()
{
    // Probably not needed for anything but OpenFOAM
    QString templateDictsDir = SimCenterPreferences::getInstance()->getAppDir() + QDir::separator()
             + QString("applications") + QDir::separator() + QString("createEVENT") + QDir::separator()
             + QString("MPM") + QDir::separator() + QString("templateOF10Dicts");

    return templateDictsDir;
}

QString SPH::simulationType()
{
    // return turbulenceModeling->simulationType();
    return QString("MPM");
}



 bool SPH::isInitialize()
{
    return caseInitialized;
}


