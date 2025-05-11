#ifndef MAPDESCRIPTOR_H
#define MAPDESCRIPTOR_H

#include <QRegularExpression>
#include <QtContainerFwd>

namespace compiler_tools::gnu {

class MapDescriptor
{
public:
    MapDescriptor() = default;
    ~MapDescriptor() = default;
public: /* fields */
    /* identificators */
    static const inline QString memLinkerInfoHeader = "Memory Configuration";
    static const inline QString memMapHeader        = "Linker script and memory map";
    static const inline QString fillIdentifier      = "*fill*";
    /* regex */
    static const inline QRegularExpression tokenSplitter = QRegularExpression{R"(\s+)"};
    static const inline QRegularExpression hexRegex = QRegularExpression{R"(\b(0[xX][0-9a-fA-F]+))", QRegularExpression::CaseInsensitiveOption};
    static const inline QRegularExpression sectionRegex = QRegularExpression{R"(^\*\((.*?)\))",
                                                                             QRegularExpression::CaseInsensitiveOption |
                                                                                 QRegularExpression::DotMatchesEverythingOption};

    /* ----- unknown section name --- */
    static const inline QString unknown  = "@UNKNOWN";
    /* markers */
    QStringList memMapIgnoreMarkers = {"LOAD ", "START GROUP", "END GROUP"};
    QStringList memMapEndMarkers = {"/DISCARD/", "OUTPUT", "LOAD linker stubs"};
    /* ----- segment names ---------- */
    QStringList sectionNames{};

public: /* functions */
    bool isEnd(const QString &line) const;
    bool isIgnore(const QString &line) const;
    bool isFill(const QString &line) const;

    std::pair<QString, quint64> readLineAddress(const QString &s) const;
    quint64 readLineSize(const QString &line) const;
    QString readLineName(const QString &line) const;
    QString readLinePath(const QString &line) const;
    QString extractFileName(const QString &fullPath) const;
};

}

#endif // MAPDESCRIPTOR_H
