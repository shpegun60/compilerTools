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

    struct Symbol;
    struct File;
    struct Section;
    struct Fill;

    using Symbols = HashIndex<QString, Symbol>;
    using Files = HashIndex<QString, File>;
    using Sections = HashIndex<QString, Section>;
    using Fills = QList<Fill>;

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
        // iteration
        Sections::Index id;
        QSet<Files::Index> idFiles;
        QSet<Symbols::Index> idSymbols;
    };

    struct Fill {
        quint64 addr;
        quint64 size;
    };

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
    Symbols     _symbols {};
    Files       _files {};
    Sections    _sections{};
    Fills       _fills {};

    MapType type = MapType::Empty;
    // logging ---------------------------------------
    QString _log{};
    QTextStream stream;
};

}

#endif // IMAPFILE_H
