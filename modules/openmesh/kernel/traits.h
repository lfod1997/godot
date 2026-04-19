// ReSharper disable CppRedundantQualifier
#ifndef OPENMESH_GODOT_KERNEL_TRAITS_H
#define OPENMESH_GODOT_KERNEL_TRAITS_H

// ReSharper disable once CppUnusedIncludeDirective
#include "vector_adapter.h"

// OpenMesh
#include "Core/Mesh/Traits.hh"

namespace OpenMesh::Godot
{
	struct Traits : DefaultTraits
	{
		typedef ::Vector3 Point;
		typedef ::Vector3 Normal;
		typedef ::Vector2 TexCoord2D;
		typedef ::Vector3 TexCoord3D;
		typedef ::Color Color;

		VertexAttributes(Attributes::Status | Attributes::Normal);
		HalfedgeAttributes(Attributes::Status | Attributes::PrevHalfedge);
		EdgeAttributes(Attributes::Status);
		FaceAttributes(Attributes::Status | Attributes::Normal);
	};
}

#endif // OPENMESH_GODOT_KERNEL_TRAITS_H
