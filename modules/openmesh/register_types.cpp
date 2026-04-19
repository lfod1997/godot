#include "register_types.h"

#include "core/object/class_db.h"
#include "openmesh_demo.h"

void initialize_openmesh_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	ClassDB::register_class<OpenMeshDemo>();
}

void uninitialize_openmesh_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	// Nothing to do here in this example.
}
