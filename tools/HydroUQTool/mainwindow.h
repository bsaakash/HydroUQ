/*********************************************************************************
**
** Copyright (c) 2020 University of California, Berkeley
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice, this
** list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice, this
** list of conditions and the following disclaimer in the documentation and/or other
** materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its contributors may
** be used to endorse or promote products derived from this software without specific
** prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
** EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
** SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
** BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
** IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
***********************************************************************************/

// Contributors:
// Ajay B Harish, Post-Doc @ SimCenter, UC Berkeley
// Dr. Frank McKenna, CTO of SimCenter, UC Berkeley
// Prof. Sanjay Govindjee, Director of SimCenter, UC Berkeley

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//*********************************************************************************
// Include user headers
//*********************************************************************************
#include "../Eigen/Dense"

//*********************************************************************************
// Include library headers
//*********************************************************************************
#include <QMainWindow>
#include <QTreeWidget>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    std::string gettoolname();

protected:
    //std::string toolname = "H20-UQ";

private slots:

    // Item tree on left
    void on_SimOptions_itemClicked(QTreeWidgetItem *item, int column);

    // Options stack
    void on_OptionsStack_currentChanged(int arg1);

    // Project settings
    void on_Led_AA_PName_editingFinished();
    void on_Led_AA_PDesc_textChanged();
    void on_Btn_AA_WDir_clicked();

    // Bathymetry (SW solutions)
    void on_Btn_BA_SelFiles_clicked();
    void on_Btn_BA_UploadSolution_clicked();
    void on_ChB_BA_UploadBox_stateChanged(int arg1);
    void on_Btn_BA_UploadFile_clicked();
    void on_Btn_BA_Previous_clicked();
    void on_Btn_BA_Next_clicked();
    void on_SWg_BA_Interface_currentChanged(int arg1);
    void on_Btn_BA_EntryAddPt_clicked();
    void on_Btn_BA_EntryRemPt_clicked();
    void on_Btn_BA_ExitAddPt_clicked();
    void on_Btn_BA_ExitRemPt_clicked();
    void on_Btn_BA_S01AddPt_clicked();
    void on_Btn_BA_S01RemPt_clicked();
    void on_Btn_BA_S02AddPt_clicked();
    void on_Btn_BA_S02RemPt_clicked();

    // Bathymetry (SW solutions from library)
    void on_ChB_BB_UploadFile_stateChanged(int arg1);
    void on_Btn_BB_UploadFile_clicked();
    void on_Btn_BB_Previous_clicked();
    void on_Btn_BB_Next_clicked();
    void on_SWg_BB_Interface_currentChanged(int arg1);
    void on_Btn_BB_EntryAddPt_clicked();
    void on_Btn_BB_EntryRemPt_clicked();
    void on_Btn_BB_ExitAddPt_clicked();
    void on_Btn_BB_ExitRemPt_clicked();
    void on_Btn_BB_S01AddPt_clicked();
    void on_Btn_BB_S01RemPt_clicked();
    void on_Btn_BB_S02AddPt_clicked();
    void on_Btn_BB_S02RemPt_clicked();

    // Bathymetry (User bathymetry)
    void on_Btn_BC_FileUpload_clicked();

    // Bathymetry (Wave flume)
    void on_Btn_BD_AddSeg_clicked();
    void on_Btn_BD_RemSeg_clicked();

    // Building
    void on_Btn_CA_AddBuild_clicked();
    void on_Btn_CA_RemBuild_clicked();

    // Meshing
    void on_Cmb_EAMeshType_currentIndexChanged(int index);
    void on_Btn_EA_AddRegion_clicked();
    void on_Btn_EA_RemRegion_clicked();

    // Initial conditions
    void on_Btn_GA_AddRegion_clicked();
    void on_Btn_GA_RemRegion_clicked();

    // Boundary conditions - user bathymetry
    void on_SWg_HB_BConditions_currentChanged(int arg1);
    void on_Btn_HB_Previous_clicked();
    void on_Btn_HB_Next_clicked();
    void on_Cmb_HB_NorthVelBC_currentIndexChanged(int index);
    void on_Cmb_HB_SouthUBC_currentIndexChanged(int index);
    void on_Cmb_HB_EastUBC_currentIndexChanged(int index);
    void on_Cmb_HB_WestUBC_currentIndexChanged(int index);
    void on_Cmb_HB_NorthPresBC_currentIndexChanged(int index);
    void on_Cmb_HB_SouthPreBC_currentIndexChanged(int index);
    void on_Cmb_HB_EastPresBC_currentIndexChanged(int index);
    void on_Cmb_HB_WestPresBC_currentIndexChanged(int index);

    // Boundary conditions - Wave flume
    void on_SWg_HC_BConditions_currentChanged(int arg1);
    void on_Btn_HC_Previous_clicked();
    void on_Btn_HC_Next_clicked();
    void on_Cmb_HC_EntryUBC_currentIndexChanged(int index);
    void on_Cmb_HC_EntryPresBC_currentIndexChanged(int index);
    void on_Cmb_HC_ExitUBC_currentIndexChanged(int index);
    void on_Cmb_HC_ExitPresBC_currentIndexChanged(int index);

    // Solver
    void on_ChB_IA_Restart_stateChanged(int arg1);

    void on_Btn_IA_UploadFiles_clicked();

private:

    // Initialize
    void initialize();

    Ui::MainWindow *ui;
    Eigen::MatrixXi optionmap; // Connects parameter tree to options widget
    std::string toolname = "H20-UQ";
    QUrl workdirUrl; // Default work directory
    QStringList optiontree,bathfilenames,solfilenames,intefilenames,restartfiles; // Filenames



};
#endif // MAINWINDOW_H