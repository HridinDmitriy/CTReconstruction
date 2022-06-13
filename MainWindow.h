#pragma once

#include "DICOMImageReader.h"
#include "ImageResize.h"
#include "MarchingCubesAlgorithm.h"
#include "OBJWriter.h"

#include <vtk-9.1/vtkImageViewer2.h>
#include <vtk-9.1/vtkInteractorStyle.h>
#include <vtk-9.1/QVTKInteractor.h>

#include <QtWidgets/QMainWindow>

#include <qelapsedtimer.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    using ImageViewer = vtkSmartPointer<vtkImageViewer2>;
    using UserInteractor = vtkSmartPointer<vtkRenderWindowInteractor>;

private slots:
    void on_selectFolderButton_clicked();

    void on_reconstructionButton_clicked();

    void on_targetResolutionSlider_valueChanged(int value);

    void on_changeTargetResolutionCheckBox_clicked(bool checked);

    void on_exportButton_clicked();

    void on_imageReaderProgressChangedEvent(double progressValue, const QString& progressText);

    void on_imageResizeProgressChangedEvent(double progressValue);

    void on_startAlgoEvent();

    void on_finishAlgoEvent();

    void on_startMCAlgoEvent();

    void on_finishMCAlgoEvent();

    void on_startImageReaderAlgoEvent();

    void on_finishImageReaderAlgoEvent();

     void on_startOBJWriterAlgoEvent();

    void on_finishOBJWriterAlgoEvent();

    void on_errorAlgoEvent(const QString& msg);

    void on_tissueDensityRadioBox_clicked(bool checked);

    void on_tissueTypeRadioBox_clicked(bool checked);

    void on_tissueDensity_valueChanged(int value);

    void on_objWriterProgressChangedEvent(float progress);

    void on_marchingCubesProgressChangedEvent(float progress);

    void on_marchingCubesProgressShiftedEvent(float deltaProgress);

    void on_scrollBar_valueChanged(int value);

    void on_openFileButton_clicked();

    void on_forwardScrollEvent();

    void on_backwardScrollEvent();

private:
    Ui::MainWindow*			ui;

    DICOMImageReader        m_imageReader;
    ImageResize             m_imageResize;
    MarchingCubesAlgorithm  m_marchingCubes;
    OBJWriter               m_objWriter;
    ImageViewer             m_viewer;
    UserInteractor          m_interactor;

    QElapsedTimer           m_timer;

    qint32                  m_lastProcessedIsovalue;
    QString                 m_lastProcessedFolder;
    QString                 m_lastOutputFile;

    float                   m_unemittedProgress; // In range [0; 100]
};
