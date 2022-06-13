#pragma once

#include <../../../../../Dev/vcpkg/packages/glm_x64-windows/include/glm/vec3.hpp>

#include <QList>

#include <memory>

class Mesh
{
public:
    void InsertNextVertex(const glm::vec3& v);

    void InsertNextTriangle(const glm::uvec3& t);

    void Merge(const std::shared_ptr<Mesh>& m);

    const QList<glm::vec3>& GetVertices() const;

    const QList<glm::uvec3>& GetTriangles() const;

private:
    QList<glm::vec3>  m_vertices;
    QList<glm::uvec3> m_triangles;
};

