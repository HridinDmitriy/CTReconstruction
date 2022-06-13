#include "ImageResize.h"

#include <vtk-9.1/vtkCommand.h>

ImageResize::ImageResize()
    : m_resize{ vtkSmartPointer<vtkImageResize>::New() }
{
    m_resize->AddObserver(
        vtkCommand::StartEvent, this, &ImageResize::resizingStartedCallback);

    m_resize->AddObserver(
        vtkCommand::EndEvent, this, &ImageResize::resizingFinishedCallback);

    m_resize->AddObserver(
        vtkCommand::ProgressEvent, this, &ImageResize::progressChangedCallback);
}

void ImageResize::Update()
{
    m_resize->Update();
}

void ImageResize::SetOutputDimensions(int width, int height, int depth)
{
    m_resize->SetOutputDimensions(width, height, depth);
}

void ImageResize::SetInputData(vtkDataObject* image)
{
    m_resize->SetInputData(image);
}

vtkSmartPointer<vtkImageResize> ImageResize::GetInternalResize() const
{
    return m_resize;
}

void ImageResize::resizingStartedCallback()
{
    emit resizingStarted();
}

void ImageResize::resizingFinishedCallback()
{
    emit resizingFinished();
}

void ImageResize::progressChangedCallback(vtkObject* caller, unsigned long, void *)
{
    auto resize = static_cast<vtkImageResize*>(caller);
    auto progressValue = resize->GetProgress();

    emit progressChanged(progressValue);
}
