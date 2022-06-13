#include "MarchingCubesAlgorithm.h"
#include "MarchingCubesTables.h"
#include "Mesh.h"

#include <QtConcurrentRun>
#include <QFutureSynchronizer>

spMesh MarchingCubesAlgorithm::ReconstructMesh(vtkSmartPointer<vtkImageData> volumetricImageData)
{
    m_imageData = volumetricImageData;

    m_imageDataDimensions = {
        static_cast<size_t>(m_imageData->GetDimensions()[0]),
        static_cast<size_t>(m_imageData->GetDimensions()[1]),
        static_cast<size_t>(m_imageData->GetDimensions()[2])
    };

    m_imageSpacing = {
        static_cast<float>(m_imageData->GetSpacing()[0]),
        static_cast<float>(m_imageData->GetSpacing()[1]),
        static_cast<float>(m_imageData->GetSpacing()[2])
    };

    ValidateImageSpacing();

    m_mesh = std::make_shared<Mesh>();

    const size_t maxThreadsCount = 20;
    const size_t slicesPerThread = (m_imageDataDimensions[2] - 1) % maxThreadsCount == 0
        ? (m_imageDataDimensions[2] - 1) / maxThreadsCount
        : (m_imageDataDimensions[2] - 1) / maxThreadsCount + 1;

    // Run parallel mesh reconstruction.

    QFutureSynchronizer<spMesh> sync;

    emit reconstructionStarted();

    for (size_t threadIdx = 0; threadIdx < maxThreadsCount; ++threadIdx)
    {
        size_t firstSlice = (threadIdx + 0) * slicesPerThread;
        size_t lastSlice = (threadIdx + 1) * slicesPerThread;

        if (lastSlice > m_imageDataDimensions[2] - 1)
        {
            lastSlice = m_imageDataDimensions[2] - 1;
        }

        sync.addFuture(
            QtConcurrent::run(
                &MarchingCubesAlgorithm::ReconstructMeshInternal,
                this,
                firstSlice,
                lastSlice
            )
        );
    }

    sync.waitForFinished();

    // Merge all meshes into one.

    auto futures = sync.futures();
    auto futuresCount = futures.size();

    for (int futureIndex = 0; futureIndex < futuresCount; ++futureIndex)
    {
        m_mesh->Merge(
            futures.at(futureIndex).result()
        );
    }

    emit progressChanged(1.0f);
    emit reconstructionFinished();

    return m_mesh;
}

spMesh MarchingCubesAlgorithm::GetMesh()
{
    return m_mesh;
}

int16_t MarchingCubesAlgorithm::GetIsoValue() const
{
    return m_isoValue;
}

void MarchingCubesAlgorithm::SetIsoValue(int16_t isoValue)
{
    m_isoValue = isoValue;
}

void MarchingCubesAlgorithm::ProcessOneSlice(size_t sliceIndex, size_t& pointsCount, const spMesh& outputMesh) const
{
    for (size_t rowIndex = 0; rowIndex < m_imageDataDimensions[0] - 1; ++rowIndex)
    {
        for (size_t colIndex = 0; colIndex < m_imageDataDimensions[1] - 1; ++colIndex)
        {
            Voxel voxel = CreateVoxelFromPixelCoord(rowIndex, colIndex, sliceIndex);

            auto caseIndex = GetEdgeIntersectionCaseIndex(voxel);

            // No intersected edges
            if (edgeIntersectionCaseTable[caseIndex] == 0x000 ||
                edgeIntersectionCaseTable[caseIndex] == 0xfff)
            {
                continue;
            }

            auto intersectionPoints = CalculateIntersectionPoints(voxel, caseIndex, pointsCount);

            // Add new points
            for (size_t edgeIndex = 0; edgeIndex < VOXEL_EDGES_COUNT; ++edgeIndex)
            {
                if (intersectionPoints[edgeIndex].second != NO_INTERSECTION)
                {
                    outputMesh->InsertNextVertex(
                        intersectionPoints[edgeIndex].first
                    );
                }
            }

            // Build triangles
            for (size_t pointIndex = 0; triangleCaseTable[caseIndex][pointIndex] != -1; pointIndex += 3)
            {
                // Insert triangle represented with indices of vertices
                outputMesh->InsertNextTriangle({
                    intersectionPoints[triangleCaseTable[caseIndex][pointIndex + 0]].second,
                    intersectionPoints[triangleCaseTable[caseIndex][pointIndex + 1]].second,
                    intersectionPoints[triangleCaseTable[caseIndex][pointIndex + 2]].second
                });
            }
        }
    }
}

