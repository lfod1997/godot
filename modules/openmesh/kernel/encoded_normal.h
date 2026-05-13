// ReSharper disable CppRedundantQualifier
#ifndef OPENMESH_GODOT_KERNEL_ENCODED_NORMAL_H
#define OPENMESH_GODOT_KERNEL_ENCODED_NORMAL_H

// Godot
#include "core/math/vector2.h"
#include "core/math/vector3.h"
#include "core/math/vector4.h"
// OpenMesh
#include "Core/Utils/vector_traits.hh"
#include "Core/Utils/GenProg.hh"

#define OMG_AXIS_WRITE_PROXY_ASN_OP_IMPL(op) \
	_FORCE_INLINE_ real_t operator op (real_t value) { \
		ERR_FAIL_COND_V(!target, 0.0f); \
		Vector3 n = target->get_normal(); \
		n[axis] op value; \
		target->set_normal(n); \
		return value; \
	}

#define OMG_AXIS_WRITE_PROXY_CMP_OP_IMPL(op, eq) \
	_FORCE_INLINE_ bool operator op (const AxisWriteProxy &p_rhs) const { \
		ERR_FAIL_COND_V(!target || !p_rhs.target, false); \
		if (target == p_rhs.target) { \
			if (unlikely(axis == p_rhs.axis)) { return eq; } \
			Vector3 n = target->get_normal(); \
			return n[axis] op n[p_rhs.axis]; \
		} \
		else { return real_t(*this) op real_t(p_rhs); } \
	}

#define OMG_ENCODED_NORMAL_CAST_ASN_OP_IMPL(op) \
	EncodedNormal& EncodedNormal::operator op (const EncodedNormal &p_rhs) { \
		return operator op (p_rhs.get_normal()); \
	}

#define OMG_ENCODED_NORMAL_CAST_MATH_OP_IMPL(op) \
	EncodedNormal EncodedNormal::operator op (const EncodedNormal &p_rhs) const { \
		return operator op (p_rhs.get_normal()); \
	}

/// Octahedron-encoded normal-and-tangent vector type, each component is 16-bit fixed precision, sizeof: 8 Bytes.
struct [[nodiscard]] EncodedNormal {
	class [[nodiscard]] AxisWriteProxy {
		friend struct EncodedNormal;

		EncodedNormal *target = nullptr;
		int axis = -1;

		_FORCE_INLINE_ AxisWriteProxy() = default;
		_FORCE_INLINE_ AxisWriteProxy(EncodedNormal *p_target, int p_axis) :
			target(p_target), axis(p_axis) {}

	public:
		// ReSharper disable once CppNonExplicitConversionOperator
		_FORCE_INLINE_ operator real_t() const { return target->get_normal()[axis]; }
		_FORCE_INLINE_ real_t operator-() const { return -operator real_t(); }
		_FORCE_INLINE_ real_t operator+() const { return operator real_t(); }

		OMG_AXIS_WRITE_PROXY_ASN_OP_IMPL(=)

		OMG_AXIS_WRITE_PROXY_CMP_OP_IMPL(!=, false)
		OMG_AXIS_WRITE_PROXY_CMP_OP_IMPL(==, true)
		OMG_AXIS_WRITE_PROXY_CMP_OP_IMPL(<, false)
		OMG_AXIS_WRITE_PROXY_CMP_OP_IMPL(<=, true)
		OMG_AXIS_WRITE_PROXY_CMP_OP_IMPL(>, false)
		OMG_AXIS_WRITE_PROXY_CMP_OP_IMPL(>=, true)
	};

	static const int AXIS_COUNT = 3;

	enum Axis {
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
	};

	union {
		struct {
			uint16_t encoded_normal[2];
			uint16_t encoded_tangent[2];
		};

		uint64_t hash;

		uint8_t data[8] = { 0 };
	};

