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

// Written: JustinBonus

#include "Celeris.h"
#include "volumetric.h"
#include <QVector>
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
// #include <QStackedWidget>
// #include "slidingstackedwidget.h"

#include <SC_DoubleLineEdit.h>
#include <SC_IntLineEdit.h>

// #include <Qt3DExtras/QCuboidMesh>
// #include <Qt3DExtras/QPhongMaterial>
// #include <Qt3DExtras/QPhongAlphaMaterial>
// #include <Qt3DExtras/Qt3DWindow>
// #include <Qt3DRender/QMesh>
// #include <Qt3DExtras/QForwardRenderer>
// #include <Qt3DRender/QCamera>
// #include <Qt3DCore/QTransform>
// #include <Qt3DCore/QEntity>
// #include <QQuaternion>
// #include <Qt3DExtras/QOrbitCameraController>
// #include <Qt3DRender/QAttribute>
// #include <Qt3DRender/QBuffer>
// #include <Qt3DRender/QGeometry>
// #include <Qt3DRender/QGeometryRenderer>


// #include <QOpenGLWidget>
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
#include <SimCenterPreferences.h>
#include <GeneralInformationWidget.h>
#include <QProcess>
#include <QDir>
#include <QTextEdit>
#include <QFormLayout>
#include <QtWebEngineWidgets>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineScriptCollection>
#include <QWebEngineScript>

#include <QUrl> 

#include <QtGui/QScreen>

// #include <QWebEngineView>
// #include <QWebEnginePage>
// #include <QWebEngineSettings>
// #include <QWebEngineProfile>
// #include <QWebEngineScriptCollection>
// #include <QWebEngineScript>
// #include <QWebEngineScriptCollection>

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

