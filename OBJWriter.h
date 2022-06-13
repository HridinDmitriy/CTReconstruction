#pragma once

#include <QObject>

#include "Mesh.h"

class OBJWriter: public QObject
{
    Q_OBJECT

public:
    void SetInputMesh(std::shared_ptr<Mesh> mesh);
    void SetFileName(const QString& fileName);
    void Write();

signals:
    void writingStarted();
    void writingFinished();
    void progressChanged(float progress);
    void errorOccured(const QString& msg);

private:
    std::shared_ptr<Mesh>   m_mesh;
    QString                 m_fileName;
};

