// ReSharper disable CppRedundantQualifier
#ifndef OPENMESH_GODOT_KERNEL_ENCODED_NORMAL_H
#define OPENMESH_GODOT_KERNEL_ENCODED_NORMAL_H

// Godot
#include "core/math/vector2.h"
#include "core/math/vector3.h"
#include "core/math/vector4.h"

/// Octahedron-encoded, normalized, normal-and-tangent vector type. Each component is 16-bit fixed precision, sizeof: 8 Bytes.
struct [[nodiscard]] EncodedNormal {
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

	_FORCE_INLINE_ explicit EncodedNormal(const Vector3 &p_norm) :
		EncodedNormal(p_norm, Vector3(p_norm.z, -p_norm.x, p_norm.y).cross(p_norm.normalized()).normalized()) {}

	_FORCE_INLINE_ EncodedNormal(const Vector3 &p_norm, const Vector4 &p_tan) :
		EncodedNormal(p_norm, reinterpret_cast<const Vector3&>(p_tan), p_tan.w) {}

	_FORCE_INLINE_ EncodedNormal(real_t p_n_x, real_t p_n_y, real_t p_n_z) :
		EncodedNormal(Vector3(p_n_x, p_n_y, p_n_z)) {}

	_FORCE_INLINE_ EncodedNormal(real_t p_n_x, real_t p_n_y, real_t p_n_z, real_t p_t_x, real_t p_t_y, real_t p_t_z, real_t p_t_d = 1.0f) :
		EncodedNormal(Vector3(p_n_x, p_n_y, p_n_z), Vector3(p_t_x, p_t_y, p_t_z), p_t_d) {}

	/// Decodes and returns the normal part, discarding the tangent part.
	_FORCE_INLINE_ explicit operator Vector3() const { return get_normal(); }

	_FORCE_INLINE_ EncodedNormal operator-() const;

	_FORCE_INLINE_ bool operator!=(const EncodedNormal &p_rhs) const { return hash != p_rhs.hash; }
	_FORCE_INLINE_ bool operator==(const EncodedNormal &p_rhs) const { return !operator!=(p_rhs); }

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

EncodedNormal EncodedNormal::operator-() const {
	const Vector3 norm = get_normal();
	real_t tan_d;
	const Vector3 tan = get_tangent(&tan_d);
	return EncodedNormal(-norm, tan, -tan_d);
}

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

#endif // OPENMESH_GODOT_KERNEL_ENCODED_NORMAL_H
