// ReSharper disable CppRedundantQualifier
#ifndef OPENMESH_GODOT_KERNEL_MESH_H
#define OPENMESH_GODOT_KERNEL_MESH_H

#include "traits.h"

// Godot
#include "core/variant/array.h"
#include "core/variant/variant.h"
#include "scene/resources/mesh.h"
// OpenMesh
#include "Core/Mesh/TriConnectivity.hh"
#include "Core/Mesh/FinalMeshItemsT.hh"
#include "Core/Mesh/AttribKernelT.hh"
#include "Core/Mesh/PolyMeshT.hh"
#include "Core/Mesh/TriMeshT.hh"
#include "Core/Utils/PropertyManager.hh"
// STL
#include <vector>

namespace OpenMesh::Godot
{
	typedef PolyMeshT<
		AttribKernelT<FinalMeshItemsT<Traits, false>, PolyConnectivity>
	> PolyMesh;

	typedef TriMeshT<
		AttribKernelT<FinalMeshItemsT<Traits, true>, TriConnectivity>
	> TriMesh;

	// FIXME: Demo code
	// Simple converter to Godot Mesh surface arrays.
	class MeshConverter
	{
	public:
		template <typename MeshT>
		static Array to_surface_arrays(const MeshT &mesh)
		{
			Array arrays;
			arrays.resize(::Mesh::ARRAY_MAX);

			// Vertices
			{
				const std::vector<typename MeshT::Point> &data = mesh.property(mesh.points_pph()).data_vector();
				Vector<typename MeshT::Point> buffer;
				buffer.resize(data.size());
				std::memcpy(buffer.ptrw(), data.data(), data.size() * sizeof(typename MeshT::Point));
				arrays[::Mesh::ARRAY_VERTEX] = buffer;
			}
			// Normals
			if (mesh.has_vertex_normals())
			{
				const std::vector<typename MeshT::Normal> &data = mesh.property(mesh.vertex_normals_pph()).data_vector();
				Vector<typename MeshT::Normal> buffer;
				buffer.resize(data.size());
				std::memcpy(buffer.ptrw(), data.data(), data.size() * sizeof(typename MeshT::Normal));
				arrays[::Mesh::ARRAY_NORMAL] = buffer;
			}
			// Colors
			if (mesh.has_vertex_colors())
			{
				const std::vector<typename MeshT::Color> &data = mesh.property(mesh.vertex_colors_pph()).data_vector();
				Vector<typename MeshT::Color> buffer;
				buffer.resize(data.size());
				std::memcpy(buffer.ptrw(), data.data(), data.size() * sizeof(typename MeshT::Color));
				arrays[::Mesh::ARRAY_COLOR] = buffer;
			}
			// UVs
			if (mesh.has_vertex_texcoords2D())
			{
				const std::vector<typename MeshT::TexCoord2D> &data = mesh.property(mesh.vertex_texcoords2D_pph()).data_vector();
				Vector<typename MeshT::TexCoord2D> buffer;
				buffer.resize(data.size());
				std::memcpy(buffer.ptrw(), data.data(), data.size() * sizeof(typename MeshT::TexCoord2D));
				arrays[::Mesh::ARRAY_TEX_UV] = buffer;
			}
			// Indices
			if (mesh.n_faces() > 0)
			{
				PackedInt32Array indices;
				// Initial estimate: 3 vertices per face
				indices.resize(mesh.n_faces() * 3);
				int32_t *iw = indices.ptrw();
				int i = 0;
				for (const FaceHandle &fh : mesh.faces())
				{
					int j = 0;
					int32_t first_idx = -1;
					int32_t prev_idx = -1;
					for (const VertexHandle &vh : mesh.fv_cw_range(fh))
					{
						// Fan triangulation
						if (j == 0)
						{
							first_idx = vh.idx();
						}
						else if (j == 1)
						{
							prev_idx = vh.idx();
						}
						else
						{
							if (i + 3 > indices.size())
							{
								indices.resize(indices.size() * 2);
								iw = indices.ptrw();
							}
							iw[i++] = first_idx;
							iw[i++] = prev_idx;
							iw[i++] = vh.idx();
							prev_idx = vh.idx();
						}
						++j;
					}
				}
				indices.resize(i);
				arrays[::Mesh::ARRAY_INDEX] = indices;
			}
			return arrays;
		}
	};
}

#endif // OPENMESH_GODOT_KERNEL_MESH_H
