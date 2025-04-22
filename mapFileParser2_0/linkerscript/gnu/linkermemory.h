#ifndef LINKERMEMORY_H
#define LINKERMEMORY_H

#include "linkerraw.h"
#include <QRegularExpression>
#include <QString>
#include <QHash>

class LinkerMemory
{
public: /* types */
    // Structure for storing data about a single memory region
    struct MemoryRegion {
        QString name;
        QString attributes; // Attributes (e.g., xrw)
        QString origin;     // Starting address (ORIGIN)
        QString length;     // Length in bytes (LENGTH)
    };

    using Data = QHash<QString, MemoryRegion>; //{name, {name, attributes, origin, length}}
public: /* functions */
    LinkerMemory() = default;
    const Data& read(const LinkerDescriptor&, const LinkerRaw&);
    inline const Data& data() const { return memoryRegions; }
    inline void clear() { memoryRegions.clear(); }
private: /* functions */
    quint64 parseSize(const QString& sizeStr);
private: /* values */
    Data memoryRegions{};
};

#endif // LINKERMEMORY_H
