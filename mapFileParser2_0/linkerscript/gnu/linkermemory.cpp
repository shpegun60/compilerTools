#include "linkermemory.h"

const LinkerMemory::Data& LinkerMemory::read(const LinkerDescriptor& descr, const LinkerRaw& raw)
{
    if (!raw.data().contains(descr.memoryBlockName)) {
        return memoryRegions;
    }

    // Regular expression with support math operations
    QRegularExpressionMatchIterator matchIter =
        descr.memoryBlockRegex.globalMatch(raw.data().value(descr.memoryBlockName));
    while (matchIter.hasNext()) {
        QRegularExpressionMatch match = matchIter.next();
        QString name = match.captured(1).trimmed();
        MemoryRegion region{name,
            match.captured(2).trimmed(),
            match.captured(3).trimmed(),
            match.captured(4).trimmed()
        };
        memoryRegions.emplace_back(std::move(region));
    }
    return memoryRegions;
}
