#pragma once

#include <vtk-9.1/vtkSmartPointer.h>
#include <vtk-9.1/vtkImageResize.h>

#include <QObject>

/// <summary>
/// ImageResize class. Allows to resize image to the desired size.
/// </summary>
class ImageResize: public QObject
{
    Q_OBJECT

public:

    /// <summary>
    /// Default ctor.
    /// </summary>
    ImageResize();

    /// <summary>
    /// Perform resizing so that the internal resize will contain
    /// resulting image.
    /// </summary>
    void Update();

    /// <summary>
    /// Set dimensions for the output image.
    /// </summary>
    void SetOutputDimensions(int width, int height, int depth);

    /// <summary>
    /// Set input image.
    /// </summary>
    void SetInputData(vtkDataObject* image);

    /// <summary>
    /// Get internal vtkImageResize instance.
    /// </summary>
    vtkSmartPointer<vtkImageResize> GetInternalResize() const;

signals:
    void resizingStarted();
    void resizingFinished();
    void progressChanged(double proggressValue);

private:
    void resizingStartedCallback();
    void resizingFinishedCallback();
    void progressChangedCallback(vtkObject*, unsigned long, void*);

private:
    vtkSmartPointer<vtkImageResize>    m_resize;
};
