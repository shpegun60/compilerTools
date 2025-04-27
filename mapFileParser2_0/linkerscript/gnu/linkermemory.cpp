#include "linkermemory.h"

const LinkerMemory::Data& LinkerMemory::read(const LinkerDescriptor& descr, const LinkerRaw& raw)
{
    auto it = raw.data().find(descr.memoryBlockName);
    if(it == raw.data().end()) {
        return memoryRegions;
    }

    // Regular expression with support math operations
    QRegularExpressionMatchIterator matchIter =
        descr.memoryBlockRegex.globalMatch(it.value());
    while (matchIter.hasNext()) {
        const auto match = matchIter.next();

        memoryRegions.emplace_back(MemoryRegion {
            match.captured(1).trimmed(), // name
            match.captured(2).trimmed(), // attributes
            match.captured(3).trimmed(), // origin
            match.captured(4).trimmed(),  // length
            false
        });
    }
    return memoryRegions;
}
