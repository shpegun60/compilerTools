#ifndef LINKERSUBSECTIONREADER_H
#define LINKERSUBSECTIONREADER_H

#include "linkerdescr.h"
#include "ilinkerfile.h"

namespace compiler_tools::gnu {

class LinkerSubSectionReader
{
public: /* types */
    using Data = ILinkerFile::SubSections;
public: /* functions */
    LinkerSubSectionReader() = delete;
    static bool read(const LinkerDescriptor&, Data&, const QString&);
    static void append(const LinkerDescriptor&, Data&, const QString&);
};

} /* namespace compiler_tools::gnu */

#endif // LINKERSUBSECTIONREADER_H
