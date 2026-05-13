#include "encoded_normal.h"

#include "core/math/math_funcs.h"

void EncodedNormal::set_normal(const Vector3 &p_norm) {
	ERR_FAIL_COND(!p_norm.is_normalized());
	const Vector3 old_norm = get_normal();
	if (p_norm.is_equal_approx(old_norm)) { return; }

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
	if (p_tan.is_equal_approx(old_tan)) {
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

// FIXME: test code
void static_test() {
	EncodedNormal n { 1.0, 0.0, 0.0 };
	n = Vector3(0, 1, 0) + n;
	n[0] = -n[0];
	if (n[0] > n[1]) {}
	EncodedNormal n2 = (n + n + n) / 3;
	n = n2 * Vector3(-1, -1, -1);

	(void) n;
	(void) n2;
}
