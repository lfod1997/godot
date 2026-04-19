## Out-of-plans

- Blend shapes: stands on topological isomorphism, but our goal is (precisely) to be able to change the topology
- `RS::ARRAY_FLAG_COMPRESS_ATTRIBUTES`: vertices, normals and tangents are mixed up togeter, making the custom kernel impossible to implement

## Features

### API

- V/HE/E/F handle types (as `RefCounted`)
- Mesh *const* operators
  - Iterators & circulators
  - Statics in `SmartRangeT`
- Mesh *data-mutating* and *topo-mutating* operators
  - General ops in `PolyConnectivity`
  - Poly-specific ops in `PolyMeshT`
  - Tri-specific ops in `TriConnectivity`, `TriMeshT`
- Tangent recalculation that uses Mikktspace
- Connectivity disambiguation (poly or tri?)
- Vertex-merging converter from `ArrayMesh` to tri-connectivity EditableMesh

### Core EditableMesh

- Custom mesh kernel that provides:
  - Transparent triangulation (provided by a custom Godot `Mesh` type)
    - Handles per-half-edge properties that give rise to added vertices (UV / split normals)
    - IBO copies every time the topology is mutated
  - Partial buffer update
    - Over-allocate VBO / ABO with a factor & do partial `mesh_surface_update_XXX_region`
- Garbage collection scheduler when mesh entities are deleted (vertex, edge, face)
- Auxiliary properties
  - Other properties that `Mesh::ArrayType` supports
  - Tangents (not an OpenMesh concept)
    - Double precision tangents? (see: `RenderingServer::_surface_set_data`)
  - User-defined properties (`PropertyManager`)
    - Unnamed (temporary) -> prop handles -> `RefCounted`
    - Named?
- LOD handling (None / Manual / Auto)
  - None: no LODs whatsoever
  - Manual: user calls regenerate_lods()
  - Auto: regenerate LODs on every mesh commit
- Support soft body
  - Basic support is just to call `PhysicsServer3D::soft_body_set_mesh(SoftBody3D::get_physics_rid(), MESH_RID)` upon mesh edit, but can be super heavy for large meshes / per-frame use cases
  - Better support is possible (as we hold the actual topology) but needs investigation
- Support *skinning* by inventing a partial weight retargeting feature?

### EditableMesh as a 3D Asset Import Option

- Polygon-preserving importers
  - **FBX**: good file size / ease of use / maturity
    - ufbx supports polygon
    - Need to preserve non-mesh objects and scene structure
  - .blend? (by silently calling Blender to export FBX)
  - OBJ?: needs investigation
- Tri-mesh-only importers
  - glTF: Import as `ArrayMesh`, then convert to EditableMesh?
  - Other formats supported by Godot?

### Utilities

- Subdivision (Catmull-Clark for quads, Sqrt-3 for tris)
- Triangulation
- Tris-to-quads operator?
- Smooth operators

## Library Comparison

Reasons why I've chosen OpenMesh, amongst all the amazing quad mesh libraries (cons with exclamation mark = "stops us from using it"):

- **[OpenMesh](https://gitlab.vci.rwth-aachen.de:9000/OpenMesh/OpenMesh)**:
  - Pros:
    - Mature (25 years old), academy-backed
    - Lots of operators like the subdividers
    - Arbitrary property support: complex mesh algorithms made easy
    - The idea of circulators enables concise C++ code
    - Only depends on STL
    - Barely throws
  - Cons:
    - Source code readability compromise:
      - Heavy use of template, with unconstrained template parameters
      - Multi-inheritance
    - Mesh garbage collection: stale handles may occur if not scheduled properly
- [CGAL](https://www.cgal.org/)
  - Pros:
    - Mature (30+ years old), industry standard
    - Tons of operators
  - Cons:
    - The two main poly mesh data structures ([`Polyhedron_3`](https://doc.cgal.org/latest/Polyhedron/classCGAL_1_1Polyhedron__3.html) and [`Surface_mesh`](https://doc.cgal.org/latest/Surface_mesh/classCGAL_1_1Surface__mesh.html)) are both GPL-licensed!
    - Gigantic library
    - Modules come from multiple authors with diverging throwing conventions
- [Yocto/GL](https://github.com/xelatihy/yocto-gl) (2.9k stars):
  - Pros:
    - Exposes raw buffers
    - Known to be extremely fast
    - Routine-oriented, easy to integrate
    - Modular, flat library structure with minimal dependencies
  - Cons:
    - No support for adding / deleting mesh entities!
    - No support for N-gons
    - Throws sometimes
- [geogram](https://github.com/BrunoLevy/geogram) (2.4k stars):
  - Pros:
    - Mature, production-tested
    - Supports very advanced operators like boolean and CSG
  - Cons:
    - Many dependencies and complex CMake-ing!
    - Throws a lot, needs much work to integrate!

The following libs depend on Eigen, thus not our consideration:

- [Polygon Mesh Processing Library (PMP)](https://www.pmp-library.org/) (1.5k stars)
- [CinoLib](https://github.com/mlivesu/cinolib) (1.1k stars)
- [Geometry Central](https://github.com/nmwsharp/geometry-central) (1.3k stars)

## Compatibility

```c++
Array EditableMesh::surface_get_arrays(int p_surface) const {
	ERR_FAIL_INDEX_V(p_surface, int(surfaces.size()), Array());
	return RS::get_singleton()->mesh_surface_get_arrays(mesh, p_surface);
}
RID EditableMesh::get_rid() const {
	return mesh;
}
EditableMesh::EditableMesh() {
	mesh = RS::get_singleton()->mesh_create();
}
EditableMesh::~EditableMesh() {
	ERR_FAIL_NULL(RenderingServer::get_singleton());
	RS::get_singleton()->free(mesh);
}
```

## Consistency

- Handle macro: `_3D_DISABLED`
