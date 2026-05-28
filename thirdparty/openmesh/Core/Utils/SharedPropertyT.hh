#ifndef OPENMESH_SHAREDPROPERTYT
#define OPENMESH_SHAREDPROPERTYT

#include "Core/IO/SR_store.hh"
#include "Core/Utils/PropertyBufferT.hh"

namespace OpenMesh {
enum Domain { VertexDomain, HalfedgeDomain, EdgeDomain, FaceDomain, MeshDomain };

template <typename T, Domain D>
class SharedPropertyT : public BaseProperty {
public:
	using Value = T;
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;

	explicit SharedPropertyT(
		const std::string &_name = "<unknown>",
		const std::string &_internal_type_name = "<unknown>") :
		BaseProperty(_name, _internal_type_name) {}

	// TODO: Copy ctor

	void reserve(size_t _n) override {} // TODO
	void resize(size_t _n) override {} // TODO
	void clear() override {} // TODO
	void push_back() override {} // TODO
	void swap(size_t _i0, size_t _i1) override {} // TODO
	void copy(size_t _io, size_t _i1) override {} // TODO
	BaseProperty* clone() const override { return nullptr; } // TODO
	size_t n_elements() const override { return 0; } // TODO
	size_t element_size() const override { return IO::size_of<T>(); }

	reference operator[](int _idx) { CRASH_NOW_MSG("Not implemented"); } // TODO
	const_reference operator[](int _idx) const { CRASH_NOW_MSG("Not implemented"); } // TODO

	//region Mesh IO (disabled)

	void set_persistent(bool _yn) override { check_and_set_persistent<T>(_yn); }

	size_t store(std::ostream &_ostr, bool _swap) const override { ERR_FAIL_V_MSG(0, "Mesh IO is disabled"); }

	size_t restore(std::istream &_istr, bool _swap) override { ERR_FAIL_V_MSG(0, "Mesh IO is disabled"); }

	std::string get_storage_name() const override { return IO::binary<T>::type_identifier(); }

	//endregion
};
}

#endif // OPENMESH_SHAREDPROPERTYT
