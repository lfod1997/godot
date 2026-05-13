#include "encoded_normal.h"

#include "core/math/math_funcs.h"

// Instead of forwarding Vector3 operators:
//	EncodedNormal& EncodedNormal::operator+=(const Vector3 &p_rhs) {
//		Vector3 v = get_normal(); // Decodes
//		v += p_rhs;
//		set_normal(v); // Decodes again inside
//		return *this;
//	}
// We have to implement these dedicated overloads to avoid decoding the normal twice.

#define ENCODED_NORMAL_ASN_OP_IMPL(op, rhs_type, cond_ident, cond_err) \
	EncodedNormal& EncodedNormal::operator op (rhs_type p_rhs) { \
		ERR_FAIL_COND_V( cond_err, *this ); \
		if ( cond_ident ) { return *this; } \
		const Vector3 old_norm = get_normal(); \
		real_t tan_d; \
		const Vector3 old_tan = get_tangent(&tan_d); \
		Vector3 new_norm(old_norm); \
		new_norm op p_rhs; \
		set_normal_only(new_norm); \
		const real_t divisor = 1.0f + new_norm.dot(old_norm); \
		if (Math::is_zero_approx(divisor)) { \
			set_tangent_only(old_tan, -tan_d); \
		} \
		else { \
			set_tangent_only(old_tan - old_tan.dot(new_norm) / divisor * (new_norm + old_norm), tan_d); \
		} \
		return *this; \
	}

#define ENCODED_NORMAL_MATH_OP_IMPL(op, rhs_type, cond_ident, cond_err) \
	EncodedNormal EncodedNormal::operator op (rhs_type p_rhs) const { \
		ERR_FAIL_COND_V( cond_err, EncodedNormal() ); \
		EncodedNormal res = *this; \
		if ( cond_ident ) { return res; } \
		const Vector3 old_norm = res.get_normal(); \
		real_t tan_d; \
		const Vector3 old_tan = res.get_tangent(&tan_d); \
		const Vector3 new_norm = old_norm op p_rhs; \
		res.set_normal_only(new_norm); \
		const real_t divisor = 1.0f + new_norm.dot(old_norm); \
		if (Math::is_zero_approx(divisor)) { \
			res.set_tangent_only(old_tan, -tan_d); \
		} \
		else { \
			res.set_tangent_only(old_tan - old_tan.dot(new_norm) / divisor * (new_norm + old_norm), tan_d); \
		} \
		return res; \
	}

void EncodedNormal::set_normal(const Vector3 &p_norm) {
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

ENCODED_NORMAL_ASN_OP_IMPL(+=, const Vector3 &, p_rhs.is_zero_approx(), false)
ENCODED_NORMAL_ASN_OP_IMPL(-=, const Vector3 &, p_rhs.is_zero_approx(), false)
ENCODED_NORMAL_ASN_OP_IMPL(*=, const Vector3 &, p_rhs.is_equal_approx(Vector3(1.0, 1.0, 1.0)), p_rhs.is_zero_approx())
ENCODED_NORMAL_ASN_OP_IMPL(/=, const Vector3 &, p_rhs.is_equal_approx(Vector3(1.0, 1.0, 1.0)), p_rhs.is_zero_approx())

ENCODED_NORMAL_ASN_OP_IMPL(*=, real_t, Math::is_equal_approx(p_rhs, 1.0f), Math::is_zero_approx(p_rhs))
ENCODED_NORMAL_ASN_OP_IMPL(/=, real_t, Math::is_equal_approx(p_rhs, 1.0f), Math::is_zero_approx(p_rhs))

ENCODED_NORMAL_MATH_OP_IMPL(+, const Vector3 &, p_rhs.is_zero_approx(), false)
ENCODED_NORMAL_MATH_OP_IMPL(-, const Vector3 &, p_rhs.is_zero_approx(), false)
ENCODED_NORMAL_MATH_OP_IMPL(*, const Vector3 &, p_rhs.is_zero_approx(), false)
ENCODED_NORMAL_MATH_OP_IMPL(/, const Vector3 &, p_rhs.is_zero_approx(), false)

ENCODED_NORMAL_MATH_OP_IMPL(*, real_t, Math::is_equal_approx(p_rhs, 1.0f), Math::is_zero_approx(p_rhs))
ENCODED_NORMAL_MATH_OP_IMPL(/, real_t, Math::is_equal_approx(p_rhs, 1.0f), Math::is_zero_approx(p_rhs))

// FIXME: test code
void static_test() {
	EncodedNormal n{ 1.0, 0.0, 0.0 };
	n = Vector3(0, 1, 0);
	n[0] *= -1;
	n[1] = n[0] + n[2];
	EncodedNormal n2 = n;
	n = n2 * Vector3(-1, -1, -1);
	n += n2 / Vector3(n2);
	n *= Vector3(-1, 0, -1);
	real_t a, b, c;
	a = b = c = n2[0];
	if (n[0] > n[1]) {}
}