	_FORCE_INLINE_ constexpr EncodedNormal() = default;
	_FORCE_INLINE_ explicit EncodedNormal(const uint8_t *p_data) { memcpy(data, p_data, sizeof(data)); }

	_FORCE_INLINE_ EncodedNormal(const Vector3 &p_norm, const Vector3 &p_tan, real_t p_tan_dir = 1.0f) {
		set_normal_only(p_norm);
		set_tangent_only(p_tan, p_tan_dir);
	}

	// This is a "widening" conversion so can be implicit.
	// ReSharper disable once CppNonExplicitConvertingConstructor
	_FORCE_INLINE_ EncodedNormal(const Vector3 &p_norm) :
		EncodedNormal(p_norm, Vector3(p_norm.z, -p_norm.x, p_norm.y).cross(p_norm.normalized()).normalized()) {}

	_FORCE_INLINE_ EncodedNormal(const Vector3 &p_norm, const Vector4 &p_tan) :
		EncodedNormal(p_norm, reinterpret_cast<const Vector3&>(p_tan), p_tan.w) {}

	_FORCE_INLINE_ EncodedNormal(real_t p_n_x, real_t p_n_y, real_t p_n_z) :
		EncodedNormal(Vector3(p_n_x, p_n_y, p_n_z)) {}

	_FORCE_INLINE_ EncodedNormal(real_t p_n_x, real_t p_n_y, real_t p_n_z, real_t p_t_x, real_t p_t_y, real_t p_t_z, real_t p_t_d = 1.0f) :
		EncodedNormal(Vector3(p_n_x, p_n_y, p_n_z), Vector3(p_t_x, p_t_y, p_t_z), p_t_d) {}

	// This is a "narrowing" conversion so must be explicit.
	/// Decodes and returns the normal part, discarding the tangent part.
	_FORCE_INLINE_ explicit operator Vector3() const { return get_normal(); }

	/// Sets the normal part, updating the tangent part to keep orthogonality.
	_FORCE_INLINE_ EncodedNormal &operator=(const Vector3 &p_norm) {
		set_normal(p_norm);
		return *this;
	}

	_FORCE_INLINE_ EncodedNormal operator-() const;

	_FORCE_INLINE_ bool operator!=(const EncodedNormal &p_rhs) const { return hash != p_rhs.hash; }
	_FORCE_INLINE_ bool operator==(const EncodedNormal &p_rhs) const { return !operator!=(p_rhs); }

	/// Mutates the normal part, updating the tangent part to keep orthogonality.
	EncodedNormal &operator+=(const Vector3 &p_rhs);
	/// Mutates the normal part, updating the tangent part to keep orthogonality.
	EncodedNormal &operator-=(const Vector3 &p_rhs);
	/// Mutates the normal part, updating the tangent part to keep orthogonality.
	EncodedNormal &operator*=(const Vector3 &p_rhs);
	/// Mutates the normal part, updating the tangent part to keep orthogonality.
	EncodedNormal &operator/=(const Vector3 &p_rhs);
	/// Mutates the normal part, updating the tangent part to keep orthogonality.
	EncodedNormal &operator*=(real_t p_rhs);
	/// Mutates the normal part, updating the tangent part to keep orthogonality.
	EncodedNormal &operator/=(real_t p_rhs);

	/// Returns a copy of the encoded normal-and-tangent vector, with its normal part mutated and tangent part updated to keep orthogonality.
	EncodedNormal operator+(const Vector3 &p_rhs) const;
	/// Returns a copy of the encoded normal-and-tangent vector, with its normal part mutated and tangent part updated to keep orthogonality.
	EncodedNormal operator-(const Vector3 &p_rhs) const;
	/// Returns a copy of the encoded normal-and-tangent vector, with its normal part mutated and tangent part updated to keep orthogonality.
	EncodedNormal operator*(const Vector3 &p_rhs) const;
	/// Returns a copy of the encoded normal-and-tangent vector, with its normal part mutated and tangent part updated to keep orthogonality.
	EncodedNormal operator/(const Vector3 &p_rhs) const;
	/// Returns a copy of the encoded normal-and-tangent vector, with its normal part mutated and tangent part updated to keep orthogonality.
	EncodedNormal operator*(real_t p_rhs) const;
	/// Returns a copy of the encoded normal-and-tangent vector, with its normal part mutated and tangent part updated to keep orthogonality.
	EncodedNormal operator/(real_t p_rhs) const;

