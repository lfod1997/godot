//=============================================================================
//
//  Implements an IOManager writer module for VTK files
//
//=============================================================================

#ifndef __VTKWRITER_HH__
#define __VTKWRITER_HH__

//=== INCLUDES ================================================================

#include <string>
#include <iosfwd>

#include "Core/System/config.h"
#include "Core/Utils/SingletonT.hh"
#include "Core/IO/exporter/BaseExporter.hh"
#include "Core/IO/writer/BaseWriter.hh"

//== NAMESPACES ===============================================================

namespace OpenMesh {
namespace IO {

//=== IMPLEMENTATION ==========================================================

class _VTKWriter_ : public BaseWriter
{
public:
    _VTKWriter_();

    std::string get_description() const override { return "VTK"; }
    std::string get_extensions()  const override { return "vtk"; }

    bool write(const std::string&, BaseExporter&, const Options& _writeOptions, std::streamsize _precision = 6) const override;
    bool write(std::ostream&, BaseExporter&, const Options& _writeOptions, std::streamsize _precision = 6) const override;

    size_t binary_size(BaseExporter&, const Options&) const override { return 0; }
};

//== TYPE DEFINITION ==========================================================

/// Declare the single entity of the OBJ writer
extern _VTKWriter_  __VTKWriterinstance;
_VTKWriter_& VTKWriter();

//=============================================================================
} // namespace IO
} // namespace OpenMesh
//=============================================================================
#endif
//=============================================================================
