#include "openmesh_demo.h"
#include "kernel/mesh.h"

// Godot
#include "core/error/error_macros.h"
#include "scene/3d/mesh_instance_3d.h"

using namespace OpenMesh::Godot;
using OpenMesh::SmartVertexHandle;

void OpenMeshDemo::set_mesh_instance(const NodePath& p_mi)
{
	ERR_MAIN_THREAD_GUARD;
	mesh_instance = p_mi;
	notify_property_list_changed();
}

NodePath OpenMeshDemo::get_mesh_instance() const
{
	ERR_MAIN_THREAD_GUARD_V(NodePath());
	return mesh_instance;
}

// Builds a simple cube.
void OpenMeshDemo::run()
{
	ERR_FAIL_COND_MSG(!get_tree(), "Not inside a tree.");

	PolyMesh mesh;

	// Generate vertices
	SmartVertexHandle vhs[8];
	vhs[0] = mesh.add_vertex({-1, -1, 1});
	vhs[1] = mesh.add_vertex({1, -1, 1});
	vhs[2] = mesh.add_vertex({1, 1, 1});
	vhs[3] = mesh.add_vertex({-1, 1, 1});
	vhs[4] = mesh.add_vertex({-1, -1, -1});
	vhs[5] = mesh.add_vertex({1, -1, -1});
	vhs[6] = mesh.add_vertex({1, 1, -1});
	vhs[7] = mesh.add_vertex({-1, 1, -1});

	// Generate faces and their normals (OpenMesh uses CCW winding order)
	// ReSharper disable CppPossiblyUnintendedObjectSlicing
	mesh.set_normal(mesh.add_face(vhs[0], vhs[1], vhs[2], vhs[3]), {0, 0, 1}); // Front
	mesh.set_normal(mesh.add_face(vhs[7], vhs[6], vhs[5], vhs[4]), {0, 0, -1}); // Back
	mesh.set_normal(mesh.add_face(vhs[1], vhs[0], vhs[4], vhs[5]), {0, -1, 0}); // Bottom
	mesh.set_normal(mesh.add_face(vhs[2], vhs[1], vhs[5], vhs[6]), {1, 0, 0}); // Right
	mesh.set_normal(mesh.add_face(vhs[3], vhs[2], vhs[6], vhs[7]), {0, 1, 0}); // Top
	mesh.set_normal(mesh.add_face(vhs[0], vhs[3], vhs[7], vhs[4]), {-1, 0, 0}); // Left
	mesh.update_vertex_normals(); // "Shade smooth"

	// Test
	ERR_FAIL_COND_MSG(mesh_instance.is_empty(), "No mesh instance assigned.");
	MeshInstance3D* mi = cast_to<MeshInstance3D>(get_node_or_null(mesh_instance));
	ERR_FAIL_COND_MSG(!mi, vformat("Invalid mesh instance assigned: '%s'.", mesh_instance));

	Ref<Mesh> ref_mesh = mi->get_mesh();
	if (ref_mesh.is_valid() && ref_mesh->get_surface_count() > 0)
	{
		print_line(vformat("Ref array:\n%s", ref_mesh->surface_get_arrays(0)));
	}

	Ref<ArrayMesh> am;
	am.instantiate();
	Array arr = MeshConverter::to_surface_arrays(mesh);
	am->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arr);
	mi->set_mesh(am);
	print_line(vformat("Generated array:\n%s", arr));
}

void OpenMeshDemo::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_mesh_instance", "mesh_instance"), &OpenMeshDemo::set_mesh_instance);
	ClassDB::bind_method(D_METHOD("get_mesh_instance"), &OpenMeshDemo::get_mesh_instance);
	ClassDB::bind_method(D_METHOD("run"), &OpenMeshDemo::run);

	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "mesh_instance", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "MeshInstance3D"),"set_mesh_instance", "get_mesh_instance");
}
