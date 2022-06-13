#include "MainWindow.h"
#include "Mesh.h"
#include "ui_MainWindow.h"

#include <vtk-9.1/vtkGenericOpenGLRenderWindow.h>
#include <vtk-9.1/vtkInteractorStyleUser.h>

#include <QFileDialog>
#include <QtConcurrentRun>
#include <QScreen>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_viewer(vtkSmartPointer<vtkImageViewer2>::New())
    , m_interactor(vtkSmartPointer<QVTKInteractor>::New())
    , m_unemittedProgress(0)
{
    ui->setupUi(this);

    move(screen()->geometry().center() - frameGeometry().center());

    m_viewer->SetRenderWindow(vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New());
    m_viewer->SetupInteractor(m_interactor);

    m_interactor->AddObserver(vtkCommand::MouseWheelForwardEvent, this, &MainWindow::on_forwardScrollEvent);
    m_interactor->AddObserver(vtkCommand::MouseWheelBackwardEvent, this, &MainWindow::on_backwardScrollEvent);

    m_interactor->SetInteractorStyle(vtkSmartPointer<vtkInteractorStyleUser>::New());

    ui->progressBar->setVisible(false);
    ui->progressText->setVisible(false);

    QObject::connect(&m_imageReader, &DICOMImageReader::readingStarted, this, &MainWindow::on_startImageReaderAlgoEvent);
    QObject::connect(&m_imageReader, &DICOMImageReader::readingFinished, this, &MainWindow::on_finishImageReaderAlgoEvent);
    QObject::connect(&m_imageReader, &DICOMImageReader::progressChanged, this, &MainWindow::on_imageReaderProgressChangedEvent);

    QObject::connect(&m_imageResize, &ImageResize::resizingStarted, this, &MainWindow::on_startAlgoEvent);
    QObject::connect(&m_imageResize, &ImageResize::resizingFinished, this, &MainWindow::on_finishAlgoEvent);
    QObject::connect(&m_imageResize, &ImageResize::progressChanged, this, &MainWindow::on_imageResizeProgressChangedEvent);

    QObject::connect(&m_marchingCubes, &MarchingCubesAlgorithm::reconstructionStarted, this, &MainWindow::on_startMCAlgoEvent);
    QObject::connect(&m_marchingCubes, &MarchingCubesAlgorithm::reconstructionFinished, this, &MainWindow::on_finishMCAlgoEvent);
    QObject::connect(&m_marchingCubes, &MarchingCubesAlgorithm::progressChanged, this, &MainWindow::on_marchingCubesProgressChangedEvent);
    QObject::connect(&m_marchingCubes, &MarchingCubesAlgorithm::progressShifted, this, &MainWindow::on_marchingCubesProgressShiftedEvent);

    QObject::connect(&m_objWriter, &OBJWriter::writingStarted, this, &MainWindow::on_startOBJWriterAlgoEvent);
    QObject::connect(&m_objWriter, &OBJWriter::writingFinished, this, &MainWindow::on_finishOBJWriterAlgoEvent);
    QObject::connect(&m_objWriter, &OBJWriter::progressChanged, this, &MainWindow::on_objWriterProgressChangedEvent);
    QObject::connect(&m_objWriter, &OBJWriter::errorOccured, this, &MainWindow::on_errorAlgoEvent);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_imageReaderProgressChangedEvent(double progressValue, const QString& progressText)
{
    ui->progressBar->setValue(progressValue * 100);
    ui->progressText->setText(progressText);
}

void MainWindow::on_imageResizeProgressChangedEvent(double progressValue)
{
    ui->progressBar->setValue(progressValue * 100);
    ui->progressText->setText("Масштабування...");
}

void MainWindow::on_startAlgoEvent()
{
    m_timer.restart();

    ui->progressBar->setVisible(true);
    ui->progressText->setVisible(true);
    ui->progressText->setText("");

    ui->progressBar->setValue(0);
}

void MainWindow::on_finishAlgoEvent()
{
    auto elapsedTime = m_timer.elapsed();

    auto progressText = QString("Готово (")
        .append(QString::number(elapsedTime / 1000.0))
        .append(QString(" с)"));

    ui->progressText->setText(progressText);
    ui->progressBar->setValue(100);

    m_timer.invalidate();
}

void MainWindow::on_startMCAlgoEvent()
{
    on_startAlgoEvent();
    ui->progressText->setText("Реконструкція...");
    ui->reconstructionButton->setEnabled(false);
    ui->exportButton->setEnabled(false);

    m_unemittedProgress = 0;
}

void MainWindow::on_finishMCAlgoEvent()
{
    on_finishAlgoEvent();
    ui->reconstructionButton->setEnabled(true);
    ui->exportButton->setEnabled(true);
}

void MainWindow::on_startImageReaderAlgoEvent()
{
    on_startAlgoEvent();
    ui->reconstructionGroupBox->setEnabled(false);
    ui->scrollBar->setEnabled(false);
}

void MainWindow::on_finishImageReaderAlgoEvent()
{
    on_finishAlgoEvent();

    auto reader = m_imageReader.GetInternalReader();

    // Set info labels and enable UI components

    auto width = reader->GetWidth();
    auto height = reader->GetHeight();
    auto sliceCount = reader->GetOutput()->GetDimensions()[2];

    setWindowTitle(
        QString("Computed Tomography Reconstruction [Path: ")
            .append(reader->GetDirectoryName())
            .append("]")
    );

    ui->fileResolution->setText(
        QString::number(width) + " x " + QString::number(height)
    );

    ui->fileCount->setText(
        QString::number(sliceCount)
    );

    ui->targetResolutionSlider->setMaximum(width);

    ui->reconstructionGroupBox->setEnabled(true);
    ui->reconstructionButton->setEnabled(true);
    ui->scrollBar->setEnabled(true);

    ui->tissueDensityLabel->setEnabled(false);
    ui->targetResolution->setEnabled(false);
    ui->huTitle->setEnabled(false);

    // Set image viewing

    m_viewer->SetInputData(reader->GetOutput());

    ui->qVTKWidget->setRenderWindow(m_viewer->GetRenderWindow());

    const auto minSlice = m_viewer->GetSliceMin();
    const auto maxSlice = m_viewer->GetSliceMax();

    ui->scrollBar->setMinimum(minSlice);
    ui->scrollBar->setMaximum(maxSlice);
    ui->scrollBar->setSliderPosition(minSlice);

    m_viewer->SetSlice(minSlice);
    m_viewer->Render();
}

void MainWindow::on_startOBJWriterAlgoEvent()
{
    on_startAlgoEvent();
    ui->reconstructionButton->setEnabled(false);
    ui->openFileButton->setEnabled(false);
}

void MainWindow::on_finishOBJWriterAlgoEvent()
{
    on_finishAlgoEvent();
    ui->reconstructionButton->setEnabled(true);
    ui->openFileButton->setEnabled(true);
}

void MainWindow::on_errorAlgoEvent(const QString& msg)
{
    ui->progressText->setText(msg);
}

void MainWindow::on_selectFolderButton_clicked()
{
    auto inputDir = QFileDialog::getExistingDirectory(this, QString("Open DICOM folder"), QDir::currentPath());

    if (inputDir.isEmpty())
        return;

    auto readImagesTask = QtConcurrent::run(
        [this, inputDir]() {
            m_imageReader.SetDirectoryName(inputDir);
            m_imageReader.Update();
        }
    );
}

void MainWindow::on_reconstructionButton_clicked()
{   
    if (ui->tissueTypeRadioBox->isChecked())
    {
        switch (ui->tissueType->currentIndex())
        {
        case 0:
            m_marchingCubes.SetIsoValue(200);
            break;
        case 1:
            m_marchingCubes.SetIsoValue(40);
            break;
        case 2:
            m_marchingCubes.SetIsoValue(-120);
            break;
        }
    }
    else
    {
        m_marchingCubes.SetIsoValue(
            ui->tissueDensity->value()
        );
    }

    m_lastProcessedIsovalue = m_marchingCubes.GetIsoValue();
    m_lastProcessedFolder = QDir(m_imageReader.GetInternalReader()->GetDirectoryName()).dirName();

    const auto resizeTask = [this]() {
        if (!ui->changeTargetResolutionCheckBox->isChecked())
            return vtkSmartPointer(m_imageReader.GetInternalReader()->GetOutput());

        auto outputResolution = ui->targetResolutionSlider->value();

        const auto& image = m_imageReader.GetInternalReader()->GetOutput();

        m_imageResize.SetInputData(image);

        m_imageResize.SetOutputDimensions(
            outputResolution, outputResolution, image->GetDimensions()[2]
        );

        m_imageResize.Update();

        return vtkSmartPointer(m_imageResize.GetInternalResize()->GetOutput());
    };

    const auto reconstructionTask = QtConcurrent::run(resizeTask)
        .then([this](QFuture<vtkSmartPointer<vtkImageData>> futureImageData) {
            m_marchingCubes.ReconstructMesh(futureImageData.result());
        }
    );
}

void MainWindow::on_targetResolutionSlider_valueChanged(int value)
{
    ui->targetResolution->setText(QString::number(value));
}

void MainWindow::on_changeTargetResolutionCheckBox_clicked(bool checked)
{
    ui->targetResolutionSlider->setEnabled(checked);
    ui->targetResolution->setEnabled(checked);
}

void MainWindow::on_exportButton_clicked()
{
    QString fileName = QString(m_lastProcessedFolder)
        .append(QString::number(m_lastProcessedIsovalue))
        .append("HU.obj");

    m_lastOutputFile = QFileDialog::getSaveFileName(this, QString("Save OBJ file"), fileName);

    if (m_lastOutputFile.isEmpty())
        return;

    m_objWriter.SetFileName(m_lastOutputFile);
    m_objWriter.SetInputMesh(m_marchingCubes.GetMesh());

    auto exportTask = QtConcurrent::run(
        [this]() {
            m_objWriter.Write();
        }
    );
}

void MainWindow::on_tissueDensityRadioBox_clicked(bool checked)
{
    ui->tissueType->setEnabled(!checked);
    ui->tissueDensity->setEnabled(checked);
    ui->tissueDensityLabel->setEnabled(checked);
    ui->huTitle->setEnabled(checked);
}


void MainWindow::on_tissueTypeRadioBox_clicked(bool checked)
{
    ui->tissueType->setEnabled(checked);
    ui->tissueDensity->setEnabled(!checked);
    ui->tissueDensityLabel->setEnabled(!checked);
    ui->huTitle->setEnabled(!checked);
}


void MainWindow::on_tissueDensity_valueChanged(int value)
{
    ui->tissueDensityLabel->setText(QString::number(value));
}

void MainWindow::on_objWriterProgressChangedEvent(float progress)
{
    ui->progressBar->setValue(progress * 100);
    ui->progressText->setText("Збереження...");
}

void MainWindow::on_marchingCubesProgressShiftedEvent(float deltaProgress)
{
    m_unemittedProgress += (deltaProgress * 100);

    if (m_unemittedProgress > 1.0)
    {
        const auto integralProgress = static_cast<int>(m_unemittedProgress);

        ui->progressBar->setValue(
            ui->progressBar->value() + integralProgress
        );

        m_unemittedProgress -= integralProgress;
    }
}

void MainWindow::on_marchingCubesProgressChangedEvent(float progress)
{
    ui->progressBar->setValue(progress * 100);
}

void MainWindow::on_scrollBar_valueChanged(int value)
{
    m_viewer->SetSlice(value);
    m_viewer->Render();
}

void MainWindow::on_openFileButton_clicked()
{
    QDesktopServices::openUrl(
        QUrl("file:///" + m_lastOutputFile)
    );
}

void MainWindow::on_forwardScrollEvent()
{
    ui->scrollBar->setValue(
        ui->scrollBar->value() - 1
    );
}

void MainWindow::on_backwardScrollEvent()
{
    ui->scrollBar->setValue(
        ui->scrollBar->value() + 1
    );
}
