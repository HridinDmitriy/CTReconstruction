#include "DICOMImageReader.h"

#include <vtk-9.1/vtkCommand.h>

DICOMImageReader::DICOMImageReader()
    : m_reader{ vtkSmartPointer<vtkDICOMImageReader>::New()  }
{
    m_reader->AddObserver(
        vtkCommand::StartEvent, this, &DICOMImageReader::readingStartedCallback);

    m_reader->AddObserver(
        vtkCommand::EndEvent, this, &DICOMImageReader::readingFinishedCallback);

    m_reader->AddObserver(
        vtkCommand::ProgressEvent, this, &DICOMImageReader::progressChangedCallback);
}

void DICOMImageReader::Update()
{
    try{
    m_reader->Update();
    }
    catch(...)
    {
        return;
    }
}

void DICOMImageReader::SetDirectoryName(const QString& fileName)
{
    m_reader->SetDirectoryName(fileName.toLocal8Bit().constData());
}

vtkSmartPointer<vtkDICOMImageReader> DICOMImageReader::GetInternalReader() const
{
    return m_reader;
}

void DICOMImageReader::readingStartedCallback()
{
    emit readingStarted();
}

void DICOMImageReader::readingFinishedCallback()
{
    emit readingFinished();
}

void DICOMImageReader::progressChangedCallback(vtkObject* caller, unsigned long, void *)
{
    auto reader = static_cast<vtkDICOMImageReader*>(caller);

    auto progressValue = reader->GetProgress();
    auto progressText = reader->GetProgressText();

    emit progressChanged(progressValue, progressText);
}
