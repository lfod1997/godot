#ifndef OPENMESH_SHAREDPROPERTYCONTAINERT
#define OPENMESH_SHAREDPROPERTYCONTAINERT

#include "Core/Utils/PropertyContainer.hh"
#include "Core/Utils/SharedPropertyT.hh"

namespace OpenMesh {

class BaseKernel;

template <Domain D>
class SharedPropertyContainerT : public PropertyContainer {};
}

#endif // OPENMESH_SHAREDPROPERTYCONTAINERT