	/// Mutates the normal part, updating the tangent part to keep orthogonality. The tangent part of the right operand is ignored.
	_FORCE_INLINE_ EncodedNormal &operator+=(const EncodedNormal &p_rhs);
	/// Mutates the normal part, updating the tangent part to keep orthogonality. The tangent part of the right operand is ignored.
	_FORCE_INLINE_ EncodedNormal &operator-=(const EncodedNormal &p_rhs);
	/// Mutates the normal part, updating the tangent part to keep orthogonality. The tangent part of the right operand is ignored.
	_FORCE_INLINE_ EncodedNormal &operator*=(const EncodedNormal &p_rhs);
	/// Mutates the normal part, updating the tangent part to keep orthogonality. The tangent part of the right operand is ignored.
	_FORCE_INLINE_ EncodedNormal &operator/=(const EncodedNormal &p_rhs);

	/// Returns a copy of the encoded normal-and-tangent vector, with its normal part mutated and tangent part updated to keep orthogonality. The tangent part of the right operand is ignored.
	_FORCE_INLINE_ EncodedNormal operator+(const EncodedNormal &p_rhs) const;
	/// Returns a copy of the encoded normal-and-tangent vector, with its normal part mutated and tangent part updated to keep orthogonality. The tangent part of the right operand is ignored.
	_FORCE_INLINE_ EncodedNormal operator-(const EncodedNormal &p_rhs) const;
	/// Returns a copy of the encoded normal-and-tangent vector, with its normal part mutated and tangent part updated to keep orthogonality. The tangent part of the right operand is ignored.
	_FORCE_INLINE_ EncodedNormal operator*(const EncodedNormal &p_rhs) const;
	/// Returns a copy of the encoded normal-and-tangent vector, with its normal part mutated and tangent part updated to keep orthogonality. The tangent part of the right operand is ignored.
	_FORCE_INLINE_ EncodedNormal operator/(const EncodedNormal &p_rhs) const;

	/// Returns a component (X, Y or Z) of the normal part.
	_FORCE_INLINE_ real_t operator[](int p_axis) const {
		ERR_FAIL_INDEX_V(p_axis, AXIS_COUNT, 0.0f);
		return get_normal()[p_axis];
	}

	/// Returns a component (X, Y or Z) of the normal part.
	_FORCE_INLINE_ AxisWriteProxy operator[](int p_axis) {
		ERR_FAIL_INDEX_V(p_axis, AXIS_COUNT, AxisWriteProxy());
		return AxisWriteProxy(this, p_axis);
	}

	/// Decodes and returns the normal part.
	_FORCE_INLINE_ Vector3 get_normal() const;
	/// Decodes and returns the tangent part, storing the sign through a float pointer.
	_FORCE_INLINE_ Vector3 get_tangent(real_t *r_sign) const;
	/// Decodes and returns the tangent part, storing the sign in the W coordinate of a Vector4.
	_FORCE_INLINE_ Vector4 get_tangent() const;
	/// Sets the normal part, updating the tangent part to keep orthogonality.
	void set_normal(const Vector3 &p_norm);
	/// Sets the tangent part, updating the normal part to keep orthogonality.
	void set_tangent(const Vector3 &p_tan, real_t p_tan_dir = 1.0f);
	/// Sets the normal part without updating the tangent part.
	_FORCE_INLINE_ void set_normal_only(const Vector3 &p_norm);
	/// Sets the tangent part without updating the normal part.
	_FORCE_INLINE_ void set_tangent_only(const Vector3 &p_tan, real_t p_tan_dir = 1.0f);
};

