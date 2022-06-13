#pragma once

#include <vtk-9.1/vtkSmartPointer.h>
#include <vtk-9.1/vtkDICOMImageReader.h>

#include <QObject>
#include <QString>

/// <summary>
/// DICOMImageReader class. Allows to obtain a volumetric image
/// from multiple slices
/// </summary>
class DICOMImageReader: public QObject
{
    Q_OBJECT

public:

    /// <summary>
    /// Default ctor.
    /// </summary>
    DICOMImageReader();

    /// <summary>
    /// Perform reading so that the internal reader will contain
    /// resulting (volumetric) image.
    /// </summary>
    void Update();

    /// <summary>
    /// Set name of the directory with input DICOM images.
    /// </summary>
    void SetDirectoryName(const QString& fileName);

	/// <summary>
    /// Get internal vtkDICOMImageReader instance.
	/// </summary>
    vtkSmartPointer<vtkDICOMImageReader> GetInternalReader() const;

signals:
    void readingStarted();
    void readingFinished();
    void progressChanged(double proggressValue, const QString& progressText);

private:
    void readingStartedCallback();
    void readingFinishedCallback();
    void progressChangedCallback(vtkObject*, unsigned long, void*);

private:
    vtkSmartPointer<vtkDICOMImageReader>    m_reader;
};
