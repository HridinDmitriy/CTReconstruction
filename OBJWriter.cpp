#include "OBJWriter.h"

void OBJWriter::SetInputMesh(std::shared_ptr<Mesh> mesh)
{
    m_mesh = std::move(mesh);
}

void OBJWriter::SetFileName(const QString &fileName)
{
    m_fileName = fileName;
}

void OBJWriter::Write()
{
    if (!m_mesh)
        return;

    FILE* file;

    auto status = fopen_s(&file, m_fileName.toLocal8Bit().constData(), "w");

    if (status != 0)
    {
        emit errorOccured("An error occured while opening file.");
    }

    emit writingStarted();

    const auto verticesCount = m_mesh->GetVertices().size();
    const auto trianglesCount = m_mesh->GetTriangles().size();

    const auto entriesCount = verticesCount + trianglesCount;
    const auto emittingPeriod = 1000;

    // Write vertices
    for (int vertexIdx = 0; vertexIdx < verticesCount; ++vertexIdx)
    {
        const auto& v = m_mesh->GetVertices().at(vertexIdx);
        fprintf(file, "v %.5g %.5g %.5g\n", v.x, v.y, v.z);

        if (vertexIdx % emittingPeriod == 0)
            emit progressChanged(vertexIdx / static_cast<float>(entriesCount));
    }

    fprintf(file, "\n");

    // Write triangles
    for (int triangleIdx = 0; triangleIdx < trianglesCount; ++triangleIdx)
    {
        const auto& t = m_mesh->GetTriangles().at(triangleIdx);
        fprintf(file, "f %d %d %d\n", t.x + 1, t.y + 1, t.z + 1);

        if (triangleIdx % emittingPeriod == 0)
            emit progressChanged((verticesCount + triangleIdx) / static_cast<float>(entriesCount));
    }

    emit progressChanged(1.0f);
    emit writingFinished();

    fclose(file);
}
