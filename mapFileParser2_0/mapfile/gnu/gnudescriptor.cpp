#include "gnudescriptor.h"

bool GnuDescriptor::isEnd(const QString &line) const
{
    for (const QString &name : memMapEndMarkers) {
        if (line.startsWith(name)) {
            return true;
        }
    }
    return false;
}

bool GnuDescriptor::isIgnore(const QString &line) const
{
    for (const QString &name : memMapIgnoreMarkers) {
        if (line.startsWith(name)) {
            return true;
        }
    }
    return false;
}

bool GnuDescriptor::isFill(const QString &line) const
{
    if (line.contains(fillIdentifier)) {
        return true;
    }
    return false;
}

/*
 * The function reads the first address found in the string as 0x... and returns it as a string.
 * If the address is not found, returns "-1".
 */
std::pair<QString, quint64> GnuDescriptor::readLineAddress(const QString &s) const
{
    const QRegularExpressionMatch match = addressRegex.match(s);
    if (match.hasMatch()) {
        bool ok = false;
        const quint64 addr = match.captured(1).toULongLong(&ok, 16);

        if(ok) {
            return {QString("0x%1").arg(addr, 0, 16), addr};
        } else {
            return {"-1", 0};
        }
    }
    return {"-1", 0};
}



std::pair<GnuDescriptor::Segment, quint64> GnuDescriptor::readSegmentSize(const QString &line) const
{
    // Split the string by whitespace characters (remove empty elements)
    const QStringList tokens = line.split(tokenSplitter, Qt::SkipEmptyParts);

    if (tokens.size() > 2) {
        const QString firstToken = tokens[0].trimmed();
        bool ok = false;
        const quint64 size = tokens[2].trimmed().toULongLong(&ok, 16);

        if(ok) {
            for (const auto &name : text) {
                if(firstToken.contains(name) && firstToken.length() == name.length()) {
                    return {Segment::Text, size};
                }
            }

            for (const auto &name : data) {
                if(firstToken.contains(name) && firstToken.length() == name.length()) {
                    return {Segment::Data, size};
                }
            }

            for (const auto &name : bss) {
                if(firstToken.contains(name) && firstToken.length() == name.length()) {
                    return {Segment::Bss, size};
                }
            }
        }
    }
    return {Segment::Unknown, 0};
}

bool GnuDescriptor::isSegmentSize(const QString &line, SegmentSizes &sz) const
{
    const auto segmentInfo = readSegmentSize(line);

    switch(segmentInfo.first) {
    case GnuDescriptor::Text:   sz.text   += segmentInfo.second; return true;
    case GnuDescriptor::Data:   sz.data   += segmentInfo.second; return true;
    case GnuDescriptor::Bss:    sz.bss    += segmentInfo.second; return true;
    case GnuDescriptor::Unknown: return false;
    }

    return false;
}

