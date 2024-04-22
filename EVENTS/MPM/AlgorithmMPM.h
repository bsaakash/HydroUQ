#ifndef ALGORITHM_MPM_H
#define ALGORITHM_MPM_H

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

/**
 *  @author  fmckenna
 *  @date    2/2017
 *  @version 1.0
 *
 *  @section DESCRIPTION
 *
 *  This is the class providing the Algorithm Tab for mpm
 */

#include <SimCenterWidget.h>

class QJsonObject;
class QJsonArray;
class SC_DoubleLineEdit;
class SC_IntLineEdit;
class SC_StringLineEdit;
class SC_ComboBox;
class SC_TableEdit;
class SC_FileEdit;
class SC_CheckBox;

class AlgorithmMPM : public SimCenterWidget
{
public:
    AlgorithmMPM(QWidget *parent = 0);
    virtual ~AlgorithmMPM();
    bool outputToJSON(QJsonObject &jsonObject);
    bool inputFromJSON(QJsonObject &jsonObject);
    bool copyFiles(QString &dirName);

    void enableFBAR (bool isChecked);
    void enableASFLIP (bool isChecked);
    void enableFBAR_fusedG2P2G (bool isChecked);

    void disableFBAR (bool isChecked);
    void disableASFLIP (bool isChecked);
    void disableFBAR_fusedG2P2G (bool isChecked);

    void setFBAR_psi (double psi);
    void setASFLIP_alpha (double alpha);
    void setASFLIP_betaMin (double betaMin);
    void setASFLIP_betaMax (double betaMax);

    double getFBAR_psi();
    double getASFLIP_alpha();
    double getASFLIP_betaMin();
    double getASFLIP_betaMax();
    // void setFBAR_fusedG2P2G (bool isChecked);

signals:

private:

  SC_ComboBox       *numericalMethod; // Particles, Mesh
  SC_DoubleLineEdit *particlesPerCell;
  SC_CheckBox       *useASFLIP; // use ASFLIP
  SC_DoubleLineEdit *ASFLIP_alpha; // ASFLIP alpha
  SC_DoubleLineEdit *ASFLIP_betaMin; // ASFLIP beta
  SC_DoubleLineEdit *ASFLIP_betaMax; // ASFLIP beta
  SC_CheckBox       *useFBAR; // use FBAR
  SC_DoubleLineEdit *FBAR_psi; // FBAR psi
  SC_CheckBox       *useFBAR_fusedG2P2G; // use FBARXSPH

};

#endif // ALGORITHM_MPM_H

