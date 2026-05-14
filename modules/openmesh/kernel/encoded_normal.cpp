#include "encoded_normal.h"

#include "core/math/math_funcs.h"
#include <limits>

static _ALWAYS_INLINE_ bool is_equal_approx_within_encoding_error(const real_t p_lhs, const real_t p_rhs) {
	static constexpr real_t ENCODE_ERROR = 2 / 65535.0;
	static constexpr real_t ENCODE_EPSILON = ENCODE_ERROR / (4 + 3 * ENCODE_ERROR) - std::numeric_limits<real_t>::epsilon();
	return p_lhs == p_rhs ? true : abs(p_lhs - p_rhs) <= ENCODE_EPSILON;
}

static _FORCE_INLINE_ bool is_equal_approx_within_encoding_error(const Vector3 &p_lhs, const Vector3 &p_rhs) {
	return is_equal_approx_within_encoding_error(p_lhs.x, p_rhs.x) && is_equal_approx_within_encoding_error(p_lhs.y, p_rhs.y) && is_equal_approx_within_encoding_error(p_lhs.z, p_rhs.z);
}

void EncodedNormal::set_normal(const Vector3 &p_norm) {
	ERR_FAIL_COND(!p_norm.is_normalized());
	const Vector3 old_norm = get_normal();
	if (is_equal_approx_within_encoding_error(p_norm, old_norm)) { return; }

	real_t tan_d;
	const Vector3 old_tan = get_tangent(&tan_d);

	set_normal_only(p_norm);
	const real_t divisor = 1.0f + p_norm.dot(old_norm);
	if (Math::is_zero_approx(divisor)) {
		// Only flipping side, flip tangent also.
		set_tangent_only(old_tan, -tan_d);
	}
	else {
		set_tangent_only(old_tan - old_tan.dot(p_norm) / divisor * (p_norm + old_norm), tan_d);
	}
}

void EncodedNormal::set_tangent(const Vector3 &p_tan, real_t p_tan_dir) {
	ERR_FAIL_COND(!p_tan.is_normalized());
	real_t tan_d;
	const Vector3 old_tan = get_tangent(&tan_d);
	const bool same_sign = signbit(tan_d) == signbit(p_tan_dir);
	if (is_equal_approx_within_encoding_error(p_tan, old_tan)) {
		if (!same_sign) {
			// Only flipping side, flip normal also.
			set_normal_only(-get_normal());
			set_tangent_only(old_tan, -tan_d);
		}
		return;
	}

	const Vector3 old_norm = get_normal();
	const real_t divisor = 1.0f + p_tan.dot(old_tan);
	if (Math::is_zero_approx(divisor)) {
		if (same_sign) {
			// Only flipping side, flip normal also.
			set_normal_only(-old_norm);
			set_tangent_only(old_tan, -tan_d);
		}
	}
	else {
		set_normal_only(old_norm - old_norm.dot(p_tan) / divisor * (p_tan + old_tan));
		set_tangent_only(p_tan, p_tan_dir);
	}
}
