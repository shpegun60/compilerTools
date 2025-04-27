#ifndef LINKERMEMORY_H
#define LINKERMEMORY_H

#include "linkerraw.h"
#include <QList>

class LinkerMemory
{
public: /* types */
    // Structure for storing data about a single memory region
    struct MemoryRegion {
        QString name;
        QString attributes; // Attributes (e.g., xrw)
        QString origin;     // Starting address (ORIGIN)
        QString length;     // Length in bytes (LENGTH)
        bool evaluated;
    };

    using Data = QList<MemoryRegion>; //{name, attributes, origin, length}
public: /* functions */
    LinkerMemory() = default;
    const Data& read(const LinkerDescriptor&, const LinkerRaw&);
    inline const Data& data() const { return memoryRegions; }
    inline void clear() { memoryRegions.clear(); }
private: /* values */
    Data memoryRegions{};
};

#endif // LINKERMEMORY_H