Vector3 EncodedNormal::get_normal() const {
	return Vector3::octahedron_decode(Vector2(encoded_normal[0] / 65535.0f, encoded_normal[1] / 65535.0f));
}

Vector3 EncodedNormal::get_tangent(real_t *r_sign) const {
	return Vector3::octahedron_tangent_decode(
		Vector2(encoded_tangent[0] / 65535.0f, encoded_tangent[1] / 65535.0f), r_sign
	);
}

Vector4 EncodedNormal::get_tangent() const {
	Vector4 res;
	reinterpret_cast<Vector3&>(res) = Vector3::octahedron_tangent_decode(
		Vector2(encoded_tangent[0] / 65535.0f, encoded_tangent[1] / 65535.0f), &res.w
	);
	return res;
}

void EncodedNormal::set_normal_only(const Vector3 &p_norm) {
	ERR_FAIL_COND(!p_norm.is_normalized());
	const Vector2 enc_norm = p_norm.octahedron_encode();
	encoded_normal[0] = (uint16_t)CLAMP(enc_norm.x * 65535, 0, 65535);
	encoded_normal[1] = (uint16_t)CLAMP(enc_norm.y * 65535, 0, 65535);
}

void EncodedNormal::set_tangent_only(const Vector3 &p_tan, real_t p_tan_dir) {
	ERR_FAIL_COND(!p_tan.is_normalized());
	const Vector2 enc_tan = p_tan.octahedron_tangent_encode(p_tan_dir);
	encoded_tangent[0] = (uint16_t)CLAMP(enc_tan.x * 65535, 0, 65535);
	encoded_tangent[1] = (uint16_t)CLAMP(enc_tan.y * 65535, 0, 65535);

	if (unlikely(encoded_tangent[0] == 0 && encoded_tangent[1] == 65535)) {
		// (1, 1) and (0, 1) decode to the same value, but (0, 1) messes with our compression detection.
		// So we sanitize here.
		encoded_tangent[0] = 65535;
	}
}

EncodedNormal EncodedNormal::operator-() const {
	const Vector3 norm = get_normal();
	real_t tan_d;
	const Vector3 tan = get_tangent(&tan_d);
	return EncodedNormal(-norm, tan, -tan_d);
}

OMG_ENCODED_NORMAL_CAST_ASN_OP_IMPL(+=)
OMG_ENCODED_NORMAL_CAST_ASN_OP_IMPL(-=)
OMG_ENCODED_NORMAL_CAST_ASN_OP_IMPL(*=)
OMG_ENCODED_NORMAL_CAST_ASN_OP_IMPL(/=)

OMG_ENCODED_NORMAL_CAST_MATH_OP_IMPL(+)
OMG_ENCODED_NORMAL_CAST_MATH_OP_IMPL(-)
OMG_ENCODED_NORMAL_CAST_MATH_OP_IMPL(*)
OMG_ENCODED_NORMAL_CAST_MATH_OP_IMPL(/)

// OpenMesh type traits
namespace OpenMesh {
template <>
struct vector_traits<::EncodedNormal> {
	typedef ::EncodedNormal vector_type;
	typedef real_t value_type;
	typedef GenProg::Int2Type<vector_type::AXIS_COUNT> typed_size;
	static const size_t size_ = vector_type::AXIS_COUNT;

	static size_t size() { return size_; }
};
}

#undef OMG_AXIS_WRITE_PROXY_ASN_OP_IMPL
#undef OMG_AXIS_WRITE_PROXY_CMP_OP_IMPL
#undef OMG_ENCODED_NORMAL_CAST_ASN_OP_IMPL
#undef OMG_ENCODED_NORMAL_CAST_MATH_OP_IMPL

#endif // OPENMESH_GODOT_KERNEL_ENCODED_NORMAL_H
