#pragma once

#include <vtk-9.1/vtkSmartPointer.h>
#include <vtk-9.1/vtkImageData.h>

#include <QObject>
#include <QString>

#include <../../../../../Dev/vcpkg/packages/glm_x64-windows/include/glm/vec3.hpp>

#include <vector>
#include <array>

class Mesh;

using isovalue_t = int16_t;
using index_t = uint8_t;
using spMesh = std::shared_ptr<Mesh>;

 /// <summary>
 /// Specifies count of the voxel vertices.
 /// </summary>
constexpr uint8_t VOXEL_VERTICES_COUNT{ 8 };

/// <summary>
/// Specifies count of the voxel edges.
/// </summary>
constexpr uint8_t VOXEL_EDGES_COUNT{ 12 };

/// <summary>
/// Specifies an index value indicating
/// no intersection for an edge.
/// </summary>
constexpr size_t NO_INTERSECTION{ std::numeric_limits<size_t>::max() };

/// <summary>
/// Represents vertex of a voxel with 
/// own position and value.
/// </summary>
struct VoxelVertex
{
    /// <summary>
    /// Absolute position in the 3D image space
    /// </summary>
    glm::vec3 position;

    /// <summary>
    /// Iso-value of the vertex
    /// </summary>
    isovalue_t value;
};

/// <summary>
/// Represents voxel as a VoxelVertex array.
/// </summary>
struct Voxel
{
    /// <summary>
    /// Represents vertex array
    /// </summary>
    std::array<VoxelVertex, VOXEL_VERTICES_COUNT> vertices;
};

/// <summary>
/// Describes local vertices positions.
/// </summary>
constexpr std::array<glm::u8vec3, VOXEL_VERTICES_COUNT> voxelVerticesPositions
{ {
    {0,0,0},
    {1,0,0},
    {1,1,0},
    {0,1,0},
    {0,0,1},
    {1,0,1},
    {1,1,1},
    {0,1,1}
} };

/// <summary>
/// Specifies each voxel edge with two points - its ends.
/// </summary>
constexpr std::array<std::array<uint8_t, 2>, VOXEL_EDGES_COUNT> voxelEdges
{ {
    {0,1}, {1,2}, {2,3}, {3,0},
    {4,5}, {5,6}, {6,7}, {7,4},
    {0,4}, {1,5}, {2,6}, {3,7}
} };

/// <summary>
/// The MarchingCubesAlgorithm class. Represents the class for 
/// mesh reconstruction from a volumetric image.
/// </summary>
class MarchingCubesAlgorithm: public QObject
{
    Q_OBJECT

public:
    /// <summary>
    /// Reconstruct mesh from the volumetric image.
    /// </summary>
    /// <param name="volumetricImageData">Specifies input volumetric data represented by 3D image.</param>
    spMesh ReconstructMesh(vtkSmartPointer<vtkImageData> volumetricImageData);

    /// <summary>
    /// Get the reconstructed mesh.
    /// </summary>
    /// <returns>Reconstructed mesh as set of vertices and triangles.</returns>
    spMesh GetMesh();

    /// <summary>
    /// Get the iso value that will be taken into account
    /// during the reconstruction. 
    /// </summary>
    /// <returns>The iso value.</returns>
    isovalue_t GetIsoValue() const;

    /// <summary>
    /// Set minimum and maximum iso values which will be taken into account
    /// during the reconstruction.
    /// </summary>
    /// <param name="isoValue">Specifes the iso value.</param>
    void SetIsoValue(isovalue_t isoValue);

signals:
    void reconstructionStarted();
    void reconstructionFinished();
    void progressChanged(float progress);
    void progressShifted(float deltaProgress);

private:
    using point_index_pair = std::pair<glm::vec3, size_t>;
    using intersection_points_t = std::vector<point_index_pair>;

    /// <summary>
    /// Reconstruct slice with index equal to the sliceIndex. Resulting
    /// points and triangles add to the outputMesh. The pointsCount is
    /// modified in accordance with number of new points created during
    /// reconstruction.
    /// </summary>
    void ProcessOneSlice(size_t sliceIndex, size_t& pointsCount, const spMesh& outputMesh) const;

    /// <summary>
    ///
    /// </summary>
    spMesh ReconstructMeshInternal(size_t startSliceIndex, size_t endSliceIndex);

    /// <summary>
    /// Create voxel from image data with origin at position (i, j, k). Each vertex 
    /// of voxel will contain respective pixel value of the image.
    /// </summary>
    /// <param name="i">Specifies column of the input 3D image.</param>
    /// <param name="j">Specifies row of the input 3D image.</param>
    /// <param name="k">Specifies slice of the input 3D image.</param>
    /// <returns>Voxel with specified position and values in its vertices according to the image data.</returns>
    Voxel CreateVoxelFromPixelCoord(size_t i, size_t j, size_t k) const;

    /// <summary>
    /// Get index of the case which describes the state of the voxel vertices relative
    /// to the isovalue. 
    /// </summary>
    /// <param name="voxel">Specifies a voxel to be tested with isovalue.</param>
    /// <returns>Index in range [0, 255]</returns>
    index_t GetEdgeIntersectionCaseIndex(const Voxel& voxel) const;

    /// <summary>
    /// Calculate intersection points by computing interpolation of the vertices of the edges.
    /// </summary>
    /// <param name="voxel">Specifies the voxel that was intersected.</param>
    /// <param name="intersectionCaseIndex">Specifies index of the case which describes the state of the voxel vertices relative
    /// to the isovalue. </param>
    /// <param name="newPointCount">Specefies the output parameter for storing the number of points.</param>
    /// <returns>The vector of pairs storing the intersection points and their indices for each edge of the voxel. 
    /// If some edge doesn't contain any intersection point, the respective pair will store the default constructed point and 
    /// std::numeric_limits&lt;size_t&gt;::max() as its index.</returns>
    intersection_points_t CalculateIntersectionPoints(const Voxel& voxel, index_t intersectionCaseIndex, size_t& pointCount) const;

    /// <summary>
    /// Compute vertex interpolation between p1 and p2 which are specified with weight values.
    /// </summary>
    /// <param name="p1">Specifies the first vertex</param>
    /// <param name="p2">Specifies the second vertex</param>
    /// <param name="val1">Specifies the weight of the first vertex</param>
    /// <param name="val2">Specifies the weight of the second vertex</param>
    /// <returns>Interpolated point between p1 and p2.</returns>
    glm::vec3 ComputeVertexInterpolation(const glm::vec3& p1, const glm::vec3& p2, isovalue_t val1, isovalue_t val2) const;

    /// <summary>
    /// Check image spacing for zero and, if true, set to default (1).
    /// </summary>
    void ValidateImageSpacing();

private:
    vtkSmartPointer<vtkImageData>   m_imageData;
    std::array<size_t, 3>           m_imageDataDimensions;
    std::array<float, 3>            m_imageSpacing;
    isovalue_t                      m_isoValue;
    spMesh                          m_mesh;
};
