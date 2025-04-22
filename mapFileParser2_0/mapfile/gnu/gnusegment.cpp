#include "gnusegment.h"

GnuDescriptor::SegmentSizes GnuSegmentReader::read(const GnuDescriptor& descr, const QString &mapData)
{
    const QStringList mapStart = mapData.split(descr.memMapHeader, Qt::SkipEmptyParts);

    // get sections info
    if(!mapStart.empty()) {
        const QStringList lines = mapStart.last().split('\n', Qt::SkipEmptyParts);
        return readBody(descr, lines);
    }

    return {};
}

GnuDescriptor::SegmentSizes GnuSegmentReader::readBody(const GnuDescriptor &descr, const QStringList &lines)
{
    int unknownCnt = 0;
    GnuSegment seg {};
    GnuDescriptor::SegmentSizes sz {};

    // clear all before read --------------------------------
    clear();
    // ------------------------------------------------------
    const int lineSize = lines.size();
    for (int i = 0; i < lineSize; ++i) {
        QString line = lines[i].trimmed();

        if (line.isEmpty()) {
            continue;
        } else if(descr.isEnd(line)) {
            break;
        } else if(descr.isIgnore(line)) {
           _ignored.append(std::move(line));
            continue;
        }

        descr.isSegmentSize(line, sz);

        // split all lines
        {
            const QRegularExpressionMatch segmentMatch = descr.segmentRegex.match(line);

            if (segmentMatch.hasMatch()) {
                if(!seg.lines().empty()) {
                    if(seg.name().isEmpty()) {
                        seg.setname(QString(descr.unknownName + "%1").arg(unknownCnt));
                        ++unknownCnt;
                    }
                    _readed.append(std::move(seg));
                }

                seg.clear();
                seg.setname(segmentMatch.captured(1));
            } else {
                seg.append(std::move(line));
            }
        }
    }

    // append last element if exists
    if(!seg.lines().empty()) {
        if(seg.name().isEmpty()) {
            seg.setname(QString(descr.unknownName + "%1").arg(unknownCnt));
        }
        _readed.append(std::move(seg));
    }

    return sz;
}

void GnuSegmentReader::clear()
{
    _readed.clear();
    _ignored.clear();
}
