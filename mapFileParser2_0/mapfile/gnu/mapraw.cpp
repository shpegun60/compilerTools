#include "mapraw.h"

namespace compiler_tools::gnu {

bool MapRaw::read(const MapDescriptor& descr, const QString& mapData)
{
    const QStringList mapStart = mapData.split(descr.memMapHeader, Qt::SkipEmptyParts);

    // get sections info
    if(!mapStart.empty()) {
        readBody(descr, mapStart.last());
    }

    return !_sections.empty();
}

void MapRaw::readBody(const MapDescriptor& descr, const QString& map)
{
    QStringList lines = map.split('\n', Qt::SkipEmptyParts);

    int unknownCnt = 0;
    Section seg {};
    seg.lines.reserve(4);

    // ------------------------------------------------------
    for (qsizetype i = 0; i < lines.size(); ++i) {
        QString line = lines[i].trimmed();

        if (line.isEmpty()) {
            continue;
        } else if(descr.isEnd(line)) {
            break;
        } else if(descr.isIgnore(line)
                   || descr.isAssignmentLine(line) || descr.isProvideLine(line) || descr.isAlignLine(line)) {
            _ignored.append(std::move(line));
            continue;
        }


        // split all lines
        {
            const QRegularExpressionMatch segmentMatch = descr.sectionRegex.match(line);

            if (segmentMatch.hasMatch()) {
                if(!seg.lines.empty()) {
                    if(seg.name.isEmpty()) {
                        seg.name = QString(descr.unknown + "%1").arg(unknownCnt);
                        ++unknownCnt;
                    }
                    _sections.emplace_back(std::move(seg));
                }
                seg.name = segmentMatch.captured(1).trimmed();
                seg.lines.clear();
                seg.lines.reserve(4);
            } else {
                seg.lines.emplace_back(std::move(line));
            }
        }
    }

    // append last element if exists
    if(!seg.lines.empty()) {
        if(seg.name.isEmpty()) {
            seg.name = QString(descr.unknown + "%1").arg(unknownCnt);
        }
        _sections.emplace_back(std::move(seg));
    }
}

}