spMesh MarchingCubesAlgorithm::ReconstructMeshInternal(size_t startSliceIndex, size_t endSliceIndex)
{
    const auto totalSlicesCount = m_imageDataDimensions[2] - 1;

    auto mesh = std::make_shared<Mesh>();

    size_t pointsCount{ 0 };

    for (size_t sliceIndex = startSliceIndex; sliceIndex < endSliceIndex; ++sliceIndex)
    {
        ProcessOneSlice(sliceIndex, pointsCount, mesh);
        emit progressShifted(1.0f / totalSlicesCount);
    }
    return mesh;
}

Voxel MarchingCubesAlgorithm::CreateVoxelFromPixelCoord(size_t i, size_t j, size_t k) const
{
    Voxel voxel{};

    for (size_t vertexNumber = 0; vertexNumber < VOXEL_VERTICES_COUNT; ++vertexNumber)
    {
        const glm::uvec3 pixelPosition = {
            i + voxelVerticesPositions[vertexNumber][0],
            j + voxelVerticesPositions[vertexNumber][1],
            k + voxelVerticesPositions[vertexNumber][2]
        };

        const glm::vec3 pointPosition = {
            pixelPosition[0] * m_imageSpacing[0],
            pixelPosition[1] * m_imageSpacing[1],
            pixelPosition[2] * m_imageSpacing[2]
        };

        const auto huValue = static_cast<isovalue_t*>(
            m_imageData->GetScalarPointer(pixelPosition[0], pixelPosition[1], pixelPosition[2])
        )[0];

        voxel.vertices[vertexNumber].position = pointPosition;
        voxel.vertices[vertexNumber].value = huValue;
    }

    return voxel;
}

index_t MarchingCubesAlgorithm::GetEdgeIntersectionCaseIndex(const Voxel& voxel) const
{
    index_t index{ 0 };

    for (size_t vertexIndex = 0; vertexIndex < VOXEL_VERTICES_COUNT; ++vertexIndex)
    {
        auto isoValue = voxel.vertices[vertexIndex].value;

        if (isoValue >= m_isoValue)
        {
            index |= (1ui8 << vertexIndex);
        }
    }
    return index;
}

MarchingCubesAlgorithm::intersection_points_t
MarchingCubesAlgorithm::CalculateIntersectionPoints(const Voxel& voxel, index_t intersectionCaseIndex, size_t& pointCount) const
{
    point_index_pair defaultPair = std::make_pair(glm::vec3{}, std::numeric_limits<size_t>::max());
    intersection_points_t intersectionPoints(VOXEL_EDGES_COUNT, defaultPair);

    for (size_t edgeIndex = 0; edgeIndex < VOXEL_EDGES_COUNT; ++edgeIndex)
    {
        // Current edge is intersected
        if (edgeIntersectionCaseTable[intersectionCaseIndex] & (1ui16 << edgeIndex))
        {
            auto vertexIndex1 = voxelEdges[edgeIndex][0];
            auto vertexIndex2 = voxelEdges[edgeIndex][1];

            intersectionPoints[edgeIndex] = std::make_pair(
                ComputeVertexInterpolation(
                    voxel.vertices[vertexIndex1].position, voxel.vertices[vertexIndex2].position,
                    voxel.vertices[vertexIndex1].value, voxel.vertices[vertexIndex2].value
                ),
                pointCount++
            );
        }
    }

    return intersectionPoints;
}

glm::vec3 MarchingCubesAlgorithm::ComputeVertexInterpolation(
    const glm::vec3& p1, const glm::vec3& p2, isovalue_t val1, isovalue_t val2) const
{
    const auto interpolationParameter = (m_isoValue - val1) /
            static_cast<float>(val2 - val1);

    return p1 + (p2 - p1) * interpolationParameter;
}

void MarchingCubesAlgorithm::ValidateImageSpacing()
{
    for (size_t idx = 0; idx < m_imageSpacing.size(); ++idx)
    {
        auto& spacing = m_imageSpacing[idx];

        if (spacing < 1e-7)
            spacing = 1;
    }
}
