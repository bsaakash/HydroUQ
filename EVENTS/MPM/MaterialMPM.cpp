/* *****************************************************************************
Copyright (c) 2016-2023, The Regents of the University of California (Regents).
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
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

#include "MaterialMPM.h"
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QTabWidget>
#include <QStackedWidget>
#include <QDebug>

#include <QJsonObject>
#include <QJsonArray>

#include <SC_ComboBox.h>
#include <SC_DoubleLineEdit.h>
#include <SC_IntLineEdit.h>
#include <SC_StringLineEdit.h>
#include <SC_TableEdit.h>
#include <SC_FileEdit.h>
#include <SC_CheckBox.h>


MaterialMPM::MaterialMPM(QWidget *parent)
  :SimCenterWidget(parent)
{

  // --- Material
  QGridLayout *layout = new QGridLayout();
  this->setLayout(layout);
  int maxWidth = 1280;
  int numRow = 0;


  QStringList fluidMaterialPresetList; fluidMaterialPresetList  << "Water (Fresh)" << "Water (Ocean)" << "Water (Soft)" << "Sand" << "Custom";  
  QStringList debrisMaterialPresetList; debrisMaterialPresetList << "Plastic" << "Rubber" <<  "Aluminum" << "Concrete" << "Wood" << "Custom";  
  QStringList structurePresetList; structurePresetList << "Aluminum" << "Concrete" << "Wood" << "Custom";  
  QStringList geotechPresetList; geotechPresetList << "Sand" << "Clay" << "Concrete" << "Wood" << "Water (Fresh)" << "Water (Ocean)" << "Water (Soft)" << "Custom";  

  QStringList materialPresetList; materialPresetList <<  "Water (Fresh)" << "Water (Ocean)" << "Water (Soft)" << "Plastic" << "Rubber" << "Aluminum" << "Concrete" << "Wood" << "Clay" << "Sand" << "Custom";  
  materialPreset = new SC_ComboBox("material_preset", materialPresetList);
  layout->addWidget(new QLabel("Material Preset"),numRow, 0, 1, 1, Qt::AlignRight);
  layout->addWidget(materialPreset, numRow++, 1);
  layout->itemAt(layout->count()-1)->widget()->setMaximumWidth(maxWidth);

  QStringList constitutiveList; constitutiveList << "JFluid" << "FixedCorotated" << "NeoHookean" << "DruckerPrager" << "CamClay" << "VonMises" <<"CoupleUP" << "Custom";  
  constitutive = new SC_ComboBox("constitutive", constitutiveList);
  layout->addWidget(new QLabel("Constitutive Law"),numRow, 0, 1, 1, Qt::AlignRight);
  layout->addWidget(constitutive, numRow++, 1);
  layout->itemAt(layout->count()-1)->widget()->setMaximumWidth(maxWidth);

  CFL = new SC_DoubleLineEdit("CFL", 0.5);
  layout->addWidget(new QLabel("CFL Number"),numRow, 0, 1, 1, Qt::AlignRight);
  layout->addWidget(CFL, numRow, 1);
  layout->itemAt(layout->count()-1)->widget()->setMaximumWidth(maxWidth);
  layout->addWidget(new QLabel(""),numRow++, 2);

  density = new SC_DoubleLineEdit("rho", 1000.0);
  layout->addWidget(new QLabel("Density"),numRow, 0, 1, 1, Qt::AlignRight);
  layout->addWidget(density, numRow, 1);
  layout->itemAt(layout->count()-1)->widget()->setMaximumWidth(maxWidth);
  layout->addWidget(new QLabel("kg/m^3"),numRow++, 2);

  // --- Elastic Properties
  QGroupBox *elasticBox = new QGroupBox("Elastic Properties");
  QGridLayout *elasticLayout = new QGridLayout();
  elasticBox->setLayout(elasticLayout);

  int elasticNumRow = 0;
  youngsModulus = new SC_DoubleLineEdit("youngs_modulus", 1e7);
  elasticLayout->addWidget(new QLabel("Young's Modulus"), elasticNumRow, 0, 1, 1, Qt::AlignRight);
  elasticLayout->addWidget(youngsModulus, elasticNumRow, 1);
  elasticLayout->itemAt(elasticLayout->count()-1)->widget()->setMaximumWidth(maxWidth);
  elasticLayout->addWidget(new QLabel("Pa"), elasticNumRow++, 2);

  poissonsRatio = new SC_DoubleLineEdit("poisson_ratio",0.3);
  elasticLayout->addWidget(new QLabel("Poisson's Ratio"), elasticNumRow, 0, 1, 1, Qt::AlignRight);
  elasticLayout->addWidget(poissonsRatio, elasticNumRow, 1);
  elasticLayout->itemAt(elasticLayout->count()-1)->widget()->setMaximumWidth(maxWidth);
  elasticLayout->addWidget(new QLabel("[0, 0.5)"), elasticNumRow++, 2);
  elasticLayout->setRowStretch(elasticNumRow,1);
  elasticLayout->setColumnStretch(1,1);
  layout->addWidget(elasticBox, numRow++, 0, 1, 3);



  // ========================
  QStackedWidget *materialStack = new QStackedWidget();
  layout->addWidget(materialStack, numRow++, 0, 1, 3);
  layout->setRowStretch(numRow + elasticNumRow + 2, 1);

  // layout->setRowStretch(0,numRow+1);



  // --- JFluid
  QGroupBox *jfluidBox = new QGroupBox("Constitutive Law Properties");
  QGridLayout *jfluidLayout = new QGridLayout();
  jfluidBox->setLayout(jfluidLayout);
  materialStack->addWidget(jfluidBox);

  numRow = 0;

  bulkModulus = new SC_DoubleLineEdit("bulk_modulus", 2.2e9);
  jfluidLayout->addWidget(new QLabel("Bulk Modulus"),numRow, 0, 1, 1, Qt::AlignRight);
  jfluidLayout->addWidget(bulkModulus, numRow, 1);
  jfluidLayout->itemAt(jfluidLayout->count()-1)->widget()->setMaximumWidth(maxWidth);
  jfluidLayout->addWidget(new QLabel("Pa"),numRow++, 2);

  viscosity = new SC_DoubleLineEdit("viscosity", 0.001);
  jfluidLayout->addWidget(new QLabel("Viscosity"),numRow, 0, 1, 1, Qt::AlignRight);
  jfluidLayout->addWidget(viscosity, numRow, 1);
  jfluidLayout->itemAt(jfluidLayout->count()-1)->widget()->setMaximumWidth(maxWidth);
  jfluidLayout->addWidget(new QLabel("Poise"),numRow++, 2);

  bulkModulusDerivative = new SC_DoubleLineEdit("gamma", 7.15);
  jfluidLayout->addWidget(new QLabel("Bulk Derivative"),numRow, 0, 1, 1, Qt::AlignRight);
  jfluidLayout->addWidget(bulkModulusDerivative, numRow, 1);
  jfluidLayout->itemAt(jfluidLayout->count()-1)->widget()->setMaximumWidth(maxWidth);
  jfluidLayout->addWidget(new QLabel("  "),numRow++, 2);
  for (int i=0; i<3; ++i) jfluidLayout->itemAt(jfluidLayout->count()-(i+1))->widget()->setToolTip("The bulk modulus derivative, gamma, is the rate of change of the bulk modulus with respect to pressure. It is related to the speed of sound in the material. Value of 7.15 common for water.");

  jfluidLayout->setRowStretch(numRow,1);
  jfluidLayout->setColumnStretch(1,1);
  // jfluidLayout->setColumnStretch(0,1);


  // lambdaModulus = new SC_DoubleLineEdit("lambda_modulus", 2.2e7);
  // layout->addWidget(new QLabel("First Lamé Parameter"),numRow, 0);
  // layout->addWidget(youngsModulus, numRow, 1);
  // layout->addWidget(new QLabel("Pa"),numRow++, 2);

  // shearModulus = new SC_DoubleLineEdit("shear_modulus", 2.2e7);
  // layout->addWidget(new QLabel("Shear Modulus"),numRow, 0);
  // layout->addWidget(bulkModulus, numRow, 1);
  // layout->addWidget(new QLabel("Pa"),numRow++, 2);


  // --- Hyper-elastic (Fixed-Corotated, Neo-Hookean)
  QGroupBox *hyperElasticBox = new QGroupBox("Constitutive Law Properties");
  QGridLayout *hyperElasticLayout = new QGridLayout();
  hyperElasticBox->setLayout(hyperElasticLayout);
  materialStack->addWidget(hyperElasticBox);

  numRow = 0;
  hyperElasticLayout->addWidget(new QLabel("Material is fully defined."), numRow++, 0);
  hyperElasticLayout->itemAt(hyperElasticLayout->count()-1)->setAlignment(Qt::AlignCenter);
  hyperElasticLayout->setRowStretch(0, 1);
  hyperElasticLayout->setColumnStretch(0,1);
  // Deprecated in favor of elastic properties box shared among materials

  // youngsModulus = new SC_DoubleLineEdit("youngs_modulus", 2.2e7);
  // hyperElasticLayout->addWidget(new QLabel("Young's Modulus"),numRow, 0);
  // hyperElasticLayout->itemAt(hyperElasticLayout->count()-1)->setAlignment(Qt::AlignRight);
  // hyperElasticLayout->addWidget(youngsModulus, numRow, 1);
  // hyperElasticLayout->addWidget(new QLabel("Pa"),numRow++, 2);

  // poissonsRatio = new SC_DoubleLineEdit("poisson_ratio",0.4);
  // hyperElasticLayout->addWidget(new QLabel("Poisson's Ratio"),numRow, 0);
  // hyperElasticLayout->itemAt(hyperElasticLayout->count()-1)->setAlignment(Qt::AlignRight);
  // hyperElasticLayout->addWidget(poissonsRatio, numRow, 1);
  // hyperElasticLayout->addWidget(new QLabel("[0, 0.5)"),numRow++, 2);
  // hyperElasticLayout->setRowStretch(0,2);
  // hyperElasticLayout->setColumnStretch(0,1);


  // --- Drucker-Prager
  QGroupBox *druckerPragerBox = new QGroupBox("Constitutive Law Properties");
  QGridLayout *druckerPragerLayout = new QGridLayout();
  druckerPragerBox->setLayout(druckerPragerLayout);
  materialStack->addWidget(druckerPragerBox);

  numRow = 0;

  cohesion = new SC_DoubleLineEdit("cohesion", 0.0001);
  druckerPragerLayout->addWidget(new QLabel("Cohesion"), numRow, 0, 1, 1, Qt::AlignRight);
  druckerPragerLayout->addWidget(cohesion, numRow, 1);
  druckerPragerLayout->itemAt(druckerPragerLayout->count()-1)->widget()->setMaximumWidth(maxWidth);
  druckerPragerLayout->addWidget(new QLabel(""), numRow++, 2);

  frictionAngle = new SC_DoubleLineEdit("friction_angle", 30.0);
  druckerPragerLayout->addWidget(new QLabel("Friction Angle"), numRow, 0, 1, 1, Qt::AlignRight);
  druckerPragerLayout->addWidget(frictionAngle, numRow, 1);
  druckerPragerLayout->itemAt(druckerPragerLayout->count()-1)->widget()->setMaximumWidth(maxWidth);
  druckerPragerLayout->addWidget(new QLabel("deg."), numRow++, 2);

  useVolumeCorrection = new SC_CheckBox("SandVolCorrection");
  druckerPragerLayout->addWidget(new QLabel("Adjust Volume?"), numRow, 0, 1, 1, Qt::AlignRight);
  druckerPragerLayout->addWidget(useVolumeCorrection, numRow++, 1);

  dilationAngle = new SC_DoubleLineEdit("dilation_angle", 0.0);
  druckerPragerLayout->addWidget(new QLabel("Dilation Angle"),numRow, 0, 1, 1, Qt::AlignRight);
  druckerPragerLayout->addWidget(dilationAngle, numRow, 1);
  druckerPragerLayout->itemAt(druckerPragerLayout->count()-1)->widget()->setMaximumWidth(maxWidth);
  druckerPragerLayout->addWidget(new QLabel("deg."), numRow++, 2);

  // beta = new SC_DoubleLineEdit("beta",0.8);
  // druckerPragerLayout->addWidget(new QLabel("Cohesion Coefficient"),numRow, 0);
  // druckerPragerLayout->addWidget(beta, numRow, 1);
  // druckerPragerLayout->addWidget(new QLabel(""),numRow++, 2);

  druckerPragerLayout->setRowStretch(0, numRow);
  druckerPragerLayout->setColumnStretch(1, 1);

  // logJp = new SC_DoubleLineEdit("logJp0", -0.01);
  // druckerPragerLayout->addWidget(new QLabel("Log. of Plastic Vol. Ratio"),numRow, 0);
  // druckerPragerLayout->addWidget(logJp, numRow, 1);
  // druckerPragerLayout->addWidget(new QLabel(""),numRow++, 2);


  // --- Non-Associative Cam-Clay (Wolper 2019, Gaume 2018, Klar 2016)
  QGroupBox *camClayBox = new QGroupBox("Constitutive Law Properties");
  QGridLayout *camClayLayout = new QGridLayout();
  camClayBox->setLayout(camClayLayout);
  materialStack->addWidget(camClayBox);

  numRow = 0;

  beta = new SC_DoubleLineEdit("beta",0.5);
  camClayLayout->addWidget(new QLabel("Cohesion Coefficient"),numRow, 0, 1, 1, Qt::AlignRight);
  camClayLayout->addWidget(beta, numRow, 1);
  camClayLayout->itemAt(camClayLayout->count()-1)->widget()->setMaximumWidth(maxWidth);
  camClayLayout->addWidget(new QLabel(""),numRow++, 2);

  Mohr = new SC_DoubleLineEdit("Mohr",2.36);
  camClayLayout->addWidget(new QLabel("Friction Coefficient"),numRow, 0, 1, 1, Qt::AlignRight);
  camClayLayout->addWidget(Mohr, numRow, 1);
  camClayLayout->itemAt(camClayLayout->count()-1)->widget()->setMaximumWidth(maxWidth);
  camClayLayout->addWidget(new QLabel(""),numRow++, 2);

  useHardening = new SC_CheckBox("hardeningOn");
  camClayLayout->addWidget(new QLabel("Allow Hardening?"),numRow, 0, 1, 1, Qt::AlignRight);
  camClayLayout->addWidget(useHardening, numRow++, 1);
  camClayLayout->itemAt(camClayLayout->count()-1)->widget()->setMaximumWidth(maxWidth);

  xi = new SC_DoubleLineEdit("xi",0.8);
  camClayLayout->addWidget(new QLabel("Hardening Coefficient"),numRow, 0, 1, 1, Qt::AlignRight);
  camClayLayout->addWidget(xi, numRow, 1);
  camClayLayout->itemAt(camClayLayout->count()-1)->widget()->setMaximumWidth(maxWidth);
  camClayLayout->addWidget(new QLabel(""),numRow++, 2);

  logJp = new SC_DoubleLineEdit("logJp0", -0.01);
  camClayLayout->addWidget(new QLabel("ln(Plastic Vol. Ratio)"),numRow, 0, 1, 1, Qt::AlignRight);
  camClayLayout->addWidget(logJp, numRow, 1);
  camClayLayout->itemAt(camClayLayout->count()-1)->widget()->setMaximumWidth(maxWidth);
  camClayLayout->addWidget(new QLabel(""),numRow++, 2);


  camClayLayout->setRowStretch(0,numRow);
  camClayLayout->setColumnStretch(1,1);


  // --- Custom
  QGroupBox *customBox = new QGroupBox("Constitutive Law Properties");
  QGridLayout *customLayout = new QGridLayout();
  customBox->setLayout(customLayout);
  materialStack->addWidget(customBox);
  // layout->setRowStretch(5,1);


  numRow = 0;

  customLayout->addWidget(new QLabel("The selected material's implementation in HydroUQ is still in-progress."),numRow++, 0);
  customLayout->itemAt(customLayout->count()-1)->setAlignment(Qt::AlignCenter);

  // Connect the material preset  QComboBox to change entries to default values if selected
  connect(materialPreset, &QComboBox::currentTextChanged, [=](QString val) {
    if (val == "Water (Fresh)") {
      QStringList shortConstitutiveList;  shortConstitutiveList << "JFluid";
      constitutive->clear();
      constitutive->addItems(shortConstitutiveList);
      constitutive->setCurrentIndex(0); 
      density->setText("1000");
      bulkModulus->setText("2.1e9");
      bulkModulusDerivative->setText("7.15");
      viscosity->setText("1.0e-3");
    } else if (val == "Water (Ocean)" ) {
      QStringList shortConstitutiveList;  shortConstitutiveList << "JFluid";
      constitutive->clear();
      constitutive->addItems(shortConstitutiveList);
      constitutive->setCurrentIndex(0);      
      density->setText("1000");
      bulkModulus->setText("2.3e9");
      bulkModulusDerivative->setText("7.15");
      viscosity->setText("1.15e-3");
    } else if (val == "Water (Soft)") {
      QStringList shortConstitutiveList;  shortConstitutiveList << "JFluid";
      constitutive->clear();
      constitutive->addItems(shortConstitutiveList);
      constitutive->setCurrentIndex(0);
      density->setText("1000");
      bulkModulus->setText("2.2e7");
      bulkModulusDerivative->setText("7.15");
      viscosity->setText("1.0e-3");

    } else if (val == "Plastic") {
      QStringList shortConstitutiveList;  shortConstitutiveList << "FixedCorotated" << "NeoHookean";
      constitutive->clear();
      constitutive->addItems(shortConstitutiveList);
      constitutive->setCurrentIndex(0);
      density->setText("981");
      youngsModulus->setText("1e8");
      poissonsRatio->setText("0.3");
    } else if (val == "Rubber") {
      QStringList shortConstitutiveList;  shortConstitutiveList << "FixedCorotated" << "NeoHookean";
      constitutive->clear();
      constitutive->addItems(shortConstitutiveList);
      constitutive->setCurrentIndex(0);
      density->setText("950");
      youngsModulus->setText("1e5");
      poissonsRatio->setText("0.4");
    } else if (val == "Aluminum") {
      QStringList shortConstitutiveList;  shortConstitutiveList << "FixedCorotated" << "NeoHookean";
      constitutive->clear();
      constitutive->addItems(shortConstitutiveList);
      constitutive->setCurrentIndex(0);
      density->setText("2700");
      youngsModulus->setText("6.9e10");
      poissonsRatio->setText("0.4");
    } else if (val == "Wood") {
      QStringList shortConstitutiveList;  shortConstitutiveList << "FixedCorotated" << "NeoHookean" << "CamClay";
      constitutive->clear();
      constitutive->addItems(shortConstitutiveList);
      constitutive->setCurrentIndex(0);

      density->setText("750");
      youngsModulus->setText("2e9");
      poissonsRatio->setText("0.25");
      
      useHardening->setChecked(true);
      logJp->setText("-0.01");
      xi->setText("0.8");
      beta->setText("1.0");
      Mohr->setText("2.36");
    } else if (val == "Clay") {
      QStringList shortConstitutiveList;  shortConstitutiveList << "CamClay" << "DruckerPrager" << "FixedCorotated" << "NeoHookean" ;
      constitutive->clear();
      constitutive->addItems(shortConstitutiveList);
      constitutive->setCurrentIndex(0);      
      density->setText("1400");
      youngsModulus->setText("5e7");
      poissonsRatio->setText("0.25");
      useVolumeCorrection->setChecked(true);
      frictionAngle->setText("30.0");
      dilationAngle->setText("0.0");
      cohesion->setText("0.0");

      useHardening->setChecked(true);
      logJp->setText("-0.01");
      xi->setText("0.8");
      beta->setText("1.0");
      Mohr->setText("2.36");
    } else if (val == "Concrete") {
      QStringList shortConstitutiveList;  shortConstitutiveList << "CamClay" << "DruckerPrager" << "FixedCorotated" << "NeoHookean";
      constitutive->clear();
      constitutive->addItems(shortConstitutiveList);
      constitutive->setCurrentIndex(0);      
      density->setText("2400");
      youngsModulus->setText("2e7");
      poissonsRatio->setText("0.15");
      useVolumeCorrection->setChecked(true);
      frictionAngle->setText("30.0");
      dilationAngle->setText("0.0");
      cohesion->setText("0.0");

      useHardening->setChecked(true);
      logJp->setText("-0.01");
      xi->setText("0.8");
      beta->setText("1.0");
      Mohr->setText("2.36");
    } else if (val == "Sand") {
      QStringList shortConstitutiveList;  shortConstitutiveList << "DruckerPrager" << "CamClay" ;
      constitutive->clear();
      constitutive->addItems(shortConstitutiveList);
      constitutive->setCurrentIndex(0);      
      density->setText("1400");
      youngsModulus->setText("1e8");
      poissonsRatio->setText("0.2");
      useVolumeCorrection->setChecked(true);
      frictionAngle->setText("30.0");
      dilationAngle->setText("0.0");
      cohesion->setText("0.0");

      useHardening->setChecked(true);
      logJp->setText("-0.01");
      xi->setText("0.8");
      beta->setText("1.0");
      Mohr->setText("2.36");
    } else {
      QStringList shortConstitutiveList;  shortConstitutiveList << "JFluid" << "FixedCorotated" << "NeoHookean" << "DruckerPrager" << "CamClay" << "Custom";  
      constitutive->clear();
      constitutive->addItems(shortConstitutiveList);
      constitutive->setCurrentIndex(0);      
      density->setText("1400");
      
      bulkModulus->setText("1e7");
      bulkModulusDerivative->setText("7.15");
      viscosity->setText("1.0e-3");

      youngsModulus->setText("5e6");
      poissonsRatio->setText("0.2");

      useVolumeCorrection->setChecked(true);
      frictionAngle->setText("30.0");
      dilationAngle->setText("0.0");
      cohesion->setText("0.0");

      useHardening->setChecked(true);
      logJp->setText("-0.01");
      xi->setText("0.8");
      beta->setText("1.0");
      Mohr->setText("2.36");
    }
  });


  connect(constitutive, &QComboBox::currentTextChanged, [=](QString val) {
    if (val == "JFluid") {
      // Hide all widgets in the elastic box for now
      for (int i = 0; i < (2*3); ++i) elasticLayout->itemAt(i)->widget()->setVisible(false);
      youngsModulus->setVisible(false);
      poissonsRatio->setVisible(false);
      elasticBox->setDisabled(true);
      elasticBox->hide();
      bulkModulus->setVisible(true);
      viscosity->setVisible(true);
      materialStack->setCurrentIndex(0);
    } else if (val == "FixedCorotated") {
      elasticBox->setEnabled(true);
      elasticBox->show();
      elasticBox->setVisible(true);
      for (int i = 0; i < (2*3); ++i) elasticLayout->itemAt(i)->widget()->setVisible(true);
      youngsModulus->setVisible(true);
      poissonsRatio->setVisible(true);
      bulkModulus->setVisible(false);
      viscosity->setVisible(false);
      materialStack->setCurrentIndex(1);
    } else if (val == "NeoHookean") {
      elasticBox->setEnabled(true);
      elasticBox->show();
      elasticBox->setVisible(true);
      for (int i = 0; i < (2*3); ++i) elasticLayout->itemAt(i)->widget()->setVisible(true);
      youngsModulus->setVisible(true);
      poissonsRatio->setVisible(true);
      bulkModulus->setVisible(false);
      viscosity->setVisible(false);
      materialStack->setCurrentIndex(1);
    } else if (val == "DruckerPrager") {
      elasticBox->setEnabled(true);
      elasticBox->show();
      elasticBox->setVisible(true);
      for (int i = 0; i < (2*3); ++i) elasticLayout->itemAt(i)->widget()->setVisible(true);
      youngsModulus->setVisible(true);
      poissonsRatio->setVisible(true);
      bulkModulus->setVisible(false);
      viscosity->setVisible(false);
      materialStack->setCurrentIndex(2);
    } else if (val == "CamClay") {
      elasticBox->setEnabled(true);
      elasticBox->show();
      elasticBox->setVisible(true);
      for (int i = 0; i < (2*3); ++i) elasticLayout->itemAt(i)->widget()->setVisible(true);
      youngsModulus->setVisible(true);
      poissonsRatio->setVisible(true);
      bulkModulus->setVisible(false);
      viscosity->setVisible(false);
      materialStack->setCurrentIndex(3);
    } else {
      elasticBox->setEnabled(true);
      elasticBox->show();
      elasticBox->setVisible(true);
      for (int i = 0; i < (2*3); ++i) elasticLayout->itemAt(i)->widget()->setVisible(true);
      youngsModulus->setVisible(true);
      poissonsRatio->setVisible(true);
      bulkModulus->setVisible(false);
      viscosity->setVisible(false);
      materialStack->setCurrentIndex(4);
    } 
  });

  connect(useHardening, &QCheckBox::stateChanged, [=](int val) {
    if (val == 2) {
      xi->setVisible(true);
      beta->setVisible(true);
      Mohr->setVisible(true);
      logJp->setVisible(true);

      xi->setEnabled(true);
      beta->setEnabled(true);
      Mohr->setEnabled(true);
      logJp->setEnabled(true);
    } else {
      xi->setVisible(false);
      beta->setVisible(false);
      Mohr->setVisible(false);
      logJp->setVisible(false);

      xi->setEnabled(false);
      beta->setEnabled(false);
      Mohr->setEnabled(false);
      logJp->setEnabled(false);
    }
  });

  connect(useVolumeCorrection, &QCheckBox::stateChanged, [=](int val) {
    if (val == 2) {
      dilationAngle->setVisible(true);
      logJp->setVisible(true);
      beta->setVisible(true);

      dilationAngle->setEnabled(true);
      logJp->setEnabled(true);
      beta->setEnabled(true);
    } else {
      dilationAngle->setVisible(false);
      logJp->setVisible(false);
      beta->setVisible(false);

      dilationAngle->setEnabled(false);
      logJp->setEnabled(false);
      beta->setEnabled(false);
    }
  });

  // Set initial constitutive law
  constitutive->setCurrentIndex(1); // FixedCorotated
  constitutive->setCurrentIndex(1); // FixedCorotated

  // Set initial material preset
  materialPreset->setCurrentIndex(4); // Rubber for beginners/comp speed
  materialPreset->setCurrentIndex(4); // Rubber for beginners/comp speed
}

MaterialMPM::~MaterialMPM()
{

}

bool
MaterialMPM::setMaterialPreset(int index)
{
  if (index < 0 || index > materialPreset->count()) {
    // qDebug() << "MaterialMPM::setMaterialPreset() - Invalid index";
    return false;
  }
  materialPreset->setCurrentIndex(index);
  return true;
}

bool
MaterialMPM::outputToJSON(QJsonObject &jsonObject)
{
  // Future schema
  QJsonObject materialObject; 
  materialObject["material_preset"] = QJsonValue(materialPreset->currentText()).toString();
  materialObject["constitutive"] = QJsonValue(constitutive->currentText()).toString();
  materialObject["CFL"] = CFL->text().toDouble(); // TODO: Rename? "cfl"? Might be reserved in other contexts
  materialObject["rho"] = density->text().toDouble();
  if (constitutive->currentText() == "JFluid") {
    materialObject["bulk_modulus"] = bulkModulus->text().toDouble();
    materialObject["viscosity"] = viscosity->text().toDouble();
    materialObject["gamma"] = bulkModulusDerivative->text().toDouble();
  }
  else if (constitutive->currentText() == "FixedCorotated") {
    materialObject["youngs_modulus"] = youngsModulus->text().toDouble();
    materialObject["poisson_ratio"] = poissonsRatio->text().toDouble();
  }
  else if (constitutive->currentText() == "DruckerPrager") {
    materialObject["youngs_modulus"] = youngsModulus->text().toDouble();
    materialObject["poisson_ratio"] = poissonsRatio->text().toDouble();
    materialObject["cohesion"] = cohesion->text().toDouble();
    materialObject["friction_angle"] = frictionAngle->text().toDouble();
    materialObject["dilation_angle"] = dilationAngle->text().toDouble(); // Needed ?
    materialObject["beta"] = beta->text().toDouble(); // Needed ?
    materialObject["SandVolCorrection"] = useVolumeCorrection->isChecked() ? QJsonValue(true).toBool() : QJsonValue(false).toBool();
  }
  else if (constitutive->currentText() == "CamClay") {
    materialObject["youngs_modulus"] = youngsModulus->text().toDouble();
    materialObject["poisson_ratio"] = poissonsRatio->text().toDouble();
    materialObject["cohesion"] = cohesion->text().toDouble(); // Needed ?
    materialObject["friction_angle"] = frictionAngle->text().toDouble(); // Needed ?
    materialObject["hardeningOn"] = useHardening->isChecked() ? QJsonValue(true).toBool() : QJsonValue(false).toBool();
    materialObject["logJp0"] = logJp->text().toDouble();
    //
    materialObject["xi"] = xi->text().toDouble();
    materialObject["beta"] = beta->text().toDouble();
    materialObject["Mohr"] = (Mohr->text().toDouble() * Mohr->text().toDouble());
    //
    materialObject["hardening_ratio"] = xi->text().toDouble();
    materialObject["cohesion_ratio"] = beta->text().toDouble(); 
    materialObject["mohr_friction"] = Mohr->text().toDouble();
  }
  jsonObject["material"] = materialObject;

  // ClaymoreUW artifacts, global material values. TODO: Deprecate
  // jsonObject["material_preset"] = QJsonValue(materialPreset->currentText()).toString();
  // jsonObject["constitutive"] = QJsonValue(constitutive->currentText()).toString();
  // jsonObject["CFL"] = CFL->text().toDouble(); // TODO: Rename? "cfl"? Might be reserved in other contexts
  // jsonObject["rho"] = density->text().toDouble();
  // jsonObject["bulk_modulus"] = bulkModulus->text().toDouble();
  // jsonObject["youngs_modulus"] = youngsModulus->text().toDouble();
  // jsonObject["poisson_ratio"] = poissonsRatio->text().toDouble();
  // jsonObject["viscosity"] = viscosity->text().toDouble();
  // // TODO: Equation of state options (Murnaghan-Tait, Cole, Birch, etc. or JFluid, PA-JB Fluid, etc.)
  // jsonObject["gamma"] = bulkModulusDerivative->text().toDouble(); // TODO: Rename
  // // jsonObject["surface_tension"] = surfaceTension->text().toDouble(); // TODO: Implement
  // jsonObject["logJp0"] = logJp->text().toDouble();
  // jsonObject["SandVolCorrection"] = useVolumeCorrection->isChecked() ? QJsonValue(true).toBool() : QJsonValue(false).toBool();
  // jsonObject["cohesion"] = cohesion->text().toDouble(); // TODO: Specify units, I believe this is log(strain) currently hence small values
  // jsonObject["friction_angle"] = frictionAngle->text().toDouble();
  // jsonObject["dilation_angle"] = dilationAngle->text().toDouble(); // TODO: Check if this is used
  // jsonObject["hardeningOn"] = useHardening->isChecked() ? QJsonValue(true).toBool() : QJsonValue(false).toBool();
  // jsonObject["xi"] = xi->text().toDouble(); // TODO: Rename
  // jsonObject["beta"] = beta->text().toDouble(); // TODO: Rename
  // jsonObject["Mohr"] = (Mohr->text().toDouble() * Mohr->text().toDouble()); // TODO: Rename
  return true;
}

bool
MaterialMPM::inputFromJSON(QJsonObject &jsonObject)
{
  return true;
}

bool
MaterialMPM::copyFiles(QString &destDir)
{
  // if (theOpenSeesPyScript->copyFile(destDir) != true)
  //   return false;
  // return theSurfaceFile->copyFile(destDir);    
  return true;
}

