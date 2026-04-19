#ifndef OPENMESH_OPENMESH_DEMO_H
#define OPENMESH_OPENMESH_DEMO_H

#include "scene/3d/mesh_instance_3d.h"
#include "scene/main/node.h"

// FIXME: Demo code
class OpenMeshDemo : public Node {
	GDCLASS(OpenMeshDemo, Node);

protected:
	NodePath mesh_instance;

	static void _bind_methods();

public:
	OpenMeshDemo() = default;

	void set_mesh_instance(const NodePath &p_mi);
	NodePath get_mesh_instance() const;

	void run();
};

#endif // OPENMESH_OPENMESH_DEMO_H
