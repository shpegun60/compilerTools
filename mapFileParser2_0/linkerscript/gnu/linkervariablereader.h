#ifndef LINKERVARIABLEREADER_H
#define LINKERVARIABLEREADER_H

#include "linkerdescr.h"
#include "ilinkerfile.h"

namespace compiler_tools::gnu {

class LinkerVariableReader
{
public: /* types */
    using Data = ILinkerFile::Variables;
public: /* functions */
    LinkerVariableReader() = delete;
    static bool read(const LinkerDescriptor&, Data&, const QString&);
};

} /* namespace compiler_tools::gnu */

#endif // LINKERVARIABLEREADER_H
