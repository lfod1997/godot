#ifndef OPENMESH_PROPERTYBUFFERT
#define OPENMESH_PROPERTYBUFFERT

#include "core/templates/local_vector.h"

namespace OpenMesh {
enum BufferType { RAM, VRAMVertexRegion, VRAMAttributeRegion, VRAMSkinRegion };

template <typename T, BufferType BType, int BOffset = 0,
	// Requires:
	typename = void,
	typename = std::enable_if_t<
		std::is_trivially_destructible_v<T> &&
		std::is_trivially_copyable_v<T>
	>>
class PropertyBufferT {};

template <typename T>
class PropertyBufferT<T, RAM> {
	LocalVector<T, size_t, true> _buffer;

public:
	_FORCE_INLINE_ void reserve(size_t p_size) { _buffer.reserve(p_size); }
	_FORCE_INLINE_ void resize(size_t p_size) { _buffer.resize(p_size); }
	_FORCE_INLINE_ void clear() { _buffer.reset(); }
	_FORCE_INLINE_ void push_back() { resize(_buffer.size() + 1); }

	_FORCE_INLINE_ void swap(size_t p_i0, size_t p_i1) {
		ERR_FAIL_UNSIGNED_INDEX(p_i0, _buffer.size());
		ERR_FAIL_UNSIGNED_INDEX(p_i1, _buffer.size());
		std::swap(_buffer[p_i0], _buffer[p_i1]);
	}
	_FORCE_INLINE_ void copy(size_t p_from, size_t p_to) {
		ERR_FAIL_UNSIGNED_INDEX(p_from, _buffer.size());
		ERR_FAIL_UNSIGNED_INDEX(p_to, _buffer.size());
		_buffer[p_to] = _buffer[p_from];
	}

	_FORCE_INLINE_ size_t is_empty() const { return _buffer.is_empty(); }
	_FORCE_INLINE_ size_t size() const { return _buffer.size(); }
	_FORCE_INLINE_ size_t capacity() const { return _buffer.get_capacity(); }

	_FORCE_INLINE_ T get(size_t p_idx) const {
		ERR_FAIL_UNSIGNED_INDEX_V(p_idx, _buffer.size(), T());
		return _buffer[p_idx];
	}

	_FORCE_INLINE_ void set(size_t p_idx, const T& p_v) {
		ERR_FAIL_UNSIGNED_INDEX(p_idx, _buffer.size());
		_buffer[p_idx] = p_v;
	}
};

template <typename T, BufferType BType, int BOffset>
class PropertyBufferT<T, BType, BOffset, std::enable_if_t<BType != RAM>> {
	LocalVector<T, size_t, true> _buffer;

public:
	// FIXME: Any op that may change the size MUST sync capacity with RS!!
	_FORCE_INLINE_ void reserve(size_t p_size) { _buffer.reserve(p_size); }
	_FORCE_INLINE_ void resize(size_t p_size) { _buffer.resize(p_size); }
	_FORCE_INLINE_ void clear() { _buffer.reset(); }
	_FORCE_INLINE_ void push_back() { resize(_buffer.size() + 1); }

	// FIXME: Any op that may mutate the data MUST sync data change with RS!!
	_FORCE_INLINE_ void swap(size_t p_i0, size_t p_i1) {
		ERR_FAIL_UNSIGNED_INDEX(p_i0, _buffer.size());
		ERR_FAIL_UNSIGNED_INDEX(p_i1, _buffer.size());
		std::swap(_buffer[p_i0], _buffer[p_i1]);
	}
	_FORCE_INLINE_ void copy(size_t p_from, size_t p_to) {
		ERR_FAIL_UNSIGNED_INDEX(p_from, _buffer.size());
		ERR_FAIL_UNSIGNED_INDEX(p_to, _buffer.size());
		_buffer[p_to] = _buffer[p_from];
	}

	_FORCE_INLINE_ size_t is_empty() const { return _buffer.is_empty(); }
	_FORCE_INLINE_ size_t size() const { return _buffer.size(); }
	_FORCE_INLINE_ size_t capacity() const { return _buffer.get_capacity(); }

	_FORCE_INLINE_ T get(size_t p_idx) const {
		ERR_FAIL_UNSIGNED_INDEX_V(p_idx, _buffer.size(), T());
		return _buffer[p_idx];
	}

	// FIXME: Any op that may mutate the data MUST sync data change with RS!!
	_FORCE_INLINE_ void set(size_t p_idx, const T& p_v) {
		ERR_FAIL_UNSIGNED_INDEX(p_idx, _buffer.size());
		_buffer[p_idx] = p_v;
	}
};
}

#endif // OPENMESH_PROPERTYBUFFERT
