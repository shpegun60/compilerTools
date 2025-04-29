#ifndef LINKERMEMORY_H
#define LINKERMEMORY_H

#include "linkerdescr.h"
#include "linkerraw.h"
#include "ilinkerfile.h"

namespace compiler_tools::gnu {

class LinkerMemory
{
public: /* types */
    using Data = QList<ILinkerFile::MemoryRegion>; //{name, attributes, origin, length}
public: /* functions */
    LinkerMemory() = default;
    const Data& read(const LinkerDescriptor&, const LinkerRaw&);
    inline const Data& data() const { return memoryRegions; }
    inline void clear() { memoryRegions.clear(); }
private: /* values */
    Data memoryRegions{};
};

} /* namespace compiler_tools::gnu */

#endif // LINKERMEMORY_H
