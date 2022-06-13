#include "Mesh.h"

#include <QtConcurrent/QtConcurrentMap>

void Mesh::InsertNextVertex(const glm::vec3& v)
{
    m_vertices.push_back(v);
}

void Mesh::InsertNextTriangle(const glm::uvec3& t)
{
    m_triangles.push_back(t);
}

void Mesh::Merge(const std::shared_ptr<Mesh> &m)
{
    const size_t verticesCount = m_vertices.size();
    const size_t trianglesCount = m_triangles.size();

    m_vertices.append(m->GetVertices());
    m_triangles.append(m->GetTriangles());

    const auto mapper = [verticesCount](auto& indices) {
        indices += verticesCount;
    };

    QtConcurrent::map(
        m_triangles.begin() + trianglesCount,
        m_triangles.end(),
        mapper
    ).waitForFinished();
}

const QList<glm::vec3> &Mesh::GetVertices() const
{
    return m_vertices;
}

const QList<glm::uvec3> &Mesh::GetTriangles() const
{
    return m_triangles;
}
