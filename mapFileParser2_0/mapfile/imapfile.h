#ifndef IMAPFILE_H
#define IMAPFILE_H

#include <QtContainerFwd>
#include <QTextStream>
#include <QSet>
#include "hashindex.h"

namespace compiler_tools {

namespace gnu {
class MapFileReader;
}

namespace clang {
class MapFileReader;
}

class IMapFile
{
    friend class IMapFileReader;
    friend class gnu::MapFileReader;
    friend class clang::MapFileReader;
public:
    IMapFile() : stream(&_log) {};
    ~IMapFile() = default;

    enum MapType {
        Empty,
        GNU,
        Clang
    };

    // fills -------------------------
    struct Fill {
        quint64 addr;
        quint64 size;
    };

    using Fills     = QList<Fill>;


    // Main symbolls -------------------------
    struct Symbol;
    struct File;
    struct Section;

    using Symbols   = HashIndex<QString, Symbol>;
    using Files     = HashIndex<QString, File>;
    using Sections  = HashIndex<QString, Section>;

    struct Symbol {
        QString name;
        QString lable;
        QString filepath;
        quint64 vram;
        std::optional<quint64> vrom;
        std::optional<quint64> size;
        // iteration
        Symbols::Index id;
        Sections::Index idSection;
        Files::Index idFile;
    };

    struct File {
        QString filepath;
        QString filename;
        quint64 vram;
        quint64 size;
        // iteration
        Files::Index id;
        QSet<Symbols::Index> idSymbols;
    };

    struct Section {
        QString name;
        quint64 vram;
        std::optional<quint64> vrom;
        quint64 size;
        int sizedev;
        // other for debug
        quint64 baddr;
        quint64 eaddr;
        // iteration
        Sections::Index id;
        QSet<Files::Index> idFiles;
        QSet<Symbols::Index> idSymbols;
    };

    /// Summary information for a section
    struct SectionSummary {
        QString name;                   // section name, e.g. ".text"
        quint64 vram;                   // starting address in RAM
        std::optional<quint64> vrom;    // starting address in ROM (if any)
        quint64 size;                   // section size in bytes
    };

    // Container for all sections with their summary information
    using SectionSummaries = HashIndex<QString, SectionSummary>;

public: /* functions */
    const Symbols& symbols() const { return _symbols; }
    const Files& files() const { return _files; }
    const Sections& sections() const { return _sections; }
    void clear();

public: /* other functions */
    inline const QString& getlog() const { return _log; }
    MapType getType() const { return type; }

private: /* other functions */
    void setType(const MapType type_) { type = type_; }
    QTextStream& log() { return stream; }

private:
    Fills       _fills {};

    Symbols     _symbols {};
    Files       _files {};
    Sections    _sections{};
    SectionSummaries _infos{};

    MapType type = MapType::Empty;
    // logging ---------------------------------------
    QString _log{};
    QTextStream stream;
};

}

#endif // IMAPFILE_H
