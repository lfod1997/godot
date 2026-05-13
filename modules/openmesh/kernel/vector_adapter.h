// ReSharper disable CppRedundantQualifier
#ifndef OPENMESH_GODOT_KERNEL_VECTOR_ADAPTER_H
#define OPENMESH_GODOT_KERNEL_VECTOR_ADAPTER_H

// OpenMesh
#include "Core/Utils/vector_traits.hh"
#include "Core/Utils/GenProg.hh"
// Godot
#include "core/typedefs.h"
#include "core/math/vector2.h"
#include "core/math/vector3.h"
#include "core/math/vector4.h"
#include "core/math/vector2i.h"
#include "core/math/vector3i.h"
#include "core/math/vector4i.h"
#include "encoded_normal.h"
// STL
#include <iterator>
#include <algorithm>

#define OMG_VECTOR_TRAITS_SPEC(VecType, ScaType) \
	template <> struct vector_traits<VecType> { \
		typedef VecType vector_type; \
		typedef ScaType value_type; \
		typedef GenProg::Int2Type<vector_type::AXIS_COUNT> typed_size; \
		static const size_t size_ = vector_type::AXIS_COUNT; \
		static size_t size() { return size_; } \
	}

#define OMG_VECTOR_REAL_OPS_IMPL(VecType, ScaType) \
	_FORCE_INLINE_ ScaType dot(const VecType &lhs, const VecType &rhs) { return lhs.dot(rhs); } \
	_FORCE_INLINE_ void swap(VecType &lhs, VecType &rhs) noexcept(noexcept(SWAP(lhs, rhs))) { SWAP(lhs, rhs); } \
	_FORCE_INLINE_ VecType &vectorize(VecType &v, const ScaType val) { \
		std::fill(std::begin(v.coord), std::end(v.coord), val); \
		return v; \
	} \
	_FORCE_INLINE_ ScaType norm(const VecType &v) { return v.length(); } \
	_FORCE_INLINE_ ScaType sqrnorm(const VecType &v) { return v.length_squared(); } \
	_FORCE_INLINE_ VecType &normalize(VecType &v) { v.normalize(); return v; } \
	_FORCE_INLINE_ VecType &maximize(VecType &lhs, VecType &rhs) \
	{ \
		std::transform( \
			std::begin(lhs.coord), std::end(lhs.coord), std::begin(rhs.coord), std::begin(lhs.coord), \
			[] (const ScaType a, const ScaType b) { return std::max(a, b); } \
		); \
		return lhs; \
	} \
	_FORCE_INLINE_ VecType &minimize(VecType &lhs, VecType &rhs) \
	{ \
		std::transform( \
			std::begin(lhs.coord), std::end(lhs.coord), std::begin(rhs.coord), std::begin(lhs.coord), \
			[] (const ScaType a, const ScaType b) { return std::min(a, b); } \
		); \
		return lhs; \
	} \
	_FORCE_INLINE_ VecType max(const VecType &lhs, const VecType &rhs) { return lhs.max(rhs); } \
	_FORCE_INLINE_ VecType min(const VecType &lhs, const VecType &rhs) { return lhs.min(rhs); }

#define OMG_VECTOR_INT_OPS_IMPL(VecType, ScaType) \
	_FORCE_INLINE_ void swap(VecType &lhs, VecType &rhs) noexcept(noexcept(SWAP(lhs, rhs))) { SWAP(lhs, rhs); } \
	_FORCE_INLINE_ VecType &vectorize(VecType &v, const ScaType val) { \
		std::fill(std::begin(v.coord), std::end(v.coord), val); \
		return v; \
	} \
	_FORCE_INLINE_ double norm(const VecType &v) { return v.length(); } \
	_FORCE_INLINE_ int64_t sqrnorm(const VecType &v) { return v.length_squared(); } \
	_FORCE_INLINE_ VecType &maximize(VecType &lhs, VecType &rhs) \
	{ \
		std::transform( \
			std::begin(lhs.coord), std::end(lhs.coord), std::begin(rhs.coord), std::begin(lhs.coord), \
			[] (const ScaType a, const ScaType b) { return std::max(a, b); } \
		); \
		return lhs; \
	} \
	_FORCE_INLINE_ VecType &minimize(VecType &lhs, VecType &rhs) \
	{ \
		std::transform( \
			std::begin(lhs.coord), std::end(lhs.coord), std::begin(rhs.coord), std::begin(lhs.coord), \
			[] (const ScaType a, const ScaType b) { return std::min(a, b); } \
		); \
		return lhs; \
	} \
	_FORCE_INLINE_ VecType max(const VecType &lhs, const VecType &rhs) { return lhs.max(rhs); } \
	_FORCE_INLINE_ VecType min(const VecType &lhs, const VecType &rhs) { return lhs.min(rhs); }

namespace OpenMesh
{
	OMG_VECTOR_TRAITS_SPEC(::Vector2, real_t);
	OMG_VECTOR_TRAITS_SPEC(::Vector3, real_t);
	OMG_VECTOR_TRAITS_SPEC(::Vector4, real_t);
	OMG_VECTOR_TRAITS_SPEC(::Vector2i, int32_t);
	OMG_VECTOR_TRAITS_SPEC(::Vector3i, int32_t);
	OMG_VECTOR_TRAITS_SPEC(::Vector4i, int32_t);

	template <> struct vector_traits<::Color> {
		typedef ::Color vector_type;
		typedef float value_type;
		typedef GenProg::Int2Type<4> typed_size;
		static const size_t size_ = 4;
		static size_t size() { return size_; }
	};

	_FORCE_INLINE_ ::Vector3 cross(const ::Vector3 &lhs, const ::Vector3 &rhs) { return lhs.cross(rhs); }

	OMG_VECTOR_REAL_OPS_IMPL(::Vector2, real_t);
	OMG_VECTOR_REAL_OPS_IMPL(::Vector3, real_t);
	OMG_VECTOR_REAL_OPS_IMPL(::Vector4, real_t);

	// For integral vectors, return types of `norm` and `sqrnorm` are hard-coded as they are in their original impls
	OMG_VECTOR_INT_OPS_IMPL(::Vector2i, int32_t);
	OMG_VECTOR_INT_OPS_IMPL(::Vector3i, int32_t);
	OMG_VECTOR_INT_OPS_IMPL(::Vector4i, int32_t);
}

#undef OMG_VECTOR_TRAITS_SPEC
#undef OMG_VECTOR_REAL_OPS_IMPL
#undef OMG_VECTOR_INT_OPS_IMPL

#endif // OPENMESH_GODOT_KERNEL_VECTOR_ADAPTER_H