Celeris::Celeris(QWidget *parent)
    :  SimCenterAppWidget(parent)
{
    int windowWidth = 1000;
    int windowWidthMin = 1000;
    int windowHeight = 1000;
    int windowHeightMin = 900;
    QWidget     *mainGroup = new QWidget();
    mainLayout = new QGridLayout();
    mainWindowLayout = new QHBoxLayout(); 

    // -----------------------------------------------------------------------------------

    Q3DScatter *graph = new Q3DScatter();
    QWidget *container = QWidget::createWindowContainer(graph);

    // if (!graph->hasContext()) {
    //     QMessageBox msgBox;
    //     msgBox.setText("Couldn't initialize the OpenGL context.");
    //     msgBox.exec();
    //     return -1;
    // }

    QSize screenSize = graph->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 4, screenSize.height() / 4));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    QWidget *volume = new QWidget();
    QHBoxLayout *hLayout = new QHBoxLayout(volume);
    QVBoxLayout *vLayout = new QVBoxLayout();
    QVBoxLayout *vLayout2 = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);
    hLayout->addLayout(vLayout2);

    volume->setWindowTitle(QStringLiteral("Bathymetry 3D Viewer & Boussinesq Wave Solver (Celeris)"));

    QCheckBox *sliceXCheckBox = new QCheckBox(volume);
    sliceXCheckBox->setText(QStringLiteral("Slice on X axis"));
    sliceXCheckBox->setChecked(false);
    QCheckBox *sliceYCheckBox = new QCheckBox(volume);
    sliceYCheckBox->setText(QStringLiteral("Slice on Y axis"));
    sliceYCheckBox->setChecked(false);
    QCheckBox *sliceZCheckBox = new QCheckBox(volume);
    sliceZCheckBox->setText(QStringLiteral("Slice on Z axis"));
    sliceZCheckBox->setChecked(false);

    QSlider *sliceXSlider = new QSlider(Qt::Horizontal, volume);
    sliceXSlider->setMinimum(0);
    sliceXSlider->setMaximum(1024);
    sliceXSlider->setValue(512);
    sliceXSlider->setEnabled(true);
    QSlider *sliceYSlider = new QSlider(Qt::Horizontal, volume);
    sliceYSlider->setMinimum(0);
    sliceYSlider->setMaximum(1024);
    sliceYSlider->setValue(512);
    sliceYSlider->setEnabled(true);
    QSlider *sliceZSlider = new QSlider(Qt::Horizontal, volume);
    sliceZSlider->setMinimum(0);
    sliceZSlider->setMaximum(1024);
    sliceZSlider->setValue(512);
    sliceZSlider->setEnabled(true);

    QCheckBox *fpsCheckBox = new QCheckBox(volume);
    fpsCheckBox->setText(QStringLiteral("Show FPS"));
    fpsCheckBox->setChecked(false);
    QLabel *fpsLabel = new QLabel(QStringLiteral(""), volume);

    QGroupBox *textureDetailGroupBox = new QGroupBox(QStringLiteral("Bathymetry detail"));

    QRadioButton *lowDetailRB = new QRadioButton(volume);
    lowDetailRB->setText(QStringLiteral("Low (128x64x128)"));
    lowDetailRB->setChecked(true);

    QRadioButton *mediumDetailRB = new QRadioButton(volume);
    mediumDetailRB->setText(QStringLiteral("Generating..."));
    mediumDetailRB->setChecked(false);
    mediumDetailRB->setEnabled(false);

    QRadioButton *highDetailRB = new QRadioButton(volume);
    highDetailRB->setText(QStringLiteral("Generating..."));
    highDetailRB->setChecked(false);
    highDetailRB->setEnabled(false);

    QVBoxLayout *textureDetailVBox = new QVBoxLayout;
    textureDetailVBox->addWidget(lowDetailRB);
    textureDetailVBox->addWidget(mediumDetailRB);
    textureDetailVBox->addWidget(highDetailRB);
    textureDetailGroupBox->setLayout(textureDetailVBox);

    QGroupBox *areaGroupBox = new QGroupBox(QStringLiteral("Show area"));

    QRadioButton *areaAllRB = new QRadioButton(volume);
    areaAllRB->setText(QStringLiteral("Full region"));
    areaAllRB->setChecked(true);

    QRadioButton *areaMineRB = new QRadioButton(volume);
    areaMineRB->setText(QStringLiteral("Analysis area"));
    areaMineRB->setChecked(false);

    QRadioButton *areaMountainRB = new QRadioButton(volume);
    areaMountainRB->setText(QStringLiteral("Wave inlet"));
    areaMountainRB->setChecked(false);

    QVBoxLayout *areaVBox = new QVBoxLayout;
    areaVBox->addWidget(areaAllRB);
    areaVBox->addWidget(areaMineRB);
    areaVBox->addWidget(areaMountainRB);
    areaGroupBox->setLayout(areaVBox);

    QCheckBox *colorTableCheckBox = new QCheckBox(volume);
    colorTableCheckBox->setText(QStringLiteral("Alternate color table"));
    colorTableCheckBox->setChecked(false);

    QLabel *sliceImageXLabel = new QLabel(volume);
    QLabel *sliceImageYLabel = new QLabel(volume);
    QLabel *sliceImageZLabel = new QLabel(volume);
    sliceImageXLabel->setMinimumSize(QSize(200, 100));
    sliceImageYLabel->setMinimumSize(QSize(200, 200));
    sliceImageZLabel->setMinimumSize(QSize(200, 100));
    sliceImageXLabel->setMaximumSize(QSize(200, 100));
    sliceImageYLabel->setMaximumSize(QSize(200, 200));
    sliceImageZLabel->setMaximumSize(QSize(200, 100));
    sliceImageXLabel->setFrameShape(QFrame::Box);
    sliceImageYLabel->setFrameShape(QFrame::Box);
    sliceImageZLabel->setFrameShape(QFrame::Box);
    sliceImageXLabel->setScaledContents(true);
    sliceImageYLabel->setScaledContents(true);
    sliceImageZLabel->setScaledContents(true);

    QSlider *alphaMultiplierSlider = new QSlider(Qt::Horizontal, volume);
    alphaMultiplierSlider->setMinimum(0);
    alphaMultiplierSlider->setMaximum(139);
    alphaMultiplierSlider->setValue(100);
    alphaMultiplierSlider->setEnabled(true);
    QLabel *alphaMultiplierLabel = new QLabel(QStringLiteral("Opacity: 1.0"));

    QCheckBox *preserveOpacityCheckBox = new QCheckBox(volume);
    preserveOpacityCheckBox->setText(QStringLiteral("Preserve opacity"));
    preserveOpacityCheckBox->setChecked(true);

    QCheckBox *transparentGroundCheckBox = new QCheckBox(volume);
    transparentGroundCheckBox->setText(QStringLiteral("Transparent ground"));
    transparentGroundCheckBox->setChecked(false);

    QCheckBox *useHighDefShaderCheckBox = new QCheckBox(volume);
    useHighDefShaderCheckBox->setText(QStringLiteral("Use HD shader"));
    useHighDefShaderCheckBox->setChecked(true);

    QLabel *performanceNoteLabel =
            new QLabel(QStringLiteral(
                           "Note: A high end graphics card is\nrecommended with the HD shader\n enabled."));
    performanceNoteLabel->setFrameShape(QFrame::Box);

    QCheckBox *drawSliceFramesCheckBox = new QCheckBox(volume);
    drawSliceFramesCheckBox->setText(QStringLiteral("Draw slice outlines"));
    drawSliceFramesCheckBox->setChecked(false);

    vLayout->addWidget(sliceXCheckBox);
    vLayout->addWidget(sliceXSlider);
    vLayout->addWidget(sliceImageXLabel);
    vLayout->addWidget(sliceYCheckBox);
    vLayout->addWidget(sliceYSlider);
    vLayout->addWidget(sliceImageYLabel);
    vLayout->addWidget(sliceZCheckBox);
    vLayout->addWidget(sliceZSlider);
    vLayout->addWidget(sliceImageZLabel);
    vLayout->addWidget(drawSliceFramesCheckBox, 1, Qt::AlignTop);
    vLayout2->addWidget(fpsCheckBox);
    vLayout2->addWidget(fpsLabel);
    vLayout2->addWidget(textureDetailGroupBox);
    vLayout2->addWidget(areaGroupBox);
    vLayout2->addWidget(colorTableCheckBox);
    vLayout2->addWidget(alphaMultiplierLabel);
    vLayout2->addWidget(alphaMultiplierSlider);
    vLayout2->addWidget(preserveOpacityCheckBox);
    vLayout2->addWidget(transparentGroundCheckBox);
    vLayout2->addWidget(useHighDefShaderCheckBox);
    vLayout2->addWidget(performanceNoteLabel, 1, Qt::AlignTop);

    modifier = new VolumetricModifier(graph);
    modifier->setFpsLabel(fpsLabel);
    modifier->setMediumDetailRB(mediumDetailRB);
    modifier->setHighDetailRB(highDetailRB);
    modifier->setSliceSliders(sliceXSlider, sliceYSlider, sliceZSlider);
    modifier->setSliceLabels(sliceImageXLabel, sliceImageYLabel, sliceImageZLabel);
    modifier->setAlphaMultiplierLabel(alphaMultiplierLabel);
    modifier->setTransparentGround(transparentGroundCheckBox->isChecked());

    QObject::connect(fpsCheckBox, &QCheckBox::stateChanged, modifier,
                     &VolumetricModifier::setFpsMeasurement);
    QObject::connect(sliceXCheckBox, &QCheckBox::stateChanged, modifier,
                     &VolumetricModifier::sliceX);
    QObject::connect(sliceYCheckBox, &QCheckBox::stateChanged, modifier,
                     &VolumetricModifier::sliceY);
    QObject::connect(sliceZCheckBox, &QCheckBox::stateChanged, modifier,
                     &VolumetricModifier::sliceZ);
    QObject::connect(sliceXSlider, &QSlider::valueChanged, modifier,
                     &VolumetricModifier::adjustSliceX);
    QObject::connect(sliceYSlider, &QSlider::valueChanged, modifier,
                     &VolumetricModifier::adjustSliceY);
    QObject::connect(sliceZSlider, &QSlider::valueChanged, modifier,
                     &VolumetricModifier::adjustSliceZ);
    QObject::connect(lowDetailRB, &QRadioButton::toggled, modifier,
                     &VolumetricModifier::toggleLowDetail);
    QObject::connect(mediumDetailRB, &QRadioButton::toggled, modifier,
                     &VolumetricModifier::toggleMediumDetail);
    QObject::connect(highDetailRB, &QRadioButton::toggled, modifier,
                     &VolumetricModifier::toggleHighDetail);
    QObject::connect(colorTableCheckBox, &QCheckBox::stateChanged, modifier,
                     &VolumetricModifier::changeColorTable);
    QObject::connect(preserveOpacityCheckBox, &QCheckBox::stateChanged, modifier,
                     &VolumetricModifier::setPreserveOpacity);
    QObject::connect(transparentGroundCheckBox, &QCheckBox::stateChanged, modifier,
                     &VolumetricModifier::setTransparentGround);
    QObject::connect(useHighDefShaderCheckBox, &QCheckBox::stateChanged, modifier,
                     &VolumetricModifier::setUseHighDefShader);
    QObject::connect(alphaMultiplierSlider, &QSlider::valueChanged, modifier,
                     &VolumetricModifier::adjustAlphaMultiplier);
    QObject::connect(areaAllRB, &QRadioButton::toggled, modifier,
                     &VolumetricModifier::toggleAreaAll);
    QObject::connect(areaMineRB, &QRadioButton::toggled, modifier,
                     &VolumetricModifier::toggleAreaMine);
    QObject::connect(areaMountainRB, &QRadioButton::toggled, modifier,
                     &VolumetricModifier::toggleAreaMountain);
    QObject::connect(drawSliceFramesCheckBox, &QCheckBox::stateChanged, modifier,
                     &VolumetricModifier::setDrawSliceFrames);

    volume->show();
    // -----------------------------------------------------------------------------------



    // QWebEngineProfile profile;
    // QWebEngineProfile * profile = view->page()->profile();

    // WebUiHandler handler;
    // profile.installUrlSchemeHandler(WebUiHandler::schemeName, &handler);
    // profile.installUrlSchemeHandler("https", &handler);
    // QWebEnginePage page;
    // m_pWebPage = new QWebEnginePage();
    // QWebEnginePage page = new QWebEnginePage();
    // page.load(QUrl("https://plynett.github.io/"));
    // m_pWebPage->load(QUrl("https://plynett.github.io/"));
    // page.load(QUrl("https://plynett.github.io/"));

    // -----------------------------------------------------------------------------------
    QFile file;
    // file.setFileName("https://plynett.github.io/js/main.js");
    // file.setFileName("https://github.com/plynett/plynett.github.io/blob/main/js/main.js");
    file.setFileName(":/EVENTS/Celeris/js/main.js");
    file.open(QIODevice::ReadOnly);
    mainJS = file.readAll();
    // jQuery.append("\nvar qt = { 'jQuery': jQuery.noConflict(true) };");
    file.close();

    view = new QWebEngineView(this);

    // Maximize the size of the instance
    // view->setMaximumSize(1920, 1080);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    // view->showMaximized();
    
    // Load the initial page
    // view->load(QUrl("https://tidesandcurrents.noaa.gov/ports/index.html?port=sf"));


    QWebEngineSettings *settings = view->settings();
    settings->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    settings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    settings->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
    settings->setAttribute(QWebEngineSettings::AllowWindowActivationFromJavaScript, true);
    settings->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);


    view->load(QUrl("https://plynett.github.io/"));

    // connect(view, &QWebEngineView::loadFinished, this, &Celeris::adjustLocation);
    // connect(view, &QWebEngineView::titleChanged, this, &Celeris::adjustTitle);
    // connect(view, &QWebEngineView::loadProgress, this, &Celeris::setProgress);
    connect(view, &QWebEngineView::loadFinished, this, &Celeris::finishLoading);

    qDebug() << "Try to run Celeris javascript...";

    // view->page()->runJavaScript("js/main.js");
    // view->page()->runJavaScript(mainJS);
    






    // view->page()->runJavaScript(mainJS)

    // Add QWebView for viewing a Boussinesq wave solver web-page using WebGPU functionality
    // view = new QWebEnginePage();
    // QWebEnginePage 
    // QWebEngineView view;
    // Set position and size of instance
    // view->setPage(m_pWebPage);
    // view->setPage(&page);
    // view->setContextMenuPolicy(Qt::NoContextMenu);
    // view->resize(800, 600);

    // QWebEngineSettings *settings = view->settings();
    // settings->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    // settings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);
    // settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    // settings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    // settings->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
    // settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    // settings->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
    // settings->setAttribute(QWebEngineSettings::AllowWindowActivationFromJavaScript, true);
    // settings->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    // settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);



    // m_pWebPage->runJavaScript("document.body.style.backgroundColor = 'lightblue';");
    // view->setGeometry(0, 0, 1920, 1080);
    // view.setGeometry(0, 0, 1920, 1080);


    // view.show(); 
    // Need hardware acceleration to run WebGPU
    // as well as javasciprt
    // Get the page

    // view->settings()->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    // view->settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);
    // view->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    // view->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    // view->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
    // view->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    // view->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
    // view->settings()->setAttribute(QWebEngineSettings::AllowWindowActivationFromJavaScript, true);
    // view->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    // view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    // view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);

    // // Hyperlink clicked

    // view->settings()->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, true);
    // view->settings()->setAttribute(QWebEngineSettings::ErrorPageEnabled, true);
    // view->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
    // view->settings()->setAttribute(QWebEngineSettings::LinksIncludedInFocusChain, true);

    // view->settings()->setAttribute(QWebEngineSettings::
    // view->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);





    // view->showMaximized();
    // view->show(); 

  

    view->setMinimumSize(650, 800);
    mainLayout->addWidget(view, 0, 0);
    volume->setMinimumSize(800, 800);
    mainLayout->addWidget(volume, 0, 1);


    // -----------------------------------------------------------------------------------


    mainGroup->setLayout(mainLayout);
    mainGroup->setMinimumWidth(windowWidthMin);
    mainGroup->setMinimumHeight(windowHeightMin);


    // // Add digital twin + scene builder on left. Add 3D visualizer on right
    QHBoxLayout *horizontalPanelLayout = new QHBoxLayout();
    QWidget *horizontalPanels = new QWidget();
    horizontalPanels->setLayout(horizontalPanelLayout);
    horizontalPanelLayout->addWidget(mainGroup);
    mainWindowLayout->addWidget(horizontalPanels);
    // mainWindowLayout->addWidget(view);
    this->setLayout(mainWindowLayout);
}


