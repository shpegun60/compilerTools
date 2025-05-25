#include "mapdescriptor.h"

#include <QFileInfo>

namespace compiler_tools::gnu {

bool MapDescriptor::isEnd(const QString& line) const
{
    for (const QString &name : memMapEndMarkers) {
        if (line.startsWith(name)) {
            return true;
        }
    }
    return false;
}

bool MapDescriptor::isIgnore(const QString& line) const
{
    for (const QString &name : memMapIgnoreMarkers) {
        if (line.contains(name)) {
            return true;
        }
    }
    return false;
}

bool MapDescriptor::isFill(const QString& line) const
{
    if (line.contains(fillIdentifier)) {
        return true;
    }
    return false;
}

std::pair<bool, quint64> MapDescriptor::readFillSize(const QString& line) const
{
    if (line.contains(fillIdentifier)) {
        return {true, readLineSize(line)};
    }
    return {false, 0};
}

bool MapDescriptor::isAssignmentLine(const QString &line) const
{
    static const QRegularExpression re(R"(\S+\s*=\s*\S+)");

    if (line.contains("operator=")) {
        return false;
    }
    return re.match(line).hasMatch();
}

bool MapDescriptor::isProvideLine(const QString &line) const
{
    static const QRegularExpression re(R"(PROVIDE\s*\(.*?\))");
    return re.match(line).hasMatch();
}

bool MapDescriptor::isAlignLine(const QString &line) const
{
    static const QRegularExpression re(R"(ALIGN\s*\(.*?\))");
    return re.match(line).hasMatch();
}



std::pair<bool, quint64> MapDescriptor::readLoadAddr(const QString &line) const
{
    static const QRegularExpression loadAddrRe(
        R"(load address\s+(0x[0-9A-Fa-f]+))",
        QRegularExpression::CaseInsensitiveOption
        );

    QRegularExpressionMatch m = loadAddrRe.match(line);
    if (m.hasMatch()) {
        bool ok = false;
        quint64 addr = m.captured(1).toULongLong(&ok, 16);
        return { ok, addr };
    }

    return { false, 0 };
}

std::pair<QString, quint64> MapDescriptor::readLineAddress(const QString& line) const
{
    const QRegularExpressionMatch match = hexRegex.match(line);
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


quint64 MapDescriptor::readLineSize(const QString& line) const
{
    static QRegularExpression re("0[xX][0-9a-fA-F]+");
    QRegularExpressionMatchIterator it = re.globalMatch(line);
    int count = 0;
    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();
        ++count;
        if (count == 2) {
            bool ok = false;
            const quint64 val = match.captured(0).toULongLong(&ok, 16);
            if(ok) {
                return val;
            }
        }
    }
    return 0;
}

// The function reads the name (the part of the string after the address) and returns it, trimming off any extra spaces.
QString MapDescriptor::readLineName(const QString &s) const
{
    static QRegularExpression re(R"(0[xX][0-9a-fA-F]+\s+(.*))");
    QRegularExpressionMatch match = re.match(s);
    if (match.hasMatch()) {
        return match.captured(1).trimmed();
    }
    return QString();
}


//! Reads the path to .o or .a(…) from a line
//! @param line input line (e.g., a line from a linker map)
//! @return path with extension .o or .a and optional (member.o), or "" if not found
QString MapDescriptor::readLinePath(const QString &line) const
{
    // Pattern catches “no spaces+.o” or “.a” with optional “(member.o)”
    static const QRegularExpression re(
        R"(([^\s]+?\.(?:o|a|obj|lib)(?:\([^)]+\))?))"
        );
    QRegularExpressionMatch m = re.match(line);
    if (m.hasMatch()) {
        return m.captured(1).trimmed();
    }
    return QString();
}

QString MapDescriptor::extractFileName(const QString &fullPath) const
{
    QFileInfo fileInfo(fullPath);
    return fileInfo.fileName(); // Повертає "crtbegin.o"
}


}
