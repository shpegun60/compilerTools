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
        memoryRegions.insert(name,
                             {name,
                                 match.captured(2).trimmed(),
                                 match.captured(3).trimmed(),
                                 match.captured(4).trimmed()
                             });
    }
    return memoryRegions;
}

quint64 LinkerMemory::parseSize(const QString &sizeStr)
{
    QHash<QChar, quint64> units = {
        {'K', 1024ULL},
        {'M', 1024ULL * 1024},
        {'G', 1024ULL * 1024 * 1024},
        {'T', 1024ULL * 1024 * 1024 * 1024}
    };

    QChar unit = '\0';
    QString numberStr {};

    // Define the unit of measurement
    for (const QChar& c : sizeStr) {
        if (c.isLetter()) {
            unit = c;
            break;
        }
        numberStr += c;
    }

    quint64 multiplier = units.value(unit, 1);
    return numberStr.toULongLong() * multiplier;
}