Celeris::~Celeris()
{

}


void Celeris::finishLoading(bool)
{
    // view->page()->runJavaScript("https://cdn.jsdelivr.net/npm/gif.js@0.2.0/dist/gif.js");
    // view->page()->runJavaScript("https://cdn.jsdelivr.net/npm/chart.js");
    // view->page()->runJavaScript("js/main.js");
    // view->page()->runJavaScript(mainJS);
    // view->page()->runJavaScript(":/EVENTS/Celeris/js/main.js");
    // view->page()->runJavaScript("https://cdn.jsdelivr.net/npm/gif.js@0.2.0/dist/gif.js");
    // view->page()->runJavaScript("https://cdn.jsdelivr.net/npm/chart.js");
    // view->page()->runJavaScript("https://github.com/plynett/plynett.github.io/js/main.js");
    // view->page()->runJavaScript("js/main.js");
    // view->page()->runJavaScript(mainJS);

    // view->page()->runJavaScript(":/main.js");
    // view->page()->runJavaScript(":/EVENTS/Celeris/js/main.js");

    // Example of running javascript, it will change the background color of the page to lightblue
    QString code = "document.body.style.backgroundColor = 'lightblue';";
    view->page()->runJavaScript(code);

    QString codeUpdateButtonLabel = "const maximizedCounts = []; const panels = document.querySelectorAll('#horizontalbar .custom-window'); panels.forEach((panel, index) => { maximizedCounts[index] = 0; const content = panel.querySelector('.window-content'); const isMaximized = window.getComputedStyle(content).display !== 'none'; const button = panel.querySelector('.minimize-button'); button.textContent = isMaximized ? '-' : '+'; });";
    view->page()->runJavaScript(codeUpdateButtonLabel);

    QString codeRunExample = "initializeWebGPUApp();";

    QString codeToolTip = "const windows = document.querySelectorAll('.window-content');   windows.forEach(windowContent => { const button = windowContent.previousElementSibling.querySelector('.minimize-button'); if (windowContent.getAttribute('data-default') === 'minimized') {  windowContent.style.display = 'none';  button.textContent = '+';  } else {  windowContent.style.display = 'block';   button.textContent = '-';  }  button.addEventListener('click', function() {   if (windowContent.style.display === 'none') {  windowContent.style.display = 'block';  this.textContent = '-';  } else {   windowContent.style.display = 'none';  this.textContent = '+';   }   });  });";
    view->page()->runJavaScript(codeToolTip);

    QString codeOnFileUpload = " function onFileUpload(event) { var inputId = event.target.id;  var label = document.querySelector('label[for=' + inputId + ']');  if (event.target.files.length > 0) {  label.style.backgroundColor = '#4CAF50';   label.style.color = 'white'; } else {  label.style.backgroundColor = '';  label.style.color = '';  } }";
    view->page()->runJavaScript(codeOnFileUpload);

    QString codeOnFileUploadListener = "var fileInputs = document.querySelectorAll('input[type=file]'); fileInputs.forEach(function (input) { input.addEventListener('change', onFileUpload); });";
    view->page()->runJavaScript(codeOnFileUploadListener);

    QString codeMouseVals = "var leftMouseIsDown = false; var rightMouseIsDown = false; var lastMouseX_right = 0;  var lastMouseY_right = 0; var lastMouseX_left = 0;   var lastMouseY_left = 0;";
    QString codeMouseEvent = "function handleMouseEvent(event) {  var rect = canvas.getBoundingClientRect();  var scaleX = calc_constants.WIDTH / rect.width;  var scaleY = calc_constants.HEIGHT / rect.height;  calc_constants.xClick = (event.clientX - rect.left) * scaleX;  calc_constants.yClick = calc_constants.HEIGHT - (event.clientY - rect.top) * scaleY; calc_constants.click_update = 1; console.log('Canvas clicked/moved at X:', calc_constants.xClick, ' Y:', calc_constants.yClick); }";
    QString codeMouseDown = "canvas.addEventListener('mousedown', function (event) {   if (event.button === 0 && calc_constants.viewType == 1) {  leftMouseIsDown = true;  handleMouseEvent(event);   } else if (event.button === 0 && calc_constants.viewType == 2) {   leftMouseIsDown = true;  lastMouseX_left = event.clientX;  lastMouseY_left = event.clientY; calc_constants.click_update = 2;  } else if (event.button === 2 && calc_constants.viewType == 1 && calc_constants.whichPanelisOpen == 6) {   rightMouseIsDown = true;  lastMouseX_right = event.clientX;   lastMouseY_right = event.clientY;  calc_constants.locationOfTimeSeries[calc_constants.changethisTimeSeries].xts = x_position; calc_constants.locationOfTimeSeries[calc_constants.changethisTimeSeries].yts = y_position;  calc_constants.click_update = 2;  calc_constants.updateTimeSeriesTx = 1;  } else if (event.button === 2 && calc_constants.viewType == 2) {   rightMouseIsDown = true;   lastMouseX_right = event.clientX; lastMouseY_right = event.clientY; calc_constants.click_update = 2;  }  });";

    QString codeMouseMove = "canvas.addEventListener('mousemove', function (event) { if (leftMouseIsDown && calc_constants.viewType == 1) {   handleMouseEvent(event);  } else if (leftMouseIsDown && calc_constants.viewType == 2) {  const deltaX = event.clientX - lastMouseX_left;  const deltaY = event.clientY - lastMouseY_left; const motion_inc = 0.001 / calc_constants.forward; calc_constants.shift_x  += deltaX * motion_inc;  calc_constants.shift_y  -= deltaY * motion_inc * calc_constants.WIDTH / calc_constants.HEIGHT;   lastMouseX_left = event.clientX; lastMouseY_left = event.clientY;  calc_constants.click_update = 2; } else if (rightMouseIsDown) {  const deltaX = event.clientX - lastMouseX_right;   calc_constants.rotationAngle_xy -= deltaX * 0.1;  astMouseX_right = event.clientX;  lastMouseY_right = event.clientY; calc_constants.click_update = 2;  } });";

    QString codeMouseUp = "canvas.addEventListener('mouseup', function (event) { if (event.button === 0) { leftMouseIsDown = false; calc_constants.click_update = 0;  } else if (event.button === 2) {  rightMouseIsDown = false; calc_constants.click_update = 0;  }  });";

    QString codeMouseLeave = "canvas.addEventListener('mouseleave', function () { leftMouseIsDown = false;  rightMouseIsDown = false;  });";

    view->page()->runJavaScript(codeMouseVals);
    view->page()->runJavaScript(codeMouseEvent);
    view->page()->runJavaScript(codeMouseDown);
    view->page()->runJavaScript(codeMouseMove);
    view->page()->runJavaScript(codeMouseUp);
    view->page()->runJavaScript(codeMouseLeave);


    // QString codeTool = "const tooltip = document.getElementById('tooltip'); let x_position = 0.0;  let y_position = 0.0;";
    // QString codeToolTipMouseMoveAsync = "canvas.addEventListener('mousemove', async (event) => { const bounds = canvas.getBoundingClientRect();  const x = event.clientX;  const y = event.clientY; const canvas_width = bounds.right - bounds.left;  const canvas_height = bounds.bottom - bounds.top;  const normalizedX = (x - bounds.left) / canvas_width;   const normalizedY = 1.0 - (y - bounds.top) / canvas_height;  calc_constants.mouse_current_canvas_indX = Math.round(normalizedX * 80);  calc_constants.mouse_current_canvas_indY = Math.round(normalizedY * 120);  x_position = normalizedX * 80 * 1;  y_position = normalizedY * 120 * 1;  tooltip.style.display = 'block';  tooltip.style.left = `${x + window.scrollX }px`;   tooltip.style.top = `${y + window.scrollY + 20}px`;   tooltip.style.backgroundColor = 'gray';  tooltip.style.color = 'white';   tooltip.style.padding = '8px';  tooltip.style.borderRadius = '4px';  });";
    // QString codeUpdateToolTip = "function updateTooltip() {  tooltip.innerHTML = `x-coordinate (m): ${x_position.toFixed(2)}<br>y-coordinate (m): ${y_position.toFixed(2)}<br>bathy/topo (m): ${calc_constants.tooltipVal_bottom.toFixed(2)} <br>friction factor: ${calc_constants.tooltipVal_friction.toFixed(3)}<br>surface elevation (m): ${calc_constants.tooltipVal_eta.toFixed(2)} <br>sig wave height (m): ${calc_constants.tooltipVal_Hs.toFixed(2)}`;  }";
    // QString codeToolTipInterval = "setInterval(updateTooltip, 100);";

    // QString codeToolTipMouseMove = "canvas.addEventListener('mouseout', () => {  tooltip.style.display = 'none';  });";

    // view->page()->runJavaScript(codeTool);
    // view->page()->runJavaScript(codeToolTipMouseMoveAsync);
    // view->page()->runJavaScript(codeUpdateToolTip);
    // view->page()->runJavaScript(codeToolTipInterval);
    // view->page()->runJavaScript(codeToolTipMouseMove);


    // view->page()->runJavaScript(mainJS);

    qDebug() << "Finished loading";
}

bool Celeris::outputToJSON(QJsonObject &jsonObject)
{
    return true;
}

bool Celeris::inputFromJSON(QJsonObject &jsonObject)
{
    return true;
}

bool Celeris::outputAppDataToJSON(QJsonObject &jsonObject)
{
    return true;
}

bool Celeris::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    return true;
}

bool Celeris::copyFiles(QString &dirName)
{
    return true;
}

void Celeris::clear(void)
{
    return;
}

bool Celeris::outputCitation(QJsonObject &jsonObject)
{
    // APA style citation
    QJsonObject citation;
    citation["citation"] = "Lynett, Patrick. (2024). Celeris-WebGPU. https://plynett.github.io/";
    citation["description"] = "Celeris-WebGPU is a high-order Boussinesq wave and NLSWE solver. It delivers faster-than-real-time simulations which are excellent for bridging geophysics with complicated batyhmetries prior to 3D CFD analysis. Celeris-WebGPU uses Graphics Processing Units (GPUs) for hardware acceleration if available.";
    jsonObject["Celeris-WebGPU"] = citation;
    return true;
}